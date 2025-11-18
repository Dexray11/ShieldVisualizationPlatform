#ifndef GEOLOGICAL3DWIDGET_H
#define GEOLOGICAL3DWIDGET_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DCore/QTransform>
#include <QVector>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "../database/TunnelProfileDAO.h"
#include "../database/BoreholeDAO.h"

class Geological3DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Geological3DWidget(int projectId, QWidget *parent = nullptr);
    ~Geological3DWidget();

    void loadData();
    void renderScene();

private:
    void setupUI();
    void setup3DView();
    void createTunnelMesh();
    void createBoreholeMarkers();
    
    // 【新增】地质可视化核心函数
    void createGeologicalColumns();  // 创建地质柱状图
    void createGeologicalVolume();  // 创建3D地质体积
    void createLayerVolume(const BoreholeData &bh1, const BoreholeLayerData &layer1,
                          const BoreholeData &bh2, const BoreholeLayerData &layer2,
                          Qt3DCore::QEntity *parent);  // 创建地层体积块
    void createLayerSurface(const BoreholeData &bh1, const BoreholeLayerData &layer1,
                           const BoreholeData &bh2, const BoreholeLayerData &layer2,
                           Qt3DCore::QEntity *parent);  // 创建两个钻孔之间的地层连接面
    
    // 【新增】改进版地质可视化函数
    void createLayersBetweenBoreholes(const BoreholeData &bh1, const BoreholeData &bh2);  // 在相邻钻孔间创建地层
    void createLayerPrism(float x1, float y1, float top1, float bottom1,
                         float x2, float y2, float top2, float bottom2,
                         const QString &rockName);  // 创建地层六面体
    void createBoreholeMarker(const BoreholeData &bh);  // 创建钻孔标记
    
    void createGroundSurface();
    void createCoordinateAxes();
    
    // 数据结构
    struct TunnelSegment {
        QVector3D topLeft;
        QVector3D topRight;
        QVector3D bottomLeft;
        QVector3D bottomRight;
    };

    int projectId;
    Qt3DExtras::Qt3DWindow *view3D;
    QWidget *container;
    Qt3DCore::QEntity *rootEntity;
    Qt3DCore::QEntity *tunnelEntity;
    Qt3DCore::QEntity *boreholeEntity;           // 【修改】仅用于钻孔标记
    Qt3DCore::QEntity *geologicalLayersEntity;   // 【新增】用于地质层
    Qt3DCore::QEntity *surfaceEntity;
    Qt3DCore::QEntity *axesEntity;
    Qt3DRender::QCamera *camera;
    Qt3DExtras::QOrbitCameraController *cameraController;
    
    QVector<TunnelSegment> tunnelSegments;
    QVector<BoreholeData> boreholes;
    
    // 场景边界（用于限制相机移动）
    QVector3D sceneMin;
    QVector3D sceneMax;
    QVector3D sceneCenter;
    float sceneRadius;  // 场景半径
    
    // 相机约束参数（仅用于边界计算）
    float minCameraDistance;  // 最小相机距离
    float maxCameraDistance;  // 最大相机距离
    
    // UI组件
    QPushButton *resetViewButton;
    QPushButton *toggleBoreholeButton;
    QPushButton *toggleSurfaceButton;
    QLabel *infoLabel;
    
    bool boreholesVisible;
    bool surfaceVisible;
};

#endif // GEOLOGICAL3DWIDGET_H
