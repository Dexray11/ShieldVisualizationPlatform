#ifndef PROJECTDAO_H
#define PROJECTDAO_H

#include "../models/Project.h"
#include <QList>
#include <QString>

/**
 * @brief 项目数据访问对象类
 * 
 * 负责项目相关的数据库操作
 */
class ProjectDAO
{
public:
    ProjectDAO();
    ~ProjectDAO();
    
    // 查询所有项目
    QList<Project> getAllProjects();
    
    // 根据ID查询项目
    Project getProjectById(int projectId);
    
    // 根据名称查询项目
    Project getProjectByName(const QString &projectName);
    
    // 根据状态查询项目
    QList<Project> getProjectsByStatus(const QString &status);
    
    // 插入新项目
    bool insertProject(const Project &project);
    
    // 更新项目信息
    bool updateProject(const Project &project);
    
    // 删除项目
    bool deleteProject(int projectId);
    
    // 获取项目总数
    int getProjectCount();
    
    // 获取项目平均进度
    double getAverageProgress();
    
    // 获取最后的错误信息
    QString getLastError() const { return lastError; }

private:
    QString lastError;
};

#endif // PROJECTDAO_H
