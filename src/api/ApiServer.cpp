#include "ApiServer.h"
#include "../database/DatabaseManager.h"
#include "../database/ExcavationParameterDAO.h"
#include "../database/ProspectingDataDAO.h"
#include "../models/ExcavationParameter.h"
#include "../models/ProspectingData.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>

ApiServer::ApiServer(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent)
    , m_tcpServer(new QTcpServer(this))
    , m_dbManager(dbManager)
    , m_port(0)
{
    connect(m_tcpServer, &QTcpServer::newConnection, 
            this, &ApiServer::onNewConnection);
}

ApiServer::~ApiServer()
{
    stop();
}

bool ApiServer::start(quint16 port)
{
    if (m_tcpServer->isListening()) {
        qWarning() << "API服务器已在运行";
        return false;
    }

    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        QString error = QString("无法启动API服务器: %1").arg(m_tcpServer->errorString());
        qCritical() << error;
        emit errorOccurred(error);
        return false;
    }

    m_port = m_tcpServer->serverPort();
    qInfo() << "API服务器已启动，端口:" << m_port;
    emit statusChanged(true);
    return true;
}

void ApiServer::stop()
{
    if (!m_tcpServer->isListening()) {
        return;
    }

    // 关闭所有客户端连接
    for (QTcpSocket* client : m_clients) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    m_clients.clear();

    m_tcpServer->close();
    m_port = 0;
    qInfo() << "API服务器已停止";
    emit statusChanged(false);
}

bool ApiServer::isRunning() const
{
    return m_tcpServer->isListening();
}

quint16 ApiServer::port() const
{
    return m_port;
}

void ApiServer::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket* client = m_tcpServer->nextPendingConnection();
        m_clients.append(client);
        
        connect(client, &QTcpSocket::readyRead, this, &ApiServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &ApiServer::onDisconnected);
        
        qDebug() << "新客户端连接:" << client->peerAddress().toString();
    }
}

void ApiServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString request = QString::fromUtf8(socket->readAll());
    processRequest(socket, request);
}

void ApiServer::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_clients.removeOne(socket);
    socket->deleteLater();
    qDebug() << "客户端断开连接";
}

void ApiServer::processRequest(QTcpSocket* socket, const QString& request)
{
    HttpRequest httpReq = parseHttpRequest(request);
    
    qDebug() << "收到请求:" << httpReq.method << httpReq.path;

    // CORS支持
    if (httpReq.method == "OPTIONS") {
        QByteArray response = "HTTP/1.1 200 OK\r\n"
                             "Access-Control-Allow-Origin: *\r\n"
                             "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                             "Access-Control-Allow-Headers: Content-Type\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
        return;
    }

    // 路由处理
    if (httpReq.method == "POST" && httpReq.path == "/api/excavation") {
        QJsonDocument doc = QJsonDocument::fromJson(httpReq.body);
        if (doc.isObject()) {
            bool success = handlePostExcavationData(doc.object());
            QJsonObject response;
            response["success"] = success;
            response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            if (success) {
                response["message"] = "掘进参数数据已保存";
                sendResponse(socket, 200, "OK", response);
            } else {
                response["error"] = "保存掘进参数数据失败";
                sendResponse(socket, 500, "Internal Server Error", response);
            }
        } else {
            sendErrorResponse(socket, 400, "无效的JSON格式");
        }
    }
    else if (httpReq.method == "POST" && httpReq.path == "/api/prospecting") {
        QJsonDocument doc = QJsonDocument::fromJson(httpReq.body);
        if (doc.isObject()) {
            bool success = handlePostProspectingData(doc.object());
            QJsonObject response;
            response["success"] = success;
            response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            if (success) {
                response["message"] = "补勘数据已保存";
                sendResponse(socket, 200, "OK", response);
            } else {
                response["error"] = "保存补勘数据失败";
                sendResponse(socket, 500, "Internal Server Error", response);
            }
        } else {
            sendErrorResponse(socket, 400, "无效的JSON格式");
        }
    }
    else if (httpReq.method == "GET" && httpReq.path == "/api/status") {
        handleGetStatus(socket);
    }
    else if (httpReq.method == "GET" && httpReq.path == "/api/projects") {
        handleGetProjects(socket);
    }
    else {
        sendErrorResponse(socket, 404, "未找到该端点");
    }
}

ApiServer::HttpRequest ApiServer::parseHttpRequest(const QString& request)
{
    HttpRequest httpReq;
    QStringList lines = request.split("\r\n");
    
    if (lines.isEmpty()) return httpReq;
    
    // 解析请求行
    QStringList requestLine = lines[0].split(" ");
    if (requestLine.size() >= 2) {
        httpReq.method = requestLine[0];
        httpReq.path = requestLine[1];
    }
    
    // 解析请求头
    int i = 1;
    while (i < lines.size() && !lines[i].isEmpty()) {
        int colonPos = lines[i].indexOf(':');
        if (colonPos > 0) {
            QString key = lines[i].left(colonPos).trimmed();
            QString value = lines[i].mid(colonPos + 1).trimmed();
            httpReq.headers[key] = value;
        }
        i++;
    }
    
    // 解析请求体
    if (i + 1 < lines.size()) {
        httpReq.body = lines[i + 1].toUtf8();
    }
    
    return httpReq;
}

void ApiServer::sendResponse(QTcpSocket* socket, int statusCode, 
                             const QString& statusText, const QJsonObject& data)
{
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    QByteArray response;
    response.append(QString("HTTP/1.1 %1 %2\r\n").arg(statusCode).arg(statusText).toUtf8());
    response.append("Content-Type: application/json; charset=utf-8\r\n");
    response.append("Access-Control-Allow-Origin: *\r\n");
    response.append(QString("Content-Length: %1\r\n").arg(jsonData.size()).toUtf8());
    response.append("\r\n");
    response.append(jsonData);
    
    socket->write(response);
    socket->flush();
    socket->disconnectFromHost();
}

void ApiServer::sendErrorResponse(QTcpSocket* socket, int statusCode, const QString& message)
{
    QJsonObject errorObj;
    errorObj["success"] = false;
    errorObj["error"] = message;
    errorObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString statusText;
    switch (statusCode) {
        case 400: statusText = "Bad Request"; break;
        case 404: statusText = "Not Found"; break;
        case 500: statusText = "Internal Server Error"; break;
        default: statusText = "Error"; break;
    }
    
    sendResponse(socket, statusCode, statusText, errorObj);
}

bool ApiServer::handlePostExcavationData(const QJsonObject& json)
{
    if (!json.contains("project_id") || !json.contains("data")) {
        qWarning() << "掘进参数数据缺少必要字段";
        return false;
    }

    int projectId = json["project_id"].toInt();
    QJsonObject data = json["data"].toObject();
    
    // 创建ExcavationParameter对象
    ExcavationParameter param;
    param.setProjectId(projectId);
    param.setExcavationTime(QDateTime::fromString(
        data["excavation_time"].toString(), Qt::ISODate));
    param.setStakeMark(data["stake_mark"].toString());
    param.setMileage(data["mileage"].toDouble());
    param.setExcavationMode(data["excavation_mode"].toString());
    param.setChamberPressure(data["chamber_pressure"].toDouble());
    param.setThrustForce(data["thrust_force"].toDouble());
    param.setCutterSpeed(data["cutter_speed"].toDouble());
    param.setCutterTorque(data["cutter_torque"].toDouble());
    param.setExcavationSpeed(data["excavation_speed"].toDouble());
    param.setGroutingPressure(data["grouting_pressure"].toDouble());
    param.setGroutingVolume(data["grouting_volume"].toDouble());
    param.setSegmentNumber(data["segment_number"].toString());
    param.setExcavationDuration(data["excavation_duration"].toInt());
    param.setIdleDuration(data["idle_duration"].toInt());
    param.setFaultDuration(data["fault_duration"].toInt());
    param.setExcavationDistance(data["excavation_distance"].toDouble());

    // 保存到数据库
    ExcavationParameterDAO dao;
    if (dao.insertExcavationParameter(param)) {
        qInfo() << "掘进参数数据已保存:" << param.getStakeMark();
        emit excavationDataReceived(projectId, data);
        return true;
    } else {
        qWarning() << "保存掘进参数数据失败:" << dao.getLastError();
        return false;
    }
}

bool ApiServer::handlePostProspectingData(const QJsonObject& json)
{
    if (!json.contains("project_id") || !json.contains("data")) {
        qWarning() << "补勘数据缺少必要字段";
        return false;
    }

    int projectId = json["project_id"].toInt();
    QJsonObject data = json["data"].toObject();
    
    // 创建ProspectingData对象
    ProspectingData prospecting;
    prospecting.setProjectId(projectId);
    prospecting.setExcavationTime(QDateTime::fromString(
        data["excavation_time"].toString(), Qt::ISODate));
    prospecting.setStakeMark(data["stake_mark"].toString());
    prospecting.setMileage(data["mileage"].toDouble());
    prospecting.setCutterForce(data["cutter_force"].toDouble());
    prospecting.setCutterPenetrationResistance(data["cutter_penetration_resistance"].toDouble());
    prospecting.setFaceFrictionTorque(data["face_friction_torque"].toDouble());
    prospecting.setPWaveVelocity(data["p_wave_velocity"].toDouble());
    prospecting.setSWaveVelocity(data["s_wave_velocity"].toDouble());
    prospecting.setWaveReflectionCoeff(data["wave_reflection_coeff"].toDouble());
    prospecting.setApparentResistivity(data["apparent_resistivity"].toDouble());
    prospecting.setStressGradient(data["stress_gradient"].toDouble());
    prospecting.setWaterProbability(data["water_probability"].toDouble());
    prospecting.setRockProperties(data["rock_properties"].toString());
    prospecting.setRockDangerLevel(data["rock_danger_level"].toString());
    prospecting.setYoungsModulus(data["youngs_modulus"].toDouble());
    prospecting.setPoissonRatio(data["poisson_ratio"].toDouble());
    prospecting.setWaveVelocityRatio(data["wave_velocity_ratio"].toDouble());
    prospecting.setRockType(data["rock_type"].toString());
    prospecting.setDistributionPattern(data["distribution_pattern"].toString());

    // 保存到数据库
    ProspectingDataDAO dao;
    int recordId = dao.insert(prospecting);
    if (recordId > 0) {
        qInfo() << "补勘数据已保存:" << prospecting.getStakeMark();
        emit prospectingDataReceived(projectId, data);
        return true;
    } else {
        qWarning() << "保存补勘数据失败:" << dao.getLastError();
        return false;
    }
}

bool ApiServer::handleGetStatus(QTcpSocket* socket)
{
    QJsonObject status;
    status["success"] = true;
    status["server"] = "Shield Platform API Server";
    status["version"] = "1.0.0";
    status["status"] = "running";
    status["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    status["port"] = m_port;
    status["connections"] = m_clients.size();
    
    sendResponse(socket, 200, "OK", status);
    return true;
}

bool ApiServer::handleGetProjects(QTcpSocket* socket)
{
    qInfo() << "=== 处理GET /api/projects请求 ===";
    
    // 获取DatabaseManager的数据库连接
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    // 打印连接信息
    qInfo() << "数据库连接名:" << db.connectionName();
    qInfo() << "数据库文件:" << db.databaseName();
    qInfo() << "数据库是否有效:" << db.isValid();
    qInfo() << "数据库是否打开:" << db.isOpen();
    
    // 验证数据库连接
    if (!db.isValid()) {
        qCritical() << "❌ 数据库连接无效";
        sendErrorResponse(socket, 500, "数据库连接无效");
        return false;
    }
    
    if (!db.isOpen()) {
        qWarning() << "⚠ 数据库未打开，尝试打开...";
        if (!db.open()) {
            qCritical() << "❌ 无法打开数据库:" << db.lastError().text();
            sendErrorResponse(socket, 500, "无法打开数据库: " + db.lastError().text());
            return false;
        }
        qInfo() << "✓ 数据库打开成功";
    }
    
    // 构建SQL查询
    QString sql = R"(
    SELECT
        project_id AS id,
        project_name AS name,
        brief,
        longitude,
        latitude,
        construction_unit,
        start_date,
        progress,
        location
    FROM projects
    ORDER BY project_id
)";
    
    qInfo() << "准备执行SQL查询:" << sql;
    
    // 使用DatabaseManager返回的数据库连接创建查询
    QSqlQuery query(db);
    
    if (!query.exec(sql)) {
        QString errorMsg = query.lastError().text();
        qCritical() << "❌ 数据库查询失败:" << errorMsg;
        qCritical() << "错误类型:" << query.lastError().type();
        qCritical() << "数据库驱动错误:" << query.lastError().driverText();
        qCritical() << "数据库错误:" << query.lastError().databaseText();
        
        // 返回更详细的错误信息
        QJsonObject errorResponse;
        errorResponse["success"] = false;
        errorResponse["error"] = errorMsg;
        errorResponse["count"] = 0;
        errorResponse["projects"] = QJsonArray();
        
        sendResponse(socket, 500, "Internal Server Error", errorResponse);
        return false;
    }
    
    qInfo() << "SQL查询执行成功";
    
    QJsonArray projectsArray;
    int count = 0;
    while (query.next()) {
        count++;
        QJsonObject proj;
        proj["id"] = query.value(0).toInt();
        proj["name"] = query.value(1).toString();
        proj["brief"] = query.value(2).toString();
        proj["longitude"] = query.value(3).toDouble();
        proj["latitude"] = query.value(4).toDouble();
        proj["construction_unit"] = query.value(5).toString();
        proj["start_time"] = query.value(6).toString();
        proj["progress"] = query.value(7).toDouble();
        proj["location"] = query.value(8).toString();
        projectsArray.append(proj);
    }
    
    qInfo() << "查询到" << count << "个项目";
    
    QJsonObject response;
    response["success"] = true;
    response["count"] = projectsArray.size();
    response["projects"] = projectsArray;
    
    qInfo() << "准备发送响应，项目数量:" << projectsArray.size();
    sendResponse(socket, 200, "OK", response);
    qInfo() << "响应已发送成功";
    
    return true;
}
