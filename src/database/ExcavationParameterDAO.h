#ifndef EXCAVATIONPARAMETERDAO_H
#define EXCAVATIONPARAMETERDAO_H

#include "../models/ExcavationParameter.h"
#include <QList>
#include <QString>

/**
 * @brief 掘进参数数据访问对象
 * 
 * 负责掘进参数的数据库操作
 */
class ExcavationParameterDAO
{
public:
    ExcavationParameterDAO();
    ~ExcavationParameterDAO() = default;

    /**
     * @brief 插入掘进参数记录
     * @param param 掘进参数对象
     * @return 是否插入成功
     */
    bool insertExcavationParameter(const ExcavationParameter &param);

    /**
     * @brief 批量插入掘进参数记录
     * @param params 掘进参数列表
     * @return 是否插入成功
     */
    bool batchInsertExcavationParameters(const QList<ExcavationParameter> &params);

    /**
     * @brief 根据项目ID查询掘进参数记录
     * @param projectId 项目ID
     * @return 掘进参数列表
     */
    QList<ExcavationParameter> getExcavationParametersByProjectId(int projectId);

    /**
     * @brief 根据项目ID和时间范围查询掘进参数记录
     * @param projectId 项目ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @return 掘进参数列表
     */
    QList<ExcavationParameter> getExcavationParametersByTimeRange(
        int projectId, 
        const QDateTime &startTime, 
        const QDateTime &endTime);

    /**
     * @brief 根据项目ID和里程范围查询掘进参数记录
     * @param projectId 项目ID
     * @param startMileage 开始里程
     * @param endMileage 结束里程
     * @return 掘进参数列表
     */
    QList<ExcavationParameter> getExcavationParametersByMileageRange(
        int projectId,
        double startMileage,
        double endMileage);

    /**
     * @brief 获取项目最新的掘进参数记录
     * @param projectId 项目ID
     * @param param 输出参数
     * @return 是否查询成功
     */
    bool getLatestExcavationParameter(int projectId, ExcavationParameter &param);

    /**
     * @brief 分页查询掘进参数记录
     * @param projectId 项目ID
     * @param page 页码（从1开始）
     * @param pageSize 每页记录数
     * @return 掘进参数列表
     */
    QList<ExcavationParameter> getExcavationParametersByPage(
        int projectId, 
        int page, 
        int pageSize);

    /**
     * @brief 获取项目掘进参数记录总数
     * @param projectId 项目ID
     * @return 记录总数
     */
    int getExcavationParametersCount(int projectId);

    /**
     * @brief 删除项目的所有掘进参数记录
     * @param projectId 项目ID
     * @return 是否删除成功
     */
    bool deleteExcavationParametersByProjectId(int projectId);

    /**
     * @brief 获取最后的错误信息
     * @return 错误信息
     */
    QString getLastError() const { return lastError; }

private:
    QString lastError;
};

#endif // EXCAVATIONPARAMETERDAO_H
