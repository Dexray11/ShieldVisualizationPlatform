#ifndef MAINMENUWINDOW_H
#define MAINMENUWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

class MainMenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenuWindow(QWidget *parent = nullptr);
    ~MainMenuWindow();

private slots:
    void onOpenProjectClicked();
    void onProjectManagementClicked();
    void onUserManualClicked();
    void onNoticeClicked();

private:
    void setupUI();
    void createTopBar();
    void createMenuButtons();

    QWidget *centralWidget;
    QPushButton *minimizeButton;
    QPushButton *closeButton;

    QPushButton *openProjectButton;
    QPushButton *projectManagementButton;
    QPushButton *userManualButton;
    QPushButton *noticeButton;
};

#endif // MAINMENUWINDOW_H
