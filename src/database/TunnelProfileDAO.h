#ifndef TUNNELPROFILEDAO_H
#define TUNNELPROFILEDAO_H

#include <QString>
#include <QVector>

/**
 * @brief 隧道断面轮廓数据结构
 */
struct TunnelProfileData {
    int profileId;         // 断面ID
    int projectId;         // 项目ID
    QString nearBorehole;  // 附近钻孔
    double mileage;        // 里程
    
    // 四个角点坐标（左上、左下、右上、右下）
    double topLeftX, topLeftY, topLeftZ;
    double bottomLeftX, bottomLeftY, bottomLeftZ;
    double topRightX, topRightY, topRightZ;
    double bottomRightX, bottomRightY, bottomRightZ;
};

/**
 * @brief 隧道断面数据访问对象
 */
class TunnelProfileDAO
{
public:
    TunnelProfileDAO();
    
    /**
     * @brief 插入隧道断面数据
     * @param profile 断面数据
     * @return 成功返回断面ID，失败返回-1
     */
    int insertProfile(const TunnelProfileData &profile);
    
    /**
     * @brief 根据项目ID获取所有隧道断面数据
     * @param projectId 项目ID
     * @return 断面数据列表
     */
    QVector<TunnelProfileData> getProfilesByProjectId(int projectId);
    
    /**
     * @brief 删除项目的所有隧道断面数据
     * @param projectId 项目ID
     * @return 成功返回true
     */
    bool deleteProfilesByProjectId(int projectId);
    
    /**
     * @brief 获取最后的错误信息
     */
    QString getLastError() const { return lastError; }
    
private:
    QString lastError;
};

#endif // TUNNELPROFILEDAO_H
