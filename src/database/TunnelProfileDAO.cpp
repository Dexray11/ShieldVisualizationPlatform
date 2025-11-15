#include "TunnelProfileDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

TunnelProfileDAO::TunnelProfileDAO()
{
    // 确保数据库已初始化
    DatabaseManager::instance();
}

int TunnelProfileDAO::insertProfile(const TunnelProfileData &profile)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    // 准备SQL语句
    query.prepare(
        "INSERT INTO tunnel_profiles ("
        "    project_id, near_borehole, mileage,"
        "    top_left_x, top_left_y, top_left_z,"
        "    bottom_left_x, bottom_left_y, bottom_left_z,"
        "    top_right_x, top_right_y, top_right_z,"
        "    bottom_right_x, bottom_right_y, bottom_right_z"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(profile.projectId);
    query.addBindValue(profile.nearBorehole);
    query.addBindValue(profile.mileage);
    query.addBindValue(profile.topLeftX);
    query.addBindValue(profile.topLeftY);
    query.addBindValue(profile.topLeftZ);
    query.addBindValue(profile.bottomLeftX);
    query.addBindValue(profile.bottomLeftY);
    query.addBindValue(profile.bottomLeftZ);
    query.addBindValue(profile.topRightX);
    query.addBindValue(profile.topRightY);
    query.addBindValue(profile.topRightZ);
    query.addBindValue(profile.bottomRightX);
    query.addBindValue(profile.bottomRightY);
    query.addBindValue(profile.bottomRightZ);
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qWarning() << "插入隧道断面失败:" << lastError;
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

QVector<TunnelProfileData> TunnelProfileDAO::getProfilesByProjectId(int projectId)
{
    QVector<TunnelProfileData> profiles;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT profile_id, project_id, near_borehole, mileage,"
        "       top_left_x, top_left_y, top_left_z,"
        "       bottom_left_x, bottom_left_y, bottom_left_z,"
        "       top_right_x, top_right_y, top_right_z,"
        "       bottom_right_x, bottom_right_y, bottom_right_z "
        "FROM tunnel_profiles "
        "WHERE project_id = ? "
        "ORDER BY mileage"
    );
    query.addBindValue(projectId);
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qWarning() << "查询隧道断面失败:" << lastError;
        return profiles;
    }
    
    while (query.next()) {
        TunnelProfileData profile;
        profile.profileId = query.value(0).toInt();
        profile.projectId = query.value(1).toInt();
        profile.nearBorehole = query.value(2).toString();
        profile.mileage = query.value(3).toDouble();
        profile.topLeftX = query.value(4).toDouble();
        profile.topLeftY = query.value(5).toDouble();
        profile.topLeftZ = query.value(6).toDouble();
        profile.bottomLeftX = query.value(7).toDouble();
        profile.bottomLeftY = query.value(8).toDouble();
        profile.bottomLeftZ = query.value(9).toDouble();
        profile.topRightX = query.value(10).toDouble();
        profile.topRightY = query.value(11).toDouble();
        profile.topRightZ = query.value(12).toDouble();
        profile.bottomRightX = query.value(13).toDouble();
        profile.bottomRightY = query.value(14).toDouble();
        profile.bottomRightZ = query.value(15).toDouble();
        
        profiles.append(profile);
    }
    
    return profiles;
}

bool TunnelProfileDAO::deleteProfilesByProjectId(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("DELETE FROM tunnel_profiles WHERE project_id = ?");
    query.addBindValue(projectId);
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qWarning() << "删除隧道断面失败:" << lastError;
        return false;
    }
    
    return true;
}
