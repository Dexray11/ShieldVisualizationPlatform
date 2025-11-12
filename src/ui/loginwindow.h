#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>

// 前向声明
class User;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void showMainMenu();

private:
    void setupUI();
    void setupWelcomeScreen();
    void setupLoginScreen();
    void createConnections();
    void initDatabase();  // 初始化数据库
    
    // 欢迎界面
    QWidget *welcomeWidget;
    QLabel *welcomeTitle;
    QLabel *welcomeSubtitle;
    QLabel *welcomeImage;
    
    // 登录界面
    QWidget *loginWidget;
    QLabel *loginTitle;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *menuButton;  // 左上角菜单按钮
    QPushButton *minimizeButton;  // 最小化按钮
    QPushButton *closeButton;     // 关闭按钮
    
    // 计时器和动画
    QTimer *welcomeTimer;
    QPropertyAnimation *fadeAnimation;
    
    // 当前登录用户
    User *currentUser;
};

#endif // LOGINWINDOW_H
