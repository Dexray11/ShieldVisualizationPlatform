#ifndef APISERVER_H
#define APISERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QDateTime>

class DatabaseManager;

/**
 * @brief HTTP API服务器
 * 接收来自传感器的实时数据或模拟数据
 */
class ApiServer : public QObject
{
    Q_OBJECT

public:
    explicit ApiServer(DatabaseManager* dbManager, QObject* parent = nullptr);
    ~ApiServer();

    // 启动服务器
    bool start(quint16 port = 8080);
    
    // 停止服务器
    void stop();
    
    // 获取服务器状态
    bool isRunning() const;
    quint16 port() const;

signals:
    // 接收到新的掘进参数数据
    void excavationDataReceived(int projectId, const QJsonObject& data);
    
    // 接收到新的补勘数据
    void prospectingDataReceived(int projectId, const QJsonObject& data);
    
    // 服务器状态改变
    void statusChanged(bool running);
    
    // 错误信号
    void errorOccurred(const QString& error);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    // 处理HTTP请求
    void processRequest(QTcpSocket* socket, const QString& request);
    
    // 解析HTTP请求
    struct HttpRequest {
        QString method;
        QString path;
        QMap<QString, QString> headers;
        QByteArray body;
    };
    HttpRequest parseHttpRequest(const QString& request);
    
    // 发送HTTP响应
    void sendResponse(QTcpSocket* socket, int statusCode, 
                     const QString& statusText, const QJsonObject& data);
    void sendErrorResponse(QTcpSocket* socket, int statusCode, 
                          const QString& message);
    
    // API端点处理
    bool handlePostExcavationData(const QJsonObject& json);
    bool handlePostProspectingData(const QJsonObject& json);
    bool handleGetStatus(QTcpSocket* socket);
    bool handleGetProjects(QTcpSocket* socket);

private:
    QTcpServer* m_tcpServer;
    DatabaseManager* m_dbManager;
    quint16 m_port;
    QList<QTcpSocket*> m_clients;
};

#endif // APISERVER_H
