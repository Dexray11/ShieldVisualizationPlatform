#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

/**
 * @brief 用户数据模型类
 * 
 * 对应数据库中的users表
 */
class User
{
public:
    User();
    User(int id, const QString &username, const QString &phone, const QString &role);
    ~User();

    // Getter方法
    int getUserId() const { return userId; }
    QString getUsername() const { return username; }
    QString getPhone() const { return phone; }
    QString getRole() const { return role; }
    QDateTime getCreatedAt() const { return createdAt; }
    QDateTime getLastLogin() const { return lastLogin; }

    // Setter方法
    void setUserId(int id) { userId = id; }
    void setUsername(const QString &name) { username = name; }
    void setPhone(const QString &phoneNumber) { phone = phoneNumber; }
    void setRole(const QString &userRole) { role = userRole; }
    void setCreatedAt(const QDateTime &time) { createdAt = time; }
    void setLastLogin(const QDateTime &time) { lastLogin = time; }

    // 角色类型常量
    static const QString ROLE_ADMIN;
    static const QString ROLE_ENGINEER;
    static const QString ROLE_VIEWER;

    // 验证方法
    bool isValid() const;
    bool isAdmin() const { return role == ROLE_ADMIN; }
    bool isEngineer() const { return role == ROLE_ENGINEER; }

private:
    int userId;
    QString username;
    QString phone;
    QString role;
    QDateTime createdAt;
    QDateTime lastLogin;
};

#endif // USER_H
