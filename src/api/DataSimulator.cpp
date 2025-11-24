#include "DataSimulator.h"
#include "../database/DatabaseManager.h"
#include "../database/ExcavationParameterDAO.h"
#include "../database/ProspectingDataDAO.h"
#include "../models/ExcavationParameter.h"
#include "../models/ProspectingData.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

DataSimulator::DataSimulator(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_dbManager(dbManager)
    , m_projectId(0)
    , m_currentMileage(3086.0)
    , m_sequenceNumber(3000)
    , m_currentRockType("中风化安山岩")
    , m_rockHardness(0.6)
    , m_intervalMs(5000)
    , m_isRunning(false)
{
    connect(m_timer, &QTimer::timeout, this, &DataSimulator::generateData);
}

DataSimulator::~DataSimulator()
{
    stop();
}

void DataSimulator::start(int projectId, int intervalMs)
{
    if (m_isRunning) {
        qWarning() << "数据模拟器已在运行";
        return;
    }

    m_projectId = projectId;
    m_intervalMs = intervalMs;
    m_isRunning = true;
    
    m_timer->start(m_intervalMs);
    qInfo() << "数据模拟器已启动，项目ID:" << m_projectId 
            << "间隔:" << m_intervalMs << "ms";
    
    emit statusChanged(true);
}

void DataSimulator::stop()
{
    if (!m_isRunning) {
        return;
    }

    m_timer->stop();
    m_isRunning = false;
    qInfo() << "数据模拟器已停止";
    
    emit statusChanged(false);
}

bool DataSimulator::isRunning() const
{
    return m_isRunning;
}

void DataSimulator::setInterval(int ms)
{
    m_intervalMs = ms;
    if (m_isRunning) {
        m_timer->setInterval(m_intervalMs);
    }
}

void DataSimulator::setProjectId(int projectId)
{
    m_projectId = projectId;
}

void DataSimulator::setCurrentMileage(double mileage)
{
    m_currentMileage = mileage;
}

void DataSimulator::setGeologicalCondition(const QString& rockType, double hardness)
{
    m_currentRockType = rockType;
    m_rockHardness = qBound(0.0, hardness, 1.0);
}

void DataSimulator::generateData()
{
    if (m_projectId <= 0) {
        qWarning() << "无效的项目ID";
        return;
    }

    // 生成掘进参数数据
    QJsonObject excavationData = generateExcavationData();
    
    // 生成补勘数据
    QJsonObject prospectingData = generateProspectingData();
    
    // 发送信号
    emit excavationDataGenerated(m_projectId, excavationData);
    emit prospectingDataGenerated(m_projectId, prospectingData);
    
    // 保存到数据库
    ExcavationParameter param;
    param.setProjectId(m_projectId);
    param.setExcavationTime(QDateTime::fromString(
        excavationData["excavation_time"].toString(), Qt::ISODate));
    param.setStakeMark(excavationData["stake_mark"].toString());
    param.setMileage(excavationData["mileage"].toDouble());
    param.setExcavationMode(excavationData["excavation_mode"].toString());
    param.setChamberPressure(excavationData["chamber_pressure"].toDouble());
    param.setThrustForce(excavationData["thrust_force"].toDouble());
    param.setCutterSpeed(excavationData["cutter_speed"].toDouble());
    param.setCutterTorque(excavationData["cutter_torque"].toDouble());
    param.setExcavationSpeed(excavationData["excavation_speed"].toDouble());
    param.setGroutingPressure(excavationData["grouting_pressure"].toDouble());
    param.setGroutingVolume(excavationData["grouting_volume"].toDouble());
    param.setSegmentNumber(excavationData["segment_number"].toString());
    param.setExcavationDuration(excavationData["excavation_duration"].toInt());
    param.setIdleDuration(excavationData["idle_duration"].toInt());
    param.setFaultDuration(excavationData["fault_duration"].toInt());
    param.setExcavationDistance(excavationData["excavation_distance"].toDouble());
    
    ExcavationParameterDAO excavationDao;
    if (!excavationDao.insertExcavationParameter(param)) {
        qWarning() << "保存掘进参数失败:" << excavationDao.getLastError();
    }
    
    ProspectingData prospecting;
    prospecting.setProjectId(m_projectId);
    prospecting.setExcavationTime(QDateTime::fromString(
        prospectingData["excavation_time"].toString(), Qt::ISODate));
    prospecting.setStakeMark(prospectingData["stake_mark"].toString());
    prospecting.setMileage(prospectingData["mileage"].toDouble());
    prospecting.setCutterForce(prospectingData["cutter_force"].toDouble());
    prospecting.setCutterPenetrationResistance(prospectingData["cutter_penetration_resistance"].toDouble());
    prospecting.setFaceFrictionTorque(prospectingData["face_friction_torque"].toDouble());
    prospecting.setPWaveVelocity(prospectingData["p_wave_velocity"].toDouble());
    prospecting.setSWaveVelocity(prospectingData["s_wave_velocity"].toDouble());
    prospecting.setWaveReflectionCoeff(prospectingData["wave_reflection_coeff"].toDouble());
    prospecting.setApparentResistivity(prospectingData["apparent_resistivity"].toDouble());
    prospecting.setStressGradient(prospectingData["stress_gradient"].toDouble());
    prospecting.setWaterProbability(prospectingData["water_probability"].toDouble());
    prospecting.setRockProperties(prospectingData["rock_properties"].toString());
    prospecting.setRockDangerLevel(prospectingData["rock_danger_level"].toString());
    prospecting.setYoungsModulus(prospectingData["youngs_modulus"].toDouble());
    prospecting.setPoissonRatio(prospectingData["poisson_ratio"].toDouble());
    prospecting.setWaveVelocityRatio(prospectingData["wave_velocity_ratio"].toDouble());
    prospecting.setRockType(prospectingData["rock_type"].toString());
    prospecting.setDistributionPattern(prospectingData["distribution_pattern"].toString());
    
    ProspectingDataDAO prospectingDao;
    int recordId = prospectingDao.insert(prospecting);
    if (recordId <= 0) {
        qWarning() << "保存补勘数据失败:" << prospectingDao.getLastError();
    }
    
    // 更新里程
    m_currentMileage += 0.05; // 每次前进5cm
    m_sequenceNumber++;
    
    qDebug() << "已生成模拟数据:" << param.getStakeMark();
}

QJsonObject DataSimulator::generateExcavationData()
{
    QJsonObject data;
    
    // 计算桩号
    int km = static_cast<int>(m_currentMileage / 1000.0);
    double m = m_currentMileage - (km * 1000.0);
    QString stakeMark = QString("K%1+%2").arg(km).arg(m, 0, 'f', 2);
    
    // 基础参数
    data["excavation_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    data["stake_mark"] = stakeMark;
    data["mileage"] = m_currentMileage;
    data["excavation_mode"] = "土压平衡";
    
    // 根据岩层类型计算参数
    double thrustForce = calculateThrustForce(m_currentRockType);
    double cutterTorque = calculateCutterTorque(m_currentRockType);
    double excavationSpeed = calculateExcavationSpeed(m_currentRockType);
    
    // 添加随机波动
    data["chamber_pressure"] = addNoise(0.25 + m_rockHardness * 0.15);
    data["thrust_force"] = addNoise(thrustForce);
    data["cutter_speed"] = addNoise(1.5);
    data["cutter_torque"] = addNoise(cutterTorque);
    data["excavation_speed"] = addNoise(excavationSpeed);
    data["grouting_pressure"] = addNoise(2.5);
    data["grouting_volume"] = addNoise(6.0);
    data["segment_number"] = QString("SG-%1").arg(m_sequenceNumber, 4, 10, QChar('0'));
    
    // 时间统计（根据掘进速度计算）
    int duration = static_cast<int>((0.05 / (excavationSpeed / 60.0)) + 0.5); // 转换为分钟
    data["excavation_duration"] = duration;
    data["idle_duration"] = QRandomGenerator::global()->bounded(0, 2); // 0-1分钟
    data["fault_duration"] = 0;
    data["excavation_distance"] = 0.05;
    
    return data;
}

QJsonObject DataSimulator::generateProspectingData()
{
    QJsonObject data;
    
    // 计算桩号
    int km = static_cast<int>(m_currentMileage / 1000.0);
    double m = m_currentMileage - (km * 1000.0);
    QString stakeMark = QString("K%1+%2").arg(km).arg(m, 0, 'f', 2);
    
    data["excavation_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    data["stake_mark"] = stakeMark;
    data["mileage"] = m_currentMileage;
    
    // 刀盘受力参数
    double baseCutterForce = 3000.0 + m_rockHardness * 1500.0;
    data["cutter_force"] = addNoise(baseCutterForce);
    data["cutter_penetration_resistance"] = addNoise(400.0 + m_rockHardness * 200.0);
    data["face_friction_torque"] = addNoise(1200.0 + m_rockHardness * 400.0);
    
    // 波速参数
    double pWaveVelocity = calculatePWaveVelocity(m_currentRockType);
    double sWaveVelocity = calculateSWaveVelocity(m_currentRockType);
    
    data["p_wave_velocity"] = addNoise(pWaveVelocity, 3.0);
    data["s_wave_velocity"] = addNoise(sWaveVelocity, 3.0);
    data["wave_reflection_coeff"] = addNoise(0.35 + m_rockHardness * 0.15, 5.0);
    
    // 地质参数
    data["apparent_resistivity"] = addNoise(85.0 + m_rockHardness * 50.0);
    data["stress_gradient"] = addNoise(0.025 + m_rockHardness * 0.01);
    data["water_probability"] = addNoise(15.0 - m_rockHardness * 10.0);
    
    // 岩层性质
    data["rock_properties"] = QString("%1，节理裂隙%2发育")
        .arg(m_currentRockType)
        .arg(m_rockHardness > 0.7 ? "较" : "");
    
    // 危险等级
    QString dangerLevel;
    if (m_rockHardness < 0.3) dangerLevel = "D";
    else if (m_rockHardness < 0.6) dangerLevel = "C";
    else if (m_rockHardness < 0.8) dangerLevel = "B";
    else dangerLevel = "A";
    data["rock_danger_level"] = dangerLevel;
    
    // 力学参数
    data["youngs_modulus"] = addNoise(30.0 + m_rockHardness * 20.0);
    data["poisson_ratio"] = addNoise(0.22 + m_rockHardness * 0.05, 2.0);
    data["wave_velocity_ratio"] = pWaveVelocity > 0 ? (sWaveVelocity / pWaveVelocity) : 0.57;
    
    data["rock_type"] = m_currentRockType;
    data["distribution_pattern"] = "层状分布";
    
    return data;
}

double DataSimulator::calculateThrustForce(const QString& rockType)
{
    double baseForce = 2500.0;
    
    if (rockType.contains("素填土") || rockType.contains("粉质")) {
        baseForce = 1200.0;
    } else if (rockType.contains("强风化")) {
        baseForce = 1800.0;
    } else if (rockType.contains("中风化")) {
        baseForce = 2500.0;
    } else if (rockType.contains("微风化")) {
        baseForce = 3200.0;
    } else if (rockType.contains("未风化")) {
        baseForce = 3800.0;
    }
    
    return baseForce;
}

double DataSimulator::calculateCutterTorque(const QString& rockType)
{
    double baseTorque = 2000.0;
    
    if (rockType.contains("素填土") || rockType.contains("粉质")) {
        baseTorque = 800.0;
    } else if (rockType.contains("强风化")) {
        baseTorque = 1600.0;
    } else if (rockType.contains("中风化")) {
        baseTorque = 2000.0;
    } else if (rockType.contains("微风化")) {
        baseTorque = 2600.0;
    } else if (rockType.contains("未风化")) {
        baseTorque = 3200.0;
    }
    
    return baseTorque;
}

double DataSimulator::calculateExcavationSpeed(const QString& rockType)
{
    double baseSpeed = 50.0; // mm/min
    
    if (rockType.contains("素填土") || rockType.contains("粉质")) {
        baseSpeed = 65.0;
    } else if (rockType.contains("强风化")) {
        baseSpeed = 45.0;
    } else if (rockType.contains("中风化")) {
        baseSpeed = 38.0;
    } else if (rockType.contains("微风化")) {
        baseSpeed = 25.0;
    } else if (rockType.contains("未风化")) {
        baseSpeed = 18.0;
    }
    
    return baseSpeed;
}

double DataSimulator::calculatePWaveVelocity(const QString& rockType)
{
    double velocity = 4200.0; // m/s
    
    if (rockType.contains("素填土") || rockType.contains("粉质")) {
        velocity = 1800.0;
    } else if (rockType.contains("强风化")) {
        velocity = 3200.0;
    } else if (rockType.contains("中风化")) {
        velocity = 4200.0;
    } else if (rockType.contains("微风化")) {
        velocity = 5500.0;
    } else if (rockType.contains("未风化")) {
        velocity = 6200.0;
    }
    
    return velocity;
}

double DataSimulator::calculateSWaveVelocity(const QString& rockType)
{
    double velocity = 2400.0; // m/s
    
    if (rockType.contains("素填土") || rockType.contains("粉质")) {
        velocity = 900.0;
    } else if (rockType.contains("强风化")) {
        velocity = 1800.0;
    } else if (rockType.contains("中风化")) {
        velocity = 2400.0;
    } else if (rockType.contains("微风化")) {
        velocity = 3100.0;
    } else if (rockType.contains("未风化")) {
        velocity = 3500.0;
    }
    
    return velocity;
}

double DataSimulator::addNoise(double value, double percentage)
{
    double range = value * percentage / 100.0;
    double noise = QRandomGenerator::global()->bounded(range * 2.0) - range;
    return value + noise;
}
