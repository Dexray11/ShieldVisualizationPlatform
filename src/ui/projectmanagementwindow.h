#ifndef PROJECTMANAGEMENTWINDOW_H
#define PROJECTMANAGEMENTWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QButtonGroup>

class ProjectManagementWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectManagementWindow(QWidget *parent = nullptr);
    ~ProjectManagementWindow();

private slots:
    void onBackClicked();
    void onNewProjectClicked();
    void onEditProject(int row);
    void onDeleteProject(int row);
    void onTabChanged(int index);
    void onPublishNews();
    void onDeleteSelectedNews();
    void onEditNews(int row);
    void onDeleteNews(int row);
    void onImportGeoData();  // 导入地质数据槽函数

private:
    void setupUI();
    void createTopBar();
    void createSidebar();
    void createTabWidget();
    void createProjectOverviewTab();
    void createWarningInfoTab();
    void createExcavationInfoTab();
    void createSupplementaryDataTab();
    void createNewsModuleTab();
    void loadProjectData();
    void showNewProjectDialog();
    
    QWidget *centralWidget;
    QWidget *sidebar;
    QTabWidget *tabWidget;
    QButtonGroup *sidebarButtons;
    
    // 项目总览标签页组件
    QTableWidget *projectTable;
    QPushButton *newProjectButton;
    
    // 其他标签页表格
    QTableWidget *warningTable;
    QTableWidget *excavationTable;
    QTableWidget *supplementaryTable;
    QTableWidget *newsTable;
    
    QPushButton *backButton;
    QPushButton *minimizeButton;
    QPushButton *closeButton;
    QPushButton *publishNewsButton;
    QPushButton *deleteNewsButton;
};

#endif // PROJECTMANAGEMENTWINDOW_H
