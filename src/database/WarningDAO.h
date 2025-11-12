#ifndef WARNINGDAO_H
#define WARNINGDAO_H

#include "../models/Warning.h"
#include <QList>
#include <QString>

/**
 * @brief 预警数据访问对象类
 * 
 * 负责预警信息相关的数据库操作
 */
class WarningDAO
{
public:
    WarningDAO();
    ~WarningDAO();
    
    // 查询所有预警
    QList<Warning> getAllWarnings();
    
    // 根据项目ID查询预警
    QList<Warning> getWarningsByProjectId(int projectId);
    
    // 根据项目名称查询预警
    QList<Warning> getWarningsByProjectName(const QString &projectName);
    
    // 根据预警级别查询预警
    QList<Warning> getWarningsByLevel(const QString &level);
    
    // 插入新预警
    bool insertWarning(const Warning &warning);
    
    // 更新预警信息
    bool updateWarning(const Warning &warning);
    
    // 删除预警
    bool deleteWarning(int warningId);
    
    // 获取某个项目的预警数量
    int getWarningCountByProject(int projectId);
    
    // 获取总预警数量
    int getTotalWarningCount();
    
    // 获取最后的错误信息
    QString getLastError() const { return lastError; }

private:
    QString lastError;
};

#endif // WARNINGDAO_H
