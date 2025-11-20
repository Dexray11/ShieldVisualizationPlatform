#include "ExcavationParameterDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

ExcavationParameterDAO::ExcavationParameterDAO()
{
}

bool ExcavationParameterDAO::insertExcavationParameter(const ExcavationParameter &param)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("INSERT INTO excavation_parameters "
                  "(project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance) "
                  "VALUES (:projectId, :excavationTime, :stakeMark, :mileage, :excavationMode, "
                  ":chamberPressure, :thrustForce, :cutterSpeed, :cutterTorque, "
                  ":excavationSpeed, :groutingPressure, :groutingVolume, :segmentNumber, "
                  ":excavationDuration, :idleDuration, :faultDuration, :excavationDistance)");
    
    query.bindValue(":projectId", param.getProjectId());
    query.bindValue(":excavationTime", param.getExcavationTime());
    query.bindValue(":stakeMark", param.getStakeMark());
    query.bindValue(":mileage", param.getMileage());
    query.bindValue(":excavationMode", param.getExcavationMode());
    query.bindValue(":chamberPressure", param.getChamberPressure());
    query.bindValue(":thrustForce", param.getThrustForce());
    query.bindValue(":cutterSpeed", param.getCutterSpeed());
    query.bindValue(":cutterTorque", param.getCutterTorque());
    query.bindValue(":excavationSpeed", param.getExcavationSpeed());
    query.bindValue(":groutingPressure", param.getGroutingPressure());
    query.bindValue(":groutingVolume", param.getGroutingVolume());
    query.bindValue(":segmentNumber", param.getSegmentNumber());
    query.bindValue(":excavationDuration", param.getExcavationDuration());
    query.bindValue(":idleDuration", param.getIdleDuration());
    query.bindValue(":faultDuration", param.getFaultDuration());
    query.bindValue(":excavationDistance", param.getExcavationDistance());
    
    if (!query.exec()) {
        lastError = "插入掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool ExcavationParameterDAO::batchInsertExcavationParameters(const QList<ExcavationParameter> &params)
{
    if (params.isEmpty()) {
        return true;
    }
    
    DatabaseManager &dbManager = DatabaseManager::instance();
    
    if (!dbManager.beginTransaction()) {
        lastError = "开始事务失败: " + dbManager.getLastError();
        return false;
    }
    
    for (const ExcavationParameter &param : params) {
        if (!insertExcavationParameter(param)) {
            dbManager.rollbackTransaction();
            return false;
        }
    }
    
    if (!dbManager.commitTransaction()) {
        lastError = "提交事务失败: " + dbManager.getLastError();
        dbManager.rollbackTransaction();
        return false;
    }
    
    return true;
}

QList<ExcavationParameter> ExcavationParameterDAO::getExcavationParametersByProjectId(int projectId)
{
    QList<ExcavationParameter> params;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT id, project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance, created_at "
                  "FROM excavation_parameters WHERE project_id = :projectId "
                  "ORDER BY excavation_time DESC");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return params;
    }
    
    while (query.next()) {
        ExcavationParameter param;
        param.setId(query.value("id").toInt());
        param.setProjectId(query.value("project_id").toInt());
        param.setExcavationTime(query.value("excavation_time").toDateTime());
        param.setStakeMark(query.value("stake_mark").toString());
        param.setMileage(query.value("mileage").toDouble());
        param.setExcavationMode(query.value("excavation_mode").toString());
        param.setChamberPressure(query.value("chamber_pressure").toDouble());
        param.setThrustForce(query.value("thrust_force").toDouble());
        param.setCutterSpeed(query.value("cutter_speed").toDouble());
        param.setCutterTorque(query.value("cutter_torque").toDouble());
        param.setExcavationSpeed(query.value("excavation_speed").toDouble());
        param.setGroutingPressure(query.value("grouting_pressure").toDouble());
        param.setGroutingVolume(query.value("grouting_volume").toDouble());
        param.setSegmentNumber(query.value("segment_number").toString());
        param.setExcavationDuration(query.value("excavation_duration").toInt());
        param.setIdleDuration(query.value("idle_duration").toInt());
        param.setFaultDuration(query.value("fault_duration").toInt());
        param.setExcavationDistance(query.value("excavation_distance").toDouble());
        param.setCreatedAt(query.value("created_at").toDateTime());
        
        params.append(param);
    }
    
    return params;
}

QList<ExcavationParameter> ExcavationParameterDAO::getExcavationParametersByTimeRange(
    int projectId, 
    const QDateTime &startTime, 
    const QDateTime &endTime)
{
    QList<ExcavationParameter> params;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT id, project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance, created_at "
                  "FROM excavation_parameters "
                  "WHERE project_id = :projectId AND excavation_time BETWEEN :startTime AND :endTime "
                  "ORDER BY excavation_time DESC");
    query.bindValue(":projectId", projectId);
    query.bindValue(":startTime", startTime);
    query.bindValue(":endTime", endTime);
    
    if (!query.exec()) {
        lastError = "按时间范围查询掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return params;
    }
    
    while (query.next()) {
        ExcavationParameter param;
        param.setId(query.value("id").toInt());
        param.setProjectId(query.value("project_id").toInt());
        param.setExcavationTime(query.value("excavation_time").toDateTime());
        param.setStakeMark(query.value("stake_mark").toString());
        param.setMileage(query.value("mileage").toDouble());
        param.setExcavationMode(query.value("excavation_mode").toString());
        param.setChamberPressure(query.value("chamber_pressure").toDouble());
        param.setThrustForce(query.value("thrust_force").toDouble());
        param.setCutterSpeed(query.value("cutter_speed").toDouble());
        param.setCutterTorque(query.value("cutter_torque").toDouble());
        param.setExcavationSpeed(query.value("excavation_speed").toDouble());
        param.setGroutingPressure(query.value("grouting_pressure").toDouble());
        param.setGroutingVolume(query.value("grouting_volume").toDouble());
        param.setSegmentNumber(query.value("segment_number").toString());
        param.setExcavationDuration(query.value("excavation_duration").toInt());
        param.setIdleDuration(query.value("idle_duration").toInt());
        param.setFaultDuration(query.value("fault_duration").toInt());
        param.setExcavationDistance(query.value("excavation_distance").toDouble());
        param.setCreatedAt(query.value("created_at").toDateTime());
        
        params.append(param);
    }
    
    return params;
}

QList<ExcavationParameter> ExcavationParameterDAO::getExcavationParametersByMileageRange(
    int projectId,
    double startMileage,
    double endMileage)
{
    QList<ExcavationParameter> params;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT id, project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance, created_at "
                  "FROM excavation_parameters "
                  "WHERE project_id = :projectId AND mileage BETWEEN :startMileage AND :endMileage "
                  "ORDER BY mileage");
    query.bindValue(":projectId", projectId);
    query.bindValue(":startMileage", startMileage);
    query.bindValue(":endMileage", endMileage);
    
    if (!query.exec()) {
        lastError = "按里程范围查询掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return params;
    }
    
    while (query.next()) {
        ExcavationParameter param;
        param.setId(query.value("id").toInt());
        param.setProjectId(query.value("project_id").toInt());
        param.setExcavationTime(query.value("excavation_time").toDateTime());
        param.setStakeMark(query.value("stake_mark").toString());
        param.setMileage(query.value("mileage").toDouble());
        param.setExcavationMode(query.value("excavation_mode").toString());
        param.setChamberPressure(query.value("chamber_pressure").toDouble());
        param.setThrustForce(query.value("thrust_force").toDouble());
        param.setCutterSpeed(query.value("cutter_speed").toDouble());
        param.setCutterTorque(query.value("cutter_torque").toDouble());
        param.setExcavationSpeed(query.value("excavation_speed").toDouble());
        param.setGroutingPressure(query.value("grouting_pressure").toDouble());
        param.setGroutingVolume(query.value("grouting_volume").toDouble());
        param.setSegmentNumber(query.value("segment_number").toString());
        param.setExcavationDuration(query.value("excavation_duration").toInt());
        param.setIdleDuration(query.value("idle_duration").toInt());
        param.setFaultDuration(query.value("fault_duration").toInt());
        param.setExcavationDistance(query.value("excavation_distance").toDouble());
        param.setCreatedAt(query.value("created_at").toDateTime());
        
        params.append(param);
    }
    
    return params;
}

bool ExcavationParameterDAO::getLatestExcavationParameter(int projectId, ExcavationParameter &param)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT id, project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance, created_at "
                  "FROM excavation_parameters "
                  "WHERE project_id = :projectId "
                  "ORDER BY excavation_time DESC LIMIT 1");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询最新掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    if (query.next()) {
        param.setId(query.value("id").toInt());
        param.setProjectId(query.value("project_id").toInt());
        param.setExcavationTime(query.value("excavation_time").toDateTime());
        param.setStakeMark(query.value("stake_mark").toString());
        param.setMileage(query.value("mileage").toDouble());
        param.setExcavationMode(query.value("excavation_mode").toString());
        param.setChamberPressure(query.value("chamber_pressure").toDouble());
        param.setThrustForce(query.value("thrust_force").toDouble());
        param.setCutterSpeed(query.value("cutter_speed").toDouble());
        param.setCutterTorque(query.value("cutter_torque").toDouble());
        param.setExcavationSpeed(query.value("excavation_speed").toDouble());
        param.setGroutingPressure(query.value("grouting_pressure").toDouble());
        param.setGroutingVolume(query.value("grouting_volume").toDouble());
        param.setSegmentNumber(query.value("segment_number").toString());
        param.setExcavationDuration(query.value("excavation_duration").toInt());
        param.setIdleDuration(query.value("idle_duration").toInt());
        param.setFaultDuration(query.value("fault_duration").toInt());
        param.setExcavationDistance(query.value("excavation_distance").toDouble());
        param.setCreatedAt(query.value("created_at").toDateTime());
        return true;
    }
    
    lastError = "未找到掘进参数记录";
    return false;
}

QList<ExcavationParameter> ExcavationParameterDAO::getExcavationParametersByPage(
    int projectId, 
    int page, 
    int pageSize)
{
    QList<ExcavationParameter> params;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    int offset = (page - 1) * pageSize;
    
    query.prepare("SELECT id, project_id, excavation_time, stake_mark, mileage, excavation_mode, "
                  "chamber_pressure, thrust_force, cutter_speed, cutter_torque, "
                  "excavation_speed, grouting_pressure, grouting_volume, segment_number, "
                  "excavation_duration, idle_duration, fault_duration, excavation_distance, created_at "
                  "FROM excavation_parameters "
                  "WHERE project_id = :projectId "
                  "ORDER BY excavation_time DESC "
                  "LIMIT :limit OFFSET :offset");
    query.bindValue(":projectId", projectId);
    query.bindValue(":limit", pageSize);
    query.bindValue(":offset", offset);
    
    if (!query.exec()) {
        lastError = "分页查询掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return params;
    }
    
    while (query.next()) {
        ExcavationParameter param;
        param.setId(query.value("id").toInt());
        param.setProjectId(query.value("project_id").toInt());
        param.setExcavationTime(query.value("excavation_time").toDateTime());
        param.setStakeMark(query.value("stake_mark").toString());
        param.setMileage(query.value("mileage").toDouble());
        param.setExcavationMode(query.value("excavation_mode").toString());
        param.setChamberPressure(query.value("chamber_pressure").toDouble());
        param.setThrustForce(query.value("thrust_force").toDouble());
        param.setCutterSpeed(query.value("cutter_speed").toDouble());
        param.setCutterTorque(query.value("cutter_torque").toDouble());
        param.setExcavationSpeed(query.value("excavation_speed").toDouble());
        param.setGroutingPressure(query.value("grouting_pressure").toDouble());
        param.setGroutingVolume(query.value("grouting_volume").toDouble());
        param.setSegmentNumber(query.value("segment_number").toString());
        param.setExcavationDuration(query.value("excavation_duration").toInt());
        param.setIdleDuration(query.value("idle_duration").toInt());
        param.setFaultDuration(query.value("fault_duration").toInt());
        param.setExcavationDistance(query.value("excavation_distance").toDouble());
        param.setCreatedAt(query.value("created_at").toDateTime());
        
        params.append(param);
    }
    
    return params;
}

int ExcavationParameterDAO::getExcavationParametersCount(int projectId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT COUNT(*) FROM excavation_parameters WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询掘进参数记录总数失败: " + query.lastError().text();
        qWarning() << lastError;
        return 0;
    }
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

bool ExcavationParameterDAO::deleteExcavationParametersByProjectId(int projectId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("DELETE FROM excavation_parameters WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "删除掘进参数记录失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}
