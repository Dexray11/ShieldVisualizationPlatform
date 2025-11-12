#include "stylehelper.h"

// 定义主题颜色
const QString StyleHelper::COLOR_PRIMARY = "#48466d";
const QString StyleHelper::COLOR_SECONDARY = "#3d84a8";
const QString StyleHelper::COLOR_ACCENT = "#46cdcf";
const QString StyleHelper::COLOR_LIGHT = "#abedd8";
const QString StyleHelper::COLOR_DARK = "#2d2d44";
const QString StyleHelper::COLOR_TEXT = "#ffffff";
const QString StyleHelper::COLOR_TEXT_DARK = "#000000";
const QString StyleHelper::COLOR_BACKGROUND = "#ffffff";
const QString StyleHelper::COLOR_BORDER = "#999999";

void StyleHelper::applyGlobalStyle(QApplication *app)
{
    app->setStyleSheet(getGlobalStyleSheet());
}

QString StyleHelper::getGlobalStyleSheet()
{
    return QString(R"(
        * {
            font-family: "Microsoft YaHei", "SimHei", Arial, sans-serif;
            font-size: 14px;
        }
        
        QMainWindow {
            background-color: %1;
        }
        
        QWidget {
            background-color: transparent;
            color: %8;
        }
        
        QLabel {
            color: %8;
            background-color: transparent;
        }
        
        QPushButton {
            background-color: %2;
            color: %6;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-weight: bold;
            min-width: 80px;
            min-height: 35px;
        }
        
        QPushButton:hover {
            background-color: %3;
        }
        
        QPushButton:pressed {
            background-color: %1;
        }
        
        QPushButton:disabled {
            background-color: %9;
            color: #999999;
        }
        
        QLineEdit {
            background-color: white;
            border: 2px solid %9;
            border-radius: 5px;
            padding: 8px 12px;
            color: %8;
            selection-background-color: %3;
        }
        
        QLineEdit:focus {
            border: 2px solid %2;
        }
        
        QTextEdit {
            background-color: white;
            border: 2px solid %9;
            border-radius: 5px;
            padding: 8px;
            color: %8;
        }
        
        QTextEdit:focus {
            border: 2px solid %2;
        }
        
        QComboBox {
            background-color: white;
            border: 2px solid %9;
            border-radius: 5px;
            padding: 8px 12px;
            color: %8;
            min-height: 35px;
        }
        
        QComboBox:hover {
            border: 2px solid %2;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid %8;
            width: 0;
            height: 0;
        }
        
        QTableWidget {
            background-color: white;
            alternate-background-color: #f9f9f9;
            border: 1px solid %9;
            gridline-color: %9;
            selection-background-color: %4;
            selection-color: %8;
        }
        
        QTableWidget::item {
            padding: 5px;
        }
        
        QHeaderView::section {
            background-color: %1;
            color: %6;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QScrollBar:vertical {
            border: none;
            background: #f0f0f0;
            width: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: %2;
            min-height: 20px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: %3;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        
        QScrollBar:horizontal {
            border: none;
            background: #f0f0f0;
            height: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:horizontal {
            background: %2;
            min-width: 20px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background: %3;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        
        QMenuBar {
            background-color: %1;
            color: %6;
            border-bottom: 1px solid %5;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 8px 15px;
        }
        
        QMenuBar::item:selected {
            background-color: %2;
        }
        
        QMenu {
            background-color: white;
            border: 1px solid %9;
        }
        
        QMenu::item {
            padding: 8px 30px;
            color: %8;
        }
        
        QMenu::item:selected {
            background-color: %4;
        }
        
        QProgressBar {
            border: 2px solid %9;
            border-radius: 5px;
            text-align: center;
            background-color: white;
            color: %8;
        }
        
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                            stop:0 %2, stop:1 %3);
            border-radius: 3px;
        }
        
        QTabWidget::pane {
            border: 1px solid %9;
            background-color: white;
        }
        
        QTabBar::tab {
            background-color: %7;
            color: %8;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        
        QTabBar::tab:selected {
            background-color: %2;
            color: %6;
        }
        
        QTabBar::tab:hover {
            background-color: %4;
        }
        
        QGroupBox {
            border: 2px solid %9;
            border-radius: 5px;
            margin-top: 10px;
            font-weight: bold;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
            color: %1;
        }
        
        QCheckBox {
            spacing: 8px;
            color: %8;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid %9;
            border-radius: 3px;
            background-color: white;
        }
        
        QCheckBox::indicator:checked {
            background-color: %2;
            border-color: %2;
        }
        
        QRadioButton {
            spacing: 8px;
            color: %8;
        }
        
        QRadioButton::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid %9;
            border-radius: 9px;
            background-color: white;
        }
        
        QRadioButton::indicator:checked {
            background-color: %2;
            border-color: %2;
        }
        
        QSpinBox, QDoubleSpinBox {
            background-color: white;
            border: 2px solid %9;
            border-radius: 5px;
            padding: 5px;
            color: %8;
        }
        
        QSpinBox:focus, QDoubleSpinBox:focus {
            border: 2px solid %2;
        }
        
        QDateEdit, QTimeEdit, QDateTimeEdit {
            background-color: white;
            border: 2px solid %9;
            border-radius: 5px;
            padding: 5px;
            color: %8;
        }
        
        QDateEdit:focus, QTimeEdit:focus, QDateTimeEdit:focus {
            border: 2px solid %2;
        }
    )").arg(COLOR_PRIMARY)      // %1
        .arg(COLOR_SECONDARY)    // %2
        .arg(COLOR_ACCENT)       // %3
        .arg(COLOR_LIGHT)        // %4
        .arg(COLOR_DARK)         // %5
        .arg(COLOR_TEXT)         // %6
        .arg(COLOR_BACKGROUND)   // %7
        .arg(COLOR_TEXT_DARK)    // %8
        .arg(COLOR_BORDER);      // %9
}

QString StyleHelper::getButtonStyle(const QString &bgColor)
{
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-weight: bold;
            min-width: 80px;
            min-height: 35px;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
        }
    )").arg(bgColor).arg(COLOR_ACCENT).arg(COLOR_PRIMARY);
}

QString StyleHelper::getLineEditStyle()
{
    return QString(R"(
        QLineEdit {
            background-color: white;
            border: 2px solid %1;
            border-radius: 5px;
            padding: 10px 15px;
            color: %2;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid %3;
        }
    )").arg(COLOR_BORDER).arg(COLOR_TEXT_DARK).arg(COLOR_SECONDARY);
}

QString StyleHelper::getTableStyle()
{
    return QString(R"(
        QTableWidget {
            background-color: white;
            alternate-background-color: #f9f9f9;
            border: 1px solid %1;
            gridline-color: %1;
        }
        QHeaderView::section {
            background-color: %2;
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
        }
    )").arg(COLOR_BORDER).arg(COLOR_PRIMARY);
}

QString StyleHelper::getPanelStyle()
{
    return QString(R"(
        QWidget {
            background-color: white;
            border-radius: 10px;
            border: 1px solid %1;
        }
    )").arg(COLOR_BORDER);
}

QString StyleHelper::getInputStyle()
{
    return QString(R"(
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: white;
            border: 2px solid %1;
            border-radius: 5px;
            padding: 8px 12px;
            color: %2;
            font-size: 14px;
            min-height: 25px;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border: 2px solid %3;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid %2;
            margin-right: 5px;
        }
    )").arg(COLOR_BORDER).arg(COLOR_TEXT_DARK).arg(COLOR_SECONDARY);
}

QString StyleHelper::getTabWidgetStyle()
{
    return QString(R"(
        QTabWidget::pane {
            border: 1px solid %1;
            border-radius: 5px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #f0f0f0;
            color: %2;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        QTabBar::tab:selected {
            background-color: %3;
            color: white;
        }
        QTabBar::tab:hover {
            background-color: %4;
        }
    )").arg(COLOR_BORDER).arg(COLOR_TEXT_DARK).arg(COLOR_SECONDARY).arg(COLOR_LIGHT);
}

QString StyleHelper::getMessageBoxStyle()
{
    return QString(R"(
        QMessageBox {
            background-color: white;
        }
        QMessageBox QLabel {
            color: #333333;
            font-size: 14px;
            min-width: 300px;
        }
        QMessageBox QPushButton {
            background-color: %1;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 8px 20px;
            min-width: 80px;
        }
        QMessageBox QPushButton:hover {
            background-color: %2;
        }
    )").arg(COLOR_SECONDARY).arg(COLOR_ACCENT);
}

void StyleHelper::showInformation(QWidget *parent, const QString &title, const QString &message)
{
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet(getMessageBoxStyle());
    msgBox.exec();
}

void StyleHelper::showWarning(QWidget *parent, const QString &title, const QString &message)
{
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet(getMessageBoxStyle());
    msgBox.exec();
}

void StyleHelper::showError(QWidget *parent, const QString &title, const QString &message)
{
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet(getMessageBoxStyle());
    msgBox.exec();
}

