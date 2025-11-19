#include "MileageDAO.h"
#include "DatabaseManager.h"
#include "../utils/CoordinateConverter.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QtMath>

MileageDAO::MileageDAO()
{
}

QSqlDatabase MileageDAO::getDatabase()
{
    return DatabaseManager::instance().getDatabase();
}

double MileageDAO::stakeMarkToMileage(const QString &stakeMark)
{
    // 解析桩号格式，如 "K1+190.00" -> 1190.00
    // 格式: K{千米}+{米}
    QRegularExpression re("K(\\d+)\\+(\\d+(?:\\.\\d+)?)");
    QRegularExpressionMatch match = re.match(stakeMark);
    
    if (match.hasMatch()) {
        double km = match.captured(1).toDouble();
        double m = match.captured(2).toDouble();
        return km * 1000.0 + m;
    }
    
    // 如果不匹配标准格式，尝试直接转换为数值
    return stakeMark.toDouble();
}

QString MileageDAO::mileageToStakeMark(double mileage)
{
    // 将数值里程转换为桩号格式
    // 1190.00 -> "K1+190.00"
    int km = static_cast<int>(mileage / 1000.0);
    double m = mileage - (km * 1000.0);
    return QString("K%1+%2").arg(km).arg(m, 0, 'f', 2);
}

bool MileageDAO::addMileagePoint(const MileagePoint &point)
{
    QSqlQuery query(getDatabase());
    
    query.prepare("INSERT INTO mileage_points (project_id, stake_mark, mileage, "
                  "latitude, longitude, elevation, near_borehole) "
                  "VALUES (:projectId, :stakeMark, :mileage, :lat, :lon, :elev, :borehole)");
    
    query.bindValue(":projectId", point.projectId);
    query.bindValue(":stakeMark", point.stakeMark);
    query.bindValue(":mileage", point.mileage);
    query.bindValue(":lat", point.latitude);
    query.bindValue(":lon", point.longitude);
    query.bindValue(":elev", point.elevation);
    query.bindValue(":borehole", point.nearBorehole);
    
    if (!query.exec()) {
        qCritical() << "添加里程点失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool MileageDAO::batchAddMileagePoints(const QList<MileagePoint> &points)
{
    QSqlDatabase db = getDatabase();
    db.transaction();
    
    bool success = true;
    for (const auto &point : points) {
        if (!addMileagePoint(point)) {
            success = false;
            break;
        }
    }
    
    if (success) {
        db.commit();
    } else {
        db.rollback();
    }
    
    return success;
}

QList<MileageDAO::MileagePoint> MileageDAO::getMileagePointsByProject(int projectId)
{
    QList<MileagePoint> points;
    QSqlQuery query(getDatabase());
    
    query.prepare("SELECT id, project_id, stake_mark, mileage, latitude, longitude, "
                  "elevation, near_borehole FROM mileage_points "
                  "WHERE project_id = :projectId ORDER BY mileage");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        qCritical() << "查询里程点失败:" << query.lastError().text();
        return points;
    }
    
    while (query.next()) {
        MileagePoint point;
        point.id = query.value(0).toInt();
        point.projectId = query.value(1).toInt();
        point.stakeMark = query.value(2).toString();
        point.mileage = query.value(3).toDouble();
        point.latitude = query.value(4).toDouble();
        point.longitude = query.value(5).toDouble();
        point.elevation = query.value(6).toDouble();
        point.nearBorehole = query.value(7).toString();
        points.append(point);
    }
    
    return points;
}

MileageDAO::MileagePoint MileageDAO::getMileagePointByStake(int projectId, const QString &stakeMark)
{
    MileagePoint point;
    point.id = -1; // 无效ID表示未找到
    
    // 先尝试直接匹配
    QSqlQuery query(getDatabase());
    query.prepare("SELECT id, project_id, stake_mark, mileage, latitude, longitude, "
                  "elevation, near_borehole FROM mileage_points "
                  "WHERE project_id = :projectId AND stake_mark = :stakeMark");
    query.bindValue(":projectId", projectId);
    query.bindValue(":stakeMark", stakeMark);
    
    if (query.exec() && query.next()) {
        point.id = query.value(0).toInt();
        point.projectId = query.value(1).toInt();
        point.stakeMark = query.value(2).toString();
        point.mileage = query.value(3).toDouble();
        point.latitude = query.value(4).toDouble();
        point.longitude = query.value(5).toDouble();
        point.elevation = query.value(6).toDouble();
        point.nearBorehole = query.value(7).toString();
        return point;
    }
    
    // 如果直接匹配失败，尝试将输入的桩号转换为数值，然后查找最近的点
    double mileage = stakeMarkToMileage(stakeMark);
    if (mileage > 0) {
        // 查找最接近的里程点（容差±5米）
        query.prepare("SELECT id, project_id, stake_mark, mileage, latitude, longitude, "
                      "elevation, near_borehole FROM mileage_points "
                      "WHERE project_id = :projectId "
                      "AND ABS(mileage - :mileage) <= 5.0 "
                      "ORDER BY ABS(mileage - :mileage) LIMIT 1");
        query.bindValue(":projectId", projectId);
        query.bindValue(":mileage", mileage);
        
        if (query.exec() && query.next()) {
            point.id = query.value(0).toInt();
            point.projectId = query.value(1).toInt();
            point.stakeMark = query.value(2).toString();
            point.mileage = query.value(3).toDouble();
            point.latitude = query.value(4).toDouble();
            point.longitude = query.value(5).toDouble();
            point.elevation = query.value(6).toDouble();
            point.nearBorehole = query.value(7).toString();
            
            qDebug() << "桩号" << stakeMark << "匹配到最近点:" << point.stakeMark 
                     << "差值:" << qAbs(point.mileage - mileage) << "米";
        }
    }
    
    return point;
}

MileageDAO::MileagePoint MileageDAO::getMileagePointByValue(int projectId, double mileage)
{
    MileagePoint point;
    point.id = -1;
    
    // 查找最接近的里程点
    QSqlQuery query(getDatabase());
    query.prepare("SELECT id, project_id, stake_mark, mileage, latitude, longitude, "
                  "elevation, near_borehole FROM mileage_points "
                  "WHERE project_id = :projectId "
                  "ORDER BY ABS(mileage - :mileage) LIMIT 1");
    query.bindValue(":projectId", projectId);
    query.bindValue(":mileage", mileage);
    
    if (!query.exec()) {
        qCritical() << "根据里程数值查询失败:" << query.lastError().text();
        return point;
    }
    
    if (query.next()) {
        point.id = query.value(0).toInt();
        point.projectId = query.value(1).toInt();
        point.stakeMark = query.value(2).toString();
        point.mileage = query.value(3).toDouble();
        point.latitude = query.value(4).toDouble();
        point.longitude = query.value(5).toDouble();
        point.elevation = query.value(6).toDouble();
        point.nearBorehole = query.value(7).toString();
    }
    
    return point;
}

MileageDAO::MileagePoint MileageDAO::getNearestMileagePoint(int projectId, double lat, double lon)
{
    MileagePoint point;
    point.id = -1;
    
    // 获取所有里程点
    QList<MileagePoint> allPoints = getMileagePointsByProject(projectId);
    
    if (allPoints.isEmpty()) {
        return point;
    }
    
    // 计算最近的点
    double minDistance = std::numeric_limits<double>::max();
    for (const auto &p : allPoints) {
        // 简单的欧几里得距离（对于小范围可以这样计算）
        double dx = (p.longitude - lon) * 111.0; // 约111km per degree
        double dy = (p.latitude - lat) * 111.0;
        double distance = qSqrt(dx * dx + dy * dy);
        
        if (distance < minDistance) {
            minDistance = distance;
            point = p;
        }
    }
    
    return point;
}

bool MileageDAO::deleteMileagePointsByProject(int projectId)
{
    QSqlQuery query(getDatabase());
    query.prepare("DELETE FROM mileage_points WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        qCritical() << "删除项目里程数据失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool MileageDAO::importFromCSV(int projectId, const QString &csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开CSV文件:" << csvFilePath;
        return false;
    }
    
    QTextStream in(&file);
    // Qt 6默认使用UTF-8编码，无需设置
    
    // 跳过表头
    QString header = in.readLine();
    
    QList<MileagePoint> points;
    int lineNumber = 1;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        
        // 跳过空行
        if (line.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = line.split(',');
        
        // 检查字段数量
        if (fields.size() < 17) {
            qWarning() << "CSV第" << lineNumber << "行字段不足，跳过";
            continue;
        }
        
        // 解析数据
        // 格式: 序号,附近钻孔,左上x,左上y,左上z,,左下x,左下y,左下z,,右上x,右上y,右上z,,右下x,右下y,右下z,
        MileagePoint point;
        point.projectId = projectId;
        point.nearBorehole = fields[1].trimmed();
        
        // 使用左上角的坐标作为代表点
        double x = fields[2].trimmed().toDouble();  // 投影坐标 X
        double y = fields[3].trimmed().toDouble();  // 投影坐标 Y
        double z = fields[4].trimmed().toDouble();  // 高程
        
        // 使用高斯-克吕格投影转WGS84经纬度
        // 青岛地区使用3度带第41带（中央子午线123°）
        double lat, lon;
        CoordinateConverter::gaussKrugerToWGS84(x, y, 41, lat, lon);
        point.latitude = lat;
        point.longitude = lon;
        point.elevation = z;
        
        // 根据序号计算里程（假设每个断面间隔一定距离）
        int sequence = fields[0].trimmed().toInt();
        point.mileage = sequence * 10.0;  // 假设每10米一个断面
        point.stakeMark = mileageToStakeMark(point.mileage);
        
        points.append(point);
    }
    
    file.close();
    
    if (points.isEmpty()) {
        qWarning() << "CSV文件中没有有效数据";
        return false;
    }
    
    // 批量插入数据
    return batchAddMileagePoints(points);
}

