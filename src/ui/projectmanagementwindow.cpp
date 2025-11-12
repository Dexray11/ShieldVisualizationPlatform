#include "projectmanagementwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include "../database/ProjectDAO.h"
#include "../database/WarningDAO.h"
#include "../models/Project.h"
#include "../models/Warning.h"
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
#include <QSqlQuery>
#include <QSqlError>

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
    
    // 问题2彻底修复：设置默认行高为50px
    newsTable->verticalHeader()->setDefaultSectionSize(50);
    
    // 问题2彻底修复：先设置所有列Stretch
    newsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 问题2彻底修复：操作列固定为200px
    newsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    newsTable->setColumnWidth(2, 200);  // 增加到200px
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
    
    // ========== 加载补勘数据（暂时清空，等待数据） ==========
    supplementaryTable->setRowCount(0);
    
    // ========== 加载新闻数据 ==========
    // 从数据库news表中读取新闻
    QSqlQuery query;
    query.prepare("SELECT content, publish_time FROM news ORDER BY publish_time DESC");
    
    if (query.exec()) {
        QList<QPair<QString, QString>> newsList;
        while (query.next()) {
            QString content = query.value(0).toString();
            QString publishTime = query.value(1).toString();
            newsList.append(qMakePair(content, publishTime));
        }
        
        newsTable->setRowCount(newsList.size());
        
        for (int row = 0; row < newsList.size(); row++) {
            newsTable->setItem(row, 0, new QTableWidgetItem(newsList[row].first));
            newsTable->setItem(row, 1, new QTableWidgetItem(newsList[row].second));
            
            newsTable->item(row, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            newsTable->item(row, 1)->setTextAlignment(Qt::AlignCenter);
            
            // 创建操作按钮
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
            
            operationLayout->addWidget(editBtn);
            operationLayout->addWidget(deleteBtn);
            
            newsTable->setCellWidget(row, 2, operationWidget);
        }
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
    Q_UNUSED(index);
    // 可以在这里处理标签页切换事件
}
