#include "ProspectingDataDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

ProspectingDataDAO::ProspectingDataDAO()
{
}

int ProspectingDataDAO::insert(const ProspectingData &data)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "INSERT INTO prospecting_data "
        "(project_id, excavation_time, stake_mark, mileage, "
        "cutter_force, cutter_penetration_resistance, face_friction_torque, "
        "p_wave_velocity, s_wave_velocity, wave_reflection_coeff, "
        "apparent_resistivity, stress_gradient, water_probability, "
        "rock_properties, rock_danger_level, "
        "youngs_modulus, poisson_ratio, wave_velocity_ratio, "
        "rock_type, distribution_pattern) "
        "VALUES "
        "(:projectId, :excavationTime, :stakeMark, :mileage, "
        ":cutterForce, :cutterPenetrationResistance, :faceFrictionTorque, "
        ":pWaveVelocity, :sWaveVelocity, :waveReflectionCoeff, "
        ":apparentResistivity, :stressGradient, :waterProbability, "
        ":rockProperties, :rockDangerLevel, "
        ":youngsModulus, :poissonRatio, :waveVelocityRatio, "
        ":rockType, :distributionPattern)"
    );
    
    query.bindValue(":projectId", data.getProjectId());
    query.bindValue(":excavationTime", data.getExcavationTime());
    query.bindValue(":stakeMark", data.getStakeMark());
    query.bindValue(":mileage", data.getMileage());
    query.bindValue(":cutterForce", data.getCutterForce());
    query.bindValue(":cutterPenetrationResistance", data.getCutterPenetrationResistance());
    query.bindValue(":faceFrictionTorque", data.getFaceFrictionTorque());
    query.bindValue(":pWaveVelocity", data.getPWaveVelocity());
    query.bindValue(":sWaveVelocity", data.getSWaveVelocity());
    query.bindValue(":waveReflectionCoeff", data.getWaveReflectionCoeff());
    query.bindValue(":apparentResistivity", data.getApparentResistivity());
    query.bindValue(":stressGradient", data.getStressGradient());
    query.bindValue(":waterProbability", data.getWaterProbability());
    query.bindValue(":rockProperties", data.getRockProperties());
    query.bindValue(":rockDangerLevel", data.getRockDangerLevel());
    query.bindValue(":youngsModulus", data.getYoungsModulus());
    query.bindValue(":poissonRatio", data.getPoissonRatio());
    query.bindValue(":waveVelocityRatio", data.getWaveVelocityRatio());
    query.bindValue(":rockType", data.getRockType());
    query.bindValue(":distributionPattern", data.getDistributionPattern());
    
    if (!query.exec()) {
        lastError = "插入补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool ProspectingDataDAO::insertBatch(const QVector<ProspectingData> &dataList)
{
    if (dataList.isEmpty()) {
        return true;
    }
    
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    if (!db.transaction()) {
        lastError = "开始事务失败: " + db.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO prospecting_data "
        "(project_id, excavation_time, stake_mark, mileage, "
        "cutter_force, cutter_penetration_resistance, face_friction_torque, "
        "p_wave_velocity, s_wave_velocity, wave_reflection_coeff, "
        "apparent_resistivity, stress_gradient, water_probability, "
        "rock_properties, rock_danger_level, "
        "youngs_modulus, poisson_ratio, wave_velocity_ratio, "
        "rock_type, distribution_pattern) "
        "VALUES "
        "(:projectId, :excavationTime, :stakeMark, :mileage, "
        ":cutterForce, :cutterPenetrationResistance, :faceFrictionTorque, "
        ":pWaveVelocity, :sWaveVelocity, :waveReflectionCoeff, "
        ":apparentResistivity, :stressGradient, :waterProbability, "
        ":rockProperties, :rockDangerLevel, "
        ":youngsModulus, :poissonRatio, :waveVelocityRatio, "
        ":rockType, :distributionPattern)"
    );
    
    for (const ProspectingData &data : dataList) {
        query.bindValue(":projectId", data.getProjectId());
        query.bindValue(":excavationTime", data.getExcavationTime());
        query.bindValue(":stakeMark", data.getStakeMark());
        query.bindValue(":mileage", data.getMileage());
        query.bindValue(":cutterForce", data.getCutterForce());
        query.bindValue(":cutterPenetrationResistance", data.getCutterPenetrationResistance());
        query.bindValue(":faceFrictionTorque", data.getFaceFrictionTorque());
        query.bindValue(":pWaveVelocity", data.getPWaveVelocity());
        query.bindValue(":sWaveVelocity", data.getSWaveVelocity());
        query.bindValue(":waveReflectionCoeff", data.getWaveReflectionCoeff());
        query.bindValue(":apparentResistivity", data.getApparentResistivity());
        query.bindValue(":stressGradient", data.getStressGradient());
        query.bindValue(":waterProbability", data.getWaterProbability());
        query.bindValue(":rockProperties", data.getRockProperties());
        query.bindValue(":rockDangerLevel", data.getRockDangerLevel());
        query.bindValue(":youngsModulus", data.getYoungsModulus());
        query.bindValue(":poissonRatio", data.getPoissonRatio());
        query.bindValue(":waveVelocityRatio", data.getWaveVelocityRatio());
        query.bindValue(":rockType", data.getRockType());
        query.bindValue(":distributionPattern", data.getDistributionPattern());
        
        if (!query.exec()) {
            lastError = "批量插入补勘数据失败: " + query.lastError().text();
            qCritical() << lastError;
            db.rollback();
            return false;
        }
    }
    
    if (!db.commit()) {
        lastError = "提交事务失败: " + db.lastError().text();
        qCritical() << lastError;
        db.rollback();
        return false;
    }
    
    return true;
}

bool ProspectingDataDAO::update(const ProspectingData &data)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "UPDATE prospecting_data SET "
        "project_id = :projectId, "
        "excavation_time = :excavationTime, "
        "stake_mark = :stakeMark, "
        "mileage = :mileage, "
        "cutter_force = :cutterForce, "
        "cutter_penetration_resistance = :cutterPenetrationResistance, "
        "face_friction_torque = :faceFrictionTorque, "
        "p_wave_velocity = :pWaveVelocity, "
        "s_wave_velocity = :sWaveVelocity, "
        "wave_reflection_coeff = :waveReflectionCoeff, "
        "apparent_resistivity = :apparentResistivity, "
        "stress_gradient = :stressGradient, "
        "water_probability = :waterProbability, "
        "rock_properties = :rockProperties, "
        "rock_danger_level = :rockDangerLevel, "
        "youngs_modulus = :youngsModulus, "
        "poisson_ratio = :poissonRatio, "
        "wave_velocity_ratio = :waveVelocityRatio, "
        "rock_type = :rockType, "
        "distribution_pattern = :distributionPattern "
        "WHERE prospecting_id = :id"
    );
    
    query.bindValue(":id", data.getId());
    query.bindValue(":projectId", data.getProjectId());
    query.bindValue(":excavationTime", data.getExcavationTime());
    query.bindValue(":stakeMark", data.getStakeMark());
    query.bindValue(":mileage", data.getMileage());
    query.bindValue(":cutterForce", data.getCutterForce());
    query.bindValue(":cutterPenetrationResistance", data.getCutterPenetrationResistance());
    query.bindValue(":faceFrictionTorque", data.getFaceFrictionTorque());
    query.bindValue(":pWaveVelocity", data.getPWaveVelocity());
    query.bindValue(":sWaveVelocity", data.getSWaveVelocity());
    query.bindValue(":waveReflectionCoeff", data.getWaveReflectionCoeff());
    query.bindValue(":apparentResistivity", data.getApparentResistivity());
    query.bindValue(":stressGradient", data.getStressGradient());
    query.bindValue(":waterProbability", data.getWaterProbability());
    query.bindValue(":rockProperties", data.getRockProperties());
    query.bindValue(":rockDangerLevel", data.getRockDangerLevel());
    query.bindValue(":youngsModulus", data.getYoungsModulus());
    query.bindValue(":poissonRatio", data.getPoissonRatio());
    query.bindValue(":waveVelocityRatio", data.getWaveVelocityRatio());
    query.bindValue(":rockType", data.getRockType());
    query.bindValue(":distributionPattern", data.getDistributionPattern());
    
    if (!query.exec()) {
        lastError = "更新补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool ProspectingDataDAO::deleteById(int id)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("DELETE FROM prospecting_data WHERE prospecting_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        lastError = "删除补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool ProspectingDataDAO::deleteByProjectId(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("DELETE FROM prospecting_data WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "删除项目补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

ProspectingData ProspectingDataDAO::selectById(int id)
{
    ProspectingData data;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM prospecting_data WHERE prospecting_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        lastError = "查询补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return data;
    }
    
    if (query.next()) {
        data.setId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
    }
    
    return data;
}

QVector<ProspectingData> ProspectingDataDAO::selectByProjectId(int projectId)
{
    QVector<ProspectingData> dataList;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM prospecting_data WHERE project_id = :projectId ORDER BY excavation_time DESC");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "查询项目补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return dataList;
    }
    
    while (query.next()) {
        ProspectingData data;
        data.setId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
        
        dataList.append(data);
    }
    
    return dataList;
}

QVector<ProspectingData> ProspectingDataDAO::selectByProjectIdWithPagination(int projectId, int offset, int limit)
{
    QVector<ProspectingData> dataList;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM prospecting_data WHERE project_id = :projectId "
                  "ORDER BY excavation_time DESC LIMIT :limit OFFSET :offset");
    query.bindValue(":projectId", projectId);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    
    if (!query.exec()) {
        lastError = "分页查询补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return dataList;
    }
    
    while (query.next()) {
        ProspectingData data;
        data.setId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
        
        dataList.append(data);
    }
    
    return dataList;
}

int ProspectingDataDAO::countByProjectId(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT COUNT(*) FROM prospecting_data WHERE project_id = :projectId");
    query.bindValue(":projectId", projectId);
    
    if (!query.exec()) {
        lastError = "统计补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return 0;
    }
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

QVector<ProspectingData> ProspectingDataDAO::selectByMileageRange(int projectId, double startMileage, double endMileage)
{
    QVector<ProspectingData> dataList;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM prospecting_data "
                  "WHERE project_id = :projectId AND mileage >= :startMileage AND mileage <= :endMileage "
                  "ORDER BY mileage ASC");
    query.bindValue(":projectId", projectId);
    query.bindValue(":startMileage", startMileage);
    query.bindValue(":endMileage", endMileage);
    
    if (!query.exec()) {
        lastError = "按里程范围查询补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return dataList;
    }
    
    while (query.next()) {
        ProspectingData data;
        data.setId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
        
        dataList.append(data);
    }
    
    return dataList;
}

QVector<ProspectingData> ProspectingDataDAO::selectLatestByProjectId(int projectId, int count)
{
    QVector<ProspectingData> dataList;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM prospecting_data WHERE project_id = :projectId "
                  "ORDER BY excavation_time DESC LIMIT :count");
    query.bindValue(":projectId", projectId);
    query.bindValue(":count", count);
    
    if (!query.exec()) {
        lastError = "查询最新补勘数据失败: " + query.lastError().text();
        qCritical() << lastError;
        return dataList;
    }
    
    while (query.next()) {
        ProspectingData data;
        data.setId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
        
        dataList.append(data);
    }
    
    return dataList;
}

QList<ProspectingData> ProspectingDataDAO::getProspectingDataByProjectId(int projectId)
{
    QVector<ProspectingData> vector = selectByProjectId(projectId);
    QList<ProspectingData> list;
    for (const auto& data : vector) {
        list.append(data);
    }
    return list;
}

QList<ProspectingData> ProspectingDataDAO::getAllProspectingData()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT prospecting_id, project_id, excavation_time, stake_mark, mileage, "
        "cutter_force, cutter_penetration_resistance, face_friction_torque, "
        "p_wave_velocity, s_wave_velocity, wave_reflection_coeff, "
        "apparent_resistivity, stress_gradient, water_probability, "
        "rock_properties, rock_danger_level, "
        "youngs_modulus, poisson_ratio, wave_velocity_ratio, "
        "rock_type, distribution_pattern, created_at "
        "FROM prospecting_data "
        "ORDER BY excavation_time DESC"
    );
    
    QList<ProspectingData> dataList;
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qWarning() << "获取所有补勘数据失败:" << lastError;
        return dataList;
    }
    
    while (query.next()) {
        ProspectingData data;
        data.setProspectingId(query.value("prospecting_id").toInt());
        data.setProjectId(query.value("project_id").toInt());
        data.setExcavationTime(query.value("excavation_time").toDateTime());
        data.setStakeMark(query.value("stake_mark").toString());
        data.setMileage(query.value("mileage").toDouble());
        data.setCutterForce(query.value("cutter_force").toDouble());
        data.setCutterPenetrationResistance(query.value("cutter_penetration_resistance").toDouble());
        data.setFaceFrictionTorque(query.value("face_friction_torque").toDouble());
        data.setPWaveVelocity(query.value("p_wave_velocity").toDouble());
        data.setSWaveVelocity(query.value("s_wave_velocity").toDouble());
        data.setWaveReflectionCoeff(query.value("wave_reflection_coeff").toDouble());
        data.setApparentResistivity(query.value("apparent_resistivity").toDouble());
        data.setStressGradient(query.value("stress_gradient").toDouble());
        data.setWaterProbability(query.value("water_probability").toDouble());
        data.setRockProperties(query.value("rock_properties").toString());
        data.setRockDangerLevel(query.value("rock_danger_level").toString());
        data.setYoungsModulus(query.value("youngs_modulus").toDouble());
        data.setPoissonRatio(query.value("poisson_ratio").toDouble());
        data.setWaveVelocityRatio(query.value("wave_velocity_ratio").toDouble());
        data.setRockType(query.value("rock_type").toString());
        data.setDistributionPattern(query.value("distribution_pattern").toString());
        data.setCreatedAt(query.value("created_at").toDateTime());
        
        dataList.append(data);
    }
    
    return dataList;
}
