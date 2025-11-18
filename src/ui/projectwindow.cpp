#include "projectwindow.h"
#include "mainmenuwindow.h"
#include "geological2dwidget.h"
#include "geological3dwidget.h"
#include "../utils/stylehelper.h"
#include "../database/BoreholeDAO.h"
#include "../database/ProjectDAO.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QIcon>
#include <QMap>
#include <algorithm>

ProjectWindow::ProjectWindow(const QString &projectName, QWidget *parent)
    : QMainWindow(parent)
    , projectName(projectName)
{
    // 设置为独立窗口,确保任务栏显示
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    
    setupUI();

    setWindowTitle(QString("项目详情 - %1").arg(projectName));
    setWindowIcon(QIcon(":/icons/app_icon.ico"));
    resize(1400, 900);
}

ProjectWindow::~ProjectWindow()
{
}

void ProjectWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    createTopBar();

    QWidget *contentWidget = new QWidget(centralWidget);
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    createSidebar();
    contentLayout->addWidget(sidebar);

    createMainContent();
    contentLayout->addWidget(mainContent, 1);

    mainLayout->addWidget(contentWidget);

    // 默认显示地图视图（工程俯视图）
    loadMapView();
}

void ProjectWindow::createTopBar()
{
    QWidget *topBar = new QWidget(centralWidget);
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);

    backButton = new QPushButton("← 工作台", topBar);
    backButton->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            font-size: 16px;
            padding: 10px 20px;
        }
        QPushButton:hover {
            background-color: %1;
            border-radius: 5px;
        }
    )").arg(StyleHelper::COLOR_SECONDARY));
    connect(backButton, &QPushButton::clicked, this, &ProjectWindow::onBackClicked);

    topLayout->addWidget(backButton);

    QPushButton *homeButton = new QPushButton(" 工作台", topBar);
    homeButton->setIcon(QIcon(":/icons/menu.png"));
    homeButton->setIconSize(QSize(20, 20));
    homeButton->setStyleSheet(QString(R"(
    QPushButton {
        background-color: transparent;
        color: white;
        border: none;
        font-size: 16px;
        padding: 10px 20px;
        margin-left: 10px;
    }
    QPushButton:hover {
        background-color: %1;
        border-radius: 5px;
    }
)").arg(StyleHelper::COLOR_SECONDARY));
    connect(homeButton, &QPushButton::clicked, [this]() {
        MainMenuWindow *mainMenu = new MainMenuWindow();
        mainMenu->show();
        this->close();
    });

    topLayout->addWidget(homeButton);
    // 添加工作台按钮 - 结束

    QLabel *titleLabel = new QLabel(QString("智能盾构地质可视化平台 - %1").arg(projectName), topBar);
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(centralWidget);
    }
    mainLayout->addWidget(topBar);
}

void ProjectWindow::createSidebar()
{
    sidebar = new QWidget(centralWidget);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    sidebarLayout->setSpacing(0);

    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            text-align: left;
            padding: 15px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: %1;
        }
        QPushButton:pressed {
            background-color: %2;
        }
    )").arg(StyleHelper::COLOR_SECONDARY).arg(StyleHelper::COLOR_ACCENT);

    mapViewButton = new QPushButton("工程俯视图", sidebar);
    mapViewButton->setStyleSheet(buttonStyle);
    connect(mapViewButton, &QPushButton::clicked, this, &ProjectWindow::onMapViewClicked);

    view2DButton = new QPushButton("二维视图", sidebar);
    view2DButton->setStyleSheet(buttonStyle);
    connect(view2DButton, &QPushButton::clicked, this, &ProjectWindow::on2DViewClicked);

    view3DButton = new QPushButton("三维视图", sidebar);
    view3DButton->setStyleSheet(buttonStyle);
    connect(view3DButton, &QPushButton::clicked, this, &ProjectWindow::on3DViewClicked);

    excavationButton = new QPushButton("掘进参数", sidebar);
    excavationButton->setStyleSheet(buttonStyle);
    connect(excavationButton, &QPushButton::clicked, this, &ProjectWindow::onExcavationParamsClicked);

    supplementaryButton = new QPushButton("补勘数据", sidebar);
    supplementaryButton->setStyleSheet(buttonStyle);
    connect(supplementaryButton, &QPushButton::clicked, this, &ProjectWindow::onSupplementaryDataClicked);

    positioningButton = new QPushButton("定位校准", sidebar);
    positioningButton->setStyleSheet(buttonStyle);
    connect(positioningButton, &QPushButton::clicked, this, &ProjectWindow::onPositioningClicked);

    sidebarLayout->addWidget(mapViewButton);
    sidebarLayout->addWidget(view2DButton);
    sidebarLayout->addWidget(view3DButton);
    sidebarLayout->addWidget(excavationButton);
    sidebarLayout->addWidget(supplementaryButton);
    sidebarLayout->addWidget(positioningButton);
    sidebarLayout->addStretch();
}

void ProjectWindow::createMainContent()
{
    mainContent = new QWidget(centralWidget);
    mainContent->setStyleSheet("background-color: #f5f5f5;");

    QVBoxLayout *layout = new QVBoxLayout(mainContent);
    layout->setContentsMargins(20, 20, 20, 20);
}

void ProjectWindow::clearMainContent()
{
    QLayout *layout = mainContent->layout();
    if (!layout) return;

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void ProjectWindow::onMapViewClicked()
{
    loadMapView();
}

void ProjectWindow::on2DViewClicked()
{
    load2DView();
}

void ProjectWindow::on3DViewClicked()
{
    load3DView();
}

void ProjectWindow::onExcavationParamsClicked()
{
    loadExcavationParams();
}

void ProjectWindow::onSupplementaryDataClicked()
{
    loadSupplementaryData();
}

void ProjectWindow::onPositioningClicked()
{
    showPositioningDialog();
}

void ProjectWindow::loadMapView()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    // 标题
    QLabel *titleLabel = new QLabel("工程俯视图", mainContent);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    // 定位输入区域
    QWidget *locationWidget = new QWidget(mainContent);
    QHBoxLayout *locationLayout = new QHBoxLayout(locationWidget);
    locationLayout->setSpacing(10);
    
    // 修复问题2：使用location.png图标，并统一高度为36px
    // 坐标输入
    QLabel *coordLabel = new QLabel(locationWidget);
    coordLabel->setPixmap(QPixmap(":/icons/location.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    coordLabel->setFixedSize(24, 36);  // 统一高度36px
    coordLabel->setAlignment(Qt::AlignCenter);
    
    coordsInput = new QLineEdit("120.36,36.23", locationWidget);
    coordsInput->setPlaceholderText("输入坐标");
    coordsInput->setFixedHeight(36);  // 修复问题2：统一高度36px
    coordsInput->setStyleSheet(StyleHelper::getInputStyle());
    coordsInput->setMaximumWidth(200);
    
    // 修复问题2：使用lock.png图标作为定位按钮图标
    QPushButton *coordLocateBtn = new QPushButton(locationWidget);
    coordLocateBtn->setIcon(QIcon(":/icons/lock.png"));
    coordLocateBtn->setIconSize(QSize(16, 16));
    coordLocateBtn->setFixedSize(36, 36);  // 修复问题2：统一高度36px
    coordLocateBtn->setStyleSheet(StyleHelper::getButtonStyle());
    coordLocateBtn->setToolTip("定位到坐标");
    
    // 桩号输入
    // 修复问题2：使用flag.png图标
    QLabel *stakeLabel = new QLabel(locationWidget);
    stakeLabel->setPixmap(QPixmap(":/icons/flag.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    stakeLabel->setFixedSize(24, 36);  // 统一高度36px
    stakeLabel->setAlignment(Qt::AlignCenter);
    stakeLabel->setStyleSheet("margin-left: 10px;");
    
    stakeInput = new QLineEdit("K1+190.00", locationWidget);
    stakeInput->setPlaceholderText("输入桩号");
    stakeInput->setFixedHeight(36);  // 修复问题2：统一高度36px
    stakeInput->setStyleSheet(StyleHelper::getInputStyle());
    stakeInput->setMaximumWidth(200);
    
    // 修复问题2：使用lock.png图标作为定位按钮图标
    QPushButton *stakeLocateBtn = new QPushButton(locationWidget);
    stakeLocateBtn->setIcon(QIcon(":/icons/lock.png"));
    stakeLocateBtn->setIconSize(QSize(16, 16));
    stakeLocateBtn->setFixedSize(36, 36);  // 修复问题2：统一高度36px
    stakeLocateBtn->setStyleSheet(StyleHelper::getButtonStyle());
    stakeLocateBtn->setToolTip("定位到桩号");
    
    locationLayout->addWidget(coordLabel);
    locationLayout->addWidget(coordsInput);
    locationLayout->addWidget(coordLocateBtn);
    locationLayout->addWidget(stakeLabel);
    locationLayout->addWidget(stakeInput);
    locationLayout->addWidget(stakeLocateBtn);
    locationLayout->addStretch();

    // 地图视图
    QLabel *mapLabel = new QLabel(mainContent);
    mapLabel->setMinimumSize(1000, 500);
    
    // 创建一个简单的地图占位图
    QPixmap mapPlaceholder(1000, 500);
    mapPlaceholder.fill(QColor("#e8f4f8"));
    QPainter painter(&mapPlaceholder);
    
    // 绘制网格背景
    painter.setPen(QPen(QColor("#d0e8f0"), 1));
    for (int i = 0; i < mapPlaceholder.width(); i += 50) {
        painter.drawLine(i, 0, i, mapPlaceholder.height());
    }
    for (int i = 0; i < mapPlaceholder.height(); i += 50) {
        painter.drawLine(0, i, mapPlaceholder.width(), i);
    }
    
    // 绘制隧道线
    painter.setPen(QPen(QColor(StyleHelper::COLOR_PRIMARY), 3));
    painter.setBrush(QBrush(QColor(StyleHelper::COLOR_PRIMARY)));
    
    // 绘制曲线隧道
    QPainterPath tunnelPath;
    tunnelPath.moveTo(100, 250);
    tunnelPath.cubicTo(300, 200, 600, 300, 900, 250);
    painter.drawPath(tunnelPath);
    
    // 绘制盾构机位置标记
    painter.setBrush(QBrush(QColor("#ff4444")));
    painter.setPen(QPen(QColor("#ff4444"), 2));
    painter.drawEllipse(QPoint(500, 250), 15, 15);
    
    // 绘制文字说明
    painter.setPen(QPen(QColor(StyleHelper::COLOR_TEXT_DARK)));
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(QRect(0, 20, mapPlaceholder.width(), 30), Qt::AlignCenter, 
                     projectName + " - 工程俯视图");
    
    painter.setFont(QFont("Arial", 12));
    painter.drawText(QRect(480, 280, 100, 20), Qt::AlignCenter, "当前位置");
    
    mapLabel->setPixmap(mapPlaceholder);
    mapLabel->setAlignment(Qt::AlignCenter);

    // 添加到布局
    layout->addWidget(titleLabel);
    layout->addWidget(locationWidget);
    layout->addWidget(mapLabel);
    layout->addStretch();
    
    QLabel *infoLabel = new QLabel("提示：输入坐标或桩号后点击定位按钮可将施工位置居中显示", mainContent);
    infoLabel->setStyleSheet("color: #666; font-size: 12px;");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
}

void ProjectWindow::load2DView()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    // 标题
    QLabel *titleLabel = new QLabel("二维视图", mainContent);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    // 创建二维地质视图widget
    Geological2DWidget *geo2DWidget = new Geological2DWidget(mainContent);
    geo2DWidget->setMinimumSize(1000, 450);
    geo2DWidget->loadBoreholeData("");  // 使用内置数据
    
    // 掌子面视图（简化显示）
    QWidget *faceViewWidget = new QWidget(mainContent);
    faceViewWidget->setFixedSize(200, 200);
    faceViewWidget->setStyleSheet("background-color: white; border: 2px solid #1565C0; border-radius: 100px;");
    
    QLabel *faceLabel = new QLabel("掌子面视图", faceViewWidget);
    faceLabel->setAlignment(Qt::AlignCenter);
    faceLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #1565C0;");
    faceLabel->setGeometry(50, 85, 100, 30);
    
    // 地层比例饼图
    QWidget *pieChartWidget = new QWidget(mainContent);
    pieChartWidget->setFixedSize(200, 150);
    pieChartWidget->setStyleSheet("background-color: white; border: 1px solid #cccccc; border-radius: 5px;");
    
    QLabel *pieTitle = new QLabel(QString::fromUtf8("地层占比"), pieChartWidget);
    pieTitle->setStyleSheet("font-size: 12px; font-weight: bold;");
    pieTitle->setGeometry(70, 10, 60, 20);
    
    // 创建地层占比标签（从数据库动态计算）
    pieContent = new QLabel(pieChartWidget);
    pieContent->setText(calculateLayerProportions());
    pieContent->setStyleSheet("font-size: 11px;");
    pieContent->setGeometry(60, 50, 100, 80);
    
    // 右侧面板布局
    QWidget *rightPanel = new QWidget(mainContent);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(faceViewWidget);
    rightLayout->addWidget(pieChartWidget);
    rightLayout->addStretch();
    
    // 主视图区域
    QWidget *viewArea = new QWidget(mainContent);
    QHBoxLayout *viewLayout = new QHBoxLayout(viewArea);
    viewLayout->addWidget(geo2DWidget, 1);
    viewLayout->addWidget(rightPanel);

    // 预警信息标题
    QLabel *warningTitle = new QLabel("预警信息", mainContent);
    warningTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; margin-top: 20px;")
                                    .arg(StyleHelper::COLOR_PRIMARY));

    // 预警信息表
    QTableWidget *warningTable = new QTableWidget(4, 6, mainContent);
    warningTable->setHorizontalHeaderLabels({"编号#", "预警级别", "预警类别", "预警阈值", "预警距离", "预警时间"});
    warningTable->setStyleSheet(StyleHelper::getTableStyle());
    warningTable->horizontalHeader()->setStretchLastSection(true);
    warningTable->setAlternatingRowColors(true);
    warningTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    warningTable->setMaximumHeight(180);

    struct WarningData {
        QString id;
        QString level;
        QString type;
        QString threshold;
        QString distance;
        QString time;
    };

    WarningData warnings[] = {
        {"16", "D", "岩溶发育", "1", "-6.6m", "2024-12-2 13:36"},
        {"17", "D", "涌水涌泥", "1", "+3.2m", "2024-12-2 13:36"},
        {"18", "C", "岩层断裂", "2", "+4.2m", "2024-12-2 13:36"},
        {"19", "D", "瓦斯区域", "1", "+12m", "2024-12-2 13:36"}
    };

    for (int row = 0; row < 4; row++) {
        warningTable->setItem(row, 0, new QTableWidgetItem(warnings[row].id));
        warningTable->setItem(row, 1, new QTableWidgetItem(warnings[row].level));
        warningTable->setItem(row, 2, new QTableWidgetItem(warnings[row].type));
        warningTable->setItem(row, 3, new QTableWidgetItem(warnings[row].threshold));
        warningTable->setItem(row, 4, new QTableWidgetItem(warnings[row].distance));
        warningTable->setItem(row, 5, new QTableWidgetItem(warnings[row].time));

        for (int col = 0; col < 6; col++) {
            warningTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }

    layout->addWidget(titleLabel);
    layout->addWidget(viewArea, 1);
    layout->addWidget(warningTitle);
    layout->addWidget(warningTable);
}

void ProjectWindow::load3DView()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    // 获取当前项目ID
    ProjectDAO dao;
    Project project = dao.getProjectByName(projectName);
    
    if (project.getProjectId() <= 0) {
        QLabel *errorLabel = new QLabel("错误：无法加载项目数据", mainContent);
        errorLabel->setStyleSheet(QString("font-size: 16px; color: red;"));
        errorLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(errorLabel);
        return;
    }
    
    // 创建3D视图widget
    Geological3DWidget *geo3DWidget = new Geological3DWidget(project.getProjectId(), mainContent);
    layout->addWidget(geo3DWidget);
}

void ProjectWindow::loadExcavationParams()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    QLabel *titleLabel = new QLabel("掘进参数", mainContent);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    // 创建参数显示面板
    QWidget *paramsPanel = new QWidget(mainContent);
    paramsPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QGridLayout *paramsLayout = new QGridLayout(paramsPanel);
    paramsLayout->setSpacing(15);
    paramsLayout->setContentsMargins(20, 20, 20, 20);

    // 添加参数
    QStringList paramLabels = {
        "掘进模式：", "土仓土压力：", "千斤顶推力：", "刀盘转速：",
        "刀盘扭矩：", "掘进速度：", "注浆压力：", "注浆量："
    };

    QStringList paramValues = {
        "土压平衡模式", "演示压力Pa", "2500t", "1.5rpm",
        "2000kN·m", "50mm/min", "2.5kg/cm²", "6m³/环"
    };

    for (int i = 0; i < paramLabels.size(); i++) {
        QLabel *label = new QLabel(paramLabels[i], paramsPanel);
        label->setStyleSheet(QString("font-weight: bold; color: %1;").arg(StyleHelper::COLOR_TEXT_DARK));

        QLabel *value = new QLabel(paramValues[i], paramsPanel);
        value->setStyleSheet(QString("color: %1;").arg(StyleHelper::COLOR_TEXT_DARK));

        paramsLayout->addWidget(label, i / 2, (i % 2) * 2);
        paramsLayout->addWidget(value, i / 2, (i % 2) * 2 + 1);
    }

    // 掘进统计信息
    QLabel *statsTitle = new QLabel("掘进统计", mainContent);
    statsTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; margin-top: 20px;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    QWidget *statsPanel = new QWidget(mainContent);
    statsPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QGridLayout *statsLayout = new QGridLayout(statsPanel);
    statsLayout->setSpacing(15);
    statsLayout->setContentsMargins(20, 20, 20, 20);

    QStringList statsLabels = {"掘进时间：", "闲置时间：", "故障时间比例：", "掘进距离："};
    QStringList statsValues = {"160min", "400min", "21.3%", "55m"};

    for (int i = 0; i < statsLabels.size(); i++) {
        QLabel *label = new QLabel(statsLabels[i], statsPanel);
        label->setStyleSheet(QString("font-weight: bold; color: %1;").arg(StyleHelper::COLOR_TEXT_DARK));

        QLabel *value = new QLabel(statsValues[i], statsPanel);
        value->setStyleSheet(QString("color: %1;").arg(StyleHelper::COLOR_TEXT_DARK));

        statsLayout->addWidget(label, i / 2, (i % 2) * 2);
        statsLayout->addWidget(value, i / 2, (i % 2) * 2 + 1);
    }

    layout->addWidget(titleLabel);
    layout->addWidget(paramsPanel);
    layout->addWidget(statsTitle);
    layout->addWidget(statsPanel);
    layout->addStretch();
}

void ProjectWindow::loadSupplementaryData()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    QLabel *titleLabel = new QLabel("补勘数据", mainContent);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    // 主要内容区域
    QWidget *contentWidget = new QWidget(mainContent);
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setSpacing(15);
    
    // 左侧区域 - 刀盘受力图
    QWidget *leftWidget = new QWidget(contentWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // 刀盘受力图占位
    QWidget *cutterPanel = new QWidget(leftWidget);
    cutterPanel->setMinimumSize(350, 350);
    cutterPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));
    
    QVBoxLayout *cutterLayout = new QVBoxLayout(cutterPanel);
    QLabel *cutterPlaceholder = new QLabel("刀盘受力图\n(预留位置)", cutterPanel);
    cutterPlaceholder->setAlignment(Qt::AlignCenter);
    cutterPlaceholder->setStyleSheet("font-size: 16px; color: #666;");
    cutterLayout->addWidget(cutterPlaceholder);
    
    // 刀盘受力情况信息
    QWidget *cutterInfoPanel = new QWidget(leftWidget);
    cutterInfoPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));
    
    QVBoxLayout *cutterInfoLayout = new QVBoxLayout(cutterInfoPanel);
    cutterInfoLayout->setContentsMargins(15, 15, 15, 15);
    cutterInfoLayout->setSpacing(8);
    
    QLabel *cutterTitle = new QLabel("刀盘受力情况:", cutterInfoPanel);
    cutterTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    cutterInfoLayout->addWidget(cutterTitle);
    
    // 刀具贯入阻力
    QWidget *resist1Widget = new QWidget(cutterInfoPanel);
    QHBoxLayout *resist1Layout = new QHBoxLayout(resist1Widget);
    resist1Layout->setContentsMargins(0, 0, 0, 0);
    resist1Layout->setSpacing(10);
    
    QLabel *cutterInfo1 = new QLabel("刀具贯入阻力:", resist1Widget);
    cutterInfo1->setMinimumWidth(120);
    QLineEdit *resist1Input = new QLineEdit(resist1Widget);
    resist1Input->setPlaceholderText("数值");
    resist1Input->setStyleSheet("QLineEdit { background-color: #f5f5f5; border: 1px solid #ccc; padding: 3px; }");
    
    resist1Layout->addWidget(cutterInfo1);
    resist1Layout->addWidget(resist1Input);
    
    // 刀盘正面摩擦力矩
    QWidget *resist2Widget = new QWidget(cutterInfoPanel);
    QHBoxLayout *resist2Layout = new QHBoxLayout(resist2Widget);
    resist2Layout->setContentsMargins(0, 0, 0, 0);
    resist2Layout->setSpacing(10);
    
    QLabel *cutterInfo2 = new QLabel("刀盘正面摩擦力矩:", resist2Widget);
    cutterInfo2->setMinimumWidth(120);
    QLineEdit *resist2Input = new QLineEdit(resist2Widget);
    resist2Input->setPlaceholderText("数值");
    resist2Input->setStyleSheet("QLineEdit { background-color: #f5f5f5; border: 1px solid #ccc; padding: 3px; }");
    
    resist2Layout->addWidget(cutterInfo2);
    resist2Layout->addWidget(resist2Input);
    
    cutterInfoLayout->addWidget(resist1Widget);
    cutterInfoLayout->addWidget(resist2Widget);
    cutterInfoLayout->addStretch();
    
    leftLayout->addWidget(cutterPanel);
    leftLayout->addWidget(cutterInfoPanel);
    leftLayout->addStretch();
    
    // 右侧区域 - 三个数据面板
    QWidget *rightWidget = new QWidget(contentWidget);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    // 物探数据面板
    QWidget *geophysicalPanel = new QWidget(rightWidget);
    geophysicalPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: #e3f2fd;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));
    
    QVBoxLayout *geophysicalLayout = new QVBoxLayout(geophysicalPanel);
    geophysicalLayout->setContentsMargins(15, 15, 15, 15);
    geophysicalLayout->setSpacing(5);
    
    QStringList geophysicalData = {
        "波速、波幅反射系数:",
        "视电阻率:",
        "应力梯度:",
        "前方5m岩石含水概率:",
        "掌子面岩石物性参数:",
        "围岩危险等级:"
    };
    
    for (const QString &item : geophysicalData) {
        QWidget *itemWidget = new QWidget(geophysicalPanel);
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(10);
        
        QLabel *label = new QLabel(item, itemWidget);
        label->setStyleSheet("font-size: 13px;");
        label->setMinimumWidth(150);
        
        QLineEdit *valueInput = new QLineEdit(itemWidget);
        valueInput->setPlaceholderText("数值");
        valueInput->setStyleSheet("QLineEdit { background-color: white; border: 1px solid #ccc; padding: 3px; }");
        valueInput->setMinimumWidth(150);
        
        itemLayout->addWidget(label);
        itemLayout->addWidget(valueInput);
        itemLayout->addStretch();
        
        geophysicalLayout->addWidget(itemWidget);
    }
    
    // 岩层参数面板(左下)
    QWidget *rockPanel = new QWidget(rightWidget);
    rockPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: #e1f5fe;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));
    
    QVBoxLayout *rockLayout = new QVBoxLayout(rockPanel);
    rockLayout->setContentsMargins(15, 15, 15, 15);
    rockLayout->setSpacing(5);
    
    QStringList rockData = {
        "岩层参数:",
        "纵波波速:",
        "横波波速:",
        "杨氏模量:",
        "横纵波速比:",
        "泊松比:"
    };
    
    for (const QString &item : rockData) {
        QWidget *itemWidget = new QWidget(rockPanel);
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(10);
        
        QLabel *label = new QLabel(item, itemWidget);
        label->setStyleSheet("font-size: 13px;");
        label->setMinimumWidth(100);
        
        QLineEdit *valueInput = new QLineEdit(itemWidget);
        valueInput->setPlaceholderText("数值");
        valueInput->setStyleSheet("QLineEdit { background-color: white; border: 1px solid #ccc; padding: 3px; }");
        valueInput->setMinimumWidth(120);
        
        itemLayout->addWidget(label);
        itemLayout->addWidget(valueInput);
        itemLayout->addStretch();
        
        rockLayout->addWidget(itemWidget);
    }
    
    // 岩层类型面板(右下)
    QWidget *rockTypePanel = new QWidget(rightWidget);
    rockTypePanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: #e1f5fe;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));
    
    QVBoxLayout *rockTypeLayout = new QVBoxLayout(rockTypePanel);
    rockTypeLayout->setContentsMargins(15, 15, 15, 15);
    rockTypeLayout->setSpacing(10);
    
    // 岩层类型输入
    QWidget *typeWidget = new QWidget(rockTypePanel);
    QHBoxLayout *typeLayout = new QHBoxLayout(typeWidget);
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->setSpacing(10);
    
    QLabel *rockTypeLabel = new QLabel("岩层类型:", typeWidget);
    rockTypeLabel->setStyleSheet("font-size: 13px;");
    rockTypeLabel->setMinimumWidth(80);
    
    QLineEdit *typeInput = new QLineEdit(typeWidget);
    typeInput->setPlaceholderText("类型");
    typeInput->setStyleSheet("QLineEdit { background-color: white; border: 1px solid #ccc; padding: 3px; }");
    
    typeLayout->addWidget(rockTypeLabel);
    typeLayout->addWidget(typeInput);
    
    // 分布规律输入
    QWidget *distWidget = new QWidget(rockTypePanel);
    QHBoxLayout *distLayout = new QHBoxLayout(distWidget);
    distLayout->setContentsMargins(0, 0, 0, 0);
    distLayout->setSpacing(10);
    
    QLabel *rockDistLabel = new QLabel("分布规律:", distWidget);
    rockDistLabel->setStyleSheet("font-size: 13px;");
    rockDistLabel->setMinimumWidth(80);
    
    QLineEdit *distInput = new QLineEdit(distWidget);
    distInput->setPlaceholderText("规律");
    distInput->setStyleSheet("QLineEdit { background-color: white; border: 1px solid #ccc; padding: 3px; }");
    
    distLayout->addWidget(rockDistLabel);
    distLayout->addWidget(distInput);
    
    // 上传时间间隔
    QWidget *uploadWidget = new QWidget(rockTypePanel);
    QHBoxLayout *uploadLayout = new QHBoxLayout(uploadWidget);
    uploadLayout->setContentsMargins(0, 0, 0, 0);
    uploadLayout->setSpacing(10);
    
    QLabel *uploadLabel = new QLabel("上传时间间隔:", uploadWidget);
    uploadLabel->setStyleSheet("font-size: 13px;");
    QLineEdit *uploadInput = new QLineEdit("5", uploadWidget);
    uploadInput->setFixedWidth(50);
    uploadInput->setStyleSheet("QLineEdit { background-color: white; border: 1px solid #ccc; padding: 3px; }");
    QLabel *uploadUnit = new QLabel("s", uploadWidget);
    uploadUnit->setStyleSheet("font-size: 13px;");
    
    // 使用主题色的深色按钮
    QPushButton *uploadCheck = new QPushButton("✓", uploadWidget);
    uploadCheck->setFixedSize(30, 30);
    uploadCheck->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
        }
    )").arg(StyleHelper::COLOR_PRIMARY)
       .arg(StyleHelper::COLOR_SECONDARY)
       .arg(StyleHelper::COLOR_ACCENT));
    
    uploadLayout->addWidget(uploadLabel);
    uploadLayout->addWidget(uploadInput);
    uploadLayout->addWidget(uploadUnit);
    uploadLayout->addWidget(uploadCheck);
    uploadLayout->addStretch();
    
    rockTypeLayout->addWidget(typeWidget);
    rockTypeLayout->addWidget(distWidget);
    rockTypeLayout->addWidget(uploadWidget);
    rockTypeLayout->addStretch();
    
    // 将下方两个面板放入水平布局
    QWidget *bottomPanels = new QWidget(rightWidget);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomPanels);
    bottomLayout->setSpacing(15);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addWidget(rockPanel);
    bottomLayout->addWidget(rockTypePanel);
    
    rightLayout->addWidget(geophysicalPanel, 2);
    rightLayout->addWidget(bottomPanels, 2);
    
    contentLayout->addWidget(leftWidget, 2);
    contentLayout->addWidget(rightWidget, 3);
    
    layout->addWidget(titleLabel);
    layout->addWidget(contentWidget);
    layout->addStretch();
}

void ProjectWindow::showPositioningDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("定位校准 - 确定盾构机位置（三选一）");
    dialog.setMinimumSize(750, 700);
    dialog.setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 25, 25, 25);

    // 标题
    QLabel *titleLabel = new QLabel("定位校准 - 确定盾构机位置（三选一）", &dialog);
    titleLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));
    titleLabel->setAlignment(Qt::AlignCenter);

    // 输入框样式
    QString inputStyle = R"(
        QLineEdit {
            padding: 8px;
            font-size: 13px;
            border: 1px solid #ccc;
            border-radius: 3px;
            background-color: white;
            color: #333;
        }
        QLineEdit:focus {
            border: 2px solid )" + QString(StyleHelper::COLOR_PRIMARY) + R"(;
        }
    )";
    
    QString labelStyle = "font-size: 13px; color: #333; font-weight: bold;";
    
    QString groupBoxStyle = QString(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 13px;
            color: %1;
            border: 1px solid #ddd;
            border-radius: 5px;
            margin-top: 10px;
            padding: 15px 10px 10px 10px;
            background-color: #fafafa;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 10px;
            padding: 0 5px;
        }
    )").arg(StyleHelper::COLOR_PRIMARY);

    // GPS定位
    QGroupBox *gpsGroup = new QGroupBox("链接GPS定位装置", &dialog);
    gpsGroup->setStyleSheet(groupBoxStyle);
    QHBoxLayout *gpsLayout = new QHBoxLayout(gpsGroup);
    gpsLayout->setContentsMargins(10, 20, 10, 10);
    gpsLayout->setSpacing(10);
    
    QPushButton *device1 = new QPushButton("演示装置1", gpsGroup);
    QPushButton *device2 = new QPushButton("演示装置2", gpsGroup);
    device1->setStyleSheet(StyleHelper::getButtonStyle());
    device2->setStyleSheet(StyleHelper::getButtonStyle());
    device1->setMinimumHeight(35);
    device2->setMinimumHeight(35);
    gpsLayout->addWidget(device1);
    gpsLayout->addWidget(device2);

    // 坐标输入
    QGroupBox *coordsGroup = new QGroupBox("输入坐标确定盾构机位置", &dialog);
    coordsGroup->setStyleSheet(groupBoxStyle);
    QGridLayout *coordsLayout = new QGridLayout(coordsGroup);
    coordsLayout->setContentsMargins(10, 25, 10, 10);
    coordsLayout->setHorizontalSpacing(10);
    coordsLayout->setVerticalSpacing(10);
    coordsLayout->setColumnStretch(1, 1);
    
    QLabel *label1 = new QLabel("前盾:", coordsGroup);
    QLabel *label2 = new QLabel("盾尾:", coordsGroup);
    QLabel *label3 = new QLabel("深度:", coordsGroup);
    QLabel *label4 = new QLabel("倾角:", coordsGroup);
    label1->setStyleSheet(labelStyle);
    label2->setStyleSheet(labelStyle);
    label3->setStyleSheet(labelStyle);
    label4->setStyleSheet(labelStyle);
    label1->setFixedWidth(50);
    label2->setFixedWidth(50);
    label3->setFixedWidth(50);
    label4->setFixedWidth(50);
    
    QLineEdit *frontShieldCoords = new QLineEdit("120.36,36.23", coordsGroup);
    QLineEdit *tailShieldCoords = new QLineEdit("120.36,36.23", coordsGroup);
    QLineEdit *depth1 = new QLineEdit("15", coordsGroup);
    QLineEdit *angle1 = new QLineEdit("9.83", coordsGroup);
    
    frontShieldCoords->setStyleSheet(inputStyle);
    tailShieldCoords->setStyleSheet(inputStyle);
    depth1->setStyleSheet(inputStyle);
    angle1->setStyleSheet(inputStyle);
    
    frontShieldCoords->setMinimumHeight(32);
    tailShieldCoords->setMinimumHeight(32);
    depth1->setMinimumHeight(32);
    angle1->setMinimumHeight(32);
    
    coordsLayout->addWidget(label1, 0, 0);
    coordsLayout->addWidget(frontShieldCoords, 0, 1);
    coordsLayout->addWidget(label2, 1, 0);
    coordsLayout->addWidget(tailShieldCoords, 1, 1);
    coordsLayout->addWidget(label3, 2, 0);
    coordsLayout->addWidget(depth1, 2, 1);
    coordsLayout->addWidget(label4, 3, 0);
    coordsLayout->addWidget(angle1, 3, 1);

    // 桩号输入
    QGroupBox *stakeGroup = new QGroupBox("输入桩号确定盾构机位置", &dialog);
    stakeGroup->setStyleSheet(groupBoxStyle);
    QGridLayout *stakeLayout = new QGridLayout(stakeGroup);
    stakeLayout->setContentsMargins(10, 25, 10, 10);
    stakeLayout->setHorizontalSpacing(10);
    stakeLayout->setVerticalSpacing(10);
    stakeLayout->setColumnStretch(1, 1);
    
    QLabel *label5 = new QLabel("前盾:", stakeGroup);
    QLabel *label6 = new QLabel("盾尾:", stakeGroup);
    QLabel *label7 = new QLabel("深度:", stakeGroup);
    QLabel *label8 = new QLabel("倾角:", stakeGroup);
    label5->setStyleSheet(labelStyle);
    label6->setStyleSheet(labelStyle);
    label7->setStyleSheet(labelStyle);
    label8->setStyleSheet(labelStyle);
    label5->setFixedWidth(50);
    label6->setFixedWidth(50);
    label7->setFixedWidth(50);
    label8->setFixedWidth(50);
    
    QLineEdit *frontStake = new QLineEdit("K1+190.265", stakeGroup);
    QLineEdit *tailStake = new QLineEdit("K1+210.265", stakeGroup);
    QLineEdit *depth2 = new QLineEdit("12", stakeGroup);
    QLineEdit *angle2 = new QLineEdit("11.60", stakeGroup);
    
    frontStake->setStyleSheet(inputStyle);
    tailStake->setStyleSheet(inputStyle);
    depth2->setStyleSheet(inputStyle);
    angle2->setStyleSheet(inputStyle);
    
    frontStake->setMinimumHeight(32);
    tailStake->setMinimumHeight(32);
    depth2->setMinimumHeight(32);
    angle2->setMinimumHeight(32);
    
    stakeLayout->addWidget(label5, 0, 0);
    stakeLayout->addWidget(frontStake, 0, 1);
    stakeLayout->addWidget(label6, 1, 0);
    stakeLayout->addWidget(tailStake, 1, 1);
    stakeLayout->addWidget(label7, 2, 0);
    stakeLayout->addWidget(depth2, 2, 1);
    stakeLayout->addWidget(label8, 3, 0);
    stakeLayout->addWidget(angle2, 3, 1);

    // 底部按钮
    QWidget *buttonWidget = new QWidget(&dialog);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addStretch();
    
    QPushButton *confirmBtn = new QPushButton("确认", buttonWidget);
    QPushButton *cancelBtn = new QPushButton("取消", buttonWidget);
    
    QString btnStyle = QString(R"(
        QPushButton {
            padding: 8px 25px;
            font-size: 13px;
            background-color: %1;
            color: white;
            border: none;
            border-radius: 3px;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: %2;
        }
    )").arg(StyleHelper::COLOR_PRIMARY).arg(StyleHelper::COLOR_SECONDARY);
    
    confirmBtn->setStyleSheet(btnStyle);
    cancelBtn->setStyleSheet(btnStyle);
    confirmBtn->setMinimumHeight(36);
    cancelBtn->setMinimumHeight(36);
    
    connect(confirmBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    buttonLayout->addWidget(confirmBtn);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(gpsGroup);
    mainLayout->addWidget(coordsGroup);
    mainLayout->addWidget(stakeGroup);
    mainLayout->addWidget(buttonWidget);

    dialog.exec();
}


void ProjectWindow::onBackClicked()
{
    emit backToDashboard();  // 发射返回信号
    this->hide();  // 隐藏而不是关闭
}

QString ProjectWindow::calculateLayerProportions()
{
    // 从数据库读取钻孔数据
    BoreholeDAO dao;
    int projectId = 1;  // 假设当前项目ID为1（实际应从项目上下文获取）
    QVector<BoreholeData> boreholes = dao.getBoreholesByProjectId(projectId);
    
    if (boreholes.isEmpty()) {
        return QString::fromUtf8("暂无数据");
    }
    
    // 统计各地层的总厚度
    QMap<QString, double> layerThickness;
    double totalThickness = 0.0;
    
    for (const auto &borehole : boreholes) {
        for (const auto &layer : borehole.layers) {
            QString layerName = layer.rockName;
            if (layerName.isEmpty()) {
                layerName = layer.layerCode;
            }
            
            layerThickness[layerName] += layer.thickness;
            totalThickness += layer.thickness;
        }
    }
    
    if (totalThickness < 0.01) {
        return QString::fromUtf8("数据计算中...");
    }
    
    // 按厚度排序，取前3个
    QList<QPair<QString, double>> sortedLayers;
    for (auto it = layerThickness.begin(); it != layerThickness.end(); ++it) {
        sortedLayers.append(qMakePair(it.key(), it.value()));
    }
    
    std::sort(sortedLayers.begin(), sortedLayers.end(), 
              [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
                  return a.second > b.second;
              });
    
    // 构建显示字符串（前3个地层）
    QString result;
    int count = qMin(3, sortedLayers.size());
    for (int i = 0; i < count; i++) {
        double percentage = (sortedLayers[i].second / totalThickness) * 100.0;
        result += QString::fromUtf8("%1: %2%\n")
                      .arg(sortedLayers[i].first)
                      .arg(QString::number(percentage, 'f', 1));
    }
    
    return result.trimmed();
}
