#include "projectmanagementwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include "../utils/GeoDataImporter.h"
#include "../database/ProjectDAO.h"
#include "../database/WarningDAO.h"
#include "../database/NewsDAO.h"
#include "../database/ExcavationParameterDAO.h"
#include "../database/ProspectingDataDAO.h"
#include "../models/Project.h"
#include "../models/Warning.h"
#include "../models/News.h"
#include "../models/ProspectingData.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QFileDialog>
#include <QApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QSqlError>
#include <QCheckBox>
#include <QTextEdit>
#include <QTextStream>
#include <QStringConverter>

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
    
    // 添加"导入地质数据"按钮
    QPushButton *importGeoDataButton = new QPushButton("📊 导入地质数据", topWidget);
    importGeoDataButton->setStyleSheet(StyleHelper::getButtonStyle() + " QPushButton { background-color: #2196F3; }");
    importGeoDataButton->setMinimumHeight(40);
    importGeoDataButton->setToolTip("为选中的项目导入钻孔数据和隧道轮廓数据");
    connect(importGeoDataButton, &QPushButton::clicked, this, &ProjectManagementWindow::onImportGeoData);
    topLayout->addWidget(importGeoDataButton);

    // 项目表格
    projectTable = new QTableWidget(0, 8, tab);
    projectTable->setHorizontalHeaderLabels({"项目名称", "简介", "经纬度坐标", "施工单位", "开始时间", "施工进度", "地理位置", "操作"});
    projectTable->setStyleSheet(StyleHelper::getTableStyle());
    
    // 设置表格的SizePolicy
    projectTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 问题2彻底修复：设置表格默认行高为50px，确保按钮有足够垂直空间
    projectTable->verticalHeader()->setDefaultSectionSize(50);
    
    // 问题2彻底修复：先设置所有列为Stretch
    projectTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 问题2彻底修复：然后将操作列设置为Fixed，宽度为200px
    projectTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    projectTable->setColumnWidth(7, 200);  // 增加到200px确保完整显示
    
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
    searchBox->setObjectName("excavationSearchBox");  // 设置对象名以便后续访问
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(250);
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    searchBtn->setToolTip("搜索");
    connect(searchBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onSearchExcavation);
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    refreshBtn->setToolTip("还原");
    connect(refreshBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onRefreshExcavation);
    
    QPushButton *filterBtn = new QPushButton(topWidget);
    filterBtn->setIcon(QIcon(":/icons/filter.png"));
    filterBtn->setIconSize(QSize(20, 20));
    filterBtn->setFixedSize(40, 40);
    filterBtn->setStyleSheet(StyleHelper::getButtonStyle());
    filterBtn->setToolTip("筛选");
    connect(filterBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onFilterExcavation);
    
    QPushButton *exportBtn = new QPushButton(topWidget);
    exportBtn->setIcon(QIcon(":/icons/export.png"));
    exportBtn->setIconSize(QSize(20, 20));
    exportBtn->setFixedSize(40, 40);
    exportBtn->setStyleSheet(StyleHelper::getButtonStyle());
    exportBtn->setToolTip("导出");
    connect(exportBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onExportExcavation);
    
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
    searchBox->setObjectName("supplementarySearchBox");
    searchBox->setPlaceholderText("搜索");
    searchBox->setStyleSheet(StyleHelper::getInputStyle());
    searchBox->setMaximumWidth(250);
    
    QPushButton *searchBtn = new QPushButton(topWidget);
    searchBtn->setIcon(QIcon(":/icons/search.png"));
    searchBtn->setIconSize(QSize(20, 20));
    searchBtn->setFixedSize(40, 40);
    searchBtn->setStyleSheet(StyleHelper::getButtonStyle());
    searchBtn->setToolTip("搜索");
    connect(searchBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onSearchSupplementary);
    
    QPushButton *refreshBtn = new QPushButton(topWidget);
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    refreshBtn->setIconSize(QSize(20, 20));
    refreshBtn->setFixedSize(40, 40);
    refreshBtn->setStyleSheet(StyleHelper::getButtonStyle());
    refreshBtn->setToolTip("还原");
    connect(refreshBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onRefreshSupplementary);
    
    QPushButton *filterBtn = new QPushButton(topWidget);
    filterBtn->setIcon(QIcon(":/icons/filter.png"));
    filterBtn->setIconSize(QSize(20, 20));
    filterBtn->setFixedSize(40, 40);
    filterBtn->setStyleSheet(StyleHelper::getButtonStyle());
    filterBtn->setToolTip("筛选");
    connect(filterBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onFilterSupplementary);
    
    QPushButton *exportBtn = new QPushButton(topWidget);
    exportBtn->setIcon(QIcon(":/icons/export.png"));
    exportBtn->setIconSize(QSize(20, 20));
    exportBtn->setFixedSize(40, 40);
    exportBtn->setStyleSheet(StyleHelper::getButtonStyle());
    exportBtn->setToolTip("导出");
    connect(exportBtn, &QPushButton::clicked, this, &ProjectManagementWindow::onExportSupplementary);
    
    topLayout->addWidget(searchBox);
    topLayout->addWidget(searchBtn);
    topLayout->addWidget(refreshBtn);
    topLayout->addWidget(filterBtn);
    topLayout->addWidget(exportBtn);
    topLayout->addStretch();

    // 补勘数据表格 - 扩展列数以包含更多字段
    supplementaryTable = new QTableWidget(0, 18, tab);
    supplementaryTable->setHorizontalHeaderLabels({
        "项目名称", "掘进时间", "桩号", "刀盘受力", "刀具贯入阻力", 
        "刀盘正面摩擦力矩", "视电阻率", "前方5m含水概率", "应力梯度", 
        "前方岩石物性参数", "围岩危险等级", "横纵波反射系数", "横波波速", 
        "纵波波速", "横纵波速比", "泊松比", "杨氏模量", "岩层类型"
    });
    supplementaryTable->setStyleSheet(StyleHelper::getTableStyle());
    supplementaryTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    supplementaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    supplementaryTable->setAlternatingRowColors(true);
    supplementaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // 设置默认列宽
    supplementaryTable->setColumnWidth(0, 120);  // 项目名称
    supplementaryTable->setColumnWidth(1, 150);  // 掘进时间
    supplementaryTable->setColumnWidth(2, 100);  // 桩号

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
    
    publishNewsButton = new QPushButton("发布新闻", topWidget);
    publishNewsButton->setStyleSheet(StyleHelper::getButtonStyle());
    publishNewsButton->setMinimumHeight(40);
    connect(publishNewsButton, &QPushButton::clicked, this, &ProjectManagementWindow::onPublishNews);
    
    deleteNewsButton = new QPushButton("删除所选", topWidget);
    deleteNewsButton->setStyleSheet(StyleHelper::getButtonStyle());
    deleteNewsButton->setMinimumHeight(40);
    connect(deleteNewsButton, &QPushButton::clicked, this, &ProjectManagementWindow::onDeleteSelectedNews);
    
    topLayout->addWidget(publishNewsButton);
    topLayout->addWidget(deleteNewsButton);
    topLayout->addStretch();

    // 新闻表格
    newsTable = new QTableWidget(0, 4, tab);
    newsTable->setHorizontalHeaderLabels({"☐", "新闻内容", "发布时间", "操作"});
    newsTable->setStyleSheet(StyleHelper::getTableStyle());
    newsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 问题2彻底修复：设置默认行高为50px
    newsTable->verticalHeader()->setDefaultSectionSize(50);
    
    // 设置列宽
    newsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    newsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);  // 复选框列
    newsTable->setColumnWidth(0, 50);
    newsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);  // 操作列
    newsTable->setColumnWidth(3, 200);
    newsTable->setAlternatingRowColors(true);

    layout->addWidget(topWidget);
    layout->addWidget(newsTable);

    tabWidget->addTab(tab, "新闻模块");
}

void ProjectManagementWindow::loadProjectData()
{
    // 创建DAO对象
    ProjectDAO projectDAO;
    WarningDAO warningDAO;
    
    // ========== 加载项目总览数据 ==========
    QList<Project> projects = projectDAO.getProjectsByStatus("active");
    projectTable->setRowCount(projects.size());
    
    for (int row = 0; row < projects.size(); row++) {
        const Project &project = projects[row];
        
        // 填充项目数据
        projectTable->setItem(row, 0, new QTableWidgetItem(project.getProjectName()));
        projectTable->setItem(row, 1, new QTableWidgetItem(project.getBrief()));
        // 组合经纬度坐标
        QString coordinates = QString("%1,%2").arg(project.getLatitude()).arg(project.getLongitude());
        projectTable->setItem(row, 2, new QTableWidgetItem(coordinates));
        projectTable->setItem(row, 3, new QTableWidgetItem(project.getConstructionUnit()));
        projectTable->setItem(row, 4, new QTableWidgetItem(project.getStartDate()));
        projectTable->setItem(row, 5, new QTableWidgetItem(QString("%1%").arg(QString::number(project.getProgress(), 'f', 1))));
        projectTable->setItem(row, 6, new QTableWidgetItem(project.getLocation()));
        
        // 设置居中对齐
        for (int col = 0; col < 7; col++) {
            projectTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
        
        // 创建操作按钮
        QWidget *operationWidget = new QWidget();
        operationWidget->setMinimumHeight(40);
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(10, 5, 10, 5);
        operationLayout->setSpacing(12);
        
        QPushButton *editBtn = new QPushButton("修改", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除", operationWidget);
        
        QString buttonStyle = R"(
            QPushButton {
                background-color: %1;
                color: white;
                border: none;
                border-radius: 4px;
                font-size: 13px;
                font-weight: bold;
                padding: 0px;
            }
            QPushButton:hover {
                background-color: %2;
            }
        )";
        
        editBtn->setStyleSheet(buttonStyle.arg("#4A90E2").arg("#357ABD"));
        deleteBtn->setStyleSheet(buttonStyle.arg("#E74C3C").arg("#C0392B"));
        
        editBtn->setFixedSize(70, 32);
        deleteBtn->setFixedSize(70, 32);
        
        connect(editBtn, &QPushButton::clicked, [this, row]() { onEditProject(row); });
        connect(deleteBtn, &QPushButton::clicked, [this, row]() { onDeleteProject(row); });
        
        operationLayout->addWidget(editBtn);
        operationLayout->addWidget(deleteBtn);
        
        projectTable->setCellWidget(row, 7, operationWidget);
    }
    
    // ========== 加载预警信息数据 ==========
    QList<Warning> warnings = warningDAO.getAllWarnings();
    warningTable->setRowCount(warnings.size());
    
    for (int row = 0; row < warnings.size(); row++) {
        const Warning &warning = warnings[row];
        
        // 根据项目ID获取项目名称
        Project project = projectDAO.getProjectById(warning.getProjectId());
        QString projectName = project.isValid() ? project.getProjectName() : "未知项目";
        
        warningTable->setItem(row, 0, new QTableWidgetItem(projectName));
        warningTable->setItem(row, 1, new QTableWidgetItem(QString::number(warning.getWarningId())));
        warningTable->setItem(row, 2, new QTableWidgetItem(warning.getWarningLevel()));
        warningTable->setItem(row, 3, new QTableWidgetItem(warning.getWarningType()));
        // 组合经纬度坐标
        QString warningCoordinates = QString("%1,%2").arg(warning.getLatitude()).arg(warning.getLongitude());
        warningTable->setItem(row, 4, new QTableWidgetItem(warningCoordinates));
        // 深度
        warningTable->setItem(row, 5, new QTableWidgetItem(QString::number(warning.getDepth())));
        // 阈值
        warningTable->setItem(row, 6, new QTableWidgetItem(QString::number(warning.getThresholdValue())));
        // 时间 - 将QDateTime转换为QString
        warningTable->setItem(row, 7, new QTableWidgetItem(warning.getWarningTime().toString("yyyy-MM-dd hh:mm:ss")));
        
        for (int col = 0; col < 8; col++) {
            warningTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }
    
    // ========== 加载掘进信息数据（暂时清空，等待数据） ==========
    excavationTable->setRowCount(0);
    
    // ========== 【修复】加载补勘数据 ==========
    supplementaryTable->setRowCount(0);
    
    ProspectingDataDAO prospectingDAO;
    QList<ProspectingData> prospectingList = prospectingDAO.getAllProspectingData();
    
    qDebug() << "加载补勘数据，共" << prospectingList.size() << "条";
    
    for (const ProspectingData &data : prospectingList) {
        int row = supplementaryTable->rowCount();
        supplementaryTable->insertRow(row);
        
        // 获取项目名称
        ProjectDAO projectDAO;
        Project project = projectDAO.getProjectById(data.getProjectId());
        
        // 填充表格数据
        supplementaryTable->setItem(row, 0, new QTableWidgetItem(project.getProjectName()));
        supplementaryTable->setItem(row, 1, new QTableWidgetItem(data.getExcavationTime().toString("yyyy-MM-dd HH:mm")));
        supplementaryTable->setItem(row, 2, new QTableWidgetItem(data.getStakeMark()));
        supplementaryTable->setItem(row, 3, new QTableWidgetItem(QString::number(data.getCutterForce(), 'f', 2)));
        supplementaryTable->setItem(row, 4, new QTableWidgetItem(QString::number(data.getCutterPenetrationResistance(), 'f', 2)));
        supplementaryTable->setItem(row, 5, new QTableWidgetItem(QString::number(data.getFaceFrictionTorque(), 'f', 2)));
        supplementaryTable->setItem(row, 6, new QTableWidgetItem(QString::number(data.getApparentResistivity(), 'f', 2)));
        supplementaryTable->setItem(row, 7, new QTableWidgetItem(QString::number(data.getWaterProbability(), 'f', 2)));
        supplementaryTable->setItem(row, 8, new QTableWidgetItem(QString::number(data.getStressGradient(), 'f', 4)));
        supplementaryTable->setItem(row, 9, new QTableWidgetItem(data.getRockProperties()));
        
        // 设置所有单元格为居中对齐
        for (int i = 0; i < 10; ++i) {
            supplementaryTable->item(row, i)->setTextAlignment(Qt::AlignCenter);
        }
    }
    
    // ========== 加载新闻数据 ==========
    NewsDAO newsDAO;
    QList<News> newsList = newsDAO.getAllNews();
    
    newsTable->setRowCount(newsList.size());
    
    for (int row = 0; row < newsList.size(); row++) {
        const News &news = newsList[row];
        
        // 添加复选框列（第0列）
        QWidget *checkboxWidget = new QWidget();
        QHBoxLayout *checkboxLayout = new QHBoxLayout(checkboxWidget);
        checkboxLayout->setContentsMargins(0, 0, 0, 0);
        checkboxLayout->setAlignment(Qt::AlignCenter);
        QCheckBox *checkbox = new QCheckBox();
        checkbox->setProperty("newsId", news.getNewsId());  // 保存新闻ID
        checkboxLayout->addWidget(checkbox);
        newsTable->setCellWidget(row, 0, checkboxWidget);
        
        // 新闻内容（第1列）
        newsTable->setItem(row, 1, new QTableWidgetItem(news.getNewsContent()));
        // 发布时间（第2列）
        newsTable->setItem(row, 2, new QTableWidgetItem(news.getPublishTime().toString("yyyy-MM-dd hh:mm:ss")));
        
        newsTable->item(row, 1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        newsTable->item(row, 2)->setTextAlignment(Qt::AlignCenter);
        
        // 创建操作按钮（第3列）
        QWidget *operationWidget = new QWidget();
        operationWidget->setMinimumHeight(40);
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(10, 5, 10, 5);
        operationLayout->setSpacing(12);
        
        QPushButton *editBtn = new QPushButton("编辑", operationWidget);
        QPushButton *deleteBtn = new QPushButton("删除", operationWidget);
        
        QString buttonStyle = R"(
            QPushButton {
                background-color: %1;
                color: white;
                border: none;
                border-radius: 4px;
                font-size: 13px;
                font-weight: bold;
                padding: 0px;
            }
            QPushButton:hover {
                background-color: %2;
            }
        )";
        
        editBtn->setStyleSheet(buttonStyle.arg("#4A90E2").arg("#357ABD"));
        deleteBtn->setStyleSheet(buttonStyle.arg("#E74C3C").arg("#C0392B"));
        
        editBtn->setFixedSize(70, 32);
        deleteBtn->setFixedSize(70, 32);
        
        // 连接信号（使用lambda传递行号）
        connect(editBtn, &QPushButton::clicked, this, [this, row]() {
            onEditNews(row);
        });
        connect(deleteBtn, &QPushButton::clicked, this, [this, row]() {
            onDeleteNews(row);
        });
        
        operationLayout->addWidget(editBtn);
        operationLayout->addWidget(deleteBtn);
        
        newsTable->setCellWidget(row, 3, operationWidget);
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
    // 当切换到掘进信息标签页（index=2）时加载数据
    if (index == 2) {
        loadExcavationData();
    }
    // 当切换到补勘数据标签页（index=3）时加载数据
    else if (index == 3) {
        loadSupplementaryData();
    }
}

void ProjectManagementWindow::onPublishNews()
{
    // 创建发布新闻对话框
    QDialog dialog(this);
    dialog.setWindowTitle("发布新闻");
    dialog.setFixedSize(600, 300);
    dialog.setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *label = new QLabel("新闻内容：", &dialog);
    label->setStyleSheet("font-size: 14px; font-weight: bold;");

    QTextEdit *contentEdit = new QTextEdit(&dialog);
    contentEdit->setPlaceholderText("请输入新闻内容...");
    contentEdit->setStyleSheet(StyleHelper::getInputStyle());
    contentEdit->setMinimumHeight(150);

    layout->addWidget(label);
    layout->addWidget(contentEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("发布");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QString newsContent = contentEdit->toPlainText().trimmed();
        
        if (newsContent.isEmpty()) {
            QMessageBox::warning(this, "警告", "新闻内容不能为空！");
            return;
        }

        // 创建新闻对象并保存到数据库
        News news;
        news.setNewsContent(newsContent);
        news.setPublishTime(QDateTime::currentDateTime());
        news.setCreatedBy(1);  // 假设当前用户ID为1，实际应该从登录信息获取

        NewsDAO newsDAO;
        if (newsDAO.addNews(news)) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("成功");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("新闻发布成功！");
            msgBox.setStyleSheet("QMessageBox { background-color: white; }");
            msgBox.exec();
            loadProjectData();  // 重新加载数据
        } else {
            QMessageBox::critical(this, "错误", "新闻发布失败：" + newsDAO.getLastError());
        }
    }
}

void ProjectManagementWindow::onDeleteSelectedNews()
{
    // 收集选中的新闻ID
    QList<int> selectedNewsIds;
    
    for (int row = 0; row < newsTable->rowCount(); row++) {
        QWidget *checkboxWidget = newsTable->cellWidget(row, 0);
        if (checkboxWidget) {
            QCheckBox *checkbox = checkboxWidget->findChild<QCheckBox*>();
            if (checkbox && checkbox->isChecked()) {
                int newsId = checkbox->property("newsId").toInt();
                selectedNewsIds.append(newsId);
            }
        }
    }
    
    if (selectedNewsIds.isEmpty()) {
        QMessageBox::warning(this, "警告", "请至少选择一条新闻！");
        return;
    }
    
    // 确认删除
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认删除");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString("确定要删除选中的 %1 条新闻吗？").arg(selectedNewsIds.size()));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    
    if (msgBox.exec() == QMessageBox::Yes) {
        NewsDAO newsDAO;
        if (newsDAO.deleteNewsList(selectedNewsIds)) {
            QMessageBox resultBox(this);
            resultBox.setWindowTitle("成功");
            resultBox.setIcon(QMessageBox::Information);
            resultBox.setText("新闻删除成功！");
            resultBox.setStyleSheet("QMessageBox { background-color: white; }");
            resultBox.exec();
            loadProjectData();  // 重新加载数据
        } else {
            QMessageBox::critical(this, "错误", "删除新闻失败：" + newsDAO.getLastError());
        }
    }
}

void ProjectManagementWindow::onEditNews(int row)
{
    if (row < 0 || row >= newsTable->rowCount()) {
        return;
    }
    
    // 获取当前新闻内容（第1列）
    QString currentContent = newsTable->item(row, 1)->text();
    
    // 获取新闻ID（第0列的复选框）
    QWidget *checkboxWidget = newsTable->cellWidget(row, 0);
    int newsId = 0;
    if (checkboxWidget) {
        QCheckBox *checkbox = checkboxWidget->findChild<QCheckBox*>();
        if (checkbox) {
            newsId = checkbox->property("newsId").toInt();
        }
    }
    
    // 创建编辑对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑新闻");
    dialog.setFixedSize(600, 300);
    dialog.setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *label = new QLabel("新闻内容：", &dialog);
    label->setStyleSheet("font-size: 14px; font-weight: bold;");

    QTextEdit *contentEdit = new QTextEdit(&dialog);
    contentEdit->setPlainText(currentContent);
    contentEdit->setStyleSheet(StyleHelper::getInputStyle());
    contentEdit->setMinimumHeight(150);

    layout->addWidget(label);
    layout->addWidget(contentEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("保存");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QString newContent = contentEdit->toPlainText().trimmed();
        
        if (newContent.isEmpty()) {
            QMessageBox::warning(this, "警告", "新闻内容不能为空！");
            return;
        }

        // 更新新闻
        NewsDAO newsDAO;
        News news = newsDAO.getNewsById(newsId);
        if (news.isValid()) {
            news.setNewsContent(newContent);
            news.setPublishTime(QDateTime::currentDateTime());  // 更新发布时间
            
            if (newsDAO.updateNews(news)) {
                QMessageBox resultBox(this);
                resultBox.setWindowTitle("成功");
                resultBox.setIcon(QMessageBox::Information);
                resultBox.setText("新闻更新成功！");
                resultBox.setStyleSheet("QMessageBox { background-color: white; }");
                resultBox.exec();
                loadProjectData();  // 重新加载数据
            } else {
                QMessageBox::critical(this, "错误", "更新新闻失败：" + newsDAO.getLastError());
            }
        }
    }
}

void ProjectManagementWindow::onDeleteNews(int row)
{
    if (row < 0 || row >= newsTable->rowCount()) {
        return;
    }
    
    // 获取新闻ID（第0列的复选框）
    QWidget *checkboxWidget = newsTable->cellWidget(row, 0);
    int newsId = 0;
    if (checkboxWidget) {
        QCheckBox *checkbox = checkboxWidget->findChild<QCheckBox*>();
        if (checkbox) {
            newsId = checkbox->property("newsId").toInt();
        }
    }
    
    // 获取新闻内容（第1列）
    QString newsContent = newsTable->item(row, 1)->text();
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认删除");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString("确定要删除这条新闻吗？\n\n%1").arg(newsContent.left(50) + "..."));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    
    if (msgBox.exec() == QMessageBox::Yes) {
        NewsDAO newsDAO;
        if (newsDAO.deleteNews(newsId)) {
            QMessageBox resultBox(this);
            resultBox.setWindowTitle("成功");
            resultBox.setIcon(QMessageBox::Information);
            resultBox.setText("新闻删除成功！");
            resultBox.setStyleSheet("QMessageBox { background-color: white; }");
            resultBox.exec();
            loadProjectData();  // 重新加载数据
        } else {
            QMessageBox::critical(this, "错误", "删除新闻失败：" + newsDAO.getLastError());
        }
    }
}

void ProjectManagementWindow::onImportGeoData()
{
    // 获取选中的项目
    QList<QTableWidgetItem*> selectedItems = projectTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString::fromUtf8("提示"));
        msgBox.setText(QString::fromUtf8("请先选择一个项目！"));
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: #0078d4; color: white; "
                            "border-radius: 4px; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    // 获取选中行的第一列（项目名称）
    int row = projectTable->selectedItems().first()->row();
    QString projectName = projectTable->item(row, 0)->text();
    
    // 从数据库获取项目ID
    ProjectDAO projectDAO;
    QVector<Project> projects = projectDAO.getAllProjects();
    int projectId = -1;
    
    for (const auto &project : projects) {
        if (project.getProjectName() == projectName) {
            projectId = project.getProjectId();
            break;
        }
    }
    
    if (projectId == -1) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(QString::fromUtf8("错误"));
        msgBox.setText(QString::fromUtf8("未找到项目ID"));
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: #0078d4; color: white; "
                            "border-radius: 4px; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    // 创建并显示导入对话框
    GeoDataImporter *importer = new GeoDataImporter(projectId, projectName, this);
    
    // 连接信号
    connect(importer, &GeoDataImporter::importCompleted, [this]() {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString::fromUtf8("成功"));
        msgBox.setText(QString::fromUtf8("地质数据导入完成！"));
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: #0078d4; color: white; "
                            "border-radius: 4px; padding: 5px 15px; }");
        msgBox.exec();
        loadProjectData();  // 重新加载数据
    });
    
    importer->show();
}

// 加载掘进信息数据
void ProjectManagementWindow::loadExcavationData()
{
    excavationTable->setRowCount(0);  // 清空表格
    
    // 从数据库查询所有项目及其掘进参数
    ProjectDAO projectDAO;
    ExcavationParameterDAO excavDAO;
    
    QList<Project> projects = projectDAO.getAllProjects();
    
    for (const Project &project : projects) {
        // 查询该项目的所有掘进参数（按时间降序）
        QList<ExcavationParameter> params = excavDAO.getExcavationParametersByPage(
            project.getProjectId(), 1, 100);  // 默认显示最近100条
        
        for (const ExcavationParameter &param : params) {
            int row = excavationTable->rowCount();
            excavationTable->insertRow(row);
            
            excavationTable->setItem(row, 0, new QTableWidgetItem(project.getProjectName()));
            excavationTable->setItem(row, 1, new QTableWidgetItem(
                param.getExcavationTime().toString("yyyy-MM-dd HH:mm:ss")));
            excavationTable->setItem(row, 2, new QTableWidgetItem(param.getStakeMark()));
            excavationTable->setItem(row, 3, new QTableWidgetItem(param.getExcavationMode()));
            excavationTable->setItem(row, 4, new QTableWidgetItem(
                QString::number(param.getChamberPressure(), 'f', 2)));
            excavationTable->setItem(row, 5, new QTableWidgetItem(
                QString::number(param.getThrustForce(), 'f', 0)));
            excavationTable->setItem(row, 6, new QTableWidgetItem(
                QString::number(param.getCutterSpeed(), 'f', 1)));
            excavationTable->setItem(row, 7, new QTableWidgetItem(
                QString::number(param.getCutterTorque(), 'f', 0)));
        }
    }
}

// 搜索掘进信息
void ProjectManagementWindow::onSearchExcavation()
{
    QLineEdit *searchBox = excavationTable->parentWidget()->findChild<QLineEdit*>("excavationSearchBox");
    if (!searchBox) return;
    
    QString keyword = searchBox->text().trimmed();
    if (keyword.isEmpty()) {
        loadExcavationData();  // 如果搜索框为空，显示所有数据
        return;
    }
    
    // 隐藏不匹配的行
    for (int row = 0; row < excavationTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < excavationTable->columnCount(); ++col) {
            QTableWidgetItem *item = excavationTable->item(row, col);
            if (item && item->text().contains(keyword, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        excavationTable->setRowHidden(row, !match);
    }
}

// 刷新掘进信息
void ProjectManagementWindow::onRefreshExcavation()
{
    loadExcavationData();
    
    // 清空搜索框
    QLineEdit *searchBox = excavationTable->parentWidget()->findChild<QLineEdit*>("excavationSearchBox");
    if (searchBox) {
        searchBox->clear();
    }
    
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("提示");
    msgBox.setText("数据已刷新");
    msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                         "QLabel { color: black; } "
                         "QPushButton { background-color: #0078d4; color: white; "
                         "border-radius: 4px; padding: 5px 15px; }");
    msgBox.exec();
}

// 筛选掘进信息
void ProjectManagementWindow::onFilterExcavation()
{
    // 创建筛选对话框
    QDialog dialog(this);
    dialog.setWindowTitle("筛选掘进信息");
    dialog.setFixedSize(500, 400);
    dialog.setStyleSheet("QDialog { background-color: white; }");
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // 项目选择
    QLabel *projectLabel = new QLabel("选择项目：", &dialog);
    QComboBox *projectCombo = new QComboBox(&dialog);
    projectCombo->addItem("全部项目", 0);
    
    ProjectDAO projectDAO;
    QList<Project> projects = projectDAO.getAllProjects();
    for (const Project &project : projects) {
        projectCombo->addItem(project.getProjectName(), project.getProjectId());
    }
    
    // 时间范围
    QLabel *timeLabel = new QLabel("时间范围：", &dialog);
    QHBoxLayout *timeLayout = new QHBoxLayout();
    QDateTimeEdit *startTime = new QDateTimeEdit(&dialog);
    startTime->setDateTime(QDateTime::currentDateTime().addDays(-7));
    startTime->setDisplayFormat("yyyy-MM-dd HH:mm");
    QLabel *toLabel = new QLabel("至", &dialog);
    QDateTimeEdit *endTime = new QDateTimeEdit(&dialog);
    endTime->setDateTime(QDateTime::currentDateTime());
    endTime->setDisplayFormat("yyyy-MM-dd HH:mm");
    timeLayout->addWidget(startTime);
    timeLayout->addWidget(toLabel);
    timeLayout->addWidget(endTime);
    
    layout->addWidget(projectLabel);
    layout->addWidget(projectCombo);
    layout->addWidget(timeLabel);
    layout->addLayout(timeLayout);
    layout->addStretch();
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("筛选");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    layout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        excavationTable->setRowCount(0);
        
        int selectedProjectId = projectCombo->currentData().toInt();
        QDateTime start = startTime->dateTime();
        QDateTime end = endTime->dateTime();
        
        ExcavationParameterDAO excavDAO;
        ProjectDAO projDAO;
        
        if (selectedProjectId == 0) {
            // 全部项目
            for (const Project &project : projects) {
                QList<ExcavationParameter> params = excavDAO.getExcavationParametersByTimeRange(
                    project.getProjectId(), start, end);
                
                for (const ExcavationParameter &param : params) {
                    int row = excavationTable->rowCount();
                    excavationTable->insertRow(row);
                    
                    excavationTable->setItem(row, 0, new QTableWidgetItem(project.getProjectName()));
                    excavationTable->setItem(row, 1, new QTableWidgetItem(
                        param.getExcavationTime().toString("yyyy-MM-dd HH:mm:ss")));
                    excavationTable->setItem(row, 2, new QTableWidgetItem(param.getStakeMark()));
                    excavationTable->setItem(row, 3, new QTableWidgetItem(param.getExcavationMode()));
                    excavationTable->setItem(row, 4, new QTableWidgetItem(
                        QString::number(param.getChamberPressure(), 'f', 2)));
                    excavationTable->setItem(row, 5, new QTableWidgetItem(
                        QString::number(param.getThrustForce(), 'f', 0)));
                    excavationTable->setItem(row, 6, new QTableWidgetItem(
                        QString::number(param.getCutterSpeed(), 'f', 1)));
                    excavationTable->setItem(row, 7, new QTableWidgetItem(
                        QString::number(param.getCutterTorque(), 'f', 0)));
                }
            }
        } else {
            // 指定项目
            QString projectName = projectCombo->currentText();
            QList<ExcavationParameter> params = excavDAO.getExcavationParametersByTimeRange(
                selectedProjectId, start, end);
            
            for (const ExcavationParameter &param : params) {
                int row = excavationTable->rowCount();
                excavationTable->insertRow(row);
                
                excavationTable->setItem(row, 0, new QTableWidgetItem(projectName));
                excavationTable->setItem(row, 1, new QTableWidgetItem(
                    param.getExcavationTime().toString("yyyy-MM-dd HH:mm:ss")));
                excavationTable->setItem(row, 2, new QTableWidgetItem(param.getStakeMark()));
                excavationTable->setItem(row, 3, new QTableWidgetItem(param.getExcavationMode()));
                excavationTable->setItem(row, 4, new QTableWidgetItem(
                    QString::number(param.getChamberPressure(), 'f', 2)));
                excavationTable->setItem(row, 5, new QTableWidgetItem(
                    QString::number(param.getThrustForce(), 'f', 0)));
                excavationTable->setItem(row, 6, new QTableWidgetItem(
                    QString::number(param.getCutterSpeed(), 'f', 1)));
                excavationTable->setItem(row, 7, new QTableWidgetItem(
                    QString::number(param.getCutterTorque(), 'f', 0)));
            }
        }
    }
}

// 导出掘进信息
void ProjectManagementWindow::onExportExcavation()
{
    if (excavationTable->rowCount() == 0) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("警告");
        msgBox.setText("没有数据可导出！");
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                             "QLabel { color: black; } "
                             "QPushButton { background-color: #0078d4; color: white; "
                             "border-radius: 4px; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this, "导出掘进信息", 
        QDir::homePath() + "/excavation_data.csv",
        "CSV文件 (*.csv)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("无法创建文件！");
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                             "QLabel { color: black; } "
                             "QPushButton { background-color: #0078d4; color: white; "
                             "border-radius: 4px; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);  // Qt 6方式
    
    // 写入表头
    QStringList headers;
    for (int col = 0; col < excavationTable->columnCount(); ++col) {
        headers << excavationTable->horizontalHeaderItem(col)->text();
    }
    out << headers.join(",") << "\n";
    
    // 写入数据
    for (int row = 0; row < excavationTable->rowCount(); ++row) {
        if (excavationTable->isRowHidden(row)) {
            continue;  // 跳过隐藏的行
        }
        
        QStringList rowData;
        for (int col = 0; col < excavationTable->columnCount(); ++col) {
            QTableWidgetItem *item = excavationTable->item(row, col);
            rowData << (item ? item->text() : "");
        }
        out << rowData.join(",") << "\n";
    }
    
    file.close();
    
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("成功");
    msgBox.setText(QString("数据已导出到：%1").arg(fileName));
    msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                         "QLabel { color: black; } "
                         "QPushButton { background-color: #0078d4; color: white; "
                         "border-radius: 4px; padding: 5px 15px; }");
    msgBox.exec();
}

// 加载补勘数据
void ProjectManagementWindow::loadSupplementaryData()
{
    supplementaryTable->setRowCount(0);
    
    // 从数据库加载所有补勘数据
    ProspectingDataDAO dao;
    ProjectDAO projectDAO;
    
    // 获取所有项目
    QList<Project> projects = projectDAO.getAllProjects();
    
    for (const Project &project : projects) {
        // 获取该项目的补勘数据
        QList<ProspectingData> dataList = dao.getProspectingDataByProjectId(project.getProjectId());
        
        for (const ProspectingData &data : dataList) {
            int row = supplementaryTable->rowCount();
            supplementaryTable->insertRow(row);
            
            // 设置数据
            supplementaryTable->setItem(row, 0, new QTableWidgetItem(project.getProjectName()));
            supplementaryTable->setItem(row, 1, new QTableWidgetItem(data.getExcavationTime().toString("yyyy-MM-dd HH:mm:ss")));
            supplementaryTable->setItem(row, 2, new QTableWidgetItem(data.getStakeMark()));
            supplementaryTable->setItem(row, 3, new QTableWidgetItem(QString::number(data.getCutterForce(), 'f', 2)));
            supplementaryTable->setItem(row, 4, new QTableWidgetItem(QString::number(data.getCutterPenetrationResistance(), 'f', 2)));
            supplementaryTable->setItem(row, 5, new QTableWidgetItem(QString::number(data.getFaceFrictionTorque(), 'f', 2)));
            supplementaryTable->setItem(row, 6, new QTableWidgetItem(QString::number(data.getApparentResistivity(), 'f', 2)));
            supplementaryTable->setItem(row, 7, new QTableWidgetItem(QString::number(data.getWaterProbability(), 'f', 1) + "%"));
            supplementaryTable->setItem(row, 8, new QTableWidgetItem(QString::number(data.getStressGradient(), 'f', 4)));
            supplementaryTable->setItem(row, 9, new QTableWidgetItem(data.getRockProperties()));
            supplementaryTable->setItem(row, 10, new QTableWidgetItem(data.getRockDangerLevel()));
            supplementaryTable->setItem(row, 11, new QTableWidgetItem(QString::number(data.getWaveReflectionCoeff(), 'f', 3)));
            supplementaryTable->setItem(row, 12, new QTableWidgetItem(QString::number(data.getSWaveVelocity(), 'f', 1)));
            supplementaryTable->setItem(row, 13, new QTableWidgetItem(QString::number(data.getPWaveVelocity(), 'f', 1)));
            supplementaryTable->setItem(row, 14, new QTableWidgetItem(QString::number(data.getWaveVelocityRatio(), 'f', 3)));
            supplementaryTable->setItem(row, 15, new QTableWidgetItem(QString::number(data.getPoissonRatio(), 'f', 3)));
            supplementaryTable->setItem(row, 16, new QTableWidgetItem(QString::number(data.getYoungsModulus(), 'f', 2)));
            supplementaryTable->setItem(row, 17, new QTableWidgetItem(data.getRockType()));
            
            // 设置所有单元格为只读
            for (int col = 0; col < supplementaryTable->columnCount(); ++col) {
                if (supplementaryTable->item(row, col)) {
                    supplementaryTable->item(row, col)->setFlags(supplementaryTable->item(row, col)->flags() & ~Qt::ItemIsEditable);
                }
            }
        }
    }
    
    qDebug() << "补勘数据加载完成，共" << supplementaryTable->rowCount() << "条记录";
}

// 搜索补勘信息
void ProjectManagementWindow::onSearchSupplementary()
{
    // 查找搜索框
    QLineEdit *searchBox = tabWidget->findChild<QLineEdit*>("supplementarySearchBox");
    if (!searchBox) {
        qWarning() << "未找到搜索框";
        return;
    }
    
    QString searchText = searchBox->text().trimmed();
    
    if (searchText.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("提示");
        msgBox.setText("请输入搜索关键词");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    // 执行搜索
    int matchCount = 0;
    for (int row = 0; row < supplementaryTable->rowCount(); ++row) {
        bool match = false;
        
        // 在所有列中搜索
        for (int col = 0; col < supplementaryTable->columnCount(); ++col) {
            QTableWidgetItem *item = supplementaryTable->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        
        supplementaryTable->setRowHidden(row, !match);
        if (match) matchCount++;
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("搜索结果");
    msgBox.setText(QString("找到 %1 条匹配记录").arg(matchCount));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                        "QLabel { color: black; } "
                        "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
    msgBox.exec();
    
    qDebug() << "补勘数据搜索完成，匹配" << matchCount << "条记录";
}

// 刷新补勘信息
void ProjectManagementWindow::onRefreshSupplementary()
{
    // 查找搜索框并清空
    QLineEdit *searchBox = tabWidget->findChild<QLineEdit*>("supplementarySearchBox");
    if (searchBox) {
        searchBox->clear();
    }
    
    // 显示所有行
    for (int row = 0; row < supplementaryTable->rowCount(); ++row) {
        supplementaryTable->setRowHidden(row, false);
    }
    
    // 重新加载数据
    loadSupplementaryData();
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("刷新成功");
    msgBox.setText("补勘数据已刷新");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                        "QLabel { color: black; } "
                        "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
    msgBox.exec();
    
    qDebug() << "补勘数据已刷新";
}

// 筛选补勘信息
void ProjectManagementWindow::onFilterSupplementary()
{
    // 创建筛选对话框
    QDialog dialog(this);
    dialog.setWindowTitle("筛选补勘数据");
    dialog.setMinimumWidth(400);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // 项目名称筛选
    QHBoxLayout *projectLayout = new QHBoxLayout();
    projectLayout->addWidget(new QLabel("项目名称:"));
    QLineEdit *projectFilter = new QLineEdit(&dialog);
    projectFilter->setPlaceholderText("输入项目名称");
    projectLayout->addWidget(projectFilter);
    layout->addLayout(projectLayout);
    
    // 围岩危险等级筛选
    QHBoxLayout *dangerLayout = new QHBoxLayout();
    dangerLayout->addWidget(new QLabel("围岩危险等级:"));
    QComboBox *dangerCombo = new QComboBox(&dialog);
    dangerCombo->addItem("全部", "");
    dangerCombo->addItem("A级", "A");
    dangerCombo->addItem("B级", "B");
    dangerCombo->addItem("C级", "C");
    dangerCombo->addItem("D级", "D");
    dangerLayout->addWidget(dangerCombo);
    layout->addLayout(dangerLayout);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("确定", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    okButton->setStyleSheet("QPushButton { background-color: #0078d4; color: white; border-radius: 4px; padding: 5px 15px; }");
    cancelButton->setStyleSheet("QPushButton { background-color: white; color: black; border: 1px solid #ccc; border-radius: 4px; padding: 5px 15px; }");
    
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    dialog.setStyleSheet("QDialog { background-color: white; }");
    
    if (dialog.exec() == QDialog::Accepted) {
        QString projectName = projectFilter->text().trimmed();
        QString dangerLevel = dangerCombo->currentData().toString();
        
        int matchCount = 0;
        for (int row = 0; row < supplementaryTable->rowCount(); ++row) {
            bool match = true;
            
            // 项目名称筛选
            if (!projectName.isEmpty()) {
                QTableWidgetItem *item = supplementaryTable->item(row, 0);
                if (!item || !item->text().contains(projectName, Qt::CaseInsensitive)) {
                    match = false;
                }
            }
            
            // 围岩危险等级筛选
            if (!dangerLevel.isEmpty() && match) {
                QTableWidgetItem *item = supplementaryTable->item(row, 10);
                if (!item || item->text() != dangerLevel) {
                    match = false;
                }
            }
            
            supplementaryTable->setRowHidden(row, !match);
            if (match) matchCount++;
        }
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("筛选结果");
        msgBox.setText(QString("筛选后显示 %1 条记录").arg(matchCount));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
        msgBox.exec();
        
        qDebug() << "补勘数据筛选完成，显示" << matchCount << "条记录";
    }
}

// 导出补勘信息
void ProjectManagementWindow::onExportSupplementary()
{
    // 检查是否有数据
    if (supplementaryTable->rowCount() == 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("提示");
        msgBox.setText("没有可导出的数据");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    // 选择保存位置
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "导出补勘数据",
        QDir::homePath() + "/supplementary_data.csv",
        "CSV Files (*.csv);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 写入CSV文件
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("无法创建文件！");
        msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                            "QLabel { color: black; } "
                            "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
        msgBox.exec();
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入表头
    QStringList headers;
    for (int col = 0; col < supplementaryTable->columnCount(); ++col) {
        headers << supplementaryTable->horizontalHeaderItem(col)->text();
    }
    out << headers.join(",") << "\n";
    
    // 写入数据
    for (int row = 0; row < supplementaryTable->rowCount(); ++row) {
        if (supplementaryTable->isRowHidden(row)) {
            continue;  // 跳过隐藏的行
        }
        
        QStringList rowData;
        for (int col = 0; col < supplementaryTable->columnCount(); ++col) {
            QTableWidgetItem *item = supplementaryTable->item(row, col);
            QString text = item ? item->text() : "";
            // 处理包含逗号或换行符的数据
            if (text.contains(",") || text.contains("\n")) {
                text = "\"" + text + "\"";
            }
            rowData << text;
        }
        out << rowData.join(",") << "\n";
    }
    
    file.close();
    
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("成功");
    msgBox.setText(QString("数据已导出到：%1").arg(fileName));
    msgBox.setStyleSheet("QMessageBox { background-color: white; } "
                        "QLabel { color: black; } "
                        "QPushButton { background-color: white; color: black; border: 1px solid #ccc; padding: 5px 15px; }");
    msgBox.exec();
    
    qDebug() << "补勘数据已导出到:" << fileName;
}
