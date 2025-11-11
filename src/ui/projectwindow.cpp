#include "projectwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
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

ProjectWindow::ProjectWindow(const QString &projectName, QWidget *parent)
    : QMainWindow(parent)
    , projectName(projectName)
{
    setupUI();

    setWindowTitle(QString("项目详情 - %1").arg(projectName));
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

    QPushButton *homeButton = new QPushButton("🏠 工作台", topBar);
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
    
    // 坐标输入
    QLabel *coordLabel = new QLabel("📍", locationWidget);
    coordLabel->setStyleSheet("font-size: 20px;");
    coordsInput = new QLineEdit("120.36,36.23", locationWidget);
    coordsInput->setPlaceholderText("输入坐标");
    coordsInput->setStyleSheet(StyleHelper::getInputStyle());
    coordsInput->setMaximumWidth(200);
    
    QPushButton *coordLocateBtn = new QPushButton("🎯", locationWidget);
    coordLocateBtn->setFixedSize(40, 40);
    coordLocateBtn->setStyleSheet(StyleHelper::getButtonStyle());
    coordLocateBtn->setToolTip("定位到坐标");
    
    // 桩号输入
    QLabel *stakeLabel = new QLabel("🚩", locationWidget);
    stakeLabel->setStyleSheet("font-size: 20px; margin-left: 20px;");
    stakeInput = new QLineEdit("K1+190.00", locationWidget);
    stakeInput->setPlaceholderText("输入桩号");
    stakeInput->setStyleSheet(StyleHelper::getInputStyle());
    stakeInput->setMaximumWidth(200);
    
    QPushButton *stakeLocateBtn = new QPushButton("🎯", locationWidget);
    stakeLocateBtn->setFixedSize(40, 40);
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

    // 地质剖面图
    QLabel *imageLabel = new QLabel(mainContent);
    imageLabel->setMinimumSize(1000, 400);

    QPixmap geological2D(":/images/geological_2d.png");
    if (!geological2D.isNull()) {
        imageLabel->setPixmap(geological2D.scaled(1000, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 创建占位图
        QPixmap placeholder(1000, 500);
        placeholder.fill(QColor("#ffffff"));
        QPainter painter(&placeholder);
        painter.setPen(QPen(QColor(StyleHelper::COLOR_BORDER), 2, Qt::DashLine));
        painter.drawRect(1, 1, placeholder.width()-2, placeholder.height()-2);
        painter.setPen(QPen(QColor(StyleHelper::COLOR_TEXT_DARK)));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "二维地质剖面");
        imageLabel->setPixmap(placeholder);
    }
    imageLabel->setAlignment(Qt::AlignCenter);

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

    // 修复：使用结构体数组而不是QStringList数组
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

        // 居中对齐
        for (int col = 0; col < 6; col++) {
            warningTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }

    layout->addWidget(titleLabel);
    layout->addWidget(imageLabel);
    layout->addWidget(warningTitle);
    layout->addWidget(warningTable);
    layout->addStretch();
}

void ProjectWindow::load3DView()
{
    clearMainContent();
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(mainContent->layout());

    QLabel *titleLabel = new QLabel("三维视图", mainContent);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    QLabel *imageLabel = new QLabel(mainContent);
    imageLabel->setMinimumSize(1000, 500);

    QPixmap geological3D(":/images/geological_3d.png");
    if (!geological3D.isNull()) {
        imageLabel->setPixmap(geological3D.scaled(1000, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 创建占位图
        QPixmap placeholder(1000, 600);
        placeholder.fill(QColor("#ffffff"));
        QPainter painter(&placeholder);
        painter.setPen(QPen(QColor(StyleHelper::COLOR_BORDER), 2, Qt::DashLine));
        painter.drawRect(1, 1, placeholder.width()-2, placeholder.height()-2);
        painter.setPen(QPen(QColor(StyleHelper::COLOR_TEXT_DARK)));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "三维地质模型\n（集成3D渲染引擎）");
        imageLabel->setPixmap(placeholder);
    }
    imageLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLabel);
    layout->addWidget(imageLabel);
    layout->addStretch();
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

    // 创建数据显示面板
    QWidget *dataPanel = new QWidget(mainContent);
    dataPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QVBoxLayout *dataLayout = new QVBoxLayout(dataPanel);
    dataLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *descLabel = new QLabel("补勘数据说明", dataPanel);
    descLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; margin-bottom: 10px;")
                                 .arg(StyleHelper::COLOR_PRIMARY));

    QLabel *infoLabel = new QLabel(
        "补勘数据通过盾构机上的传感器实时采集，主要包括：\n\n"
        "1. 刀盘受力情况\n"
        "   - 刀具贯入阻力\n"
        "   - 刀盘正面摩擦力矩\n\n"
        "2. 物探数据\n"
        "   - 波速、波幅反射系数\n"
        "   - 视电阻率\n"
        "   - 应力梯度\n\n"
        "3. 前方地质预测\n"
        "   - 前方5m岩石含水概率\n"
        "   - 掌子面岩石物性参数\n"
        "   - 围岩危险等级\n\n"
        "4. 岩层参数\n"
        "   - 纵波波速、横波波速\n"
        "   - 杨氏模量、泊松比\n"
        "   - 岩层类型和分布规律",
        dataPanel);
    infoLabel->setStyleSheet(QString("color: %1; line-height: 1.8;").arg(StyleHelper::COLOR_TEXT_DARK));
    infoLabel->setWordWrap(true);

    dataLayout->addWidget(descLabel);
    dataLayout->addWidget(infoLabel);

    layout->addWidget(titleLabel);
    layout->addWidget(dataPanel);
    layout->addStretch();
}

void ProjectWindow::showPositioningDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("定位校准");
    dialog.setFixedSize(650, 600);
    dialog.setStyleSheet("QDialog { background-color: #f5f5f5; }");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("定位校准 - 确定盾构机位置（三选一）", &dialog);
    titleLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1; background-color: transparent;")
                                  .arg(StyleHelper::COLOR_PRIMARY));

    // 改进的输入框样式 - 更大的文本框，更好的对比度
    QString inputStyle = R"(
        QLineEdit {
            padding: 8px 12px;
            font-size: 14px;
            border: 2px solid #ccc;
            border-radius: 4px;
            background-color: white;
            color: #333;
            min-height: 30px;
        }
        QLineEdit:focus {
            border: 2px solid )" + QString(StyleHelper::COLOR_PRIMARY) + R"(;
        }
    )";
    
    QString groupBoxStyle = QString(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            color: %1;
            border: 2px solid #ddd;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 15px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
            color: %1;
            background-color: white;
        }
    )").arg(StyleHelper::COLOR_PRIMARY);

    // GPS定位
    QGroupBox *gpsGroup = new QGroupBox("链接GPS定位装置", &dialog);
    gpsGroup->setStyleSheet(groupBoxStyle);
    QHBoxLayout *gpsLayout = new QHBoxLayout(gpsGroup);
    gpsLayout->setContentsMargins(15, 10, 15, 10);
    QPushButton *device1 = new QPushButton("演示装置1", &dialog);
    QPushButton *device2 = new QPushButton("演示装置2", &dialog);
    device1->setStyleSheet(StyleHelper::getButtonStyle());
    device2->setStyleSheet(StyleHelper::getButtonStyle());
    device1->setMinimumHeight(40);
    device2->setMinimumHeight(40);
    gpsLayout->addWidget(device1);
    gpsLayout->addWidget(device2);

    // 坐标输入
    QGroupBox *coordsGroup = new QGroupBox("输入坐标确定盾构机位置", &dialog);
    coordsGroup->setStyleSheet(groupBoxStyle);
    QFormLayout *coordsLayout = new QFormLayout(coordsGroup);
    coordsLayout->setContentsMargins(15, 15, 15, 15);
    coordsLayout->setSpacing(12);
    coordsLayout->setLabelAlignment(Qt::AlignRight);
    
    QLabel *label1 = new QLabel("前盾：", &dialog);
    QLabel *label2 = new QLabel("盾尾：", &dialog);
    QLabel *label3 = new QLabel("深度：", &dialog);
    QLabel *label4 = new QLabel("倾角：", &dialog);
    label1->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label2->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label3->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label4->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    
    QLineEdit *frontShieldCoords = new QLineEdit("120.36,36.23", &dialog);
    QLineEdit *tailShieldCoords = new QLineEdit("120.36,36.23", &dialog);
    QLineEdit *depth1 = new QLineEdit("15", &dialog);
    QLineEdit *angle1 = new QLineEdit("9.83", &dialog);
    
    frontShieldCoords->setStyleSheet(inputStyle);
    tailShieldCoords->setStyleSheet(inputStyle);
    depth1->setStyleSheet(inputStyle);
    angle1->setStyleSheet(inputStyle);
    
    coordsLayout->addRow(label1, frontShieldCoords);
    coordsLayout->addRow(label2, tailShieldCoords);
    coordsLayout->addRow(label3, depth1);
    coordsLayout->addRow(label4, angle1);

    // 桩号输入
    QGroupBox *stakeGroup = new QGroupBox("输入桩号确定盾构机位置", &dialog);
    stakeGroup->setStyleSheet(groupBoxStyle);
    QFormLayout *stakeLayout = new QFormLayout(stakeGroup);
    stakeLayout->setContentsMargins(15, 15, 15, 15);
    stakeLayout->setSpacing(12);
    stakeLayout->setLabelAlignment(Qt::AlignRight);
    
    QLabel *label5 = new QLabel("前盾：", &dialog);
    QLabel *label6 = new QLabel("盾尾：", &dialog);
    QLabel *label7 = new QLabel("深度：", &dialog);
    QLabel *label8 = new QLabel("倾角：", &dialog);
    label5->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label6->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label7->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    label8->setStyleSheet("font-size: 14px; color: #333; background-color: transparent;");
    
    QLineEdit *frontStake = new QLineEdit("K1+190.265", &dialog);
    QLineEdit *tailStake = new QLineEdit("K1+210.265", &dialog);
    QLineEdit *depth2 = new QLineEdit("12", &dialog);
    QLineEdit *angle2 = new QLineEdit("11.60", &dialog);
    
    frontStake->setStyleSheet(inputStyle);
    tailStake->setStyleSheet(inputStyle);
    depth2->setStyleSheet(inputStyle);
    angle2->setStyleSheet(inputStyle);
    
    stakeLayout->addRow(label5, frontStake);
    stakeLayout->addRow(label6, tailStake);
    stakeLayout->addRow(label7, depth2);
    stakeLayout->addRow(label8, angle2);

    // 按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("确认");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    buttonBox->button(QDialogButtonBox::Ok)->setMinimumHeight(40);
    buttonBox->button(QDialogButtonBox::Cancel)->setMinimumHeight(40);
    buttonBox->setStyleSheet(QString(R"(
        QPushButton {
            padding: 8px 24px;
            font-size: 14px;
            font-weight: bold;
        }
    )") + StyleHelper::getButtonStyle());
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(titleLabel);
    layout->addWidget(gpsGroup);
    layout->addWidget(coordsGroup);
    layout->addWidget(stakeGroup);
    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "提示", "定位校准已完成！");
    }
}

void ProjectWindow::onBackClicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}
