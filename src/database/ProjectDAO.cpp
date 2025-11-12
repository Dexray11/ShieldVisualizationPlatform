#include "ProjectDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

ProjectDAO::ProjectDAO()
{
}

ProjectDAO::~ProjectDAO()
{
}

QList<Project> ProjectDAO::getAllProjects()
{
    QList<Project> projects;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    QString sql = "SELECT project_id, project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "map_2d_path, map_3d_path, created_at, updated_at "
                  "FROM projects ORDER BY created_at DESC";
    
    if (!query.exec(sql)) {
        lastError = "查询项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return projects;
    }
    
    while (query.next()) {
        Project project;
        project.setProjectId(query.value("project_id").toInt());
        project.setProjectName(query.value("project_name").toString());
        project.setBrief(query.value("brief").toString());
        project.setLatitude(query.value("latitude").toDouble());
        project.setLongitude(query.value("longitude").toDouble());
        project.setConstructionUnit(query.value("construction_unit").toString());
        project.setStartDate(query.value("start_date").toString());
        project.setProgress(query.value("progress").toDouble());
        project.setLocation(query.value("location").toString());
        project.setStatus(query.value("status").toString());
        project.setMap2DPath(query.value("map_2d_path").toString());
        project.setMap3DPath(query.value("map_3d_path").toString());
        project.setCreatedAt(query.value("created_at").toDateTime());
        project.setUpdatedAt(query.value("updated_at").toDateTime());
        
        projects.append(project);
    }
    
    return projects;
}

Project ProjectDAO::getProjectById(int projectId)
{
    Project project;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT project_id, project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "map_2d_path, map_3d_path, created_at, updated_at "
                  "FROM projects WHERE project_id = :id");
    query.bindValue(":id", projectId);
    
    if (!query.exec()) {
        lastError = "查询项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return project;
    }
    
    if (query.next()) {
        project.setProjectId(query.value("project_id").toInt());
        project.setProjectName(query.value("project_name").toString());
        project.setBrief(query.value("brief").toString());
        project.setLatitude(query.value("latitude").toDouble());
        project.setLongitude(query.value("longitude").toDouble());
        project.setConstructionUnit(query.value("construction_unit").toString());
        project.setStartDate(query.value("start_date").toString());
        project.setProgress(query.value("progress").toDouble());
        project.setLocation(query.value("location").toString());
        project.setStatus(query.value("status").toString());
        project.setMap2DPath(query.value("map_2d_path").toString());
        project.setMap3DPath(query.value("map_3d_path").toString());
        project.setCreatedAt(query.value("created_at").toDateTime());
        project.setUpdatedAt(query.value("updated_at").toDateTime());
    }
    
    return project;
}

Project ProjectDAO::getProjectByName(const QString &projectName)
{
    Project project;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT project_id, project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "map_2d_path, map_3d_path, created_at, updated_at "
                  "FROM projects WHERE project_name = :name");
    query.bindValue(":name", projectName);
    
    if (!query.exec()) {
        lastError = "查询项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return project;
    }
    
    if (query.next()) {
        project.setProjectId(query.value("project_id").toInt());
        project.setProjectName(query.value("project_name").toString());
        project.setBrief(query.value("brief").toString());
        project.setLatitude(query.value("latitude").toDouble());
        project.setLongitude(query.value("longitude").toDouble());
        project.setConstructionUnit(query.value("construction_unit").toString());
        project.setStartDate(query.value("start_date").toString());
        project.setProgress(query.value("progress").toDouble());
        project.setLocation(query.value("location").toString());
        project.setStatus(query.value("status").toString());
        project.setMap2DPath(query.value("map_2d_path").toString());
        project.setMap3DPath(query.value("map_3d_path").toString());
        project.setCreatedAt(query.value("created_at").toDateTime());
        project.setUpdatedAt(query.value("updated_at").toDateTime());
    }
    
    return project;
}

QList<Project> ProjectDAO::getProjectsByStatus(const QString &status)
{
    QList<Project> projects;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT project_id, project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "map_2d_path, map_3d_path, created_at, updated_at "
                  "FROM projects WHERE status = :status ORDER BY created_at DESC");
    query.bindValue(":status", status);
    
    if (!query.exec()) {
        lastError = "查询项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return projects;
    }
    
    while (query.next()) {
        Project project;
        project.setProjectId(query.value("project_id").toInt());
        project.setProjectName(query.value("project_name").toString());
        project.setBrief(query.value("brief").toString());
        project.setLatitude(query.value("latitude").toDouble());
        project.setLongitude(query.value("longitude").toDouble());
        project.setConstructionUnit(query.value("construction_unit").toString());
        project.setStartDate(query.value("start_date").toString());
        project.setProgress(query.value("progress").toDouble());
        project.setLocation(query.value("location").toString());
        project.setStatus(query.value("status").toString());
        project.setMap2DPath(query.value("map_2d_path").toString());
        project.setMap3DPath(query.value("map_3d_path").toString());
        project.setCreatedAt(query.value("created_at").toDateTime());
        project.setUpdatedAt(query.value("updated_at").toDateTime());
        
        projects.append(project);
    }
    
    return projects;
}

bool ProjectDAO::insertProject(const Project &project)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO projects (project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "map_2d_path, map_3d_path) "
                  "VALUES (:name, :brief, :lat, :lon, :unit, :date, :progress, "
                  ":location, :status, :map2d, :map3d)");
    
    query.bindValue(":name", project.getProjectName());
    query.bindValue(":brief", project.getBrief());
    query.bindValue(":lat", project.getLatitude());
    query.bindValue(":lon", project.getLongitude());
    query.bindValue(":unit", project.getConstructionUnit());
    query.bindValue(":date", project.getStartDate());
    query.bindValue(":progress", project.getProgress());
    query.bindValue(":location", project.getLocation());
    query.bindValue(":status", project.getStatus());
    query.bindValue(":map2d", project.getMap2DPath());
    query.bindValue(":map3d", project.getMap3DPath());
    
    if (!query.exec()) {
        lastError = "插入项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool ProjectDAO::updateProject(const Project &project)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("UPDATE projects SET project_name = :name, brief = :brief, "
                  "latitude = :lat, longitude = :lon, construction_unit = :unit, "
                  "start_date = :date, progress = :progress, location = :location, "
                  "status = :status, map_2d_path = :map2d, map_3d_path = :map3d, "
                  "updated_at = CURRENT_TIMESTAMP "
                  "WHERE project_id = :id");
    
    query.bindValue(":id", project.getProjectId());
    query.bindValue(":name", project.getProjectName());
    query.bindValue(":brief", project.getBrief());
    query.bindValue(":lat", project.getLatitude());
    query.bindValue(":lon", project.getLongitude());
    query.bindValue(":unit", project.getConstructionUnit());
    query.bindValue(":date", project.getStartDate());
    query.bindValue(":progress", project.getProgress());
    query.bindValue(":location", project.getLocation());
    query.bindValue(":status", project.getStatus());
    query.bindValue(":map2d", project.getMap2DPath());
    query.bindValue(":map3d", project.getMap3DPath());
    
    if (!query.exec()) {
        lastError = "更新项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool ProjectDAO::deleteProject(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM projects WHERE project_id = :id");
    query.bindValue(":id", projectId);
    
    if (!query.exec()) {
        lastError = "删除项目失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

int ProjectDAO::getProjectCount()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    if (!query.exec("SELECT COUNT(*) as count FROM projects")) {
        lastError = "查询项目数量失败: " + query.lastError().text();
        qWarning() << lastError;
        return 0;
    }
    
    if (query.next()) {
        return query.value("count").toInt();
    }
    
    return 0;
}

double ProjectDAO::getAverageProgress()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    if (!query.exec("SELECT AVG(progress) as avg_progress FROM projects")) {
        lastError = "查询平均进度失败: " + query.lastError().text();
        qWarning() << lastError;
        return 0.0;
    }
    
    if (query.next()) {
        return query.value("avg_progress").toDouble();
    }
    
    return 0.0;
}
