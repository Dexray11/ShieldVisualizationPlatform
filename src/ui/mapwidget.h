#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QMap>
#include <QPainter>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCache>
#include <QTimer>

/**
 * @brief 地图显示组件 - 使用高德地图瓦片服务
 * 
 * 支持地图缩放、拖动、项目标记显示
 * 单击标记显示项目信息，双击进入项目详情
 * 改进：使用高德地图，更好的网络请求管理，支持外部点击关闭弹窗
 */
class MapWidget : public QWidget
{
    Q_OBJECT

public:
    struct ProjectMarker {
        QString projectName;
        QString brief;
        QString constructionUnit;
        double latitude;
        double longitude;
        QPoint screenPos;  // 屏幕坐标
    };

    // 瓦片键（用于缓存） - 必须是public才能被qHash访问
    struct TileKey {
        int x, y, zoom;
        
        bool operator==(const TileKey &other) const {
            return x == other.x && y == other.y && zoom == other.zoom;
        }
    };

    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    // 添加项目标记
    void addProjectMarker(const QString &projectName, const QString &brief, 
                         const QString &unit, double lat, double lon);
    
    // 清除所有标记
    void clearMarkers();
    
    // 设置地图中心
    void setCenter(double lat, double lon);
    
    // 设置缩放级别 (3-18)
    void setZoomLevel(int zoom);
    
    // 获取当前缩放级别
    int getZoomLevel() const { return zoomLevel; }
    
    // 聚焦到某个标记
    void focusOnMarker(const QString &projectName);
    
    // 检查点击位置是否在标记上（用于外部判断是否需要关闭弹窗）
    bool isClickOnMarker(const QPoint &pos) const;

signals:
    // 单击项目标记时发出信号
    void projectMarkerClicked(const QString &projectName, const QString &brief, 
                             const QString &unit, const QPoint &screenPos);
    
    // 双击项目标记时发出信号
    void projectMarkerDoubleClicked(const QString &projectName);
    
    // 点击地图空白区域时发出信号（用于关闭弹窗）
    void mapAreaClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTileDownloaded();
    void onClickTimer();

private:
    // 瓦片结构
    struct Tile {
        int x, y, zoom;
        QPixmap pixmap;
        
        bool operator==(const Tile &other) const {
            return x == other.x && y == other.y && zoom == other.zoom;
        }
    };
    
    // 将经纬度转换为瓦片坐标
    QPointF latLonToTile(double lat, double lon, int zoom) const;
    
    // 将瓦片坐标转换为屏幕坐标
    QPoint tileToScreen(double tileX, double tileY) const;
    
    // 将经纬度转换为屏幕坐标
    QPoint latLonToScreen(double lat, double lon) const;
    
    // 下载瓦片
    void downloadTile(int x, int y, int zoom);
    
    // 获取瓦片URL - 使用高德地图
    QString getTileUrl(int x, int y, int zoom) const;
    
    // 检查点击是否在某个标记附近
    int findMarkerAt(const QPoint &pos) const;
    
    // 更新所有标记的屏幕坐标
    void updateMarkerPositions();
    
    // 加载可见的瓦片
    void loadVisibleTiles();
    
    // 绘制瓦片
    void drawTiles(QPainter &painter);
    
    // 绘制标记
    void drawMarkers(QPainter &painter);
    
    // 限制地图边界
    void constrainCenter();
    
    // 清理网络请求
    void cleanupNetworkRequests();

private:
    QPixmap locationIcon;
    QList<ProjectMarker> markers;
    
    // 地图状态
    double centerLat;    // 中心点纬度
    double centerLon;    // 中心点经度
    int zoomLevel;       // 缩放级别 (3-18)
    
    // 拖动状态
    bool isDragging;
    QPoint lastMousePos;
    QPoint dragStartPos;
    
    // 瓦片缓存
    QCache<QString, QPixmap> tileCache;
    QMap<QString, QNetworkReply*> pendingDownloads;
    QNetworkAccessManager *networkManager;
    
    // 点击检测
    QTimer *clickTimer;
    bool waitingForDoubleClick;
    int lastClickedMarker;
    
    // 地图类型选择（0=高德矢量图，1=高德卫星图，2=OpenStreetMap）
    int mapType;
    
    static constexpr int TILE_SIZE = 256;
    static constexpr int MARKER_HIT_RADIUS = 20;
    static constexpr int MIN_ZOOM = 3;
    static constexpr int MAX_ZOOM = 18;
    static constexpr int DOUBLE_CLICK_INTERVAL = 300; // 毫秒
};

// 为TileKey实现哈希函数
inline uint qHash(const MapWidget::TileKey &key, uint seed = 0)
{
    return qHash(QString("%1_%2_%3").arg(key.x).arg(key.y).arg(key.zoom), seed);
}

#endif // MAPWIDGET_H
