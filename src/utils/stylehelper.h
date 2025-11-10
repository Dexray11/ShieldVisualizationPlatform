#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QApplication>
#include <QString>
#include <QColor>

class StyleHelper
{
public:
    // 主题颜色
    static const QString COLOR_PRIMARY;      // #48466d
    static const QString COLOR_SECONDARY;    // #3d84a8
    static const QString COLOR_ACCENT;       // #46cdcf
    static const QString COLOR_LIGHT;        // #abedd8
    static const QString COLOR_DARK;         // #2d2d44
    static const QString COLOR_TEXT;         // #ffffff
    static const QString COLOR_TEXT_DARK;    // #333333
    static const QString COLOR_BACKGROUND;   // #f5f5f5
    static const QString COLOR_BORDER;       // #cccccc
    
    // 应用全局样式
    static void applyGlobalStyle(QApplication *app);
    
    // 获取全局样式表
    static QString getGlobalStyleSheet();
    
    // 获取按钮样式
    static QString getButtonStyle(const QString &bgColor = COLOR_SECONDARY);
    
    // 获取输入框样式
    static QString getLineEditStyle();
    
    // 获取表格样式
    static QString getTableStyle();
    
    // 获取面板样式
    static QString getPanelStyle();
    
    // 获取输入框样式（通用）
    static QString getInputStyle();
    
    // 获取标签页样式
    static QString getTabWidgetStyle();
    
private:
    StyleHelper() = delete;
};

#endif // STYLEHELPER_H
