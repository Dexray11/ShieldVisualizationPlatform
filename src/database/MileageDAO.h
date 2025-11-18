#ifndef MILEAGEDAO_H
#define MILEAGEDAO_H

#include <QString>
#include <QList>
#include <QSqlDatabase>

/**
 * @brief 里程/桩号数据访问对象
 * 
 * 管理项目中的里程（桩号）数据
 * 支持坐标与桩号的相互转换
 */
class MileageDAO
{
public:
    struct MileagePoint {
        int id;
        int projectId;
        QString stakeMark;     // 桩号，如 "K1+190.00"
        double mileage;        // 实际里程数值（米）
        double latitude;       // 纬度
        double longitude;      // 经度
        double elevation;      // 高程
        QString nearBorehole;  // 附近钻孔编号
    };

    MileageDAO();
    
    // 添加里程点
    bool addMileagePoint(const MileagePoint &point);
    
    // 批量添加里程点
    bool batchAddMileagePoints(const QList<MileagePoint> &points);
    
    // 获取项目的所有里程点
    QList<MileagePoint> getMileagePointsByProject(int projectId);
    
    // 根据桩号获取坐标
    MileagePoint getMileagePointByStake(int projectId, const QString &stakeMark);
    
    // 根据里程数值获取最近的桩号点
    MileagePoint getMileagePointByValue(int projectId, double mileage);
    
    // 根据坐标获取最近的桩号点
    MileagePoint getNearestMileagePoint(int projectId, double lat, double lon);
    
    // 删除项目的所有里程数据
    bool deleteMileagePointsByProject(int projectId);
    
    // 从CSV导入里程数据
    bool importFromCSV(int projectId, const QString &csvFilePath);
    
private:
    QSqlDatabase getDatabase();
    
    // 将字符串桩号转换为数值里程（如 "K1+190.00" -> 1190.00）
    double stakeMarkToMileage(const QString &stakeMark);
    
    // 将数值里程转换为桩号字符串（如 1190.00 -> "K1+190.00"）
    QString mileageToStakeMark(double mileage);
};

#endif // MILEAGEDAO_H
