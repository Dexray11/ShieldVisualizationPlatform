#ifndef WARNING_H
#define WARNING_H

#include <QString>
#include <QDateTime>

/**
 * @brief 预警模型类
 * 
 * 表示一个预警信息的完整数据
 */
class Warning
{
public:
    Warning();
    Warning(int id, int projectId, int number, const QString &level,
            const QString &type, double lat, double lon, double depth,
            int threshold, double distance, const QDateTime &time);
    
    // Getters
    int getWarningId() const { return warningId; }
    int getProjectId() const { return projectId; }
    int getWarningNumber() const { return warningNumber; }
    QString getWarningLevel() const { return warningLevel; }
    QString getWarningType() const { return warningType; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    double getDepth() const { return depth; }
    int getThresholdValue() const { return thresholdValue; }
    double getDistance() const { return distance; }
    QDateTime getWarningTime() const { return warningTime; }
    
    // Setters
    void setWarningId(int id) { warningId = id; }
    void setProjectId(int id) { projectId = id; }
    void setWarningNumber(int number) { warningNumber = number; }
    void setWarningLevel(const QString &level) { warningLevel = level; }
    void setWarningType(const QString &type) { warningType = type; }
    void setLatitude(double lat) { latitude = lat; }
    void setLongitude(double lon) { longitude = lon; }
    void setDepth(double d) { depth = d; }
    void setThresholdValue(int value) { thresholdValue = value; }
    void setDistance(double dist) { distance = dist; }
    void setWarningTime(const QDateTime &time) { warningTime = time; }
    
    // 检查预警是否有效
    bool isValid() const { return warningId > 0; }

private:
    int warningId;
    int projectId;
    int warningNumber;
    QString warningLevel;
    QString warningType;
    double latitude;
    double longitude;
    double depth;
    int thresholdValue;
    double distance;
    QDateTime warningTime;
};

#endif // WARNING_H
