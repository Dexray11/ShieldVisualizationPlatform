#include "UserDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>

UserDAO::UserDAO()
{
}

UserDAO::~UserDAO()
{
}

QString UserDAO::hashPassword(const QString &password) const
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    );
    return QString(hash.toHex());
}

bool UserDAO::verifyPassword(const QString &password, const QString &hash) const
{
    return hashPassword(password) == hash;
}

bool UserDAO::validateUser(const QString &username, const QString &password)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT user_id, password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec()) {
        lastError = "查询用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    if (!query.next()) {
        lastError = "用户名不存在";
        return false;
    }
    
    int userId = query.value("user_id").toInt();
    QString storedHash = query.value("password_hash").toString();
    
    if (!verifyPassword(password, storedHash)) {
        lastError = "密码错误";
        return false;
    }
    
    // 更新最后登录时间
    updateLastLogin(userId);
    
    return true;
}

User* UserDAO::getUserByUsername(const QString &username)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT user_id, username, phone, role, created_at, last_login "
                  "FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec()) {
        lastError = "查询用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return nullptr;
    }
    
    if (!query.next()) {
        lastError = "用户不存在";
        return nullptr;
    }
    
    User* user = new User();
    user->setUserId(query.value("user_id").toInt());
    user->setUsername(query.value("username").toString());
    user->setPhone(query.value("phone").toString());
    user->setRole(query.value("role").toString());
    user->setCreatedAt(query.value("created_at").toDateTime());
    user->setLastLogin(query.value("last_login").toDateTime());
    
    return user;
}

User* UserDAO::getUserById(int userId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT user_id, username, phone, role, created_at, last_login "
                  "FROM users WHERE user_id = :userId");
    query.bindValue(":userId", userId);
    
    if (!query.exec()) {
        lastError = "查询用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return nullptr;
    }
    
    if (!query.next()) {
        lastError = "用户不存在";
        return nullptr;
    }
    
    User* user = new User();
    user->setUserId(query.value("user_id").toInt());
    user->setUsername(query.value("username").toString());
    user->setPhone(query.value("phone").toString());
    user->setRole(query.value("role").toString());
    user->setCreatedAt(query.value("created_at").toDateTime());
    user->setLastLogin(query.value("last_login").toDateTime());
    
    return user;
}

int UserDAO::createUser(const User &user, const QString &password)
{
    // 检查用户名是否已存在
    if (usernameExists(user.getUsername())) {
        lastError = "用户名已存在";
        return -1;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("INSERT INTO users (username, password_hash, phone, role) "
                  "VALUES (:username, :password, :phone, :role)");
    query.bindValue(":username", user.getUsername());
    query.bindValue(":password", hashPassword(password));
    query.bindValue(":phone", user.getPhone());
    query.bindValue(":role", user.getRole());
    
    if (!query.exec()) {
        lastError = "创建用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool UserDAO::updateUser(const User &user)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("UPDATE users SET username = :username, phone = :phone, "
                  "role = :role WHERE user_id = :userId");
    query.bindValue(":username", user.getUsername());
    query.bindValue(":phone", user.getPhone());
    query.bindValue(":role", user.getRole());
    query.bindValue(":userId", user.getUserId());
    
    if (!query.exec()) {
        lastError = "更新用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool UserDAO::deleteUser(int userId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("DELETE FROM users WHERE user_id = :userId");
    query.bindValue(":userId", userId);
    
    if (!query.exec()) {
        lastError = "删除用户失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool UserDAO::updateLastLogin(int userId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("UPDATE users SET last_login = :lastLogin WHERE user_id = :userId");
    query.bindValue(":lastLogin", QDateTime::currentDateTime());
    query.bindValue(":userId", userId);
    
    if (!query.exec()) {
        lastError = "更新登录时间失败: " + query.lastError().text();
        qWarning() << lastError;
        return false;
    }
    
    return true;
}

bool UserDAO::changePassword(int userId, const QString &oldPassword, const QString &newPassword)
{
    // 先验证旧密码
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT password_hash FROM users WHERE user_id = :userId");
    query.bindValue(":userId", userId);
    
    if (!query.exec() || !query.next()) {
        lastError = "查询用户失败";
        return false;
    }
    
    QString storedHash = query.value("password_hash").toString();
    
    if (!verifyPassword(oldPassword, storedHash)) {
        lastError = "旧密码错误";
        return false;
    }
    
    // 更新为新密码
    query.prepare("UPDATE users SET password_hash = :password WHERE user_id = :userId");
    query.bindValue(":password", hashPassword(newPassword));
    query.bindValue(":userId", userId);
    
    if (!query.exec()) {
        lastError = "修改密码失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

QList<User*> UserDAO::getAllUsers()
{
    QList<User*> users;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    if (!query.exec("SELECT user_id, username, phone, role, created_at, last_login FROM users")) {
        lastError = "查询用户列表失败: " + query.lastError().text();
        qCritical() << lastError;
        return users;
    }
    
    while (query.next()) {
        User* user = new User();
        user->setUserId(query.value("user_id").toInt());
        user->setUsername(query.value("username").toString());
        user->setPhone(query.value("phone").toString());
        user->setRole(query.value("role").toString());
        user->setCreatedAt(query.value("created_at").toDateTime());
        user->setLastLogin(query.value("last_login").toDateTime());
        users.append(user);
    }
    
    return users;
}

bool UserDAO::usernameExists(const QString &username)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toInt() > 0;
}
