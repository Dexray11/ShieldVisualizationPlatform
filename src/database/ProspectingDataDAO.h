#ifndef PROSPECTINGDATADAO_H
#define PROSPECTINGDATADAO_H

#include "../models/ProspectingData.h"
#include <QVector>
#include <QString>

/**
 * @brief 补勘数据访问对象类
 * 
 * 负责补勘数据的数据库操作
 */
class ProspectingDataDAO
{
public:
    ProspectingDataDAO();
    ~ProspectingDataDAO() = default;

    /**
     * @brief 插入补勘数据记录
     * @param data 补勘数据对象
     * @return 成功返回记录ID，失败返回-1
     */
    int insert(const ProspectingData &data);

    /**
     * @brief 批量插入补勘数据记录
     * @param dataList 补勘数据对象列表
     * @return 成功返回true
     */
    bool insertBatch(const QVector<ProspectingData> &dataList);

    /**
     * @brief 更新补勘数据记录
     * @param data 补勘数据对象
     * @return 成功返回true
     */
    bool update(const ProspectingData &data);

    /**
     * @brief 根据ID删除记录
     * @param id 记录ID
     * @return 成功返回true
     */
    bool deleteById(int id);

    /**
     * @brief 根据项目ID删除所有记录
     * @param projectId 项目ID
     * @return 成功返回true
     */
    bool deleteByProjectId(int projectId);

    /**
     * @brief 根据ID查询记录
     * @param id 记录ID
     * @return 成功返回数据对象，失败返回空对象
     */
    ProspectingData selectById(int id);

    /**
     * @brief 根据项目ID查询所有记录
     * @param projectId 项目ID
     * @return 数据对象列表
     */
    QVector<ProspectingData> selectByProjectId(int projectId);

    /**
     * @brief 分页查询项目的补勘数据
     * @param projectId 项目ID
     * @param offset 偏移量
     * @param limit 限制数量
     * @return 数据对象列表
     */
    QVector<ProspectingData> selectByProjectIdWithPagination(int projectId, int offset, int limit);

    /**
     * @brief 查询项目的补勘数据记录总数
     * @param projectId 项目ID
     * @return 记录总数
     */
    int countByProjectId(int projectId);

    /**
     * @brief 根据桩号范围查询记录
     * @param projectId 项目ID
     * @param startMileage 起始里程
     * @param endMileage 结束里程
     * @return 数据对象列表
     */
    QVector<ProspectingData> selectByMileageRange(int projectId, double startMileage, double endMileage);

    /**
     * @brief 查询项目最新的补勘数据记录
     * @param projectId 项目ID
     * @param count 记录数量
     * @return 数据对象列表
     */
    QVector<ProspectingData> selectLatestByProjectId(int projectId, int count = 10);

    /**
     * @brief 获取项目的所有补勘数据（便捷方法）
     * @param projectId 项目ID
     * @return 数据对象列表
     */
    QList<ProspectingData> getProspectingDataByProjectId(int projectId);

    /**
     * @brief 获取所有补勘数据
     * @return 数据对象列表
     */
    QList<ProspectingData> getAllProspectingData();

    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString getLastError() const { return lastError; }

private:
    QString lastError;
};

#endif // PROSPECTINGDATADAO_H
