#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "src/ui/loginwindow.h"
#include "src/utils/stylehelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    a.setApplicationName("智能盾构地质可视化平台");
    a.setOrganizationName("山东科技大学");
    a.setApplicationVersion("1.0.0");
    
    // 应用全局样式
    StyleHelper::applyGlobalStyle(&a);
    
    // 显示登录窗口
    LoginWindow loginWindow;
    loginWindow.show();
    
    return a.exec();
}
