#include "projectmanagementwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QApplication>
#include <QScreen>

ProjectManagementWindow::ProjectManagementWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadProjectData();

    setWindowTitle("智能盾构地质可视化平台 - 项目管理");
    resize(1400, 900);

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

ProjectManagementWindow::~ProjectManagementWindow()
{
}

void ProjectManagementWindow::setupUI()
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

    createTabWidget();
    contentLayout->addWidget(tabWidget, 1);

    mainLayout->addWidget(contentWidget);
}

void ProjectManagementWindow::createTopBar()
{
    QWidget *topBar = new QWidget(centralWidget);
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);

    backButton = new QPushButton("← 返回主界面", topBar);
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
    connect(backButton, &QPushButton::clicked, this, &ProjectManagementWindow::onBackClicked);

    topLayout->addWidget(backButton);

    QLabel *titleLabel = new QLabel("智能盾构地质可视化平台 - 项目管理", topBar);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    minimizeButton = new QPushButton("－", topBar);
    minimizeButton->setFixedSize(40, 40);
    minimizeButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            color: white;
            font-size: 20px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 20px;
        }
    )");
    connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);

    closeButton = new QPushButton("×", topBar);
    closeButton->setFixedSize(40, 40);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            color: white;
            font-size: 24px;
        }
        QPushButton:hover {
            background-color: rgba(255, 0, 0, 0.7);
            border-radius: 20px;
        }
    )");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    topLayout->addWidget(minimizeButton);
    topLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(centralWidget);
    }
    mainLayout->addWidget(topBar);
}

void ProjectManagementWindow::createSidebar()
{
    sidebar = new QWidget(centralWidget);
    sidebar->setFixedWidth(180);
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
        QPushButton:checked {
            background-color: %2;
        }
    )").arg(StyleHelper::COLOR_SECONDARY).arg(StyleHelper::COLOR_ACCENT);

    QList<QString> menuItems = {"项目总览", "预警信息", "掘进信息", "补勘数据", "新闻模块"};
    
    for (const QString &item : menuItems) {
        QPushButton *btn = new QPushButton(item, sidebar);
        btn->setStyleSheet(buttonStyle);
        btn->setCheckable(true);
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();
}

void ProjectManagementWindow::createTabWidget()
{
    tabWidget = new QTabWidget(centralWidget);
    tabWidget->setStyleSheet(StyleHelper::getTabWidgetStyle());

    createProjectOverviewTab();
    createWarningInfoTab();
    createExcavationInfoTab();
    createSupplementaryDataTab();
    createNewsModuleTab();

    connect(tabWidget, &QTabWidget::currentChanged, this, &ProjectManagementWindow::onTabChanged);
}

void ProjectManagementWindow::createProjectOverviewTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 搜索栏和操作按钮
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QLineEdit *searchBox = new QLineEdit(topWidget);
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(300);
    
    QPushButton *searchBtn = new QPushButton("🔍", topWidget);
    searchBtn->setFixedSize(40, 40);
    
    QPushButton *refreshBtn = new QPushButton("🔄", topWidget);
    refreshBtn->setFixedSize(40, 40);
    
    topLayout->addWidget(searchBox);
    topLayout->addWidget(searchBtn);
    topLayout->addWidget(refreshBtn);
    topLayout->addStretch();
    
    newProjectButton = new QPushButton("新建项目", topWidget);
    newProjectButton->setStyleSheet(StyleHelper::getButtonStyle());
    newProjectButton->setMinimumHeight(40);
    connect(newProjectButton, &QPushButton::clicked, this, &ProjectManagementWindow::onNewProjectClicked);
    topLayout->addWidget(newProjectButton);

    // 项目表格
    projectTable = new QTableWidget(0, 8, tab);
    projectTable->setHorizontalHeaderLabels({"项目名称", "简介", "经纬度坐标", "施工单位", "开始时间", "施工进度", "地理位置", "操作"});
    projectTable->setStyleSheet(StyleHelper::getTableStyle());
    projectTable->horizontalHeader()->setStretchLastSection(true);
    projectTable->setAlternatingRowColors(true);
    projectTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    layout->addWidget(topWidget);
    layout->addWidget(projectTable);

    // 分页控件
    QWidget *pageWidget = new QWidget(tab);
    QHBoxLayout *pageLayout = new QHBoxLayout(pageWidget);
    pageLayout->addWidget(new QLabel("共4条", pageWidget));
    pageLayout->addStretch();
    pageLayout->addWidget(new QPushButton("<", pageWidget));
    pageLayout->addWidget(new QLabel("1", pageWidget));
    pageLayout->addWidget(new QPushButton(">", pageWidget));
    
    layout->addWidget(pageWidget);

    tabWidget->addTab(tab, "项目总览");
}

void ProjectManagementWindow::createWarningInfoTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 搜索栏
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QLineEdit *searchBox = new QLineEdit(topWidget);
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(300);
    topLayout->addWidget(searchBox);
    topLayout->addStretch();

    // 预警信息表格
    warningTable = new QTableWidget(0, 8, tab);
    warningTable->setHorizontalHeaderLabels({"项目名称", "编号", "预警级别", "预警类别", "预警坐标", "预警深度", "预警阈值", "预警时间"});
    warningTable->setStyleSheet(StyleHelper::getTableStyle());
    warningTable->horizontalHeader()->setStretchLastSection(true);
    warningTable->setAlternatingRowColors(true);

    layout->addWidget(topWidget);
    layout->addWidget(warningTable);

    tabWidget->addTab(tab, "预警信息");
}

void ProjectManagementWindow::createExcavationInfoTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 掘进参数表格
    excavationTable = new QTableWidget(0, 8, tab);
    excavationTable->setHorizontalHeaderLabels({"项目名称", "掘进时间", "掘进坐标/桩号", "掘进模式", "土仓土压力", "千斤顶推力", "刀盘转速", "刀盘扭矩"});
    excavationTable->setStyleSheet(StyleHelper::getTableStyle());
    excavationTable->horizontalHeader()->setStretchLastSection(true);
    excavationTable->setAlternatingRowColors(true);

    layout->addWidget(excavationTable);

    tabWidget->addTab(tab, "掘进信息");
}

void ProjectManagementWindow::createSupplementaryDataTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 补勘数据表格
    supplementaryTable = new QTableWidget(0, 10, tab);
    supplementaryTable->setHorizontalHeaderLabels({"项目名称", "掘进时间", "掘进坐标/桩号", "刀盘受力", "刀具贯入阻力", 
                                                   "刀盘正面摩擦力矩", "视电阻率", "前方5m含水概率", "应力梯度", "前方岩石物性参数"});
    supplementaryTable->setStyleSheet(StyleHelper::getTableStyle());
    supplementaryTable->horizontalHeader()->setStretchLastSection(true);
    supplementaryTable->setAlternatingRowColors(true);

    layout->addWidget(supplementaryTable);

    tabWidget->addTab(tab, "补勘数据");
}

void ProjectManagementWindow::createNewsModuleTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 顶部操作按钮
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QPushButton *publishBtn = new QPushButton("发布新闻", topWidget);
    publishBtn->setStyleSheet(StyleHelper::getButtonStyle());
    publishBtn->setMinimumHeight(40);
    
    QPushButton *deleteBtn = new QPushButton("删除所选", topWidget);
    deleteBtn->setStyleSheet(StyleHelper::getButtonStyle());
    deleteBtn->setMinimumHeight(40);
    
    topLayout->addWidget(publishBtn);
    topLayout->addWidget(deleteBtn);
    topLayout->addStretch();

    // 新闻表格
    newsTable = new QTableWidget(0, 3, tab);
    newsTable->setHorizontalHeaderLabels({"新闻内容", "发布时间", "操作"});
    newsTable->setStyleSheet(StyleHelper::getTableStyle());
    newsTable->horizontalHeader()->setStretchLastSection(true);
    newsTable->setAlternatingRowColors(true);

    layout->addWidget(topWidget);
    layout->addWidget(newsTable);

    tabWidget->addTab(tab, "新闻模块");
}

void ProjectManagementWindow::loadProjectData()
{
    // 加载项目总览数据
    projectTable->setRowCount(4);
    
    QStringList projects[] = {
        {"济南环城高速", "测试简介", "测试坐标", "测试单位", "2024-11-28", "91%", "山东济南"},
        {"老虎山隧道", "测试简介", "测试坐标", "测试单位", "2024-11-28", "66.7%", "山东青岛"},
        {"青岛沿海公路", "测试简介", "测试坐标", "测试单位", "2024-11-28", "56%", "山东青岛"},
        {"演示项目", "测试简介", "测试坐标", "测试单位", "2024-11-28", "80%", "山东青岛"}
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 7; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(projects[row][col]);
            item->setTextAlignment(Qt::AlignCenter);
            projectTable->setItem(row, col, item);
        }
        
        // 操作列
        QWidget *operationWidget = new QWidget();
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(5, 2, 5, 2);
        
        QPushButton *editBtn = new QPushButton("修改", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除", operationWidget);
        editBtn->setStyleSheet("padding: 5px 10px;");
        deleteBtn->setStyleSheet("padding: 5px 10px;");
        
        connect(editBtn, &QPushButton::clicked, [this, row]() { onEditProject(row); });
        connect(deleteBtn, &QPushButton::clicked, [this, row]() { onDeleteProject(row); });
        
        operationLayout->addWidget(editBtn);
        operationLayout->addWidget(deleteBtn);
        projectTable->setCellWidget(row, 7, operationWidget);
    }
    
    // 加载预警信息数据（示例）
    warningTable->setRowCount(4);
    QStringList warnings[] = {
        {"青岛沿海公路", "1", "D", "岩溶发育", "演示坐标", "演示深度", "1", "2024-12-3 9:50:46"},
        {"青岛沿海公路", "2", "D", "涌水涌泥", "演示坐标", "演示深度", "1", "2024-12-3 9:50:51"},
        {"青岛沿海公路", "3", "D", "岩层断裂", "演示坐标", "演示深度", "1", "2024-12-3 9:50:56"},
        {"青岛沿海公路", "4", "D", "瓦斯区域", "演示坐标", "演示深度", "1", "2024-12-3 9:50:01"}
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 8; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(warnings[row][col]);
            item->setTextAlignment(Qt::AlignCenter);
            warningTable->setItem(row, col, item);
        }
    }
    
    // 加载新闻数据
    newsTable->setRowCount(4);
    QStringList news[] = {
        {"北京地铁在建线路11条线（段）盾构法施工区间占比68%", "2024-12-3 9:50:46"},
        {"甘肃天陇铁路柳林隧道正洞掘进破万米大关", "2024-12-3 9:50:51"},
        {"宜兴高铁长岗岭隧道顺利贯通", "2024-12-3 9:50:56"},
        {"云兰高速全线12座隧道贯通", "2024-12-3 9:50:01"}
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 2; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(news[row][col]);
            item->setTextAlignment(Qt::AlignCenter);
            newsTable->setItem(row, col, item);
        }
        
        // 操作列
        QWidget *operationWidget = new QWidget();
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(5, 2, 5, 2);
        
        QPushButton *editBtn = new QPushButton("编辑新闻", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除新闻", operationWidget);
        editBtn->setStyleSheet("padding: 5px 10px;");
        deleteBtn->setStyleSheet("padding: 5px 10px;");
        
        operationLayout->addWidget(editBtn);
        operationLayout->addWidget(deleteBtn);
        newsTable->setCellWidget(row, 2, operationWidget);
    }
}

void ProjectManagementWindow::showNewProjectDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建项目");
    dialog.setFixedSize(500, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(12);

    QLineEdit *nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("演示项目");
    nameEdit->setStyleSheet(StyleHelper::getInputStyle());

    QLineEdit *briefEdit = new QLineEdit(&dialog);
    briefEdit->setPlaceholderText("演示简介");
    briefEdit->setStyleSheet(StyleHelper::getInputStyle());

    QLineEdit *coordsEdit = new QLineEdit(&dialog);
    coordsEdit->setPlaceholderText("演示坐标");
    coordsEdit->setStyleSheet(StyleHelper::getInputStyle());
    QPushButton *importCoordsBtn = new QPushButton("导入", &dialog);

    QLineEdit *unitEdit = new QLineEdit(&dialog);
    unitEdit->setPlaceholderText("演示施工单位");
    unitEdit->setStyleSheet(StyleHelper::getInputStyle());

    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate(), &dialog);
    dateEdit->setCalendarPopup(true);
    dateEdit->setStyleSheet(StyleHelper::getInputStyle());

    QSpinBox *progressSpin = new QSpinBox(&dialog);
    progressSpin->setRange(0, 100);
    progressSpin->setValue(91);
    progressSpin->setSuffix("%");
    progressSpin->setStyleSheet(StyleHelper::getInputStyle());

    QLineEdit *locationEdit = new QLineEdit(&dialog);
    locationEdit->setPlaceholderText("山东青岛");
    locationEdit->setStyleSheet(StyleHelper::getInputStyle());
    QPushButton *importLocationBtn = new QPushButton("导入", &dialog);

    QPushButton *import2DBtn = new QPushButton("导入", &dialog);
    QPushButton *import3DBtn = new QPushButton("导入", &dialog);

    formLayout->addRow("名称：", nameEdit);
    formLayout->addRow("简介：", briefEdit);
    
    QHBoxLayout *coordsLayout = new QHBoxLayout();
    coordsLayout->addWidget(coordsEdit);
    coordsLayout->addWidget(importCoordsBtn);
    formLayout->addRow("经纬度坐标：", coordsLayout);
    
    formLayout->addRow("施工单位：", unitEdit);
    formLayout->addRow("开始时间：", dateEdit);
    formLayout->addRow("施工进度：", progressSpin);
    
    QHBoxLayout *locationLayout = new QHBoxLayout();
    locationLayout->addWidget(locationEdit);
    locationLayout->addWidget(importLocationBtn);
    formLayout->addRow("地理位置：", locationLayout);
    
    formLayout->addRow("二维地质图：", import2DBtn);
    formLayout->addRow("三维地质图：", import3DBtn);

    layout->addLayout(formLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("确认");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "提示", "项目创建成功！");
        loadProjectData();
    }
}

void ProjectManagementWindow::onBackClicked()
{
    MainMenuWindow *mainMenu = new MainMenuWindow();
    mainMenu->show();
    this->close();
}

void ProjectManagementWindow::onNewProjectClicked()
{
    showNewProjectDialog();
}

void ProjectManagementWindow::onEditProject(int row)
{
    QMessageBox::information(this, "编辑项目", 
                             QString("编辑项目：%1").arg(projectTable->item(row, 0)->text()));
}

void ProjectManagementWindow::onDeleteProject(int row)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "删除项目",
        QString("确定要删除项目 '%1' 吗？").arg(projectTable->item(row, 0)->text()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        projectTable->removeRow(row);
        QMessageBox::information(this, "提示", "项目已删除！");
    }
}

void ProjectManagementWindow::onTabChanged(int index)
{
    // 可以在这里处理标签页切换事件
}
