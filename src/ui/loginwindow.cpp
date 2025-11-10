#include "loginwindow.h"
#include "mainmenuwindow.h"
#include "../utils/stylehelper.h"
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , welcomeWidget(nullptr)
    , loginWidget(nullptr)
    , welcomeTimer(nullptr)
{
    setupUI();
    createConnections();
    
    // 设置窗口属性
    setWindowTitle("智能盾构地质可视化平台");
    setFixedSize(1200, 800);
    
    // 窗口居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 先显示欢迎界面
    welcomeWidget->show();
    loginWidget->hide();
    
    // 2-3秒后切换到登录界面
    welcomeTimer = new QTimer(this);
    welcomeTimer->setSingleShot(true);
    connect(welcomeTimer, &QTimer::timeout, this, &LoginWindow::showMainMenu);
    welcomeTimer->start(2500);  // 2.5秒
}

LoginWindow::~LoginWindow()
{
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
    
    // 欢迎图片（需要提供大学校园图片）
    welcomeImage = new QLabel(welcomeWidget);
    welcomeImage->setPixmap(QPixmap(":/images/welcome_bg.jpg").scaled(800, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    welcomeImage->setAlignment(Qt::AlignCenter);
    
    // 标题
    welcomeTitle = new QLabel("欢迎使用智能盾构地质可视化平台", welcomeWidget);
    welcomeTitle->setStyleSheet(QString("color: white; font-size: 32px; font-weight: bold; margin-top: 30px;"));
    welcomeTitle->setAlignment(Qt::AlignCenter);
    
    // 副标题
    welcomeSubtitle = new QLabel("——本平台由山东科技大学开发", welcomeWidget);
    welcomeSubtitle->setStyleSheet(QString("color: %1; font-size: 18px; margin-top: 10px;").arg(StyleHelper::COLOR_LIGHT));
    welcomeSubtitle->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(welcomeImage);
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
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "用户名和密码不能为空！");
        return;
    }

    // 跳转到主菜单
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
