#ifndef PROSPECTINGDATA_H
#define PROSPECTINGDATA_H

#include <QString>
#include <QDateTime>

/**
 * @brief 补勘数据模型类
 * 
 * 存储盾构机前方地质补勘信息
 * 包括刀盘受力、地质探测和岩层参数
 */
class ProspectingData
{
public:
    ProspectingData();
    ~ProspectingData() = default;

    // Getters
    int getId() const { return id; }
    int getProspectingId() const { return id; }  // 别名方法,与getId相同
    int getProjectId() const { return projectId; }
    QDateTime getExcavationTime() const { return excavationTime; }
    QString getStakeMark() const { return stakeMark; }
    double getMileage() const { return mileage; }
    
    // 刀盘受力相关
    double getCutterForce() const { return cutterForce; }
    double getCutterPenetrationResistance() const { return cutterPenetrationResistance; }
    double getFaceFrictionTorque() const { return faceFrictionTorque; }
    
    // 地质探测相关
    double getPWaveVelocity() const { return pWaveVelocity; }
    double getSWaveVelocity() const { return sWaveVelocity; }
    double getWaveReflectionCoeff() const { return waveReflectionCoeff; }
    double getApparentResistivity() const { return apparentResistivity; }
    double getStressGradient() const { return stressGradient; }
    double getWaterProbability() const { return waterProbability; }
    
    // 掌子面参数
    QString getRockProperties() const { return rockProperties; }
    QString getRockDangerLevel() const { return rockDangerLevel; }
    
    // 岩层参数
    double getYoungsModulus() const { return youngsModulus; }
    double getPoissonRatio() const { return poissonRatio; }
    double getWaveVelocityRatio() const { return waveVelocityRatio; }
    QString getRockType() const { return rockType; }
    QString getDistributionPattern() const { return distributionPattern; }
    
    QDateTime getCreatedAt() const { return createdAt; }

    // Setters
    void setId(int value) { id = value; }
    void setProspectingId(int value) { id = value; }  // 别名方法,与setId相同
    void setProjectId(int value) { projectId = value; }
    void setExcavationTime(const QDateTime &value) { excavationTime = value; }
    void setStakeMark(const QString &value) { stakeMark = value; }
    void setMileage(double value) { mileage = value; }
    
    void setCutterForce(double value) { cutterForce = value; }
    void setCutterPenetrationResistance(double value) { cutterPenetrationResistance = value; }
    void setFaceFrictionTorque(double value) { faceFrictionTorque = value; }
    
    void setPWaveVelocity(double value) { pWaveVelocity = value; }
    void setSWaveVelocity(double value) { sWaveVelocity = value; }
    void setWaveReflectionCoeff(double value) { waveReflectionCoeff = value; }
    void setApparentResistivity(double value) { apparentResistivity = value; }
    void setStressGradient(double value) { stressGradient = value; }
    void setWaterProbability(double value) { waterProbability = value; }
    
    void setRockProperties(const QString &value) { rockProperties = value; }
    void setRockDangerLevel(const QString &value) { rockDangerLevel = value; }
    
    void setYoungsModulus(double value) { youngsModulus = value; }
    void setPoissonRatio(double value) { poissonRatio = value; }
    void setWaveVelocityRatio(double value) { waveVelocityRatio = value; }
    void setRockType(const QString &value) { rockType = value; }
    void setDistributionPattern(const QString &value) { distributionPattern = value; }
    
    void setCreatedAt(const QDateTime &value) { createdAt = value; }

private:
    int id;                                     // 记录ID
    int projectId;                              // 项目ID
    QDateTime excavationTime;                   // 掘进时间
    QString stakeMark;                          // 桩号
    double mileage;                             // 里程
    
    // 刀盘受力情况
    double cutterForce;                         // 刀盘受力 (kN)
    double cutterPenetrationResistance;         // 刀具贯入阻力 (kN)
    double faceFrictionTorque;                  // 刀盘正面摩擦力矩 (kN·m)
    
    // 地质探测数据
    double pWaveVelocity;                       // 纵波波速 (m/s)
    double sWaveVelocity;                       // 横波波速 (m/s)
    double waveReflectionCoeff;                 // 波幅反射系数
    double apparentResistivity;                 // 视电阻率 (Ω·m)
    double stressGradient;                      // 应力梯度 (MPa/m)
    double waterProbability;                    // 前方5m含水概率 (%)
    
    // 掌子面参数
    QString rockProperties;                     // 岩石物性参数描述
    QString rockDangerLevel;                    // 围岩危险等级 (A/B/C/D)
    
    // 岩层参数
    double youngsModulus;                       // 杨氏模量 (GPa)
    double poissonRatio;                        // 泊松比
    double waveVelocityRatio;                   // 横纵波速比
    QString rockType;                           // 岩层类型
    QString distributionPattern;                // 分布规律
    
    QDateTime createdAt;                        // 创建时间
};

#endif // PROSPECTINGDATA_H
