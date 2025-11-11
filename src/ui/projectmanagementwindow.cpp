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
    
    sidebarButtons = new QButtonGroup(this);
    sidebarButtons->setExclusive(true);
    
    for (int i = 0; i < menuItems.size(); i++) {
        QPushButton *btn = new QPushButton(menuItems[i], sidebar);
        btn->setStyleSheet(buttonStyle);
        btn->setCheckable(true);
        sidebarButtons->addButton(btn, i);
        sidebarLayout->addWidget(btn);
        
        // 连接按钮点击到标签页切换
        connect(btn, &QPushButton::clicked, [this, i]() {
            if (tabWidget) {
                tabWidget->setCurrentIndex(i);
            }
        });
    }
    
    // 默认选中第一个按钮
    if (sidebarButtons->button(0)) {
        sidebarButtons->button(0)->setChecked(true);
    }

    sidebarLayout->addStretch();
}

void ProjectManagementWindow::createTabWidget()
{
    tabWidget = new QTabWidget(centralWidget);
    
    // 隐藏标签栏,只保留内容区
    tabWidget->tabBar()->hide();
    
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
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    
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
    
    // 设置表格的SizePolicy
    projectTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 设置表格默认行高 - 关键!
    projectTable->verticalHeader()->setDefaultSectionSize(40);
    
    // 设置列可随窗口调整大小
    projectTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 固定操作列宽度
    projectTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    projectTable->setColumnWidth(7, 140);
    
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

    // 搜索栏和按钮
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QLineEdit *searchBox = new QLineEdit(topWidget);
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(250);
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    searchBtn->setToolTip("搜索");
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    refreshBtn->setToolTip("还原");
    
    QPushButton *filterBtn = new QPushButton(topWidget);
    filterBtn->setIcon(QIcon(":/icons/filter.png"));
    filterBtn->setIconSize(QSize(20, 20));
    filterBtn->setFixedSize(40, 40);
    filterBtn->setStyleSheet(StyleHelper::getButtonStyle());
    filterBtn->setToolTip("筛选");
    
    QPushButton *exportBtn = new QPushButton(topWidget);
    exportBtn->setIcon(QIcon(":/icons/export.png"));
    exportBtn->setIconSize(QSize(20, 20));
    exportBtn->setFixedSize(40, 40);
    exportBtn->setStyleSheet(StyleHelper::getButtonStyle());
    exportBtn->setToolTip("导出");
    
    topLayout->addWidget(searchBox);
    topLayout->addWidget(searchBtn);
    topLayout->addWidget(refreshBtn);
    topLayout->addWidget(filterBtn);
    topLayout->addWidget(exportBtn);
    topLayout->addStretch();

    // 预警信息表格
    warningTable = new QTableWidget(0, 8, tab);
    warningTable->setHorizontalHeaderLabels({"项目名称", "编号", "预警级别", "预警类别", "预警坐标", "预警深度", "预警阈值", "预警时间"});
    warningTable->setStyleSheet(StyleHelper::getTableStyle());
    warningTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    warningTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

    // 搜索栏和按钮
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QLineEdit *searchBox = new QLineEdit(topWidget);
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(250);
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    searchBtn->setToolTip("搜索");
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    refreshBtn->setToolTip("还原");
    
    QPushButton *filterBtn = new QPushButton(topWidget);
    filterBtn->setIcon(QIcon(":/icons/filter.png"));
    filterBtn->setIconSize(QSize(20, 20));
    filterBtn->setFixedSize(40, 40);
    filterBtn->setStyleSheet(StyleHelper::getButtonStyle());
    filterBtn->setToolTip("筛选");
    
    QPushButton *exportBtn = new QPushButton(topWidget);
    exportBtn->setIcon(QIcon(":/icons/export.png"));
    exportBtn->setIconSize(QSize(20, 20));
    exportBtn->setFixedSize(40, 40);
    exportBtn->setStyleSheet(StyleHelper::getButtonStyle());
    exportBtn->setToolTip("导出");
    
    topLayout->addWidget(searchBox);
    topLayout->addWidget(searchBtn);
    topLayout->addWidget(refreshBtn);
    topLayout->addWidget(filterBtn);
    topLayout->addWidget(exportBtn);
    topLayout->addStretch();

    // 掘进参数表格
    excavationTable = new QTableWidget(0, 8, tab);
    excavationTable->setHorizontalHeaderLabels({"项目名称", "掘进时间", "掘进坐标/桩号", "掘进模式", "土仓土压力", "千斤顶推力", "刀盘转速", "刀盘扭矩"});
    excavationTable->setStyleSheet(StyleHelper::getTableStyle());
    excavationTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    excavationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    excavationTable->setAlternatingRowColors(true);

    layout->addWidget(topWidget);
    layout->addWidget(excavationTable);

    tabWidget->addTab(tab, "掘进信息");
}

void ProjectManagementWindow::createSupplementaryDataTab()
{
    QWidget *tab = new QWidget(tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 搜索栏和按钮
    QWidget *topWidget = new QWidget(tab);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    
    QLineEdit *searchBox = new QLineEdit(topWidget);
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(250);
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    searchBtn->setToolTip("搜索");
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    refreshBtn->setToolTip("还原");
    
    QPushButton *filterBtn = new QPushButton(topWidget);
    filterBtn->setIcon(QIcon(":/icons/filter.png"));
    filterBtn->setIconSize(QSize(20, 20));
    filterBtn->setFixedSize(40, 40);
    filterBtn->setStyleSheet(StyleHelper::getButtonStyle());
    filterBtn->setToolTip("筛选");
    
    QPushButton *exportBtn = new QPushButton(topWidget);
    exportBtn->setIcon(QIcon(":/icons/export.png"));
    exportBtn->setIconSize(QSize(20, 20));
    exportBtn->setFixedSize(40, 40);
    exportBtn->setStyleSheet(StyleHelper::getButtonStyle());
    exportBtn->setToolTip("导出");
    
    topLayout->addWidget(searchBox);
    topLayout->addWidget(searchBtn);
    topLayout->addWidget(refreshBtn);
    topLayout->addWidget(filterBtn);
    topLayout->addWidget(exportBtn);
    topLayout->addStretch();

    // 补勘数据表格
    supplementaryTable = new QTableWidget(0, 10, tab);
    supplementaryTable->setHorizontalHeaderLabels({"项目名称", "掘进时间", "掘进坐标/桩号", "刀盘受力", "刀具贯入阻力", 
                                                   "刀盘正面摩擦力矩", "视电阻率", "前方5m含水概率", "应力梯度", "前方岩石物性参数"});
    supplementaryTable->setStyleSheet(StyleHelper::getTableStyle());
    supplementaryTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    supplementaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    supplementaryTable->setAlternatingRowColors(true);

    layout->addWidget(topWidget);
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
    newsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 设置默认行高
    newsTable->verticalHeader()->setDefaultSectionSize(40);
    
    newsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    newsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    newsTable->setColumnWidth(2, 140);
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
        
        // 操作列 - 简化设计
        QWidget *operationWidget = new QWidget();
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(5, 5, 5, 5);
        operationLayout->setSpacing(8);
        
        QPushButton *editBtn = new QPushButton("修改", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除", operationWidget);
        
        // 简化样式 - 按钮改小
        editBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #4A90E2;
                color: white;
                border: none;
                border-radius: 3px;
                font-size: 12px;
            }
            QPushButton:hover {
                background-color: #357ABD;
            }
        )");
        
        deleteBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #E74C3C;
                color: white;
                border: none;
                border-radius: 3px;
                font-size: 12px;
            }
            QPushButton:hover {
                background-color: #C0392B;
            }
        )");
        
        // 改小按钮尺寸
        editBtn->setFixedSize(50, 24);
        deleteBtn->setFixedSize(50, 24);
        
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
    
    // 设置列宽:新闻内容占更多空间,操作栏合适宽度
    newsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    newsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    newsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    newsTable->setColumnWidth(2, 160);  // 增加操作列宽度到160px
    
    QStringList news[] = {
        {"北京地铁在建线路11条线（段）盾构法施工区间占比68%", "2024-12-3 9:50:46"},
        {"甘肃天陇铁路柳林隧道正洞掘进破万米大关", "2024-12-3 9:50:51"},
        {"宜兴高铁长岗岭隧道顺利贯通", "2024-12-3 9:50:56"},
        {"云兰高速全线12座隧道贯通", "2024-12-3 9:50:01"}
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 2; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(news[row][col]);
            if (col == 0) {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            } else {
                item->setTextAlignment(Qt::AlignCenter);
            }
            newsTable->setItem(row, col, item);
        }
        
        // 操作列 - 简化设计
        QWidget *operationWidget = new QWidget();
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(5, 5, 5, 5);
        operationLayout->setSpacing(8);
        
        QPushButton *editBtn = new QPushButton("编辑", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除", operationWidget);
        
        // 简化样式 - 按钮改小
        editBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #4A90E2;
                color: white;
                border: none;
                border-radius: 3px;
                font-size: 12px;
            }
            QPushButton:hover {
                background-color: #357ABD;
            }
        )");
        
        deleteBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #E74C3C;
                color: white;
                border: none;
                border-radius: 3px;
                font-size: 12px;
            }
            QPushButton:hover {
                background-color: #C0392B;
            }
        )");
        
        // 改小按钮尺寸
        editBtn->setFixedSize(50, 24);
        deleteBtn->setFixedSize(50, 24);
        
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
    dialog.setStyleSheet("QDialog { background-color: white; }");

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
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("提示");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("项目创建成功！");
        msgBox.setStyleSheet("QMessageBox { background-color: white; }");
        msgBox.exec();
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
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("编辑项目");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(QString("编辑项目：%1").arg(projectTable->item(row, 0)->text()));
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    msgBox.exec();
}

void ProjectManagementWindow::onDeleteProject(int row)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("删除项目");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString("确定要删除项目 '%1' 吗？").arg(projectTable->item(row, 0)->text()));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    
    if (msgBox.exec() == QMessageBox::Yes) {
        projectTable->removeRow(row);
        QMessageBox resultBox(this);
        resultBox.setWindowTitle("提示");
        resultBox.setIcon(QMessageBox::Information);
        resultBox.setText("项目已删除！");
        resultBox.setStyleSheet("QMessageBox { background-color: white; }");
        resultBox.exec();
    }
}

void ProjectManagementWindow::onTabChanged(int index)
{
    // 可以在这里处理标签页切换事件
}
