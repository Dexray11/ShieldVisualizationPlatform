#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>

class ApiServer;
class DataSimulator;
class DatabaseManager;

/**
 * @brief API管理器单例类
 * 统一管理API服务器和数据模拟器
 */
class ApiManager : public QObject
{
    Q_OBJECT

public:
    static ApiManager* instance();
    
    // 初始化（在登录成功后调用）
    void initialize(DatabaseManager* dbManager);
    
    // 启动API服务器
    bool startApiServer(quint16 port = 8080);
    
    // 停止API服务器
    void stopApiServer();
    
    // 启动数据模拟器
    void startSimulator(int projectId, int intervalMs = 5000);
    
    // 停止数据模拟器
    void stopSimulator();
    
    // 获取组件
    ApiServer* apiServer() const { return m_apiServer; }
    DataSimulator* dataSimulator() const { return m_dataSimulator; }
    
    // 获取状态
    bool isApiServerRunning() const;
    bool isSimulatorRunning() const;

signals:
    void apiServerStatusChanged(bool running);
    void simulatorStatusChanged(bool running);

private:
    ApiManager(QObject* parent = nullptr);
    ~ApiManager();
    Q_DISABLE_COPY(ApiManager)

private:
    static ApiManager* s_instance;
    ApiServer* m_apiServer;
    DataSimulator* m_dataSimulator;
    DatabaseManager* m_dbManager;
    bool m_initialized;
};

#endif // APIMANAGER_H
