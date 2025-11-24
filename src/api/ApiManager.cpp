#include "ApiManager.h"
#include "ApiServer.h"
#include "DataSimulator.h"
#include "../database/DatabaseManager.h"

#include <QDebug>

ApiManager* ApiManager::s_instance = nullptr;

ApiManager::ApiManager(QObject* parent)
    : QObject(parent)
    , m_apiServer(nullptr)
    , m_dataSimulator(nullptr)
    , m_dbManager(nullptr)
    , m_initialized(false)
{
}

ApiManager::~ApiManager()
{
    if (m_apiServer) {
        m_apiServer->stop();
        delete m_apiServer;
    }
    
    if (m_dataSimulator) {
        m_dataSimulator->stop();
        delete m_dataSimulator;
    }
}

ApiManager* ApiManager::instance()
{
    if (!s_instance) {
        s_instance = new ApiManager();
    }
    return s_instance;
}

void ApiManager::initialize(DatabaseManager* dbManager)
{
    // 如果已经初始化过，直接返回
    if (m_initialized) {
        return;
    }
    
    m_dbManager = dbManager;
    
    if (!m_apiServer) {
        m_apiServer = new ApiServer(dbManager, this);
        connect(m_apiServer, &ApiServer::statusChanged,
                this, &ApiManager::apiServerStatusChanged);
    }
    
    if (!m_dataSimulator) {
        m_dataSimulator = new DataSimulator(dbManager, this);
        connect(m_dataSimulator, &DataSimulator::statusChanged,
                this, &ApiManager::simulatorStatusChanged);
    }
    
    m_initialized = true;
    qInfo() << "API管理器已初始化";
}

bool ApiManager::startApiServer(quint16 port)
{
    if (!m_apiServer) {
        qWarning() << "API服务器未初始化";
        return false;
    }
    
    return m_apiServer->start(port);
}

void ApiManager::stopApiServer()
{
    if (m_apiServer) {
        m_apiServer->stop();
    }
}

void ApiManager::startSimulator(int projectId, int intervalMs)
{
    if (!m_dataSimulator) {
        qWarning() << "数据模拟器未初始化";
        return;
    }
    
    m_dataSimulator->start(projectId, intervalMs);
}

void ApiManager::stopSimulator()
{
    if (m_dataSimulator) {
        m_dataSimulator->stop();
    }
}

bool ApiManager::isApiServerRunning() const
{
    return m_apiServer && m_apiServer->isRunning();
}

bool ApiManager::isSimulatorRunning() const
{
    return m_dataSimulator && m_dataSimulator->isRunning();
}
