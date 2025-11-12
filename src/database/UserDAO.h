#ifndef USERDAO_H
#define USERDAO_H

#include "src/models/User.h"
#include <QString>
#include <QList>

/**
 * @brief 用户数据访问对象类
 * 
 * 提供用户相关的数据库操作接口
 */
class UserDAO
{
public:
    UserDAO();
    ~UserDAO();

    /**
     * @brief 验证用户登录
     * @param username 用户名
     * @param password 密码（明文）
     * @return 验证成功返回true，失败返回false
     */
    bool validateUser(const QString &username, const QString &password);

    /**
     * @brief 根据用户名获取用户信息
     * @param username 用户名
     * @return 用户对象指针，如果不存在返回nullptr（需要调用者释放内存）
     */
    User* getUserByUsername(const QString &username);

    /**
     * @brief 根据用户ID获取用户信息
     * @param userId 用户ID
     * @return 用户对象指针，如果不存在返回nullptr（需要调用者释放内存）
     */
    User* getUserById(int userId);

    /**
     * @brief 创建新用户
     * @param user 用户对象
     * @param password 密码（明文）
     * @return 成功返回新用户的ID，失败返回-1
     */
    int createUser(const User &user, const QString &password);

    /**
     * @brief 更新用户信息
     * @param user 用户对象
     * @return 成功返回true，失败返回false
     */
    bool updateUser(const User &user);

    /**
     * @brief 删除用户
     * @param userId 用户ID
     * @return 成功返回true，失败返回false
     */
    bool deleteUser(int userId);

    /**
     * @brief 更新用户最后登录时间
     * @param userId 用户ID
     * @return 成功返回true，失败返回false
     */
    bool updateLastLogin(int userId);

    /**
     * @brief 修改用户密码
     * @param userId 用户ID
     * @param oldPassword 旧密码（明文）
     * @param newPassword 新密码（明文）
     * @return 成功返回true，失败返回false
     */
    bool changePassword(int userId, const QString &oldPassword, const QString &newPassword);

    /**
     * @brief 获取所有用户列表
     * @return 用户列表
     */
    QList<User*> getAllUsers();

    /**
     * @brief 检查用户名是否存在
     * @param username 用户名
     * @return 存在返回true，不存在返回false
     */
    bool usernameExists(const QString &username);

    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString getLastError() const { return lastError; }

private:
    /**
     * @brief 加密密码
     * @param password 明文密码
     * @return 加密后的密码哈希值
     */
    QString hashPassword(const QString &password) const;

    /**
     * @brief 验证密码
     * @param password 明文密码
     * @param hash 数据库中的密码哈希值
     * @return 匹配返回true，不匹配返回false
     */
    bool verifyPassword(const QString &password, const QString &hash) const;

private:
    QString lastError;
};

#endif // USERDAO_H
