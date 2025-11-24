#include "mainmenuwindow.h"
#include "dashboardwindow.h"
#include "projectmanagementwindow.h"
#include "../utils/stylehelper.h"
#include "../api/ApiManager.h"
#include "../database/DatabaseManager.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QIcon>

MainMenuWindow::MainMenuWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    setWindowTitle("智能盾构地质可视化平台 - 主界面");
    resize(1200, 800);

    // 窗口居中
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 启动API服务器（端口8080） - 只在首次启动
    ApiManager* apiMgr = ApiManager::instance();
    
    // 如果API服务器未运行，则初始化并启动
    if (!apiMgr->isApiServerRunning()) {
        apiMgr->initialize(&DatabaseManager::instance());
        
        if (apiMgr->startApiServer(8080)) {
            qInfo() << "API服务器已启动，端口: 8080";
        } else {
            qWarning() << "API服务器启动失败";
        }
    } else {
        qDebug() << "API服务器已在运行，无需重复启动";
    }
}

MainMenuWindow::~MainMenuWindow()
{
}

void MainMenuWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    createTopBar();
    createMenuButtons();
}

void MainMenuWindow::createTopBar()
{
    QWidget *topBar = new QWidget(centralWidget);
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);

    QLabel *titleLabel = new QLabel("智能盾构地质可视化平台", topBar);
    titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
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
    mainLayout->addWidget(topBar);
}

void MainMenuWindow::createMenuButtons()
{
    QWidget *buttonContainer = new QWidget(centralWidget);
    buttonContainer->setStyleSheet(QString("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);")
                                       .arg(StyleHelper::COLOR_PRIMARY)
                                       .arg(StyleHelper::COLOR_SECONDARY));

    QVBoxLayout *containerLayout = new QVBoxLayout(buttonContainer);
    containerLayout->setContentsMargins(50, 50, 50, 50);
    containerLayout->setSpacing(30);

    // 标题
    QLabel *welcomeLabel = new QLabel("欢迎使用智能盾构地质可视化平台", buttonContainer);
    welcomeLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    welcomeLabel->setAlignment(Qt::AlignCenter);

    containerLayout->addWidget(welcomeLabel);
    containerLayout->addSpacing(50);

    // 创建2x2的网格布局放置四个按钮
    QWidget *gridWidget = new QWidget(buttonContainer);
    QGridLayout *gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(30);

    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: rgba(255, 255, 255, 0.9);
            color: %1;
            border: none;
            border-radius: 15px;
            font-size: 20px;
            font-weight: bold;
            min-width: 250px;
            min-height: 150px;
        }
        QPushButton:hover {
            background-color: white;
            color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
            color: white;
        }
    )").arg(StyleHelper::COLOR_PRIMARY)
                              .arg(StyleHelper::COLOR_SECONDARY)
                              .arg(StyleHelper::COLOR_ACCENT);

    openProjectButton = new QPushButton("打开项目", gridWidget);
    openProjectButton->setIcon(QIcon(":/icons/folder.png"));
    openProjectButton->setIconSize(QSize(64, 64));
    openProjectButton->setStyleSheet(buttonStyle);
    connect(openProjectButton, &QPushButton::clicked, this, &MainMenuWindow::onOpenProjectClicked);

    projectManagementButton = new QPushButton("项目管理", gridWidget);
    projectManagementButton->setIcon(QIcon(":/icons/add.png"));
    projectManagementButton->setIconSize(QSize(64, 64));
    projectManagementButton->setStyleSheet(buttonStyle);
    connect(projectManagementButton, &QPushButton::clicked, this, &MainMenuWindow::onProjectManagementClicked);

    userManualButton = new QPushButton("使用手册", gridWidget);
    userManualButton->setIcon(QIcon(":/icons/manual.png"));
    userManualButton->setIconSize(QSize(64, 64));
    userManualButton->setStyleSheet(buttonStyle);
    connect(userManualButton, &QPushButton::clicked, this, &MainMenuWindow::onUserManualClicked);

    noticeButton = new QPushButton("注意事项", gridWidget);
    noticeButton->setIcon(QIcon(":/icons/warning.png"));
    noticeButton->setIconSize(QSize(64, 64));
    noticeButton->setStyleSheet(buttonStyle);
    connect(noticeButton, &QPushButton::clicked, this, &MainMenuWindow::onNoticeClicked);

    gridLayout->addWidget(openProjectButton, 0, 0);
    gridLayout->addWidget(projectManagementButton, 0, 1);
    gridLayout->addWidget(userManualButton, 1, 0);
    gridLayout->addWidget(noticeButton, 1, 1);

    // 居中网格
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(gridWidget);
    centerLayout->addStretch();

    containerLayout->addLayout(centerLayout);
    containerLayout->addStretch();

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    mainLayout->addWidget(buttonContainer);
}

void MainMenuWindow::onOpenProjectClicked()
{
    // 修复问题1：打开项目后直接展示全部项目界面（左侧边栏）
    DashboardWindow *dashboard = new DashboardWindow();
    dashboard->show();
    this->hide();
}

void MainMenuWindow::onProjectManagementClicked()
{
    ProjectManagementWindow *projectMgmt = new ProjectManagementWindow();
    projectMgmt->show();
    this->hide();
}

void MainMenuWindow::onUserManualClicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("使用手册");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("智能盾构地质可视化平台使用手册\n\n"
                   "1. 打开项目：查看和管理在建项目的大数据可视化\n"
                   "2. 项目管理:新建、编辑、删除项目,管理预警信息\n"
                   "3. 使用手册：查看系统使用说明\n"
                   "4. 注意事项：查看系统使用注意事项\n\n"
                   "详细手册请参考系统文档。");
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    msgBox.exec();
}

void MainMenuWindow::onNoticeClicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("注意事项");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("系统使用注意事项：\n\n"
                   "1. 请确保定位数据准确，定期进行定位校准\n"
                   "2. 注意监控预警信息，及时处理高级别预警\n"
                   "3. 定期备份项目数据\n"
                   "4. 补勘数据应及时上传和更新\n"
                   "5. 系统维护期间请勿进行关键操作\n\n"
                   "如有问题请联系技术支持。");
    msgBox.setStyleSheet("QMessageBox { background-color: white; }");
    msgBox.exec();
}
