#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include <QApplication>

const QString DatabaseManager::DB_CONNECTION_NAME = "shield_db_connection";
const QString DatabaseManager::DB_DRIVER = "QSQLITE";
const QString DatabaseManager::DB_FILE_NAME = "shield_platform.db";

DatabaseManager::DatabaseManager()
    : initialized(false)
{
    // 数据库文件路径：应用程序目录下的data文件夹
    QString appPath = QApplication::applicationDirPath();
    QDir dataDir(appPath + "/data");
    
    // 如果data目录不存在，创建它
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }
    
    databasePath = dataDir.absoluteFilePath(DB_FILE_NAME);
    qDebug() << "数据库路径:" << databasePath;
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase()
{
    QMutexLocker locker(&mutex);
    
    if (initialized && database.isOpen()) {
        return true;
    }
    
    // 检查SQLite驱动是否可用
    if (!QSqlDatabase::isDriverAvailable(DB_DRIVER)) {
        lastError = "SQLite驱动不可用";
        qCritical() << lastError;
        return false;
    }
    
    // 创建数据库连接
    database = QSqlDatabase::addDatabase(DB_DRIVER, DB_CONNECTION_NAME);
    database.setDatabaseName(databasePath);
    
    // 打开数据库
    if (!database.open()) {
        lastError = "无法打开数据库: " + database.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "数据库连接成功";
    
    // 检查数据库文件是否是新建的（判断是否存在users表）
    bool isNewDatabase = !tableExists("users");
    
    if (isNewDatabase) {
        qDebug() << "检测到新数据库，开始创建表结构...";
        
        // 创建表
        if (!createTables()) {
            lastError = "创建数据库表失败";
            qCritical() << lastError;
            return false;
        }
        
        // 插入默认数据
        if (!insertDefaultData()) {
            lastError = "插入默认数据失败";
            qWarning() << lastError;
            // 注意：这里不返回false，因为表已经创建成功
        }
    }
    
    initialized = true;
    return true;
}

QSqlDatabase DatabaseManager::getDatabase()
{
    if (!initialized) {
        initDatabase();
    }
    return QSqlDatabase::database(DB_CONNECTION_NAME);
}

bool DatabaseManager::isConnected() const
{
    return initialized && database.isOpen();
}

void DatabaseManager::closeDatabase()
{
    QMutexLocker locker(&mutex);
    
    if (database.isOpen()) {
        database.close();
        qDebug() << "数据库连接已关闭";
    }
    
    initialized = false;
}

bool DatabaseManager::executeQuery(const QString &query)
{
    QSqlQuery sqlQuery(getDatabase());
    
    if (!sqlQuery.exec(query)) {
        lastError = "SQL执行失败: " + sqlQuery.lastError().text();
        qCritical() << lastError << "\nSQL:" << query;
        return false;
    }
    
    return true;
}

bool DatabaseManager::beginTransaction()
{
    if (!getDatabase().transaction()) {
        lastError = "开始事务失败: " + getDatabase().lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::commitTransaction()
{
    if (!getDatabase().commit()) {
        lastError = "提交事务失败: " + getDatabase().lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!getDatabase().rollback()) {
        lastError = "回滚事务失败: " + getDatabase().lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::tableExists(const QString &tableName)
{
    QSqlQuery query(database);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:tableName");
    query.bindValue(":tableName", tableName);
    
    if (query.exec() && query.next()) {
        return true;
    }
    
    return false;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(database);
    
    // 创建用户表
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            username VARCHAR(50) UNIQUE NOT NULL,
            password_hash VARCHAR(255) NOT NULL,
            phone VARCHAR(20),
            role VARCHAR(20) DEFAULT 'viewer',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_login DATETIME
        )
    )";
    
    if (!query.exec(createUsersTable)) {
        lastError = "创建users表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "users表创建成功";
    
    // 创建项目表
    QString createProjectsTable = R"(
        CREATE TABLE IF NOT EXISTS projects (
            project_id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_name VARCHAR(100) NOT NULL,
            brief TEXT,
            latitude REAL,
            longitude REAL,
            construction_unit VARCHAR(100),
            start_date DATE,
            progress REAL DEFAULT 0,
            location VARCHAR(200),
            status VARCHAR(20) DEFAULT 'active',
            map_2d_path VARCHAR(500),
            map_3d_path VARCHAR(500),
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createProjectsTable)) {
        lastError = "创建projects表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "projects表创建成功";
    
    // 创建预警信息表
    QString createWarningsTable = R"(
        CREATE TABLE IF NOT EXISTS warnings (
            warning_id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            warning_number INTEGER,
            warning_level VARCHAR(10),
            warning_type VARCHAR(50),
            latitude REAL,
            longitude REAL,
            depth REAL,
            threshold_value INTEGER,
            distance REAL,
            warning_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (project_id) REFERENCES projects(project_id)
        )
    )";
    
    if (!query.exec(createWarningsTable)) {
        lastError = "创建warnings表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "warnings表创建成功";
    
    // 创建新闻表
    QString createNewsTable = R"(
        CREATE TABLE IF NOT EXISTS news (
            news_id INTEGER PRIMARY KEY AUTOINCREMENT,
            news_content TEXT NOT NULL,
            publish_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            created_by INTEGER,
            FOREIGN KEY (created_by) REFERENCES users(user_id)
        )
    )";
    
    if (!query.exec(createNewsTable)) {
        lastError = "创建news表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "news表创建成功";
    
    return true;
}

bool DatabaseManager::insertDefaultData()
{
    QSqlQuery query(database);
    
    // 密码加密函数
    auto hashPassword = [](const QString &password) -> QString {
        QByteArray hash = QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
        );
        return QString(hash.toHex());
    };
    
    // 插入默认管理员账户
    // 用户名: admin, 密码: admin123
    query.prepare("INSERT INTO users (username, password_hash, phone, role) "
                  "VALUES (:username, :password, :phone, :role)");
    query.bindValue(":username", "admin");
    query.bindValue(":password", hashPassword("admin123"));
    query.bindValue(":phone", "18500000000");
    query.bindValue(":role", "admin");
    
    if (!query.exec()) {
        qWarning() << "插入管理员账户失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "默认管理员账户创建成功 (用户名: admin, 密码: admin123)";
    
    // 插入测试工程师账户
    // 用户名: engineer, 密码: eng123
    query.prepare("INSERT INTO users (username, password_hash, phone, role) "
                  "VALUES (:username, :password, :phone, :role)");
    query.bindValue(":username", "engineer");
    query.bindValue(":password", hashPassword("eng123"));
    query.bindValue(":phone", "18566666666");
    query.bindValue(":role", "engineer");
    
    if (!query.exec()) {
        qWarning() << "插入工程师账户失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "测试工程师账户创建成功 (用户名: engineer, 密码: eng123)";
    
    // 插入示例项目
    query.prepare("INSERT INTO projects (project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status) "
                  "VALUES (:name, :brief, :lat, :lon, :unit, :date, :progress, :location, :status)");
    query.bindValue(":name", "青岛地铁6号线示例区间");
    query.bindValue(":brief", "这是一个示例项目，展示系统功能");
    query.bindValue(":lat", 36.0671);
    query.bindValue(":lon", 120.3826);
    query.bindValue(":unit", "中铁隧道局");
    query.bindValue(":date", "2024-01-01");
    query.bindValue(":progress", 35.5);
    query.bindValue(":location", "山东省青岛市");
    query.bindValue(":status", "active");
    
    if (!query.exec()) {
        qWarning() << "插入示例项目失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "示例项目创建成功";
    
    return true;
}
