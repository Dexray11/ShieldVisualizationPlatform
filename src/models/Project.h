#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QDateTime>

/**
 * @brief 项目模型类
 * 
 * 表示一个盾构项目的完整信息
 */
class Project
{
public:
    Project();
    Project(int id, const QString &name, const QString &brief,
            double lat, double lon, const QString &unit,
            const QString &startDate, double progress,
            const QString &location, const QString &status);
    
    // Getters
    int getProjectId() const { return projectId; }
    QString getProjectName() const { return projectName; }
    QString getBrief() const { return brief; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    QString getConstructionUnit() const { return constructionUnit; }
    QString getStartDate() const { return startDate; }
    double getProgress() const { return progress; }
    QString getLocation() const { return location; }
    QString getStatus() const { return status; }
    QString getMap2DPath() const { return map2DPath; }
    QString getMap3DPath() const { return map3DPath; }
    QString getEmergencyContact1Name() const { return emergencyContact1Name; }
    QString getEmergencyContact1Phone() const { return emergencyContact1Phone; }
    QString getEmergencyContact2Name() const { return emergencyContact2Name; }
    QString getEmergencyContact2Phone() const { return emergencyContact2Phone; }
    QDateTime getCreatedAt() const { return createdAt; }
    QDateTime getUpdatedAt() const { return updatedAt; }
    double getCurrentMileage() const { return currentMileage; }
    double getStartMileage() const { return startMileage; }
    double getEndMileage() const { return endMileage; }
    
    // Setters
    void setProjectId(int id) { projectId = id; }
    void setProjectName(const QString &name) { projectName = name; }
    void setBrief(const QString &description) { brief = description; }
    void setLatitude(double lat) { latitude = lat; }
    void setLongitude(double lon) { longitude = lon; }
    void setConstructionUnit(const QString &unit) { constructionUnit = unit; }
    void setStartDate(const QString &date) { startDate = date; }
    void setProgress(double prog) { progress = prog; }
    void setLocation(const QString &loc) { location = loc; }
    void setStatus(const QString &stat) { status = stat; }
    void setMap2DPath(const QString &path) { map2DPath = path; }
    void setMap3DPath(const QString &path) { map3DPath = path; }
    void setEmergencyContact1Name(const QString &name) { emergencyContact1Name = name; }
    void setEmergencyContact1Phone(const QString &phone) { emergencyContact1Phone = phone; }
    void setEmergencyContact2Name(const QString &name) { emergencyContact2Name = name; }
    void setEmergencyContact2Phone(const QString &phone) { emergencyContact2Phone = phone; }
    void setCreatedAt(const QDateTime &time) { createdAt = time; }
    void setUpdatedAt(const QDateTime &time) { updatedAt = time; }
    void setCurrentMileage(double mileage) { currentMileage = mileage; }
    void setStartMileage(double mileage) { startMileage = mileage; }
    void setEndMileage(double mileage) { endMileage = mileage; }
    
    // 检查项目是否有效
    bool isValid() const { return projectId > 0; }

private:
    int projectId;
    QString projectName;
    QString brief;
    double latitude;
    double longitude;
    QString constructionUnit;
    QString startDate;
    double progress;
    QString location;
    QString status;
    QString map2DPath;
    QString map3DPath;
    QString emergencyContact1Name;
    QString emergencyContact1Phone;
    QString emergencyContact2Name;
    QString emergencyContact2Phone;
    QDateTime createdAt;
    QDateTime updatedAt;
    double currentMileage;
    double startMileage;
    double endMileage;
};

#endif // PROJECT_H
