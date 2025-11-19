#ifndef SHIELDPOSITIONDAO_H
#define SHIELDPOSITIONDAO_H

#include <QString>
#include <QSqlDatabase>
#include <QDateTime>

/**
 * @brief 盾构机位置数据访问对象
 * 
 * 管理盾构机的实时位置信息
 */
class ShieldPositionDAO
{
public:
    struct ShieldPosition {
        int id;
        int projectId;
        double frontLatitude;       // 前盾纬度
        double frontLongitude;      // 前盾经度
        QString frontStakeMark;     // 前盾桩号
        double rearLatitude;        // 盾尾纬度
        double rearLongitude;       // 盾尾经度
        QString rearStakeMark;      // 盾尾桩号
        double depth;               // 深度（米）
        double inclination;         // 倾角（度）
        int positioningMethod;      // 定位方式：0-GPS, 1-坐标, 2-桩号
        QDateTime updatedAt;        // 更新时间
        
        // 上次保存的位置（用于复位功能）
        double previousFrontLatitude;
        double previousFrontLongitude;
        QString previousFrontStakeMark;
        double previousRearLatitude;
        double previousRearLongitude;
        QString previousRearStakeMark;
        double previousDepth;
        double previousInclination;
    };

    ShieldPositionDAO();
    
    // 保存或更新盾构机位置
    bool savePosition(const ShieldPosition &position);
    
    // 获取项目的盾构机位置
    ShieldPosition getPosition(int projectId);
    
    // 检查项目是否有盾构机位置记录
    bool hasPosition(int projectId);
    
    // 删除项目的盾构机位置
    bool deletePosition(int projectId);

private:
    QSqlDatabase getDatabase();
};

#endif // SHIELDPOSITIONDAO_H
