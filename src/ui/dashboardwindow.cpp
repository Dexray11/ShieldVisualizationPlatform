#include "dashboardwindow.h"
#include "projectwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include "../database/ProjectDAO.h"
#include "../database/WarningDAO.h"
#include "../models/Project.h"
#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QMessageBox>
#include <QIcon>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadProjects();

    setWindowTitle("智能盾构地质可视化平台 - 可视化平台");
    setWindowIcon(QIcon(":/icons/app_icon.ico"));
    resize(1400, 900);

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

DashboardWindow::~DashboardWindow()
{
}

void DashboardWindow::setupUI()
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

    QWidget *middleContent = new QWidget(contentWidget);
    middleContent->setStyleSheet("background-color: #f5f5f5;");
    QVBoxLayout *middleLayout = new QVBoxLayout(middleContent);
    middleLayout->setContentsMargins(20, 20, 20, 20);
    middleLayout->setSpacing(15);

    createMapView();
    middleLayout->addWidget(mapWidget, 2);

    createProjectList();
    middleLayout->addWidget(projectTable, 1);

    contentLayout->addWidget(middleContent, 1);

    QWidget *rightPanel = new QWidget(contentWidget);
    rightPanel->setFixedWidth(300);
    rightPanel->setStyleSheet("background-color: #f5f5f5;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 20, 20, 20);
    rightLayout->setSpacing(15);

    createStatisticsPanel();
    rightLayout->addWidget(statisticsPanel);

    // 恢复联系人面板（只在选中具体项目时显示）
    createContactPanel();
    rightLayout->addWidget(contactPanel);
    contactPanel->hide();  // 默认隐藏，点击具体项目时显示

    rightLayout->addStretch();

    contentLayout->addWidget(rightPanel);

    mainLayout->addWidget(contentWidget);
}

void DashboardWindow::createTopBar()
{
    QWidget *topBar = new QWidget(centralWidget);
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);

    // 返回按钮
    QPushButton *backButton = new QPushButton("← 返回", topBar);
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
    connect(backButton, &QPushButton::clicked, [this]() {
        MainMenuWindow *mainMenu = new MainMenuWindow();
        mainMenu->show();
        this->close();
    });

    // 工作台按钮
    workbenchButton = new QPushButton(" 工作台", topBar);
    workbenchButton->setIcon(QIcon(":/icons/menu.png"));
    workbenchButton->setIconSize(QSize(20, 20));
    workbenchButton->setStyleSheet(QString(R"(
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
    connect(workbenchButton, &QPushButton::clicked, this, &DashboardWindow::onWorkbenchClicked);

    topLayout->addWidget(backButton);
    topLayout->addWidget(workbenchButton);

    QLabel *titleLabel = new QLabel("智能盾构地质可视化平台 - 大数据可视化", topBar);
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

void DashboardWindow::createSidebar()
{
    sidebar = new QWidget(centralWidget);
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 20, 0, 0);
    sidebarLayout->setSpacing(0);

    QLabel *sidebarTitle = new QLabel("在建项目", sidebar);
    sidebarTitle->setStyleSheet("color: white; font-size: 18px; font-weight: bold; padding: 15px;");
    sidebarLayout->addWidget(sidebarTitle);

    projectList = new QListWidget(sidebar);
    projectList->setStyleSheet(QString(R"(
        QListWidget {
            background-color: transparent;
            border: none;
            color: white;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 15px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        QListWidget::item:hover {
            background-color: %1;
        }
        QListWidget::item:selected {
            background-color: %2;
        }
    )").arg(StyleHelper::COLOR_SECONDARY).arg(StyleHelper::COLOR_ACCENT));

    QListWidgetItem *allProjectsItem = new QListWidgetItem("📋 全部项目");
    projectList->addItem(allProjectsItem);

    // 从数据库加载项目列表
    ProjectDAO projectDAO;
    QList<Project> projects = projectDAO.getProjectsByStatus("active");
    
    for (const Project &project : projects) {
        projectList->addItem("📁 " + project.getProjectName());
    }

    connect(projectList, &QListWidget::itemClicked,
            this, &DashboardWindow::onProjectListItemClicked);
    connect(projectList, &QListWidget::itemDoubleClicked,
            this, &DashboardWindow::onProjectListItemDoubleClicked);

    sidebarLayout->addWidget(projectList);
}

void DashboardWindow::createMapView()
{
    mapWidget = new QWidget(centralWidget);
    mapWidget->setMinimumHeight(400);
    mapWidget->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QVBoxLayout *mapLayout = new QVBoxLayout(mapWidget);

    mapLabel = new QLabel(mapWidget);
    mapLabel->setAlignment(Qt::AlignCenter);

    QPixmap mapPixmap(":/images/map_qingdao.png");
    if (!mapPixmap.isNull()) {
        mapLabel->setPixmap(mapPixmap.scaled(800, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap placeholder(800, 500);
        placeholder.fill(QColor("#e0e0e0"));
        QPainter painter(&placeholder);
        painter.setPen(QPen(QColor(StyleHelper::COLOR_SECONDARY), 2));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "后期导入地图api");
        mapLabel->setPixmap(placeholder);
    }

    mapLayout->addWidget(mapLabel);
}

void DashboardWindow::createProjectList()
{
    projectTable = new QTableWidget(centralWidget);
    projectTable->setColumnCount(7);
    projectTable->setHorizontalHeaderLabels({"工程名称", "工程简介", "施工单位", "开始时间",
                                             "进度", "地理位置", "操作"});

    projectTable->setStyleSheet(StyleHelper::getTableStyle());
    projectTable->horizontalHeader()->setStretchLastSection(true);
    projectTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    projectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    projectTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectTable->setAlternatingRowColors(true);

    connect(projectTable, &QTableWidget::cellDoubleClicked,
            this, &DashboardWindow::onProjectSelected);
}

void DashboardWindow::createStatisticsPanel()
{
    statisticsPanel = new QWidget(centralWidget);
    statisticsPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QVBoxLayout *statsLayout = new QVBoxLayout(statisticsPanel);
    statsLayout->setContentsMargins(15, 15, 15, 15);

    statisticsLabel = new QLabel("在建项目统计", statisticsPanel);
    statisticsLabel->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
                                       .arg(StyleHelper::COLOR_TEXT_DARK));
    statisticsLabel->setAlignment(Qt::AlignCenter);
    statisticsLabel->setWordWrap(true);

    statsLayout->addWidget(statisticsLabel);

    // 创建一个容器来保存进度条,以便动态更新
    progressContainer = new QWidget(statisticsPanel);
    progressContainerLayout = new QVBoxLayout(progressContainer);
    progressContainerLayout->setContentsMargins(0, 0, 0, 0);
    progressContainerLayout->setSpacing(5);
    
    statsLayout->addWidget(progressContainer);
    statsLayout->addStretch();
}

void DashboardWindow::createContactPanel()
{
    contactPanel = new QWidget(centralWidget);
    contactPanel->setStyleSheet(QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(StyleHelper::COLOR_BORDER));

    QVBoxLayout *contactLayout = new QVBoxLayout(contactPanel);
    contactLayout->setContentsMargins(15, 15, 15, 15);

    QLabel *contactTitle = new QLabel("紧急技术联系人：", contactPanel);
    contactTitle->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
                                    .arg(StyleHelper::COLOR_TEXT_DARK));

    contact1Label = new QLabel("张三  电话：15555555555", contactPanel);
    contact1Label->setStyleSheet(QString("color: %1; font-size: 13px; margin-top: 10px;")
                                     .arg(StyleHelper::COLOR_TEXT_DARK));

    contact2Label = new QLabel("李四  电话：16666666666", contactPanel);
    contact2Label->setStyleSheet(QString("color: %1; font-size: 13px; margin-top: 5px;")
                                     .arg(StyleHelper::COLOR_TEXT_DARK));

    contactLayout->addWidget(contactTitle);
    contactLayout->addWidget(contact1Label);
    contactLayout->addWidget(contact2Label);
    contactLayout->addStretch();
}

void DashboardWindow::loadProjects()
{
    showAllProjects();
}

void DashboardWindow::onProjectSelected(int row, int column)
{
    Q_UNUSED(column);

    if (row >= 0 && row < projectTable->rowCount()) {
        selectedProject = projectTable->item(row, 0)->text();
        openProjectView();
    }
}

void DashboardWindow::onProjectMarkerClicked(const QString &projectName)
{
    selectedProject = projectName;
    openProjectView();
}

void DashboardWindow::onWorkbenchClicked()
{
    MainMenuWindow *mainMenu = new MainMenuWindow();
    mainMenu->show();
    this->close();
}

void DashboardWindow::openProjectView()
{
    if (!selectedProject.isEmpty()) {
        // 传入nullptr而不是this,让ProjectWindow成为独立顶层窗口
        ProjectWindow *projectWin = new ProjectWindow(selectedProject, nullptr);
        projectWin->setAttribute(Qt::WA_DeleteOnClose);
        
        // 连接返回信号 - 当ProjectWindow返回时，显示Dashboard并关闭ProjectWindow
        connect(projectWin, &ProjectWindow::backToDashboard, this, [this, projectWin]() {
            this->show();  // 显示Dashboard
            projectWin->close();  // 关闭ProjectWindow(会自动删除，因为有WA_DeleteOnClose)
        });
        
        projectWin->show();
        this->hide();
    }
}

void DashboardWindow::onProjectListItemClicked(QListWidgetItem *item)
{
    QString text = item->text();

    if (text.startsWith("📋")) {
        showAllProjects();
        contactPanel->hide();  // 全部项目时隐藏联系人
    } else {
        // 移除emoji和空格，emoji占用多个字符
        QString projectName = text;
        projectName.remove("📁 ");  // 移除文件夹emoji和空格
        showSingleProject(projectName);
        contactPanel->show();  // 具体项目时显示联系人
    }
}

void DashboardWindow::onProjectListItemDoubleClicked(QListWidgetItem *item)
{
    QString text = item->text();
    if (!text.startsWith("📋")) {
        QString projectName = text;
        projectName.remove("📁 ");  // 移除文件夹emoji和空格
        selectedProject = projectName;
        openProjectView();
    }
}

void DashboardWindow::showAllProjects()
{
    ProjectDAO projectDAO;
    WarningDAO warningDAO;
    
    // 从数据库获取所有活动项目
    QList<Project> projects = projectDAO.getProjectsByStatus("active");
    
    // 设置表格行数
    projectTable->setRowCount(projects.size());
    
    // 填充表格数据
    for (int i = 0; i < projects.size(); i++) {
        const Project &project = projects[i];
        
        projectTable->setItem(i, 0, new QTableWidgetItem(project.getProjectName()));
        projectTable->setItem(i, 1, new QTableWidgetItem(project.getBrief()));
        projectTable->setItem(i, 2, new QTableWidgetItem(project.getConstructionUnit()));
        projectTable->setItem(i, 3, new QTableWidgetItem(project.getStartDate()));
        projectTable->setItem(i, 4, new QTableWidgetItem(QString("%1%").arg(QString::number(project.getProgress(), 'f', 1))));
        projectTable->setItem(i, 5, new QTableWidgetItem(project.getLocation()));
        projectTable->setItem(i, 6, new QTableWidgetItem("查看"));

        for (int col = 0; col < 7; col++) {
            projectTable->item(i, col)->setTextAlignment(Qt::AlignCenter);
        }
    }

    // 更新统计信息
    int projectCount = projectDAO.getProjectCount();
    double avgProgress = projectDAO.getAverageProgress();
    int warningCount = warningDAO.getTotalWarningCount();
    
    statisticsLabel->setText(QString("在建项目统计\n\n"
                                    "项目总数: %1\n"
                                    "平均进度: %2%\n"
                                    "预警数量: %3")
                            .arg(projectCount)
                            .arg(QString::number(avgProgress, 'f', 1))
                            .arg(warningCount));
    
    // 清除旧的进度条
    QLayoutItem *item;
    while ((item = progressContainerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // 为每个项目创建进度条
    for (const auto& project : projects) {
        QWidget *progressWidget = new QWidget(progressContainer);
        QVBoxLayout *progressLayout = new QVBoxLayout(progressWidget);
        progressLayout->setContentsMargins(0, 5, 0, 5);
        progressLayout->setSpacing(5);

        QLabel *projectLabel = new QLabel(project.getProjectName(), progressWidget);
        projectLabel->setStyleSheet("font-size: 12px;");
        projectLabel->setWordWrap(true);

        QProgressBar *progressBar = new QProgressBar(progressWidget);
        int progress = static_cast<int>(project.getProgress());
        progressBar->setValue(progress);
        progressBar->setFormat(QString("%1%").arg(QString::number(project.getProgress(), 'f', 1)));
        progressBar->setStyleSheet(QString(R"(
            QProgressBar {
                border: none;
                border-radius: 5px;
                text-align: center;
                background-color: #f0f0f0;
                color: %1;
                height: 20px;
            }
            QProgressBar::chunk {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                                stop:0 %2, stop:1 %3);
                border-radius: 5px;
            }
        )").arg(StyleHelper::COLOR_TEXT_DARK)
                                       .arg(StyleHelper::COLOR_SECONDARY)
                                       .arg(StyleHelper::COLOR_ACCENT));

        progressLayout->addWidget(projectLabel);
        progressLayout->addWidget(progressBar);

        progressContainerLayout->addWidget(progressWidget);
    }
}

void DashboardWindow::showSingleProject(const QString &projectName)
{
    ProjectDAO projectDAO;
    WarningDAO warningDAO;
    
    // 从数据库获取项目信息
    Project project = projectDAO.getProjectByName(projectName);
    
    if (!project.isValid()) {
        StyleHelper::showWarning(this, "错误", "找不到项目信息");
        return;
    }
    
    // 设置表格行数为1，显示单个项目
    projectTable->setRowCount(1);

    projectTable->setItem(0, 0, new QTableWidgetItem(project.getProjectName()));
    projectTable->setItem(0, 1, new QTableWidgetItem(project.getBrief()));
    projectTable->setItem(0, 2, new QTableWidgetItem(project.getConstructionUnit()));
    projectTable->setItem(0, 3, new QTableWidgetItem(project.getStartDate()));
    projectTable->setItem(0, 4, new QTableWidgetItem(QString("%1%").arg(QString::number(project.getProgress(), 'f', 1))));
    projectTable->setItem(0, 5, new QTableWidgetItem(project.getLocation()));
    projectTable->setItem(0, 6, new QTableWidgetItem("查看"));

    for (int col = 0; col < 7; col++) {
        projectTable->item(0, col)->setTextAlignment(Qt::AlignCenter);
    }

    // 获取该项目的预警数量
    int warningCount = warningDAO.getWarningCountByProject(project.getProjectId());

    // 更新统计信息
    statisticsLabel->setText(QString("%1\n\n"
                                     "当前进度: %2%\n"
                                     "预警数量: %3\n"
                                     "开工日期: %4")
                            .arg(projectName)
                            .arg(QString::number(project.getProgress(), 'f', 1))
                            .arg(warningCount)
                            .arg(project.getStartDate()));

    // 联系人信息（这里可以扩展为从数据库读取，目前使用默认值）
    contact1Label->setText("张三  电话：15555555555");
    contact2Label->setText("李四  电话：16666666666");
    
    // 清除旧的进度条
    QLayoutItem *item;
    while ((item = progressContainerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // 显示当前项目的进度条
    QWidget *progressWidget = new QWidget(progressContainer);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressWidget);
    progressLayout->setContentsMargins(0, 5, 0, 5);
    progressLayout->setSpacing(5);

    QLabel *projectLabel = new QLabel(projectName, progressWidget);
    projectLabel->setStyleSheet("font-size: 12px;");
    projectLabel->setWordWrap(true);

    QProgressBar *progressBar = new QProgressBar(progressWidget);
    int progress = static_cast<int>(project.getProgress());
    progressBar->setValue(progress);
    progressBar->setFormat(QString("%1%").arg(QString::number(project.getProgress(), 'f', 1)));
    progressBar->setStyleSheet(QString(R"(
        QProgressBar {
            border: none;
            border-radius: 5px;
            text-align: center;
            background-color: #f0f0f0;
            color: %1;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                            stop:0 %2, stop:1 %3);
            border-radius: 5px;
        }
    )").arg(StyleHelper::COLOR_TEXT_DARK)
                                   .arg(StyleHelper::COLOR_SECONDARY)
                                   .arg(StyleHelper::COLOR_ACCENT));

    progressLayout->addWidget(projectLabel);
    progressLayout->addWidget(progressBar);

    progressContainerLayout->addWidget(progressWidget);
}
