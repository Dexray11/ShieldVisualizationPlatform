#include "geological2dwidget.h"
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
    
    // 初始化地层颜色
    layerColors["①"] = QColor(139, 69, 19);      // 素填土 - 棕色
    layerColors["⑯9"] = QColor(205, 133, 63);    // 强风化安山岩 - 浅棕
    layerColors["⑰9"] = QColor(160, 160, 160);   // 中风化安山岩 - 灰色
    layerColors["⑱9"] = QColor(128, 128, 128);   // 微风化安山岩 - 深灰
    layerColors["⑱93"] = QColor(105, 105, 105);  // 微风化安山岩(碎裂状) - 更深灰
    layerColors["②4"] = QColor(210, 180, 140);   // 粉质黏土 - 棕褐
    layerColors["③8"] = QColor(244, 164, 96);    // 残积砂质黏性土 - 橙棕
    
    // 默认颜色
    layerColors["default"] = QColor(200, 200, 200);
}

Geological2DWidget::~Geological2DWidget()
{
}

void Geological2DWidget::loadBoreholeData(const QString &excelPath)
{
    // 由于Qt没有内置Excel读取，这里使用硬编码数据
    // 实际项目中应使用QXlsx或其他库
    
    boreholes.clear();
    
    // 从Excel数据中提取的钻孔信息
    // 钻孔1: M8ZZ3-TQD-2
    Borehole bh1;
    bh1.id = 1;
    bh1.code = "M8ZZ3-TQD-2";
    bh1.x = 124559.16;
    bh1.y = 214931.465;
    bh1.surfaceElevation = 22.53;
    bh1.mileage = 2540.4;
    
    BoreholeLayer l1;
    l1.layerCode = "①"; l1.rockName = "素填土";
    l1.topElevation = 22.53; l1.bottomElevation = 21.53; l1.thickness = 1.0;
    bh1.layers.append(l1);
    
    BoreholeLayer l2;
    l2.layerCode = "⑯9"; l2.rockName = "强风化安山岩";
    l2.topElevation = 21.53; l2.bottomElevation = 17.93; l2.thickness = 3.6;
    bh1.layers.append(l2);
    
    BoreholeLayer l3;
    l3.layerCode = "⑰9"; l3.rockName = "中风化安山岩";
    l3.topElevation = 17.93; l3.bottomElevation = 16.53; l3.thickness = 1.4;
    bh1.layers.append(l3);
    
    BoreholeLayer l4;
    l4.layerCode = "⑱93"; l4.rockName = "微风化安山岩(碎裂状)";
    l4.topElevation = 16.53; l4.bottomElevation = 8.53; l4.thickness = 8.0;
    bh1.layers.append(l4);
    
    boreholes.append(bh1);
    
    // 钻孔2: M8ZZ3-TQD-3
    Borehole bh2;
    bh2.id = 2;
    bh2.code = "M8ZZ3-TQD-3";
    bh2.x = 124541.523;
    bh2.y = 214885.046;
    bh2.surfaceElevation = 24.05;
    bh2.mileage = 2590.1;
    
    l1.layerCode = "①"; l1.rockName = "素填土";
    l1.topElevation = 24.05; l1.bottomElevation = 22.45; l1.thickness = 1.6;
    bh2.layers.append(l1);
    
    l2.layerCode = "⑯9"; l2.rockName = "强风化安山岩";
    l2.topElevation = 22.45; l2.bottomElevation = 21.45; l2.thickness = 1.0;
    bh2.layers.append(l2);
    
    l3.layerCode = "⑰9"; l3.rockName = "中风化安山岩";
    l3.topElevation = 21.45; l3.bottomElevation = 20.05; l3.thickness = 1.4;
    bh2.layers.append(l3);
    
    l4.layerCode = "⑱9"; l4.rockName = "微风化安山岩";
    l4.topElevation = 20.05; l4.bottomElevation = 7.05; l4.thickness = 13.0;
    bh2.layers.append(l4);
    
    boreholes.append(bh2);
    
    // 钻孔3
    Borehole bh3;
    bh3.id = 3;
    bh3.code = "M8ZZ3-TQD-4";
    bh3.x = 124520.0;
    bh3.y = 214840.0;
    bh3.surfaceElevation = 23.20;
    bh3.mileage = 2640.0;
    
    l1.topElevation = 23.20; l1.bottomElevation = 21.80; l1.thickness = 1.4;
    bh3.layers.append(l1);
    
    l2.topElevation = 21.80; l2.bottomElevation = 19.50; l2.thickness = 2.3;
    bh3.layers.append(l2);
    
    l3.topElevation = 19.50; l3.bottomElevation = 17.80; l3.thickness = 1.7;
    bh3.layers.append(l3);
    
    l4.layerCode = "⑱9"; l4.rockName = "微风化安山岩";
    l4.topElevation = 17.80; l4.bottomElevation = 6.20; l4.thickness = 11.6;
    bh3.layers.append(l4);
    
    boreholes.append(bh3);
    
    // 钻孔4
    Borehole bh4;
    bh4.id = 4;
    bh4.code = "M8ZZ3-TQD-5";
    bh4.x = 124500.0;
    bh4.y = 214795.0;
    bh4.surfaceElevation = 22.80;
    bh4.mileage = 2690.0;
    
    l1.topElevation = 22.80; l1.bottomElevation = 21.60; l1.thickness = 1.2;
    bh4.layers.append(l1);
    
    l2.topElevation = 21.60; l2.bottomElevation = 18.90; l2.thickness = 2.7;
    bh4.layers.append(l2);
    
    l3.topElevation = 18.90; l3.bottomElevation = 16.50; l3.thickness = 2.4;
    bh4.layers.append(l3);
    
    l4.topElevation = 16.50; l4.bottomElevation = 5.80; l4.thickness = 10.7;
    bh4.layers.append(l4);
    
    boreholes.append(bh4);
    
    // 钻孔5
    Borehole bh5;
    bh5.id = 5;
    bh5.code = "M8ZZ3-TQD-6";
    bh5.x = 124480.0;
    bh5.y = 214750.0;
    bh5.surfaceElevation = 22.40;
    bh5.mileage = 2740.0;
    
    l1.topElevation = 22.40; l1.bottomElevation = 21.20; l1.thickness = 1.2;
    bh5.layers.append(l1);
    
    l2.topElevation = 21.20; l2.bottomElevation = 18.00; l2.thickness = 3.2;
    bh5.layers.append(l2);
    
    l3.topElevation = 18.00; l3.bottomElevation = 15.80; l3.thickness = 2.2;
    bh5.layers.append(l3);
    
    l4.topElevation = 15.80; l4.bottomElevation = 5.40; l4.thickness = 10.4;
    bh5.layers.append(l4);
    
    boreholes.append(bh5);
    
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
    update();
}

void Geological2DWidget::loadTunnelData(const QString &excelPath)
{
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

QColor Geological2DWidget::getLayerColor(const QString &layerCode)
{
    if (layerColors.contains(layerCode)) {
        return layerColors[layerCode];
    }
    return layerColors["default"];
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
    
    // 找出所有地层类型
    QStringList layerTypes;
    layerTypes << "①" << "⑯9" << "⑰9" << "⑱9" << "⑱93";
    
    // 为每个地层绘制连续的多边形
    for (const QString &layerCode : layerTypes) {
        QPolygonF polygon;
        bool hasLayer = false;
        
        // 从左到右收集每个钻孔的该地层顶底
        QVector<QPair<double, QPair<double, double>>> layerBounds; // mileage, (top, bottom)
        
        for (const auto &bh : sortedBH) {
            for (const auto &layer : bh.layers) {
                if (layer.layerCode == layerCode) {
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
        
        // 填充地层
        QColor fillColor = getLayerColor(layerCode);
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
    int legendX = width() - 180;
    int legendY = marginTop + 10;
    int boxSize = 15;
    int spacing = 20;
    
    // 图例背景
    painter.fillRect(legendX - 10, legendY - 5, 175, 130, QColor(255, 255, 255, 230));
    painter.setPen(QPen(QColor("#cccccc"), 1));
    painter.drawRect(legendX - 10, legendY - 5, 175, 130);
    
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.setPen(Qt::black);
    painter.drawText(legendX, legendY + 12, "图例");
    
    legendY += spacing;
    
    painter.setFont(QFont("Microsoft YaHei", 8));
    
    // 地层图例
    QMap<QString, QString> layerNames;
    layerNames["①"] = "素填土";
    layerNames["⑯9"] = "强风化安山岩";
    layerNames["⑰9"] = "中风化安山岩";
    layerNames["⑱9"] = "微风化安山岩";
    layerNames["⑱93"] = "微风化安山岩(碎裂)";
    
    QStringList order = {"①", "⑯9", "⑰9", "⑱9"};
    
    for (const QString &code : order) {
        QColor color = getLayerColor(code);
        color.setAlpha(180);
        painter.fillRect(legendX, legendY, boxSize, boxSize, color);
        painter.setPen(Qt::black);
        painter.drawRect(legendX, legendY, boxSize, boxSize);
        painter.drawText(legendX + boxSize + 5, legendY + 12, layerNames[code]);
        legendY += spacing;
    }
    
    // 隧道图例
    painter.setPen(QPen(QColor("#1565C0"), 2));
    painter.drawRect(legendX, legendY, boxSize, boxSize);
    painter.drawText(legendX + boxSize + 5, legendY + 12, "隧道断面");
}
