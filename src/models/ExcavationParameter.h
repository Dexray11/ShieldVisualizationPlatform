#ifndef EXCAVATIONPARAMETER_H
#define EXCAVATIONPARAMETER_H

#include <QString>
#include <QDateTime>

/**
 * @brief 掘进参数模型类
 * 
 * 存储盾构机掘进参数信息
 */
class ExcavationParameter
{
public:
    ExcavationParameter();
    ~ExcavationParameter() = default;

    // Getters
    int getId() const { return id; }
    int getProjectId() const { return projectId; }
    QDateTime getExcavationTime() const { return excavationTime; }
    QString getStakeMark() const { return stakeMark; }
    double getMileage() const { return mileage; }
    QString getExcavationMode() const { return excavationMode; }
    double getChamberPressure() const { return chamberPressure; }
    double getThrustForce() const { return thrustForce; }
    double getCutterSpeed() const { return cutterSpeed; }
    double getCutterTorque() const { return cutterTorque; }
    double getExcavationSpeed() const { return excavationSpeed; }
    double getGroutingPressure() const { return groutingPressure; }
    double getGroutingVolume() const { return groutingVolume; }
    QString getSegmentNumber() const { return segmentNumber; }
    int getExcavationDuration() const { return excavationDuration; }
    int getIdleDuration() const { return idleDuration; }
    int getFaultDuration() const { return faultDuration; }
    double getExcavationDistance() const { return excavationDistance; }
    QDateTime getCreatedAt() const { return createdAt; }

    // Setters
    void setId(int value) { id = value; }
    void setProjectId(int value) { projectId = value; }
    void setExcavationTime(const QDateTime &value) { excavationTime = value; }
    void setStakeMark(const QString &value) { stakeMark = value; }
    void setMileage(double value) { mileage = value; }
    void setExcavationMode(const QString &value) { excavationMode = value; }
    void setChamberPressure(double value) { chamberPressure = value; }
    void setThrustForce(double value) { thrustForce = value; }
    void setCutterSpeed(double value) { cutterSpeed = value; }
    void setCutterTorque(double value) { cutterTorque = value; }
    void setExcavationSpeed(double value) { excavationSpeed = value; }
    void setGroutingPressure(double value) { groutingPressure = value; }
    void setGroutingVolume(double value) { groutingVolume = value; }
    void setSegmentNumber(const QString &value) { segmentNumber = value; }
    void setExcavationDuration(int value) { excavationDuration = value; }
    void setIdleDuration(int value) { idleDuration = value; }
    void setFaultDuration(int value) { faultDuration = value; }
    void setExcavationDistance(double value) { excavationDistance = value; }
    void setCreatedAt(const QDateTime &value) { createdAt = value; }

private:
    int id;                          // 记录ID
    int projectId;                   // 项目ID
    QDateTime excavationTime;        // 掘进时间
    QString stakeMark;               // 桩号
    double mileage;                  // 里程
    QString excavationMode;          // 掘进模式
    double chamberPressure;          // 土仓土压力
    double thrustForce;              // 千斤顶推力
    double cutterSpeed;              // 刀盘转速
    double cutterTorque;             // 刀盘扭矩
    double excavationSpeed;          // 掘进速度
    double groutingPressure;         // 注浆压力
    double groutingVolume;           // 注浆量
    QString segmentNumber;           // 管片编号
    int excavationDuration;          // 掘进时长（分钟）
    int idleDuration;                // 闲置时长（分钟）
    int faultDuration;               // 故障时长（分钟）
    double excavationDistance;       // 掘进距离（米）
    QDateTime createdAt;             // 创建时间
};

#endif // EXCAVATIONPARAMETER_H
