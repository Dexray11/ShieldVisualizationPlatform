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
            emergency_contact1_name VARCHAR(50),
            emergency_contact1_phone VARCHAR(20),
            emergency_contact2_name VARCHAR(50),
            emergency_contact2_phone VARCHAR(20),
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
    
    // 创建钻孔表
    QString createBoreholesTable = R"(
        CREATE TABLE IF NOT EXISTS boreholes (
            borehole_id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            borehole_code VARCHAR(50) NOT NULL,
            x_coordinate REAL,
            y_coordinate REAL,
            surface_elevation REAL,
            mileage REAL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (project_id) REFERENCES projects(project_id)
        )
    )";
    
    if (!query.exec(createBoreholesTable)) {
        lastError = "创建boreholes表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "boreholes表创建成功";
    
    // 创建钻孔地层表
    QString createBoreholLayersTable = R"(
        CREATE TABLE IF NOT EXISTS borehole_layers (
            layer_id INTEGER PRIMARY KEY AUTOINCREMENT,
            borehole_id INTEGER NOT NULL,
            layer_number INTEGER,
            layer_code VARCHAR(20),
            era_genesis VARCHAR(50),
            rock_name VARCHAR(100),
            bottom_elevation REAL,
            bottom_depth REAL,
            thickness REAL,
            characteristics TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (borehole_id) REFERENCES boreholes(borehole_id)
        )
    )";
    
    if (!query.exec(createBoreholLayersTable)) {
        lastError = "创建borehole_layers表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "borehole_layers表创建成功";
    
    // 创建隧道轮廓表
    QString createTunnelProfilesTable = R"(
        CREATE TABLE IF NOT EXISTS tunnel_profiles (
            profile_id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            near_borehole VARCHAR(50),
            mileage REAL,
            top_left_x REAL,
            top_left_y REAL,
            top_left_z REAL,
            bottom_left_x REAL,
            bottom_left_y REAL,
            bottom_left_z REAL,
            top_right_x REAL,
            top_right_y REAL,
            top_right_z REAL,
            bottom_right_x REAL,
            bottom_right_y REAL,
            bottom_right_z REAL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (project_id) REFERENCES projects(project_id)
        )
    )";
    
    if (!query.exec(createTunnelProfilesTable)) {
        lastError = "创建tunnel_profiles表失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    qDebug() << "tunnel_profiles表创建成功";
    
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
    
    // 插入示例项目：青岛沿海公路
    query.prepare("INSERT INTO projects (project_name, brief, latitude, longitude, "
                  "construction_unit, start_date, progress, location, status, "
                  "emergency_contact1_name, emergency_contact1_phone, "
                  "emergency_contact2_name, emergency_contact2_phone) "
                  "VALUES (:name, :brief, :lat, :lon, :unit, :date, :progress, :location, :status, "
                  ":contact1Name, :contact1Phone, :contact2Name, :contact2Phone)");
    query.bindValue(":name", "青岛沿海公路");
    query.bindValue(":brief", "测试简介");
    query.bindValue(":lat", 36.0671);
    query.bindValue(":lon", 120.3826);
    query.bindValue(":unit", "测试单位");
    query.bindValue(":date", "2024-11-28");
    query.bindValue(":progress", 66.7);
    query.bindValue(":location", "山东青岛");
    query.bindValue(":status", "active");
    query.bindValue(":contact1Name", "张三");
    query.bindValue(":contact1Phone", "15555555555");
    query.bindValue(":contact2Name", "李四");
    query.bindValue(":contact2Phone", "16666666666");
    
    if (!query.exec()) {
        qWarning() << "插入示例项目失败:" << query.lastError().text();
        return false;
    }
    
    int projectId = query.lastInsertId().toInt();
    qDebug() << "示例项目创建成功, ID:" << projectId;
    
    // 为项目插入预警信息（根据PDF中的预警表格数据）
    query.prepare("INSERT INTO warnings (project_id, warning_number, warning_level, "
                  "warning_type, latitude, longitude, depth, threshold_value, distance, warning_time) "
                  "VALUES (:pid, :num, :level, :type, :lat, :lon, :depth, :threshold, :distance, :time)");
    
    // 预警1: 岩溶发育
    query.bindValue(":pid", projectId);
    query.bindValue(":num", 16);
    query.bindValue(":level", "D");
    query.bindValue(":type", "岩溶发育");
    query.bindValue(":lat", 36.0671);
    query.bindValue(":lon", 120.3826);
    query.bindValue(":depth", 15.5);
    query.bindValue(":threshold", 1);
    query.bindValue(":distance", -6.6);
    query.bindValue(":time", QDateTime::fromString("2024-12-02 13:36:00", "yyyy-MM-dd HH:mm:ss"));
    if (!query.exec()) {
        qWarning() << "插入预警1失败:" << query.lastError().text();
    }
    
    // 预警2: 涌水涌泥
    query.bindValue(":pid", projectId);
    query.bindValue(":num", 17);
    query.bindValue(":level", "D");
    query.bindValue(":type", "涌水涌泥");
    query.bindValue(":lat", 36.0672);
    query.bindValue(":lon", 120.3827);
    query.bindValue(":depth", 16.2);
    query.bindValue(":threshold", 1);
    query.bindValue(":distance", 3.2);
    query.bindValue(":time", QDateTime::fromString("2024-12-02 13:36:00", "yyyy-MM-dd HH:mm:ss"));
    if (!query.exec()) {
        qWarning() << "插入预警2失败:" << query.lastError().text();
    }
    
    // 预警3: 岩层断裂
    query.bindValue(":pid", projectId);
    query.bindValue(":num", 18);
    query.bindValue(":level", "C");
    query.bindValue(":type", "岩层断裂");
    query.bindValue(":lat", 36.0673);
    query.bindValue(":lon", 120.3828);
    query.bindValue(":depth", 17.8);
    query.bindValue(":threshold", 2);
    query.bindValue(":distance", 4.2);
    query.bindValue(":time", QDateTime::fromString("2024-12-02 13:36:00", "yyyy-MM-dd HH:mm:ss"));
    if (!query.exec()) {
        qWarning() << "插入预警3失败:" << query.lastError().text();
    }
    
    // 预警4: 瓦斯区域
    query.bindValue(":pid", projectId);
    query.bindValue(":num", 19);
    query.bindValue(":level", "D");
    query.bindValue(":type", "瓦斯区域");
    query.bindValue(":lat", 36.0674);
    query.bindValue(":lon", 120.3829);
    query.bindValue(":depth", 18.5);
    query.bindValue(":threshold", 1);
    query.bindValue(":distance", 12.0);
    query.bindValue(":time", QDateTime::fromString("2024-12-02 13:36:00", "yyyy-MM-dd HH:mm:ss"));
    if (!query.exec()) {
        qWarning() << "插入预警4失败:" << query.lastError().text();
    }
    
    // 插入新闻
    query.prepare("INSERT INTO news (news_content, created_by) VALUES (:content, :createdBy)");
    
    query.bindValue(":content", "北京地铁在建线路11条线（段）盾构法施工区间占比68%");
    query.bindValue(":createdBy", 1);
    if (!query.exec()) {
        qWarning() << "插入新闻1失败:" << query.lastError().text();
    }
    
    query.bindValue(":content", "甘肃天陇铁路柳林隧道正洞掘进破万米大关");
    query.bindValue(":createdBy", 1);
    if (!query.exec()) {
        qWarning() << "插入新闻2失败:" << query.lastError().text();
    }
    
    query.bindValue(":content", "宜兴高铁长岗岭隧道顺利贯通");
    query.bindValue(":createdBy", 1);
    if (!query.exec()) {
        qWarning() << "插入新闻3失败:" << query.lastError().text();
    }
    
    query.bindValue(":content", "云兰高速全线12座隧道贯通");
    query.bindValue(":createdBy", 1);
    if (!query.exec()) {
        qWarning() << "插入新闻4失败:" << query.lastError().text();
    }
    
    qDebug() << "默认数据插入完成";
    return true;
}
