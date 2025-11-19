#include "ShieldPositionDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

ShieldPositionDAO::ShieldPositionDAO()
{
}

QSqlDatabase ShieldPositionDAO::getDatabase()
{
    return DatabaseManager::instance().getDatabase();
}

bool ShieldPositionDAO::savePosition(const ShieldPosition &position)
{
    QSqlQuery query(getDatabase());
    
    // 检查是否已存在记录
    if (hasPosition(position.projectId)) {
        // 先读取当前位置，将其保存到previous字段
        ShieldPosition currentPos = getPosition(position.projectId);
        
        // 更新记录：将当前位置移到previous，新位置保存到current
        query.prepare("UPDATE shield_position SET "
                      "previous_front_latitude = :prevFrontLat, previous_front_longitude = :prevFrontLon, "
                      "previous_front_stake_mark = :prevFrontStake, "
                      "previous_rear_latitude = :prevRearLat, previous_rear_longitude = :prevRearLon, "
                      "previous_rear_stake_mark = :prevRearStake, "
                      "previous_depth = :prevDepth, previous_inclination = :prevIncl, "
                      "front_latitude = :frontLat, front_longitude = :frontLon, front_stake_mark = :frontStake, "
                      "rear_latitude = :rearLat, rear_longitude = :rearLon, rear_stake_mark = :rearStake, "
                      "depth = :depth, inclination = :incl, positioning_method = :method, "
                      "updated_at = CURRENT_TIMESTAMP "
                      "WHERE project_id = :projectId");
        
        // 绑定previous值（当前位置）
        query.bindValue(":prevFrontLat", currentPos.frontLatitude);
        query.bindValue(":prevFrontLon", currentPos.frontLongitude);
        query.bindValue(":prevFrontStake", currentPos.frontStakeMark);
        query.bindValue(":prevRearLat", currentPos.rearLatitude);
        query.bindValue(":prevRearLon", currentPos.rearLongitude);
        query.bindValue(":prevRearStake", currentPos.rearStakeMark);
        query.bindValue(":prevDepth", currentPos.depth);
        query.bindValue(":prevIncl", currentPos.inclination);
        
        qDebug() << "保存前位置到previous - 前盾:" << currentPos.frontStakeMark 
                 << "盾尾:" << currentPos.rearStakeMark;
    } else {
        // 插入新记录（初次保存，previous字段为空）
        query.prepare("INSERT INTO shield_position "
                      "(project_id, front_latitude, front_longitude, front_stake_mark, "
                      "rear_latitude, rear_longitude, rear_stake_mark, depth, inclination, positioning_method) "
                      "VALUES (:projectId, :frontLat, :frontLon, :frontStake, "
                      ":rearLat, :rearLon, :rearStake, :depth, :incl, :method)");
    }
    
    // 绑定新位置值
    query.bindValue(":projectId", position.projectId);
    query.bindValue(":frontLat", position.frontLatitude);
    query.bindValue(":frontLon", position.frontLongitude);
    query.bindValue(":frontStake", position.frontStakeMark);
    query.bindValue(":rearLat", position.rearLatitude);
    query.bindValue(":rearLon", position.rearLongitude);
    query.bindValue(":rearStake", position.rearStakeMark);
    query.bindValue(":depth", position.depth);
    query.bindValue(":incl", position.inclination);
    query.bindValue(":method", position.positioningMethod);
    
    if (!query.exec()) {
        qCritical() << "保存盾构机位置失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "盾构机位置已保存 - 项目ID:" << position.projectId 
             << "前盾:" << position.frontStakeMark 
             << "盾尾:" << position.rearStakeMark;
    
    return true;
}

ShieldPositionDAO::ShieldPosition ShieldPositionDAO::getPosition(int projectId)
{
    ShieldPosition position;
    position.id = -1;
    position.projectId = projectId;
    
    // 初始化previous字段为0
    position.previousFrontLatitude = 0.0;
    position.previousFrontLongitude = 0.0;
    position.previousRearLatitude = 0.0;
    position.previousRearLongitude = 0.0;
    position.previousDepth = 0.0;
    position.previousInclination = 0.0;
    
    QSqlQuery query(getDatabase());
    query.prepare("SELECT id, front_latitude, front_longitude, front_stake_mark, "
                  "rear_latitude, rear_longitude, rear_stake_mark, depth, inclination, "
                  "positioning_method, updated_at, "
                  "previous_front_latitude, previous_front_longitude, previous_front_stake_mark, "
                  "previous_rear_latitude, previous_rear_longitude, previous_rear_stake_mark, "
                  "previous_depth, previous_inclination "
                  "FROM shield_position WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        qCritical() << "查询盾构机位置失败:" << query.lastError().text();
        return position;
    }
    
    if (query.next()) {
        position.id = query.value(0).toInt();
        position.frontLatitude = query.value(1).toDouble();
        position.frontLongitude = query.value(2).toDouble();
        position.frontStakeMark = query.value(3).toString();
        position.rearLatitude = query.value(4).toDouble();
        position.rearLongitude = query.value(5).toDouble();
        position.rearStakeMark = query.value(6).toString();
        position.depth = query.value(7).toDouble();
        position.inclination = query.value(8).toDouble();
        position.positioningMethod = query.value(9).toInt();
        position.updatedAt = query.value(10).toDateTime();
        
        // 读取previous字段
        position.previousFrontLatitude = query.value(11).toDouble();
        position.previousFrontLongitude = query.value(12).toDouble();
        position.previousFrontStakeMark = query.value(13).toString();
        position.previousRearLatitude = query.value(14).toDouble();
        position.previousRearLongitude = query.value(15).toDouble();
        position.previousRearStakeMark = query.value(16).toString();
        position.previousDepth = query.value(17).toDouble();
        position.previousInclination = query.value(18).toDouble();
        
        if (!position.previousFrontStakeMark.isEmpty()) {
            qDebug() << "读取到previous位置 - 前盾:" << position.previousFrontStakeMark 
                     << "盾尾:" << position.previousRearStakeMark;
        }
    }
    
    return position;
}

bool ShieldPositionDAO::hasPosition(int projectId)
{
    QSqlQuery query(getDatabase());
    query.prepare("SELECT COUNT(*) FROM shield_position WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

bool ShieldPositionDAO::deletePosition(int projectId)
{
    QSqlQuery query(getDatabase());
    query.prepare("DELETE FROM shield_position WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        qCritical() << "删除盾构机位置失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}
