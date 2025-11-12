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
    QDateTime getCreatedAt() const { return createdAt; }
    QDateTime getUpdatedAt() const { return updatedAt; }
    
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
    void setCreatedAt(const QDateTime &time) { createdAt = time; }
    void setUpdatedAt(const QDateTime &time) { updatedAt = time; }
    
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
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // PROJECT_H
