#include "geological3dwidget.h"
#include "../utils/stylehelper.h"
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <cfloat>

// 根据岩性名称返回对应的颜色
QColor getColorForRockType(const QString &rockName) 
{
    // 素填土
    if (rockName.contains("素填土") || rockName.contains("填土")) {
        return QColor(218, 165, 32);  // 金橙色
    }
    
    // 强风化岩
    if (rockName.contains("强风化")) {
        return QColor(205, 133, 63);  // 秘鲁色（棕橙色）
    }
    
    // 中风化岩
    if (rockName.contains("中风化")) {
        return QColor(160, 82, 45);  // 赭褐色
    }
    
    // 微风化岩
    if (rockName.contains("微风化")) {
        return QColor(105, 105, 105);  // 暗灰色
    }
    
    // 粘土类
    if (rockName.contains("粘土") || rockName.contains("黏土")) {
        if (rockName.contains("淤泥质")) {
            return QColor(100, 149, 237);
        } else if (rockName.contains("粉质")) {
            return QColor(210, 180, 140);
        }
        return QColor(255, 215, 0);
    }
    
    // 淤泥类
    if (rockName.contains("淤泥")) {
        return QColor(70, 130, 180);
    }
    
    // 砂土类
    if (rockName.contains("砂") || rockName.contains("沙")) {
        if (rockName.contains("细")) {
            return QColor(244, 164, 96);
        } else if (rockName.contains("中")) {
            return QColor(222, 184, 135);
        } else if (rockName.contains("粗")) {
            return QColor(188, 143, 143);
        }
        return QColor(210, 180, 140);
    }
    
    // 砾石类
    if (rockName.contains("砾") || rockName.contains("卵石")) {
        return QColor(169, 169, 169);
    }
    
    // 岩石类
    if (rockName.contains("岩")) {
        if (rockName.contains("泥")) {
            return QColor(112, 128, 144);
        } else if (rockName.contains("砂")) {
            return QColor(160, 82, 45);
        }
        return QColor(105, 105, 105);
    }
    
    // 默认颜色
    return QColor(200, 200, 200);
}

Geological3DWidget::Geological3DWidget(int projectId, QWidget *parent)
    : QWidget(parent)
    , projectId(projectId)
    , view3D(nullptr)
    , container(nullptr)
    , rootEntity(nullptr)
    , tunnelEntity(nullptr)
    , boreholeEntity(nullptr)
    , geologicalLayersEntity(nullptr)
    , surfaceEntity(nullptr)
    , axesEntity(nullptr)
    , camera(nullptr)
    , cameraController(nullptr)
    , sceneMin(0, 0, 0)
    , sceneMax(0, 0, 0)
    , sceneCenter(0, 0, 0)
    , sceneRadius(200.0f)
    , minCameraDistance(0.0f)
    , maxCameraDistance(0.0f)
    , boreholesVisible(true)
    , surfaceVisible(true)
{
    qDebug() << "Geological3DWidget构造函数开始，projectId:" << projectId;
    
    try {
        setupUI();
        qDebug() << "setupUI完成";
        
        loadData();
        qDebug() << "loadData完成";
        
        renderScene();
        qDebug() << "renderScene完成";
    }
    catch (const std::exception& e) {
        qCritical() << "Geological3DWidget初始化异常:" << e.what();
    }
    catch (...) {
        qCritical() << "Geological3DWidget初始化发生未知异常";
    }
    
    qDebug() << "Geological3DWidget构造完成";
}

Geological3DWidget::~Geological3DWidget()
{
}

void Geological3DWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    
    // 控制按钮栏
    QWidget *controlBar = new QWidget(this);
    controlBar->setStyleSheet(QString("background-color: white; border: 1px solid %1;").arg(StyleHelper::COLOR_BORDER));
    QHBoxLayout *controlLayout = new QHBoxLayout(controlBar);
    controlLayout->setContentsMargins(10, 5, 10, 5);
    
    QLabel *titleLabel = new QLabel("三维地质模型", this);
    titleLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(StyleHelper::COLOR_PRIMARY));
    
    resetViewButton = new QPushButton("重置视图", this);
    resetViewButton->setStyleSheet(StyleHelper::getButtonStyle());
    connect(resetViewButton, &QPushButton::clicked, this, &Geological3DWidget::setup3DView);
    
    toggleBoreholeButton = new QPushButton("隐藏钻孔", this);
    toggleBoreholeButton->setStyleSheet(StyleHelper::getButtonStyle());
    connect(toggleBoreholeButton, &QPushButton::clicked, [this]() {
        boreholesVisible = !boreholesVisible;
        if (boreholeEntity) {
            boreholeEntity->setEnabled(boreholesVisible);
        }
        toggleBoreholeButton->setText(boreholesVisible ? "隐藏钻孔" : "显示钻孔");
    });
    
    toggleSurfaceButton = new QPushButton("隐藏地表", this);
    toggleSurfaceButton->setStyleSheet(StyleHelper::getButtonStyle());
    connect(toggleSurfaceButton, &QPushButton::clicked, [this]() {
        surfaceVisible = !surfaceVisible;
        if (surfaceEntity) {
            surfaceEntity->setEnabled(surfaceVisible);
        }
        toggleSurfaceButton->setText(surfaceVisible ? "隐藏地表" : "显示地表");
    });
    
    infoLabel = new QLabel("", this);
    infoLabel->setStyleSheet(QString("color: %1;").arg(StyleHelper::COLOR_TEXT_DARK));
    
    controlLayout->addWidget(titleLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(infoLabel);
    controlLayout->addWidget(resetViewButton);
    controlLayout->addWidget(toggleBoreholeButton);
    controlLayout->addWidget(toggleSurfaceButton);
    
    mainLayout->addWidget(controlBar);
    
    // 创建3D视图
    view3D = new Qt3DExtras::Qt3DWindow();
    if (!view3D) {
        qCritical() << "创建Qt3DWindow失败";
        return;
    }
    
    qDebug() << "Qt3DWindow创建成功";
    view3D->defaultFrameGraph()->setClearColor(QColor("#e8f4f8"));  // 浅蓝色背景
    container = QWidget::createWindowContainer(view3D, this);
    container->setMinimumSize(800, 600);
    
    mainLayout->addWidget(container, 1);
}

void Geological3DWidget::loadData()
{
    tunnelSegments.clear();
    boreholes.clear();
    
    // 加载隧道轮廓数据
    TunnelProfileDAO tunnelDAO;
    QVector<TunnelProfileData> profiles = tunnelDAO.getProfilesByProjectId(projectId);
    
    qDebug() << "加载的隧道断面数量:" << profiles.size();
    
    for (const auto &profile : profiles) {
        TunnelSegment segment;
        segment.topLeft = QVector3D(profile.topLeftX, profile.topLeftY, profile.topLeftZ);
        segment.topRight = QVector3D(profile.topRightX, profile.topRightY, profile.topRightZ);
        segment.bottomLeft = QVector3D(profile.bottomLeftX, profile.bottomLeftY, profile.bottomLeftZ);
        segment.bottomRight = QVector3D(profile.bottomRightX, profile.bottomRightY, profile.bottomRightZ);
        tunnelSegments.append(segment);
    }
    
    // 加载钻孔数据
    BoreholeDAO boreholeDAO;
    boreholes = boreholeDAO.getBoreholesByProjectId(projectId);
    
    qDebug() << "加载的钻孔数量:" << boreholes.size();
    
    // 按照里程排序钻孔
    std::sort(boreholes.begin(), boreholes.end(), [](const BoreholeData &a, const BoreholeData &b) {
        return a.mileage < b.mileage;
    });
    
    // 输出信息
    QString info = QString("钻孔: %1个 | 隧道断面: %2个")
        .arg(boreholes.size())
        .arg(tunnelSegments.size());
    if (infoLabel) {
        infoLabel->setText(info);
    }
}

void Geological3DWidget::renderScene()
{
    if (!view3D) {
        qWarning() << "view3D为空，无法渲染场景";
        return;
    }
    
    // 创建根实体
    rootEntity = new Qt3DCore::QEntity();
    
    // 创建各个部分
    createCoordinateAxes();      // 坐标轴
    createGroundSurface();       // 地表
    createGeologicalVolume();    // 地质体 - 改进版
    createTunnelMesh();          // 隧道 - 改进版
    
    // 设置场景
    view3D->setRootEntity(rootEntity);
    setup3DView();
    
    qDebug() << "✓ 场景渲染完成";
}

void Geological3DWidget::createTunnelMesh()
{
    if (!rootEntity) {
        qWarning() << "rootEntity为空";
        return;
    }
    
    if (tunnelSegments.isEmpty()) {
        qWarning() << "没有隧道数据";
        return;
    }
    
    tunnelEntity = new Qt3DCore::QEntity(rootEntity);
    
    qDebug() << "开始创建改进的隧道模型，断面数:" << tunnelSegments.size();
    
    // 计算隧道中心线的所有点
    QVector<QVector3D> centerPoints;
    float totalRadius = 0.0f;
    
    for (int i = 0; i < tunnelSegments.size(); i++) {
        const TunnelSegment &seg = tunnelSegments[i];
        
        // 计算断面中心点（4个角点的平均）
        QVector3D center = (seg.topLeft + seg.topRight + seg.bottomLeft + seg.bottomRight) / 4.0f;
        centerPoints.append(center);
        
        // 计算隧道半径（宽度和高度的平均值的一半）
        float width = seg.topLeft.distanceToPoint(seg.topRight);
        float height = seg.topLeft.distanceToPoint(seg.bottomLeft);
        totalRadius += (width + height) / 4.0f;
    }
    
    float avgRadius = totalRadius / centerPoints.size();
    
    qDebug() << "隧道中心点数量:" << centerPoints.size();
    qDebug() << "平均隧道半径:" << avgRadius;
    
    // 【修改】根据钻孔范围限制隧道渲染 - 彻底修复无限延伸问题
    float minBoreholeX = FLT_MAX, maxBoreholeX = -FLT_MAX;
    float minBoreholeY = FLT_MAX, maxBoreholeY = -FLT_MAX;
    
    for (const auto &bh : boreholes) {
        minBoreholeX = qMin(minBoreholeX, static_cast<float>(bh.x));
        maxBoreholeX = qMax(maxBoreholeX, static_cast<float>(bh.x));
        minBoreholeY = qMin(minBoreholeY, static_cast<float>(bh.y));
        maxBoreholeY = qMax(maxBoreholeY, static_cast<float>(bh.y));
    }
    
    // 扩展钻孔范围20%作为隧道渲染边界
    float rangeX = maxBoreholeX - minBoreholeX;
    float rangeY = maxBoreholeY - minBoreholeY;
    float extendRatio = 0.2f;
    minBoreholeX -= rangeX * extendRatio;
    maxBoreholeX += rangeX * extendRatio;
    minBoreholeY -= rangeY * extendRatio;
    maxBoreholeY += rangeY * extendRatio;
    
    qDebug() << "钻孔范围: X[" << minBoreholeX << "," << maxBoreholeX 
             << "] Y[" << minBoreholeY << "," << maxBoreholeY << "]";
    
    int renderedSegments = 0;
    for (int i = 0; i < centerPoints.size() - 1; i++) {
        QVector3D p1 = centerPoints[i];
        QVector3D p2 = centerPoints[i + 1];
        
        // 【关键】检查隧道段是否在钻孔范围内，跳过范围外的段
        QVector3D midPoint = (p1 + p2) / 2.0f;
        if (midPoint.x() < minBoreholeX || midPoint.x() > maxBoreholeX ||
            midPoint.y() < minBoreholeY || midPoint.y() > maxBoreholeY) {
            continue;  // 跳过范围外的隧道段
        }
        
        // 计算两点间距离和方向
        QVector3D direction = p2 - p1;
        float length = direction.length();
        
        if (length < 0.01f) continue;  // 跳过太短的段
        
        direction.normalize();
        
        // 创建圆柱体实体
        Qt3DCore::QEntity *tunnelSegEntity = new Qt3DCore::QEntity(tunnelEntity);
        
        // 创建圆柱体网格
        Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
        cylinder->setRadius(avgRadius);
        cylinder->setLength(length);
        cylinder->setRings(8);
        cylinder->setSlices(32);
        
        // 计算变换矩阵
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        
        // 圆柱体中心位置（使用之前计算的midPoint）
        transform->setTranslation(midPoint);
        
        // 计算旋转：将圆柱体的Y轴（默认方向）对齐到direction
        QVector3D defaultDir(0, 1, 0);  // Qt3D圆柱体默认沿Y轴
        QVector3D rotationAxis = QVector3D::crossProduct(defaultDir, direction);
        
        if (rotationAxis.length() > 0.0001f) {
            rotationAxis.normalize();
            float dotProduct = QVector3D::dotProduct(defaultDir, direction);
            // 限制dotProduct在[-1, 1]范围内
            dotProduct = qBound(-1.0f, dotProduct, 1.0f);
            float angle = std::acos(dotProduct);
            transform->setRotation(QQuaternion::fromAxisAndAngle(rotationAxis, qRadiansToDegrees(angle)));
        } else {
            // 如果方向几乎平行或反平行，使用简单旋转
            if (QVector3D::dotProduct(defaultDir, direction) < 0) {
                transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 180));
            }
        }
        
        // 创建材质
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor(70, 130, 220, 200));  // 蓝色，半透明
        material->setAmbient(QColor(40, 80, 150));
        material->setSpecular(QColor(120, 170, 255, 100));
        material->setShininess(60.0f);
        
        // 添加组件
        tunnelSegEntity->addComponent(cylinder);
        tunnelSegEntity->addComponent(transform);
        tunnelSegEntity->addComponent(material);
        
        renderedSegments++;  // 计数实际渲染的段数
    }
    
    qDebug() << "✓ 创建了" << renderedSegments << "段隧道（共" << (centerPoints.size() - 1) 
             << "段数据，只渲染钻孔范围内的）";
}

void Geological3DWidget::createGeologicalVolume()
{
    if (!rootEntity || boreholes.isEmpty()) {
        qWarning() << "无法创建地质体";
        return;
    }
    
    // 【修改】创建两个独立的Entity：一个用于地质层，一个用于钻孔标记
    geologicalLayersEntity = new Qt3DCore::QEntity(rootEntity);  // 地质层
    boreholeEntity = new Qt3DCore::QEntity(rootEntity);          // 钻孔标记
    
    qDebug() << "开始创建改进的地质体模型";
    
    // 计算整体范围
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;
    
    for (const auto &bh : boreholes) {
        minX = qMin(minX, static_cast<float>(bh.x));
        maxX = qMax(maxX, static_cast<float>(bh.x));
        minY = qMin(minY, static_cast<float>(bh.y));
        maxY = qMax(maxY, static_cast<float>(bh.y));
        maxZ = qMax(maxZ, static_cast<float>(bh.surfaceElevation));
        
        // 找到最深的钻孔底部
        for (const auto &layer : bh.layers) {
            float bottomZ = bh.surfaceElevation - layer.bottomDepth;
            minZ = qMin(minZ, bottomZ);
        }
    }
    
    // 扩展边界，形成规则的长方体
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;
    
    float extendRatio = 0.1f;  // 扩展10%
    minX -= rangeX * extendRatio;
    maxX += rangeX * extendRatio;
    minY -= rangeY * extendRatio;
    maxY += rangeY * extendRatio;
    minZ -= rangeZ * 0.05f;  // 底部少扩展一点
    
    qDebug() << "地质体范围: X[" << minX << "," << maxX << "] Y[" << minY << "," << maxY << "] Z[" << minZ << "," << maxZ << "]";
    
    // === 创建规则的地质层结构 ===
    // 方法：在相邻钻孔之间创建规则的层状结构
    for (int i = 0; i < boreholes.size() - 1; i++) {
        const BoreholeData &bh1 = boreholes[i];
        const BoreholeData &bh2 = boreholes[i + 1];
        
        // 为每对钻孔创建地层连接
        createLayersBetweenBoreholes(bh1, bh2);
        
        // 创建钻孔位置标记
        createBoreholeMarker(bh1);
        if (i == boreholes.size() - 2) {
            createBoreholeMarker(bh2);
        }
    }
    
    qDebug() << "✓ 创建了规则的地质层结构";
}

void Geological3DWidget::createLayersBetweenBoreholes(const BoreholeData &bh1, const BoreholeData &bh2)
{
    // 获取两个钻孔的所有地层，按深度组织
    QMap<QString, QPair<float, float>> layers1, layers2;  // 岩性名称 -> (顶部标高, 底部标高)
    
    float currentTop1 = bh1.surfaceElevation;
    for (const auto &layer : bh1.layers) {
        float bottomZ = bh1.surfaceElevation - layer.bottomDepth;
        layers1[layer.rockName] = qMakePair(currentTop1, bottomZ);
        currentTop1 = bottomZ;
    }
    
    float currentTop2 = bh2.surfaceElevation;
    for (const auto &layer : bh2.layers) {
        float bottomZ = bh2.surfaceElevation - layer.bottomDepth;
        layers2[layer.rockName] = qMakePair(currentTop2, bottomZ);
        currentTop2 = bottomZ;
    }
    
    // 找到共同的地层并创建连接体
    for (auto it1 = layers1.constBegin(); it1 != layers1.constEnd(); ++it1) {
        QString rockName = it1.key();
        
        // 如果第二个钻孔也有这个岩性
        if (layers2.contains(rockName)) {
            auto pair1 = it1.value();
            auto pair2 = layers2[rockName];
            
            float top1 = pair1.first;
            float bottom1 = pair1.second;
            float top2 = pair2.first;
            float bottom2 = pair2.second;
            
            // 创建六面体连接这两个钻孔的对应地层
            createLayerPrism(bh1.x, bh1.y, top1, bottom1,
                           bh2.x, bh2.y, top2, bottom2,
                           rockName);
        }
    }
}

void Geological3DWidget::createLayerPrism(float x1, float y1, float top1, float bottom1,
                                         float x2, float y2, float top2, float bottom2,
                                         const QString &rockName)
{
    // 【修改】创建六面体几何体 - 使用geologicalLayersEntity而不是boreholeEntity
    Qt3DCore::QEntity *prismEntity = new Qt3DCore::QEntity(geologicalLayersEntity);
    
    // 计算地层宽度（垂直于隧道方向）
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    // 增大地层宽度以形成可见的地质体
    float width = qMax(distance * 0.8f, 30.0f);  // 宽度至少30米，或钻孔间距的80%
    
    // 计算垂直于连线的方向
    QVector3D forward(dx, dy, 0);
    if (forward.length() > 0.001f) {
        forward.normalize();
    } else {
        forward = QVector3D(1, 0, 0);
    }
    QVector3D up(0, 0, 1);
    QVector3D right = QVector3D::crossProduct(forward, up).normalized();
    
    // 定义8个顶点（扩展地层宽度）
    // 钻孔1位置的4个点
    QVector3D offset1 = right * (width / 2.0f);
    QVector3D v0 = QVector3D(x1, y1, top1) - offset1;     // 左上
    QVector3D v1 = QVector3D(x1, y1, top1) + offset1;     // 右上
    QVector3D v2 = QVector3D(x1, y1, bottom1) + offset1;  // 右下
    QVector3D v3 = QVector3D(x1, y1, bottom1) - offset1;  // 左下
    
    // 钻孔2位置的4个点
    QVector3D offset2 = right * (width / 2.0f);
    QVector3D v4 = QVector3D(x2, y2, top2) - offset2;     // 左上
    QVector3D v5 = QVector3D(x2, y2, top2) + offset2;     // 右上
    QVector3D v6 = QVector3D(x2, y2, bottom2) + offset2;  // 右下
    QVector3D v7 = QVector3D(x2, y2, bottom2) - offset2;  // 左下
    
    // 创建六面体的12个三角形（每面2个三角形）
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    
    // 顶面
    vertices << v0 << v1 << v5 << v0 << v5 << v4;
    for(int i = 0; i < 6; i++) normals << QVector3D(0, 0, 1);
    
    // 底面
    vertices << v3 << v7 << v6 << v3 << v6 << v2;
    for(int i = 0; i < 6; i++) normals << QVector3D(0, 0, -1);
    
    // 前面（远离观察者）
    vertices << v0 << v4 << v7 << v0 << v7 << v3;
    QVector3D normal1 = -right;
    for(int i = 0; i < 6; i++) normals << normal1;
    
    // 后面（靠近观察者）
    vertices << v1 << v2 << v6 << v1 << v6 << v5;
    QVector3D normal2 = right;
    for(int i = 0; i < 6; i++) normals << normal2;
    
    // 左侧面
    vertices << v0 << v3 << v2 << v0 << v2 << v1;
    QVector3D normal3 = -forward;
    for(int i = 0; i < 6; i++) normals << normal3;
    
    // 右侧面
    vertices << v4 << v5 << v6 << v4 << v6 << v7;
    QVector3D normal4 = forward;
    for(int i = 0; i < 6; i++) normals << normal4;
    
    // 创建顶点数据缓冲
    QByteArray vertexData;
    vertexData.resize(vertices.size() * 3 * sizeof(float));
    float *vertexPtr = reinterpret_cast<float*>(vertexData.data());
    for (const auto &v : vertices) {
        *vertexPtr++ = v.x();
        *vertexPtr++ = v.y();
        *vertexPtr++ = v.z();
    }
    
    // 创建法线数据缓冲
    QByteArray normalData;
    normalData.resize(normals.size() * 3 * sizeof(float));
    float *normalPtr = reinterpret_cast<float*>(normalData.data());
    for (const auto &n : normals) {
        *normalPtr++ = n.x();
        *normalPtr++ = n.y();
        *normalPtr++ = n.z();
    }
    
    // 顶点缓冲
    Qt3DCore::QBuffer *vertexBuffer = new Qt3DCore::QBuffer();
    vertexBuffer->setData(vertexData);
    
    Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute();
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexBuffer);
    positionAttribute->setCount(vertices.size());
    
    // 法线缓冲
    Qt3DCore::QBuffer *normalBuffer = new Qt3DCore::QBuffer();
    normalBuffer->setData(normalData);
    
    Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute();
    normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(normalBuffer);
    normalAttribute->setCount(normals.size());
    
    // 创建几何体
    Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry();
    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(normalAttribute);
    
    Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer();
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    
    // 材质
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    QColor layerColor = getColorForRockType(rockName);
    material->setDiffuse(layerColor);
    material->setAmbient(layerColor.darker(130));
    material->setSpecular(QColor(255, 255, 255, 50));
    material->setShininess(40.0f);
    
    prismEntity->addComponent(geometryRenderer);
    prismEntity->addComponent(material);
}

void Geological3DWidget::createBoreholeMarker(const BoreholeData &bh)
{
    // 在钻孔位置创建一个垂直的圆柱标记
    Qt3DCore::QEntity *marker = new Qt3DCore::QEntity(boreholeEntity);
    
    // 计算钻孔深度
    float depth = 0;
    if (!bh.layers.isEmpty()) {
        depth = bh.layers.last().bottomDepth;
    }
    
    float topZ = bh.surfaceElevation;
    float bottomZ = topZ - depth;
    
    // 创建较粗的圆柱以便更明显
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(1.2f);  // 增大半径
    cylinder->setLength(depth);
    cylinder->setRings(8);
    cylinder->setSlices(16);
    
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    transform->setTranslation(QVector3D(bh.x, bh.y, (topZ + bottomZ) / 2.0f));
    
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(255, 60, 60));  // 明亮的红色
    material->setAmbient(QColor(180, 30, 30));
    material->setSpecular(QColor(255, 150, 150));
    material->setShininess(60.0f);
    
    marker->addComponent(cylinder);
    marker->addComponent(transform);
    marker->addComponent(material);
    
    // 在顶部添加一个更大的球体标记
    Qt3DCore::QEntity *sphere = new Qt3DCore::QEntity(boreholeEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRadius(2.5f);  // 增大球体
    sphereMesh->setRings(16);
    sphereMesh->setSlices(16);
    
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
    sphereTransform->setTranslation(QVector3D(bh.x, bh.y, topZ + 1.0f));  // 稍微抬高
    
    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(QColor(255, 100, 0));  // 橙红色
    sphereMaterial->setAmbient(QColor(200, 80, 0));
    sphereMaterial->setSpecular(QColor(255, 200, 100));
    sphereMaterial->setShininess(80.0f);
    
    sphere->addComponent(sphereMesh);
    sphere->addComponent(sphereTransform);
    sphere->addComponent(sphereMaterial);
}

void Geological3DWidget::createBoreholeMarkers()
{
    // 已在createGeologicalVolume中创建
}

void Geological3DWidget::createGeologicalColumns()
{
    // 此函数已被createGeologicalVolume替代
}

void Geological3DWidget::createLayerVolume(const BoreholeData &bh1, const BoreholeLayerData &layer1,
                                          const BoreholeData &bh2, const BoreholeLayerData &layer2,
                                          Qt3DCore::QEntity *parent)
{
    // 此函数已被createLayerPrism替代
    Q_UNUSED(bh1);
    Q_UNUSED(layer1);
    Q_UNUSED(bh2);
    Q_UNUSED(layer2);
    Q_UNUSED(parent);
}

void Geological3DWidget::createLayerSurface(const BoreholeData &bh1, const BoreholeLayerData &layer1,
                                           const BoreholeData &bh2, const BoreholeLayerData &layer2,
                                           Qt3DCore::QEntity *parent)
{
    // 此函数已被createLayerPrism替代
    Q_UNUSED(bh1);
    Q_UNUSED(layer1);
    Q_UNUSED(bh2);
    Q_UNUSED(layer2);
    Q_UNUSED(parent);
}

void Geological3DWidget::createGroundSurface()
{
    if (!rootEntity) {
        qWarning() << "rootEntity为空";
        return;
    }
    
    if (boreholes.isEmpty() && tunnelSegments.isEmpty()) {
        qWarning() << "没有数据";
        return;
    }
    
    surfaceEntity = new Qt3DCore::QEntity(rootEntity);
    
    // 计算地表范围
    float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
    
    for (const auto &bh : boreholes) {
        minX = qMin(minX, static_cast<float>(bh.x));
        maxX = qMax(maxX, static_cast<float>(bh.x));
        minY = qMin(minY, static_cast<float>(bh.y));
        maxY = qMax(maxY, static_cast<float>(bh.y));
        maxZ = qMax(maxZ, static_cast<float>(bh.surfaceElevation));
    }
    
    // 扩大范围
    float extendX = (maxX - minX) * 0.15f;
    float extendY = (maxY - minY) * 0.15f;
    minX -= extendX;
    maxX += extendX;
    minY -= extendY;
    maxY += extendY;
    
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float width = maxX - minX;
    float height = maxY - minY;
    float surfaceZ = maxZ + 1.0f;
    
    // 【修改】创建地表平面 - 调整网格分辨率和尺寸，防止出现绿面异常
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
    planeMesh->setWidth(width);
    planeMesh->setHeight(height);
    planeMesh->setMeshResolution(QSize(10, 10));  // 【修改】降低网格密度，从20x20改为10x10
    
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    transform->setTranslation(QVector3D(centerX, centerY, surfaceZ));
    // 【修改】确保平面正确朝向（法线向上）
    transform->setRotationX(0);  // 确保没有旋转
    
    // 【修改】使用完全不透明的材质，避免透明度导致的渲染异常
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(144, 238, 144));  // 【修改】浅绿色，完全不透明（去除alpha）
    material->setAmbient(QColor(107, 142, 35));   // 【修改】完全不透明
    material->setSpecular(QColor(200, 255, 200)); // 【修改】完全不透明，增加亮度
    material->setShininess(50.0f);  // 增加光泽度
    
    surfaceEntity->addComponent(planeMesh);
    surfaceEntity->addComponent(transform);
    surfaceEntity->addComponent(material);
    
    qDebug() << "✓ 创建了地表平面";
}

void Geological3DWidget::createCoordinateAxes()
{
    if (!rootEntity) {
        qWarning() << "rootEntity为空";
        return;
    }
    
    axesEntity = new Qt3DCore::QEntity(rootEntity);
    
    float axisLength = 50.0f;
    float axisRadius = 0.5f;
    
    // X轴 (红色)
    Qt3DCore::QEntity *xAxis = new Qt3DCore::QEntity(axesEntity);
    Qt3DExtras::QCylinderMesh *xCylinder = new Qt3DExtras::QCylinderMesh();
    xCylinder->setRadius(axisRadius);
    xCylinder->setLength(axisLength);
    
    Qt3DCore::QTransform *xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation(QVector3D(axisLength/2, 0, 0));
    xTransform->setRotationZ(90);
    
    Qt3DExtras::QPhongMaterial *xMaterial = new Qt3DExtras::QPhongMaterial();
    xMaterial->setDiffuse(QColor(255, 0, 0));
    xMaterial->setSpecular(QColor(255, 200, 200));
    xMaterial->setShininess(50.0f);
    
    xAxis->addComponent(xCylinder);
    xAxis->addComponent(xTransform);
    xAxis->addComponent(xMaterial);
    
    // Y轴 (绿色)
    Qt3DCore::QEntity *yAxis = new Qt3DCore::QEntity(axesEntity);
    Qt3DExtras::QCylinderMesh *yCylinder = new Qt3DExtras::QCylinderMesh();
    yCylinder->setRadius(axisRadius);
    yCylinder->setLength(axisLength);
    
    Qt3DCore::QTransform *yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation(QVector3D(0, axisLength/2, 0));
    yTransform->setRotationX(90);
    
    Qt3DExtras::QPhongMaterial *yMaterial = new Qt3DExtras::QPhongMaterial();
    yMaterial->setDiffuse(QColor(0, 255, 0));
    yMaterial->setSpecular(QColor(200, 255, 200));
    yMaterial->setShininess(50.0f);
    
    yAxis->addComponent(yCylinder);
    yAxis->addComponent(yTransform);
    yAxis->addComponent(yMaterial);
    
    // Z轴 (蓝色) - 向上
    Qt3DCore::QEntity *zAxis = new Qt3DCore::QEntity(axesEntity);
    Qt3DExtras::QCylinderMesh *zCylinder = new Qt3DExtras::QCylinderMesh();
    zCylinder->setRadius(axisRadius);
    zCylinder->setLength(axisLength);
    
    Qt3DCore::QTransform *zTransform = new Qt3DCore::QTransform();
    zTransform->setTranslation(QVector3D(0, 0, axisLength/2));
    
    Qt3DExtras::QPhongMaterial *zMaterial = new Qt3DExtras::QPhongMaterial();
    zMaterial->setDiffuse(QColor(0, 0, 255));
    zMaterial->setSpecular(QColor(200, 200, 255));
    zMaterial->setShininess(50.0f);
    
    zAxis->addComponent(zCylinder);
    zAxis->addComponent(zTransform);
    zAxis->addComponent(zMaterial);
    
    qDebug() << "✓ 创建了坐标轴";
}

void Geological3DWidget::setup3DView()
{
    if (!view3D) {
        qWarning() << "view3D为空";
        return;
    }
    
    if (!camera) {
        camera = view3D->camera();
        if (!camera) {
            qWarning() << "无法获取相机";
            return;
        }
    }
    
    // 计算场景中心和范围
    QVector3D center(0, 0, 0);
    float maxDistance = 200.0f;
    
    if (!boreholes.isEmpty()) {
        float minX = FLT_MAX, maxX = -FLT_MAX;
        float minY = FLT_MAX, maxY = -FLT_MAX;
        float minZ = FLT_MAX, maxZ = -FLT_MAX;
        
        for (const auto &bh : boreholes) {
            minX = qMin(minX, static_cast<float>(bh.x));
            maxX = qMax(maxX, static_cast<float>(bh.x));
            minY = qMin(minY, static_cast<float>(bh.y));
            maxY = qMax(maxY, static_cast<float>(bh.y));
            maxZ = qMax(maxZ, static_cast<float>(bh.surfaceElevation));
            
            for (const auto &layer : bh.layers) {
                float bottomZ = bh.surfaceElevation - layer.bottomDepth;
                minZ = qMin(minZ, bottomZ);
            }
        }
        
        sceneMin = QVector3D(minX, minY, minZ);
        sceneMax = QVector3D(maxX, maxY, maxZ);
        center = (sceneMin + sceneMax) / 2.0f;
        
        QVector3D diagonal = sceneMax - sceneMin;
        maxDistance = diagonal.length();
        
        qDebug() << "场景边界: Min" << sceneMin << "Max" << sceneMax;
        qDebug() << "场景中心:" << center;
        qDebug() << "场景对角线:" << maxDistance;
    }
    
    // 保存场景参数
    sceneCenter = center;
    sceneRadius = maxDistance / 2.0f;
    
    // 计算合适的相机距离范围
    minCameraDistance = sceneRadius * 0.5f;  // 最近可以到场景半径的一半
    maxCameraDistance = sceneRadius * 4.0f;  // 最远可以到场景半径的4倍
    
    // 设置初始相机位置 - 从东南方斜上方观看
    float initialDistance = sceneRadius * 2.0f;  // 初始距离为场景半径的2倍
    
    QVector3D cameraPosition = center + QVector3D(
        initialDistance * 0.6f,   // 东
        -initialDistance * 0.6f,  // 南
        initialDistance * 0.5f    // 上
    );
    
    // 配置相机透视
    camera->lens()->setPerspectiveProjection(
        45.0f,                      // 视场角（从50降到45，减少畸变）
        16.0f / 9.0f,              // 宽高比
        sceneRadius * 0.01f,       // 近裁剪面（动态计算）
        maxCameraDistance * 3.0f   // 远裁剪面（确保能看到最远处）
    );
    
    camera->setPosition(cameraPosition);
    camera->setViewCenter(center);
    camera->setUpVector(QVector3D(0, 0, 1));  // Z轴向上
    
    // 创建或更新相机控制器
    if (!cameraController) {
        cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
        cameraController->setCamera(camera);
    }
    
    // 设置相机控制参数 - 关键改进点
    // 1. 降低旋转速度，让操作更平滑
    cameraController->setLookSpeed(120.0f);  // 降低旋转速度（之前是180）
    
    // 2. 设置合适的线性移动速度
    cameraController->setLinearSpeed(sceneRadius * 0.8f);  // 降低移动速度
    
    // 3. 设置缩放速度（通过鼠标滚轮）
    cameraController->setZoomInLimit(minCameraDistance);  // 设置最近距离限制
    
    qDebug() << "✓ 相机设置完成";
    qDebug() << "  - 相机位置:" << cameraPosition;
    qDebug() << "  - 视角中心:" << center;
    qDebug() << "  - 场景半径:" << sceneRadius;
    qDebug() << "  - 相机距离范围:" << minCameraDistance << "至" << maxCameraDistance;
    qDebug() << "  - 线性速度:" << cameraController->linearSpeed();
    qDebug() << "  - 旋转速度:" << cameraController->lookSpeed();
}

