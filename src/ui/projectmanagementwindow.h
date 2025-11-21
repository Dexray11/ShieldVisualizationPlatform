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
    void onEditProject(int projectId);
    void onDeleteProject(int projectId);
    void onTabChanged(int index);
    void onPublishNews();
    void onDeleteSelectedNews();
    void onEditNews(int row);
    void onDeleteNews(int row);
    void onImportGeoData();  // 导入地质数据槽函数
    void onSearchExcavation();  // 搜索掘进信息
    void onRefreshExcavation();  // 刷新掘进信息
    void onFilterExcavation();  // 筛选掘进信息
    void onExportExcavation();  // 导出掘进信息
    void onSearchSupplementary();  // 搜索补勘信息
    void onRefreshSupplementary();  // 刷新补勘信息
    void onFilterSupplementary();  // 筛选补勘信息
    void onExportSupplementary();  // 导出补勘信息
    void onSearchProject();  // 搜索项目
    void onRefreshProject();  // 刷新项目
    void onSearchWarning();  // 搜索预警信息
    void onRefreshWarning();  // 刷新预警信息
    void onFilterWarning();  // 筛选预警信息
    void onExportWarning();  // 导出预警信息

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
    void loadWarningData();  // 加载预警信息数据
    void loadExcavationData();  // 加载掘进信息数据
    void loadSupplementaryData();  // 加载补勘数据
    void showNewProjectDialog();
    
    QWidget *centralWidget;
    QWidget *sidebar;
    QTabWidget *tabWidget;
    QButtonGroup *sidebarButtons;
    
    // 项目总览标签页组件
    QTableWidget *projectTable;
    QPushButton *newProjectButton;
    QLineEdit *projectSearchEdit;
    QPushButton *searchProjectButton;
    QPushButton *refreshProjectButton;
    
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
