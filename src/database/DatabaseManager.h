#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QMutex>

/**
 * @brief 数据库管理类（单例模式）
 * 
 * 负责数据库连接的创建、维护和关闭
 * 提供数据库操作的基础方法
 */
class DatabaseManager
{
public:
    // 获取单例实例
    static DatabaseManager& instance();
    
    // 初始化数据库
    bool initDatabase();
    
    // 获取数据库连接
    QSqlDatabase getDatabase();
    
    // 检查数据库是否已连接
    bool isConnected() const;
    
    // 关闭数据库连接
    void closeDatabase();
    
    // 获取最后的错误信息
    QString getLastError() const { return lastError; }
    
    // 执行SQL语句
    bool executeQuery(const QString &query);
    
    // 开始事务
    bool beginTransaction();
    
    // 提交事务
    bool commitTransaction();
    
    // 回滚事务
    bool rollbackTransaction();

private:
    DatabaseManager();
    ~DatabaseManager();
    
    // 禁止拷贝
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // 创建数据库表
    bool createTables();
    
    // 插入默认数据
    bool insertDefaultData();
    
    // 检查表是否存在
    bool tableExists(const QString &tableName);

private:
    QSqlDatabase database;
    QString databasePath;
    QString lastError;
    QMutex mutex;
    bool initialized;
    
    static const QString DB_CONNECTION_NAME;
    static const QString DB_DRIVER;
    static const QString DB_FILE_NAME;
};

#endif // DATABASEMANAGER_H
