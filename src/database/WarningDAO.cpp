#include "WarningDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

WarningDAO::WarningDAO()
{
}

WarningDAO::~WarningDAO()
{
}

QList<Warning> WarningDAO::getAllWarnings()
{
    QList<Warning> warnings;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    QString sql = "SELECT warning_id, project_id, warning_number, warning_level, "
                  "warning_type, latitude, longitude, depth, threshold_value, "
                  "distance, warning_time "
                  "FROM warnings ORDER BY warning_time DESC";
    
    if (!query.exec(sql)) {
        lastError = "查询预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return warnings;
    }
    
    while (query.next()) {
        Warning warning;
        warning.setWarningId(query.value("warning_id").toInt());
        warning.setProjectId(query.value("project_id").toInt());
        warning.setWarningNumber(query.value("warning_number").toInt());
        warning.setWarningLevel(query.value("warning_level").toString());
        warning.setWarningType(query.value("warning_type").toString());
        warning.setLatitude(query.value("latitude").toDouble());
        warning.setLongitude(query.value("longitude").toDouble());
        warning.setDepth(query.value("depth").toDouble());
        warning.setThresholdValue(query.value("threshold_value").toInt());
        warning.setDistance(query.value("distance").toDouble());
        warning.setWarningTime(query.value("warning_time").toDateTime());
        
        warnings.append(warning);
    }
    
    return warnings;
}

QList<Warning> WarningDAO::getWarningsByProjectId(int projectId)
{
    QList<Warning> warnings;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT warning_id, project_id, warning_number, warning_level, "
                  "warning_type, latitude, longitude, depth, threshold_value, "
                  "distance, warning_time "
                  "FROM warnings WHERE project_id = :projectId ORDER BY warning_time DESC");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询项目预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return warnings;
    }
    
    while (query.next()) {
        Warning warning;
        warning.setWarningId(query.value("warning_id").toInt());
        warning.setProjectId(query.value("project_id").toInt());
        warning.setWarningNumber(query.value("warning_number").toInt());
        warning.setWarningLevel(query.value("warning_level").toString());
        warning.setWarningType(query.value("warning_type").toString());
        warning.setLatitude(query.value("latitude").toDouble());
        warning.setLongitude(query.value("longitude").toDouble());
        warning.setDepth(query.value("depth").toDouble());
        warning.setThresholdValue(query.value("threshold_value").toInt());
        warning.setDistance(query.value("distance").toDouble());
        warning.setWarningTime(query.value("warning_time").toDateTime());
        
        warnings.append(warning);
    }
    
    return warnings;
}

QList<Warning> WarningDAO::getWarningsByProjectName(const QString &projectName)
{
    QList<Warning> warnings;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT w.warning_id, w.project_id, w.warning_number, w.warning_level, "
                  "w.warning_type, w.latitude, w.longitude, w.depth, w.threshold_value, "
                  "w.distance, w.warning_time "
                  "FROM warnings w "
                  "JOIN projects p ON w.project_id = p.project_id "
                  "WHERE p.project_name = :projectName "
                  "ORDER BY w.warning_time DESC");
    query.bindValue(":projectName", projectName);
    
    if (!query.exec()) {
        lastError = "查询项目预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return warnings;
    }
    
    while (query.next()) {
        Warning warning;
        warning.setWarningId(query.value("warning_id").toInt());
        warning.setProjectId(query.value("project_id").toInt());
        warning.setWarningNumber(query.value("warning_number").toInt());
        warning.setWarningLevel(query.value("warning_level").toString());
        warning.setWarningType(query.value("warning_type").toString());
        warning.setLatitude(query.value("latitude").toDouble());
        warning.setLongitude(query.value("longitude").toDouble());
        warning.setDepth(query.value("depth").toDouble());
        warning.setThresholdValue(query.value("threshold_value").toInt());
        warning.setDistance(query.value("distance").toDouble());
        warning.setWarningTime(query.value("warning_time").toDateTime());
        
        warnings.append(warning);
    }
    
    return warnings;
}

QList<Warning> WarningDAO::getWarningsByLevel(const QString &level)
{
    QList<Warning> warnings;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT warning_id, project_id, warning_number, warning_level, "
                  "warning_type, latitude, longitude, depth, threshold_value, "
                  "distance, warning_time "
                  "FROM warnings WHERE warning_level = :level ORDER BY warning_time DESC");
    query.bindValue(":level", level);
    
    if (!query.exec()) {
        lastError = "查询预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return warnings;
    }
    
    while (query.next()) {
        Warning warning;
        warning.setWarningId(query.value("warning_id").toInt());
        warning.setProjectId(query.value("project_id").toInt());
        warning.setWarningNumber(query.value("warning_number").toInt());
        warning.setWarningLevel(query.value("warning_level").toString());
        warning.setWarningType(query.value("warning_type").toString());
        warning.setLatitude(query.value("latitude").toDouble());
        warning.setLongitude(query.value("longitude").toDouble());
        warning.setDepth(query.value("depth").toDouble());
        warning.setThresholdValue(query.value("threshold_value").toInt());
        warning.setDistance(query.value("distance").toDouble());
        warning.setWarningTime(query.value("warning_time").toDateTime());
        
        warnings.append(warning);
    }
    
    return warnings;
}

bool WarningDAO::insertWarning(const Warning &warning)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO warnings (project_id, warning_number, warning_level, "
                  "warning_type, latitude, longitude, depth, threshold_value, distance, warning_time) "
                  "VALUES (:projectId, :number, :level, :type, :lat, :lon, :depth, "
                  ":threshold, :distance, :time)");
    
    query.bindValue(":projectId", warning.getProjectId());
    query.bindValue(":number", warning.getWarningNumber());
    query.bindValue(":level", warning.getWarningLevel());
    query.bindValue(":type", warning.getWarningType());
    query.bindValue(":lat", warning.getLatitude());
    query.bindValue(":lon", warning.getLongitude());
    query.bindValue(":depth", warning.getDepth());
    query.bindValue(":threshold", warning.getThresholdValue());
    query.bindValue(":distance", warning.getDistance());
    query.bindValue(":time", warning.getWarningTime());
    
    if (!query.exec()) {
        lastError = "插入预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool WarningDAO::updateWarning(const Warning &warning)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("UPDATE warnings SET project_id = :projectId, warning_number = :number, "
                  "warning_level = :level, warning_type = :type, latitude = :lat, "
                  "longitude = :lon, depth = :depth, threshold_value = :threshold, "
                  "distance = :distance, warning_time = :time "
                  "WHERE warning_id = :id");
    
    query.bindValue(":id", warning.getWarningId());
    query.bindValue(":projectId", warning.getProjectId());
    query.bindValue(":number", warning.getWarningNumber());
    query.bindValue(":level", warning.getWarningLevel());
    query.bindValue(":type", warning.getWarningType());
    query.bindValue(":lat", warning.getLatitude());
    query.bindValue(":lon", warning.getLongitude());
    query.bindValue(":depth", warning.getDepth());
    query.bindValue(":threshold", warning.getThresholdValue());
    query.bindValue(":distance", warning.getDistance());
    query.bindValue(":time", warning.getWarningTime());
    
    if (!query.exec()) {
        lastError = "更新预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool WarningDAO::deleteWarning(int warningId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM warnings WHERE warning_id = :id");
    query.bindValue(":id", warningId);
    
    if (!query.exec()) {
        lastError = "删除预警失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

int WarningDAO::getWarningCountByProject(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) as count FROM warnings WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询预警数量失败: " + query.lastError().text();
        qWarning() << lastError;
        return 0;
    }
    
    if (query.next()) {
        return query.value("count").toInt();
    }
    
    return 0;
}

int WarningDAO::getTotalWarningCount()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    QSqlQuery query(db);
    if (!query.exec("SELECT COUNT(*) as count FROM warnings")) {
        lastError = "查询预警总数失败: " + query.lastError().text();
        qWarning() << lastError;
        return 0;
    }
    
    if (query.next()) {
        return query.value("count").toInt();
    }
    
    return 0;
}
