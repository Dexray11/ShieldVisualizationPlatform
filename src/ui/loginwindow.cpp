#include "loginwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include "../database/DatabaseManager.h"
#include "../database/UserDAO.h"
#include "../models/User.h"
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , welcomeWidget(nullptr)
    , loginWidget(nullptr)
    , welcomeTimer(nullptr)
    , currentUser(nullptr)
{
    qDebug() << "LoginWindow: 开始构造函数";
    
    // 初始化数据库
    initDatabase();
    qDebug() << "LoginWindow: 数据库初始化完成";
    
    setupUI();
    qDebug() << "LoginWindow: UI设置完成";
    
    createConnections();
    qDebug() << "LoginWindow: 信号槽连接完成";
    
    // 设置窗口属性
    setWindowTitle("智能盾构地质可视化平台");
    setFixedSize(1200, 800);
    qDebug() << "LoginWindow: 窗口属性设置完成";
    
    // 窗口居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    qDebug() << "LoginWindow: 窗口位置设置完成";
    
    // 先显示欢迎界面
    welcomeWidget->show();
    loginWidget->hide();
    qDebug() << "LoginWindow: 欢迎界面已显示";
    
    // 2-3秒后切换到登录界面
    welcomeTimer = new QTimer(this);
    welcomeTimer->setSingleShot(true);
    connect(welcomeTimer, &QTimer::timeout, this, &LoginWindow::showMainMenu);
    welcomeTimer->start(2500);  // 2.5秒
    
    qDebug() << "LoginWindow: 构造函数完成，准备显示窗口";
}

LoginWindow::~LoginWindow()
{
    if (currentUser) {
        delete currentUser;
        currentUser = nullptr;
    }
}

void LoginWindow::setupUI()
{
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建堆叠布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 设置欢迎界面和登录界面
    setupWelcomeScreen();
    setupLoginScreen();
    
    mainLayout->addWidget(welcomeWidget);
    mainLayout->addWidget(loginWidget);
}

void LoginWindow::setupWelcomeScreen()
{
    welcomeWidget = new QWidget(this);
    welcomeWidget->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::COLOR_PRIMARY));
    
    QVBoxLayout *layout = new QVBoxLayout(welcomeWidget);
    layout->setAlignment(Qt::AlignCenter);
    
    // 欢迎图片（如果存在）
    welcomeImage = new QLabel(welcomeWidget);
    QPixmap welcomePixmap(":/images/zhongjiaologo.png");
    if (!welcomePixmap.isNull()) {
        // 图片存在，显示图片
        welcomeImage->setPixmap(welcomePixmap.scaled(800, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        welcomeImage->setAlignment(Qt::AlignCenter);
        layout->addWidget(welcomeImage);
        qDebug() << "欢迎图片加载成功";
    } else {
        // 图片不存在，用大学Logo文字代替
        welcomeImage->setStyleSheet("font-size: 48px; font-weight: bold; color: white; margin: 50px;");
        welcomeImage->setText("🎓");
        welcomeImage->setAlignment(Qt::AlignCenter);
        layout->addWidget(welcomeImage);
        qDebug() << "欢迎图片不存在，使用文字替代";
    }
    
    // 标题
    welcomeTitle = new QLabel("欢迎使用智能盾构地质可视化平台", welcomeWidget);
    welcomeTitle->setStyleSheet(QString("color: white; font-size: 32px; font-weight: bold; margin-top: 30px;"));
    welcomeTitle->setAlignment(Qt::AlignCenter);
    
    // 副标题
    welcomeSubtitle = new QLabel("——本平台由中交第一航务工程局有限公司开发", welcomeWidget);
    welcomeSubtitle->setStyleSheet(QString("color: %1; font-size: 18px; margin-top: 10px;").arg(StyleHelper::COLOR_LIGHT));
    welcomeSubtitle->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(welcomeTitle);
    layout->addWidget(welcomeSubtitle);
    layout->addStretch();
}

void LoginWindow::setupLoginScreen()
{
    loginWidget = new QWidget(this);
    
    // 背景设置
    loginWidget->setStyleSheet(QString(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                      stop:0 %1, stop:1 %2);
        }
    )").arg(StyleHelper::COLOR_PRIMARY).arg(StyleHelper::COLOR_SECONDARY));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(loginWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 顶部工具栏
    QWidget *topBar = new QWidget(loginWidget);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background-color: transparent;");
    
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 10, 10, 10);
    
    // 菜单按钮
    menuButton = new QPushButton(topBar);
    menuButton->setIcon(QIcon(":/icons/menu.png"));  // 需要菜单图标，32x32像素
    menuButton->setIconSize(QSize(24, 24));
    menuButton->setFixedSize(40, 40);
    menuButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 20px;
        }
    )");
    
    topLayout->addWidget(menuButton);
    topLayout->addStretch();
    
    // 最小化和关闭按钮
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
    
    mainLayout->addWidget(topBar);
    mainLayout->addStretch();
    
    // 登录表单容器
    QWidget *loginContainer = new QWidget(loginWidget);
    loginContainer->setFixedSize(400, 350);
    loginContainer->setStyleSheet(QString(R"(
        QWidget {
            background-color: rgba(255, 255, 255, 0.95);
            border-radius: 15px;
        }
    )"));
    
    QVBoxLayout *containerLayout = new QVBoxLayout(loginContainer);
    containerLayout->setContentsMargins(40, 40, 40, 40);
    containerLayout->setSpacing(20);
    
    // 标题
    loginTitle = new QLabel("智能盾构地质可视化平台", loginContainer);
    loginTitle->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: bold;").arg(StyleHelper::COLOR_PRIMARY));
    loginTitle->setAlignment(Qt::AlignCenter);
    
    // 用户名输入框
    usernameEdit = new QLineEdit(loginContainer);
    usernameEdit->setPlaceholderText("手机号 / 用户名");
    usernameEdit->setText("admin");  // 默认值
    usernameEdit->setFixedHeight(45);
    usernameEdit->setStyleSheet(QString(R"(
        QLineEdit {
            background-color: white;
            border: 2px solid %1;
            border-radius: 8px;
            padding: 0 15px;
            font-size: 14px;
            color: %2;
        }
        QLineEdit:focus {
            border: 2px solid %3;
        }
    )").arg(StyleHelper::COLOR_BORDER)
       .arg(StyleHelper::COLOR_TEXT_DARK)
       .arg(StyleHelper::COLOR_SECONDARY));
    
    // 密码输入框
    passwordEdit = new QLineEdit(loginContainer);
    passwordEdit->setPlaceholderText("密码");
    passwordEdit->setText("admin");  // 默认值
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedHeight(45);
    passwordEdit->setStyleSheet(usernameEdit->styleSheet());
    
    // 登录按钮
    loginButton = new QPushButton("登录", loginContainer);
    loginButton->setFixedHeight(45);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
        }
    )").arg(StyleHelper::COLOR_SECONDARY)
       .arg(StyleHelper::COLOR_ACCENT)
       .arg(StyleHelper::COLOR_PRIMARY));
    
    containerLayout->addWidget(loginTitle);
    containerLayout->addSpacing(20);
    containerLayout->addWidget(usernameEdit);
    containerLayout->addWidget(passwordEdit);
    containerLayout->addWidget(loginButton);
    containerLayout->addStretch();
    
    // 将登录容器居中
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(loginContainer);
    centerLayout->addStretch();
    
    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch();
}

void LoginWindow::createConnections()
{
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        StyleHelper::showWarning(this, "登录失败", "用户名和密码不能为空！");
        return;
    }

    // 使用数据库验证用户
    UserDAO userDAO;
    if (!userDAO.validateUser(username, password)) {
        StyleHelper::showWarning(this, "登录失败", 
            "用户名或密码错误！\n错误信息：" + userDAO.getLastError());
        // 清空密码框
        passwordEdit->clear();
        passwordEdit->setFocus();
        return;
    }

    // 获取用户信息
    currentUser = userDAO.getUserByUsername(username);
    if (!currentUser) {
        StyleHelper::showError(this, "系统错误", "获取用户信息失败！");
        return;
    }

    qDebug() << "用户登录成功:" << currentUser->getUsername() 
             << "角色:" << currentUser->getRole();

    // 登录成功，跳转到主菜单
    MainMenuWindow *mainMenu = new MainMenuWindow();
    mainMenu->show();
    this->close();
}

void LoginWindow::showMainMenu()
{
    // 淡出欢迎界面
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(welcomeWidget);
    welcomeWidget->setGraphicsEffect(effect);
    
    fadeAnimation = new QPropertyAnimation(effect, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    
    connect(fadeAnimation, &QPropertyAnimation::finished, [this]() {
        welcomeWidget->hide();
        loginWidget->show();
        
        // 淡入登录界面
        QGraphicsOpacityEffect *loginEffect = new QGraphicsOpacityEffect(loginWidget);
        loginWidget->setGraphicsEffect(loginEffect);
        
        QPropertyAnimation *loginFade = new QPropertyAnimation(loginEffect, "opacity");
        loginFade->setDuration(500);
        loginFade->setStartValue(0.0);
        loginFade->setEndValue(1.0);
        loginFade->start(QPropertyAnimation::DeleteWhenStopped);
    });
    
    fadeAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

void LoginWindow::initDatabase()
{
    qDebug() << "正在初始化数据库...";
    
    if (!DatabaseManager::instance().initDatabase()) {
        QString error = DatabaseManager::instance().getLastError();
        StyleHelper::showError(this, "数据库初始化失败", 
            "无法初始化数据库，系统将无法正常运行！\n\n错误信息：" + error);
        qCritical() << "数据库初始化失败:" << error;
    } else {
        qDebug() << "数据库初始化成功";
    }
}
