#ifndef GEOLOGICAL2DWIDGET_H
#define GEOLOGICAL2DWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>
#include <QMap>

struct BoreholeLayer {
    QString layerCode;
    QString rockName;
    double topElevation;
    double bottomElevation;
    double thickness;
    QString description;
};

struct Borehole {
    int id;
    QString code;
    double x;
    double y;
    double surfaceElevation;
    double mileage;
    QVector<BoreholeLayer> layers;
};

struct TunnelSection {
    int id;
    QString nearBorehole;
    double leftTopX, leftTopY, leftTopZ;
    double leftBottomX, leftBottomY, leftBottomZ;
    double rightTopX, rightTopY, rightTopZ;
    double rightBottomX, rightBottomY, rightBottomZ;
};

class Geological2DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Geological2DWidget(QWidget *parent = nullptr);
    ~Geological2DWidget();

    void loadBoreholeData(const QString &excelPath);
    void loadTunnelData(const QString &excelPath);
    void setShieldPosition(double mileage);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawBackground(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawBoreholes(QPainter &painter);
    void drawGeologicalLayers(QPainter &painter);
    void drawTunnel(QPainter &painter);
    void drawShieldMachine(QPainter &painter);
    void drawLegend(QPainter &painter);
    void drawScale(QPainter &painter);
    
    QColor getLayerColor(const QString &rockName);
    QString getLayerPattern(const QString &layerCode);
    
    double worldToScreenX(double worldX);
    double worldToScreenY(double worldY);
    
    QVector<Borehole> boreholes;
    QVector<TunnelSection> tunnelSections;
    
    double minX, maxX, minY, maxY;
    double minElevation, maxElevation;
    double minMileage, maxMileage;
    
    double shieldMileage;
    double shieldTopZ;
    double shieldBottomZ;
    
    int marginLeft, marginRight, marginTop, marginBottom;
    
    QMap<QString, QColor> layerColors;
    
    bool dataLoaded;
};

#endif // GEOLOGICAL2DWIDGET_H
