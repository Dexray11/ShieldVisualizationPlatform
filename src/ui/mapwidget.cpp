#include "mapwidget.h"
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QNetworkRequest>
#include <QtMath>
#include <QApplication>

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
    , centerLat(36.2)      // 青岛崂山
    , centerLon(120.6)
    , zoomLevel(11)        // 提高初始缩放级别，显示更多细节
    , isDragging(false)
    , waitingForDoubleClick(false)
    , lastClickedMarker(-1)
    , mapType(0)           // 默认使用高德矢量图
{
    // 加载位置图标
    locationIcon = QPixmap(":/icons/location.png");
    if (!locationIcon.isNull()) {
        locationIcon = locationIcon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);
    
    // 设置瓦片缓存大小 (100MB)
    tileCache.setMaxCost(100 * 1024 * 1024);
    
    // 初始化点击计时器
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    connect(clickTimer, &QTimer::timeout, this, &MapWidget::onClickTimer);
    
    setMinimumSize(600, 400);
    setMouseTracking(true);
    
    // 加载初始瓦片
    loadVisibleTiles();
    
    qDebug() << "MapWidget初始化完成 - 中心:" << centerLat << "," << centerLon << " 缩放:" << zoomLevel;
}

MapWidget::~MapWidget()
{
    // 清理所有网络请求
    cleanupNetworkRequests();
}

void MapWidget::cleanupNetworkRequests()
{
    // 断开所有信号连接并中止所有待下载的瓦片
    for (auto it = pendingDownloads.begin(); it != pendingDownloads.end(); ++it) {
        QNetworkReply *reply = it.value();
        if (reply) {
            // 断开信号连接，防止在析构时触发槽函数
            reply->disconnect(this);
            reply->abort();
            reply->deleteLater();
        }
    }
    pendingDownloads.clear();
}

void MapWidget::addProjectMarker(const QString &projectName, const QString &brief,
                                 const QString &unit, double lat, double lon)
{
    ProjectMarker marker;
    marker.projectName = projectName;
    marker.brief = brief;
    marker.constructionUnit = unit;
    marker.latitude = lat;
    marker.longitude = lon;
    marker.screenPos = latLonToScreen(lat, lon);
    
    markers.append(marker);
    update();
    
    qDebug() << "添加项目标记:" << projectName << "坐标:" << lat << "," << lon;
}

void MapWidget::clearMarkers()
{
    markers.clear();
    update();
}

void MapWidget::setCenter(double lat, double lon)
{
    centerLat = lat;
    centerLon = lon;
    constrainCenter();
    updateMarkerPositions();
    loadVisibleTiles();
    update();
}

void MapWidget::setZoomLevel(int zoom)
{
    if (zoom < MIN_ZOOM) zoom = MIN_ZOOM;
    if (zoom > MAX_ZOOM) zoom = MAX_ZOOM;
    
    if (zoom != zoomLevel) {
        zoomLevel = zoom;
        updateMarkerPositions();
        loadVisibleTiles();
        update();
        qDebug() << "缩放级别:" << zoomLevel;
    }
}

void MapWidget::focusOnMarker(const QString &projectName)
{
    for (const ProjectMarker &marker : markers) {
        if (marker.projectName == projectName) {
            setCenter(marker.latitude, marker.longitude);
            break;
        }
    }
}

bool MapWidget::isClickOnMarker(const QPoint &pos) const
{
    return findMarkerAt(pos) >= 0;
}

QPointF MapWidget::latLonToTile(double lat, double lon, int zoom) const
{
    double n = qPow(2.0, zoom);
    double x = (lon + 180.0) / 360.0 * n;
    double latRad = qDegreesToRadians(lat);
    double y = (1.0 - qLn(qTan(latRad) + 1.0 / qCos(latRad)) / M_PI) / 2.0 * n;
    return QPointF(x, y);
}

QPoint MapWidget::tileToScreen(double tileX, double tileY) const
{
    QPointF centerTile = latLonToTile(centerLat, centerLon, zoomLevel);
    
    int centerX = width() / 2;
    int centerY = height() / 2;
    
    int screenX = centerX + (int)((tileX - centerTile.x()) * TILE_SIZE);
    int screenY = centerY + (int)((tileY - centerTile.y()) * TILE_SIZE);
    
    return QPoint(screenX, screenY);
}

QPoint MapWidget::latLonToScreen(double lat, double lon) const
{
    QPointF tile = latLonToTile(lat, lon, zoomLevel);
    return tileToScreen(tile.x(), tile.y());
}

QString MapWidget::getTileUrl(int x, int y, int zoom) const
{
    // 使用高德地图瓦片服务 - 更适合中国地区，显示更清晰的道路、建筑、区划等信息
    // 支持多种地图类型：
    // - 矢量图（适合显示道路、建筑、文字标注）
    // - 卫星图（显示真实地形）
    
    if (mapType == 0) {
        // 高德矢量图 - 显示道路、建筑、POI、区划等详细信息
        // 使用多个子域名分散负载
        int serverIndex = (x + y) % 4 + 1;
        return QString("https://wprd0%1.is.autonavi.com/appmaptile?lang=zh_cn&size=1&style=7&x=%2&y=%3&z=%4")
            .arg(serverIndex)
            .arg(x)
            .arg(y)
            .arg(zoom);
    } else if (mapType == 1) {
        // 高德卫星图
        int serverIndex = (x + y) % 4 + 1;
        return QString("https://webst0%1.is.autonavi.com/appmaptile?style=6&x=%2&y=%3&z=%4")
            .arg(serverIndex)
            .arg(x)
            .arg(y)
            .arg(zoom);
    } else {
        // OpenStreetMap备用（如果需要）
        char subdomain = 'a' + (x + y) % 3;
        return QString("https://%1.tile.openstreetmap.org/%2/%3/%4.png")
            .arg(subdomain)
            .arg(zoom)
            .arg(x)
            .arg(y);
    }
}

void MapWidget::downloadTile(int x, int y, int zoom)
{
    QString key = QString("%1_%2_%3").arg(x).arg(y).arg(zoom);
    
    // 检查是否已在缓存中
    if (tileCache.contains(key)) {
        return;
    }
    
    // 检查是否正在下载
    if (pendingDownloads.contains(key)) {
        return;
    }
    
    // 开始下载
    QString url = getTileUrl(x, y, zoom);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::User, key);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    request.setRawHeader("Referer", "https://www.amap.com/");
    
    QNetworkReply *reply = networkManager->get(request);
    pendingDownloads[key] = reply;
    
    connect(reply, &QNetworkReply::finished, this, &MapWidget::onTileDownloaded);
    
    // 添加错误处理连接
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    // Qt 5.15+ 和 Qt 6 使用 errorOccurred
    connect(reply, &QNetworkReply::errorOccurred,
            [this, key](QNetworkReply::NetworkError error) {
                if (error != QNetworkReply::OperationCanceledError) {
                    qWarning() << "瓦片下载错误:" << key << "错误代码:" << error;
                }
            });
#else
    // Qt 5.14 及更早版本使用 error
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this, key](QNetworkReply::NetworkError error) {
                if (error != QNetworkReply::OperationCanceledError) {
                    qWarning() << "瓦片下载错误:" << key << "错误代码:" << error;
                }
            });
#endif
}

void MapWidget::onTileDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString key = reply->request().attribute(QNetworkRequest::User).toString();
    pendingDownloads.remove(key);
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap *pixmap = new QPixmap();
        
        if (pixmap->loadFromData(data)) {
            // 存入缓存
            tileCache.insert(key, pixmap, pixmap->width() * pixmap->height() * pixmap->depth() / 8);
            update();
        } else {
            delete pixmap;
            qWarning() << "无法加载瓦片数据:" << key;
        }
    } else if (reply->error() != QNetworkReply::OperationCanceledError) {
        // 只在非取消操作时打印错误
        qWarning() << "下载瓦片失败:" << key << reply->errorString();
    }
    
    reply->deleteLater();
}

void MapWidget::loadVisibleTiles()
{
    QPointF centerTile = latLonToTile(centerLat, centerLon, zoomLevel);
    
    int tilesX = (width() / TILE_SIZE) + 2;
    int tilesY = (height() / TILE_SIZE) + 2;
    
    int startX = (int)centerTile.x() - tilesX / 2;
    int startY = (int)centerTile.y() - tilesY / 2;
    
    int maxTile = (1 << zoomLevel) - 1;
    
    for (int dy = 0; dy <= tilesY; dy++) {
        for (int dx = 0; dx <= tilesX; dx++) {
            int tileX = startX + dx;
            int tileY = startY + dy;
            
            // 限制瓦片坐标
            if (tileX < 0 || tileX > maxTile || tileY < 0 || tileY > maxTile) {
                continue;
            }
            
            downloadTile(tileX, tileY, zoomLevel);
        }
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // 填充背景
    painter.fillRect(rect(), QColor("#e0e8f0"));
    
    // 绘制瓦片
    drawTiles(painter);
    
    // 绘制标记
    drawMarkers(painter);
    
    // 绘制版权信息
    painter.setPen(QColor("#666666"));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    QString copyright = (mapType == 0 || mapType == 1) ? 
        "© 高德地图" : "© OpenStreetMap contributors";
    
    painter.drawText(rect().adjusted(5, 0, -5, -5), 
                    Qt::AlignBottom | Qt::AlignRight, 
                    copyright);
}

void MapWidget::drawTiles(QPainter &painter)
{
    QPointF centerTile = latLonToTile(centerLat, centerLon, zoomLevel);
    
    int tilesX = (width() / TILE_SIZE) + 2;
    int tilesY = (height() / TILE_SIZE) + 2;
    
    int startX = (int)centerTile.x() - tilesX / 2;
    int startY = (int)centerTile.y() - tilesY / 2;
    
    int maxTile = (1 << zoomLevel) - 1;
    
    for (int dy = 0; dy <= tilesY; dy++) {
        for (int dx = 0; dx <= tilesX; dx++) {
            int tileX = startX + dx;
            int tileY = startY + dy;
            
            if (tileX < 0 || tileX > maxTile || tileY < 0 || tileY > maxTile) {
                continue;
            }
            
            QString key = QString("%1_%2_%3").arg(tileX).arg(tileY).arg(zoomLevel);
            QPixmap *pixmap = tileCache.object(key);
            
            if (pixmap) {
                QPoint screenPos = tileToScreen(tileX, tileY);
                painter.drawPixmap(screenPos, *pixmap);
            } else {
                // 绘制占位符
                QPoint screenPos = tileToScreen(tileX, tileY);
                QRect tileRect(screenPos, QSize(TILE_SIZE, TILE_SIZE));
                painter.fillRect(tileRect, QColor("#f0f0f0"));
                painter.setPen(QPen(QColor("#cccccc"), 0.5));
                painter.drawRect(tileRect);
                
                // 显示加载提示
                painter.setPen(QColor("#999999"));
                QFont font = painter.font();
                font.setPointSize(8);
                painter.setFont(font);
                painter.drawText(tileRect, Qt::AlignCenter, "加载中...");
            }
        }
    }
}

void MapWidget::drawMarkers(QPainter &painter)
{
    for (const ProjectMarker &marker : markers) {
        // 绘制位置图标
        if (!locationIcon.isNull()) {
            int iconWidth = locationIcon.width();
            int iconHeight = locationIcon.height();
            QPoint iconPos(marker.screenPos.x() - iconWidth / 2, 
                          marker.screenPos.y() - iconHeight);
            painter.drawPixmap(iconPos, locationIcon);
        } else {
            // 备用标记 - 更醒目的样式
            painter.setPen(QPen(QColor("#d32f2f"), 3));
            painter.setBrush(QColor("#f44336"));
            painter.drawEllipse(marker.screenPos, 10, 10);
            
            // 绘制白色内圈
            painter.setBrush(QColor("#ffffff"));
            painter.drawEllipse(marker.screenPos, 4, 4);
        }
        
        // 绘制项目名称
        QFont font = painter.font();
        font.setPointSize(9);
        font.setBold(true);
        painter.setFont(font);
        
        QRect textRect(marker.screenPos.x() - 80, marker.screenPos.y() + 5, 160, 25);
        
        // 绘制文本背景 - 半透明白色
        painter.fillRect(textRect, QColor(255, 255, 255, 230));
        painter.setPen(QPen(QColor("#2196F3"), 1));
        painter.drawRect(textRect);
        
        // 绘制文本
        painter.setPen(QColor("#1565C0"));
        painter.drawText(textRect, Qt::AlignCenter, marker.projectName);
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastMousePos = event->pos();
        dragStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    
    QWidget::mousePressEvent(event);
}

void MapWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 取消单击计时器
        clickTimer->stop();
        waitingForDoubleClick = false;
        
        int markerIndex = findMarkerAt(event->pos());
        if (markerIndex >= 0) {
            emit projectMarkerDoubleClicked(markers[markerIndex].projectName);
        } else {
            // 双击地图放大
            setZoomLevel(zoomLevel + 1);
        }
    }
    
    QWidget::mouseDoubleClickEvent(event);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        
        // 计算地图移动
        double n = qPow(2.0, zoomLevel);
        double deltaLon = -delta.x() * 360.0 / (n * TILE_SIZE);
        double deltaLat = delta.y() * 180.0 / (n * TILE_SIZE);
        
        centerLon += deltaLon;
        centerLat += deltaLat;
        
        constrainCenter();
        updateMarkerPositions();
        loadVisibleTiles();
        update();
    } else {
        // 鼠标悬停效果
        int markerIndex = findMarkerAt(event->pos());
        if (markerIndex >= 0) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
        
        // 检查是否是点击（没有拖动很远）
        QPoint delta = event->pos() - dragStartPos;
        if (delta.manhattanLength() < 5) {
            int markerIndex = findMarkerAt(event->pos());
            if (markerIndex >= 0) {
                // 开始等待双击
                waitingForDoubleClick = true;
                lastClickedMarker = markerIndex;
                clickTimer->start(DOUBLE_CLICK_INTERVAL);
            } else {
                // 点击了地图空白区域，发出信号
                emit mapAreaClicked();
            }
        }
    }
    
    QWidget::mouseReleaseEvent(event);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    
    if (delta > 0) {
        setZoomLevel(zoomLevel + 1);
    } else if (delta < 0) {
        setZoomLevel(zoomLevel - 1);
    }
    
    QWidget::wheelEvent(event);
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateMarkerPositions();
    loadVisibleTiles();
}

void MapWidget::onClickTimer()
{
    if (waitingForDoubleClick && lastClickedMarker >= 0) {
        // 单击事件
        const ProjectMarker &marker = markers[lastClickedMarker];
        emit projectMarkerClicked(marker.projectName, marker.brief, 
                                 marker.constructionUnit, marker.screenPos);
    }
    
    waitingForDoubleClick = false;
    lastClickedMarker = -1;
}

int MapWidget::findMarkerAt(const QPoint &pos) const
{
    for (int i = 0; i < markers.size(); i++) {
        const ProjectMarker &marker = markers[i];
        
        int dx = pos.x() - marker.screenPos.x();
        int dy = pos.y() - marker.screenPos.y();
        double distance = qSqrt(dx * dx + dy * dy);
        
        if (distance <= MARKER_HIT_RADIUS) {
            return i;
        }
    }
    
    return -1;
}

void MapWidget::updateMarkerPositions()
{
    for (ProjectMarker &marker : markers) {
        marker.screenPos = latLonToScreen(marker.latitude, marker.longitude);
    }
}

void MapWidget::constrainCenter()
{
    // 限制纬度
    if (centerLat > 85.0) centerLat = 85.0;
    if (centerLat < -85.0) centerLat = -85.0;
    
    // 限制经度
    while (centerLon > 180.0) centerLon -= 360.0;
    while (centerLon < -180.0) centerLon += 360.0;
}
