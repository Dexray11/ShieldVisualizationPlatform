#include "geological2dwidget.h"
#include "../database/BoreholeDAO.h"
#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QDebug>
#include <cmath>
#include <algorithm>

Geological2DWidget::Geological2DWidget(QWidget *parent)
    : QWidget(parent)
    , minX(0), maxX(0), minY(0), maxY(0)
    , minElevation(0), maxElevation(0)
    , minMileage(0), maxMileage(0)
    , shieldMileage(0)
    , shieldTopZ(0), shieldBottomZ(0)
    , marginLeft(80), marginRight(40), marginTop(40), marginBottom(60)
    , dataLoaded(false)
{
    setMinimumSize(800, 400);
    setStyleSheet("background-color: white;");
    
    // 初始化地层颜色 - 使用岩石名称作为key（更稳定可靠）
    layerColors[QString::fromUtf8("素填土")] = QColor(139, 69, 19);           // 深棕色
    layerColors[QString::fromUtf8("强风化安山岩")] = QColor(220, 20, 60);    // 深红色
    layerColors[QString::fromUtf8("中风化安山岩")] = QColor(70, 130, 180);   // 钢蓝色
    layerColors[QString::fromUtf8("微风化安山岩")] = QColor(47, 79, 79);     // 深青灰
    layerColors[QString::fromUtf8("微风化安山岩(碎裂状)")] = QColor(25, 25, 25);  // 深黑灰
    layerColors[QString::fromUtf8("粉质黏土")] = QColor(210, 180, 140);      // 棕褐色
    layerColors[QString::fromUtf8("残积砂质黏性土")] = QColor(244, 164, 96);  // 橙棕色
    
    // 默认颜色
    layerColors[QString::fromUtf8("default")] = QColor(200, 200, 200);
}

Geological2DWidget::~Geological2DWidget()
{
}

void Geological2DWidget::loadBoreholeData(const QString &excelPath)
{
    Q_UNUSED(excelPath);  // 不再从Excel读取
    
    boreholes.clear();
    
    // 从数据库读取钻孔数据
    // 假设当前项目ID为1（实际应用中应从项目上下文获取）
    int projectId = 1;
    
    BoreholeDAO dao;
    QVector<BoreholeData> dbBoreholes = dao.getBoreholesByProjectId(projectId);
    
    if (dbBoreholes.isEmpty()) {
        qWarning() << "未找到项目" << projectId << "的钻孔数据";
        dataLoaded = false;
        update();
        return;
    }
    
    qDebug() << "从数据库加载了" << dbBoreholes.size() << "个钻孔";
    
    // 将数据库数据转换为界面使用的数据结构
    for (const auto &dbBh : dbBoreholes) {
        Borehole bh;
        bh.id = dbBh.boreholeId;
        bh.code = dbBh.boreholeCode;
        bh.x = dbBh.x;
        bh.y = dbBh.y;
        bh.surfaceElevation = dbBh.surfaceElevation;
        bh.mileage = dbBh.mileage;
        
        // 转换地层数据
        for (const auto &dbLayer : dbBh.layers) {
            BoreholeLayer layer;
            layer.layerCode = dbLayer.layerCode;
            layer.rockName = dbLayer.rockName;
            layer.bottomElevation = dbLayer.bottomElevation;
            layer.thickness = dbLayer.thickness;
            
            // 计算顶部标高（底部标高 + 厚度）
            layer.topElevation = dbLayer.bottomElevation + dbLayer.thickness;
            
            bh.layers.append(layer);
        }
        
        boreholes.append(bh);
    }
    
    // 计算范围
    if (!boreholes.isEmpty()) {
        minMileage = maxMileage = boreholes[0].mileage;
        minElevation = maxElevation = boreholes[0].surfaceElevation;
        
        for (const auto &bh : boreholes) {
            minMileage = std::min(minMileage, bh.mileage);
            maxMileage = std::max(maxMileage, bh.mileage);
            maxElevation = std::max(maxElevation, bh.surfaceElevation);
            
            for (const auto &layer : bh.layers) {
                minElevation = std::min(minElevation, layer.bottomElevation);
            }
        }
        
        // 添加一些边距
        double mileageRange = maxMileage - minMileage;
        minMileage -= mileageRange * 0.1;
        maxMileage += mileageRange * 0.1;
        
        double elevRange = maxElevation - minElevation;
        minElevation -= elevRange * 0.1;
        maxElevation += elevRange * 0.1;
    }
    
    // 设置默认盾构机位置
    shieldMileage = (minMileage + maxMileage) / 2.0;
    shieldTopZ = 17.0;
    shieldBottomZ = 11.0;
    
    dataLoaded = true;
    qDebug() << "钻孔数据加载完成，里程范围:" << minMileage << "~" << maxMileage;
    qDebug() << "高程范围:" << minElevation << "~" << maxElevation;
    update();
}

void Geological2DWidget::loadTunnelData(const QString &excelPath)
{
    Q_UNUSED(excelPath);
    // 隧道数据已在钻孔数据中考虑
    // 这里可以加载更详细的隧道线形数据
}

void Geological2DWidget::setShieldPosition(double mileage)
{
    shieldMileage = mileage;
    update();
}

double Geological2DWidget::worldToScreenX(double mileage)
{
    double drawWidth = width() - marginLeft - marginRight;
    return marginLeft + (mileage - minMileage) / (maxMileage - minMileage) * drawWidth;
}

double Geological2DWidget::worldToScreenY(double elevation)
{
    double drawHeight = height() - marginTop - marginBottom;
    // Y轴反转（屏幕坐标向下增加）
    return marginTop + (maxElevation - elevation) / (maxElevation - minElevation) * drawHeight;
}

QColor Geological2DWidget::getLayerColor(const QString &rockName)
{
    if (layerColors.contains(rockName)) {
        return layerColors[rockName];
    }
    return layerColors[QString::fromUtf8("default")];
}

void Geological2DWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBackground(painter);
    
    if (!dataLoaded || boreholes.isEmpty()) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Microsoft YaHei", 14));
        painter.drawText(rect(), Qt::AlignCenter, "请加载地质数据");
        return;
    }
    
    drawGrid(painter);
    drawGeologicalLayers(painter);
    drawTunnel(painter);
    drawBoreholes(painter);
    drawShieldMachine(painter);
    drawScale(painter);
    drawLegend(painter);
}

void Geological2DWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void Geological2DWidget::drawBackground(QPainter &painter)
{
    painter.fillRect(rect(), Qt::white);
    
    // 绘制边框
    painter.setPen(QPen(QColor("#cccccc"), 1));
    painter.drawRect(marginLeft, marginTop, 
                     width() - marginLeft - marginRight, 
                     height() - marginTop - marginBottom);
}

void Geological2DWidget::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor("#e0e0e0"), 1, Qt::DotLine));
    
    // 水平网格线（高程）
    int numHLines = 6;
    double elevStep = (maxElevation - minElevation) / numHLines;
    for (int i = 0; i <= numHLines; i++) {
        double elev = minElevation + i * elevStep;
        double y = worldToScreenY(elev);
        painter.drawLine(marginLeft, y, width() - marginRight, y);
        
        // 标注高程
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9));
        painter.drawText(5, y + 4, QString::number(elev, 'f', 1) + "m");
        painter.setPen(QPen(QColor("#e0e0e0"), 1, Qt::DotLine));
    }
    
    // 垂直网格线（里程）
    int numVLines = 8;
    double mileageStep = (maxMileage - minMileage) / numVLines;
    for (int i = 0; i <= numVLines; i++) {
        double mileage = minMileage + i * mileageStep;
        double x = worldToScreenX(mileage);
        painter.drawLine(x, marginTop, x, height() - marginBottom);
        
        // 标注里程
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9));
        QString mileageStr = QString("K%1+%2")
                                 .arg(int(mileage / 1000))
                                 .arg(int(mileage) % 1000, 3, 10, QChar('0'));
        painter.drawText(x - 30, height() - marginBottom + 15, mileageStr);
        painter.setPen(QPen(QColor("#e0e0e0"), 1, Qt::DotLine));
    }
}

void Geological2DWidget::drawGeologicalLayers(QPainter &painter)
{
    if (boreholes.size() < 2) return;
    
    // 按里程排序钻孔
    QVector<Borehole> sortedBH = boreholes;
    std::sort(sortedBH.begin(), sortedBH.end(), 
              [](const Borehole &a, const Borehole &b) { return a.mileage < b.mileage; });
    
    // 找出所有地层类型（使用rockName）
    QStringList layerTypes;
    layerTypes << QString::fromUtf8("素填土") 
               << QString::fromUtf8("强风化安山岩") 
               << QString::fromUtf8("中风化安山岩") 
               << QString::fromUtf8("微风化安山岩")
               << QString::fromUtf8("微风化安山岩(碎裂状)");
    
    // 为每个地层绘制连续的多边形
    for (const QString &rockName : layerTypes) {
        QPolygonF polygon;
        bool hasLayer = false;
        
        // 从左到右收集每个钻孔的该地层顶底
        QVector<QPair<double, QPair<double, double>>> layerBounds; // mileage, (top, bottom)
        
        for (const auto &bh : sortedBH) {
            for (const auto &layer : bh.layers) {
                if (layer.rockName == rockName) {  // 改用rockName匹配
                    layerBounds.append(qMakePair(bh.mileage, 
                                                  qMakePair(layer.topElevation, layer.bottomElevation)));
                    hasLayer = true;
                    break;
                }
            }
        }
        
        if (!hasLayer || layerBounds.size() < 2) continue;
        
        // 构建多边形
        // 上边界（从左到右）
        for (int i = 0; i < layerBounds.size(); i++) {
            double x = worldToScreenX(layerBounds[i].first);
            double y = worldToScreenY(layerBounds[i].second.first);
            polygon << QPointF(x, y);
        }
        
        // 下边界（从右到左）
        for (int i = layerBounds.size() - 1; i >= 0; i--) {
            double x = worldToScreenX(layerBounds[i].first);
            double y = worldToScreenY(layerBounds[i].second.second);
            polygon << QPointF(x, y);
        }
        
        // 填充地层（使用rockName获取颜色）
        QColor fillColor = getLayerColor(rockName);
        fillColor.setAlpha(180);
        painter.setBrush(QBrush(fillColor));
        painter.setPen(QPen(fillColor.darker(120), 1));
        painter.drawPolygon(polygon);
    }
}

void Geological2DWidget::drawBoreholes(QPainter &painter)
{
    painter.setFont(QFont("Microsoft YaHei", 8));
    
    for (const auto &bh : boreholes) {
        double x = worldToScreenX(bh.mileage);
        double yTop = worldToScreenY(bh.surfaceElevation);
        double yBottom = worldToScreenY(bh.layers.last().bottomElevation);
        
        // 绘制钻孔柱
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(x, yTop, x, yBottom);
        
        // 绘制钻孔编号
        painter.setPen(Qt::black);
        painter.save();
        painter.translate(x, yTop - 5);
        painter.rotate(-45);
        painter.drawText(0, 0, bh.code);
        painter.restore();
        
        // 绘制地层分界线
        painter.setPen(QPen(Qt::black, 1));
        for (const auto &layer : bh.layers) {
            double yLayer = worldToScreenY(layer.bottomElevation);
            painter.drawLine(x - 8, yLayer, x + 8, yLayer);
        }
    }
}

void Geological2DWidget::drawTunnel(QPainter &painter)
{
    if (boreholes.size() < 2) return;
    
    // 绘制隧道轮廓（简化为矩形）
    double tunnelTop = 17.0;
    double tunnelBottom = 11.0;
    
    double x1 = worldToScreenX(minMileage + (maxMileage - minMileage) * 0.05);
    double x2 = worldToScreenX(maxMileage - (maxMileage - minMileage) * 0.05);
    double y1 = worldToScreenY(tunnelTop);
    double y2 = worldToScreenY(tunnelBottom);
    
    // 隧道外轮廓
    painter.setPen(QPen(QColor("#1565C0"), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(x1, y1, x2 - x1, y2 - y1);
    
    // 隧道内部填充（透明蓝色）
    QColor tunnelColor(33, 150, 243, 50);
    painter.fillRect(x1, y1, x2 - x1, y2 - y1, tunnelColor);
    
    // 标注"隧道"
    painter.setPen(QColor("#1565C0"));
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    painter.drawText((x1 + x2) / 2 - 20, (y1 + y2) / 2, "隧道");
}

void Geological2DWidget::drawShieldMachine(QPainter &painter)
{
    double x = worldToScreenX(shieldMileage);
    double yTop = worldToScreenY(shieldTopZ);
    double yBottom = worldToScreenY(shieldBottomZ);
    double shieldWidth = 40;
    
    // 盾构机主体
    QRectF shieldRect(x - shieldWidth / 2, yTop, shieldWidth, yBottom - yTop);
    
    // 绘制盾构机（简化为矩形）
    QLinearGradient gradient(shieldRect.topLeft(), shieldRect.bottomLeft());
    gradient.setColorAt(0, QColor("#FF5722"));
    gradient.setColorAt(1, QColor("#E64A19"));
    
    painter.setBrush(gradient);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRoundedRect(shieldRect, 5, 5);
    
    // 刀盘（前端圆形）
    double cutterRadius = (yBottom - yTop) / 2;
    painter.setBrush(QColor("#795548"));
    painter.drawEllipse(QPointF(x + shieldWidth / 2, (yTop + yBottom) / 2), 
                        cutterRadius * 0.8, cutterRadius);
    
    // 刀盘细节
    painter.setPen(QPen(Qt::white, 2));
    double cx = x + shieldWidth / 2;
    double cy = (yTop + yBottom) / 2;
    for (int i = 0; i < 6; i++) {
        double angle = i * 60 * M_PI / 180;
        painter.drawLine(cx, cy, 
                        cx + cos(angle) * cutterRadius * 0.7,
                        cy + sin(angle) * cutterRadius * 0.9);
    }
    
    // 盾构机标签
    painter.setPen(Qt::black);
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.drawText(x - 25, yTop - 10, "盾构机");
    
    // 当前位置标记
    painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
    painter.drawLine(x, marginTop, x, height() - marginBottom);
    
    QString posStr = QString("当前位置: K%1+%2")
                         .arg(int(shieldMileage / 1000))
                         .arg(int(shieldMileage) % 1000, 3, 10, QChar('0'));
    painter.setPen(Qt::red);
    painter.drawText(x - 50, marginTop - 5, posStr);
}

void Geological2DWidget::drawScale(QPainter &painter)
{
    painter.setPen(Qt::black);
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    
    // 标题
    painter.drawText(width() / 2 - 80, 25, "二维地质剖面图");
    
    // 比例尺
    double scaleBarWidth = 100;
    double x = width() - marginRight - scaleBarWidth - 10;
    double y = height() - 15;
    
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(x, y, x + scaleBarWidth, y);
    painter.drawLine(x, y - 5, x, y + 5);
    painter.drawLine(x + scaleBarWidth, y - 5, x + scaleBarWidth, y + 5);
    
    // 计算实际距离
    double pixelsPerMeter = (width() - marginLeft - marginRight) / (maxMileage - minMileage);
    double realDistance = scaleBarWidth / pixelsPerMeter;
    
    painter.setFont(QFont("Arial", 8));
    painter.drawText(x + scaleBarWidth / 2 - 20, y - 8, 
                    QString("%1 m").arg(realDistance, 0, 'f', 1));
}

void Geological2DWidget::drawLegend(QPainter &painter)
{
    // 图例尺寸参数优化
    int boxSize = 16;      // 缩小颜色方块 20→16
    int spacing = 22;      // 减小间距 28→22
    int padding = 10;      // 内边距
    int titleHeight = 25;  // 标题高度
    
    // 计算图例框尺寸
    int itemCount = 5;  // 4个地层 + 1个隧道
    int legendWidth = 160;  // 缩小宽度 195→160
    int legendHeight = titleHeight + itemCount * spacing + padding;
    
    // 图例位置 - 改到右下角，避免遮挡主要内容
    int legendX = width() - legendWidth - 20;
    int legendY = height() - legendHeight - marginBottom - 10;
    
    // 保存painter状态
    painter.save();
    
    // 图例背景 - 白色填充
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawRect(legendX, legendY, legendWidth, legendHeight);
    
    // 绘制边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(100, 100, 100), 2));
    painter.drawRect(legendX, legendY, legendWidth, legendHeight);
    
    // 标题
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    painter.setPen(Qt::black);
    painter.drawText(legendX + padding, legendY + 18, QString::fromUtf8("图例"));
    
    // 内容起始位置
    int contentY = legendY + titleHeight + 5;
    int contentX = legendX + padding;
    
    // 内容字体
    painter.setFont(QFont("Microsoft YaHei", 9));
    
    // 地层图例
    struct LegendItem {
        QString name;
        QColor color;
    };
    
    QVector<LegendItem> legendItems;
    legendItems.append({QString::fromUtf8("素填土"), QColor(139, 69, 19)});
    legendItems.append({QString::fromUtf8("强风化安山岩"), QColor(220, 20, 60)});
    legendItems.append({QString::fromUtf8("中风化安山岩"), QColor(70, 130, 180)});
    legendItems.append({QString::fromUtf8("微风化安山岩"), QColor(47, 79, 79)});
    
    for (const auto &item : legendItems) {
        // 绘制颜色方块
        painter.setBrush(item.color);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(contentX, contentY, boxSize, boxSize);
        
        // 绘制图例文字
        painter.setPen(Qt::black);
        painter.drawText(contentX + boxSize + 6, contentY + 12, item.name);
        contentY += spacing;
    }
    
    // 隧道图例
    painter.setPen(QPen(QColor("#1565C0"), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(contentX, contentY, boxSize, boxSize);
    painter.setPen(Qt::black);
    painter.drawText(contentX + boxSize + 6, contentY + 12, QString::fromUtf8("隧道断面"));
    
    // 恢复painter状态
    painter.restore();
}


