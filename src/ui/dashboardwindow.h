#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QListWidgetItem>
#include <QFrame>
#include "mapwidget.h"

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

private slots:
    void onProjectSelected(int row, int column);
    void onProjectMarkerClicked(const QString &projectName, const QString &brief,
                               const QString &unit, const QPoint &screenPos);
    void onProjectMarkerDoubleClicked(const QString &projectName);
    void onWorkbenchClicked();
    void openProjectView();
    void onProjectListItemClicked(QListWidgetItem *item);
    void onProjectListItemDoubleClicked(QListWidgetItem *item);
    void hideProjectInfoPopup();

private:
    void setupUI();
    void createTopBar();
    void createSidebar();
    void createMapView();
    void createProjectList();
    void createStatisticsPanel();
    void createContactPanel();
    void loadProjects();
    void showAllProjects();
    void showSingleProject(const QString &projectName);
    
    // UI组件
    QWidget *centralWidget;
    QPushButton *workbenchButton;
    QPushButton *minimizeButton;
    QPushButton *closeButton;
    
    // 侧边栏
    QWidget *sidebar;
    QListWidget *projectList;
    
    // 地图区域
    MapWidget *mapWidget;
    
    // 项目信息弹窗
    QFrame *projectInfoPopup;
    QLabel *popupNameLabel;
    QLabel *popupBriefLabel;
    QLabel *popupUnitLabel;
    
    // 项目列表表格
    QTableWidget *projectTable;
    
    // 统计面板
    QWidget *statisticsPanel;
    QLabel *statisticsLabel;
    QList<QProgressBar*> progressBars;
    QWidget *progressContainer;
    QVBoxLayout *progressContainerLayout;
    
    // 联系人面板
    QWidget *contactPanel;
    QLabel *contact1Label;
    QLabel *contact2Label;
    
    // 当前选中的项目
    QString selectedProject;
};

#endif // DASHBOARDWINDOW_H
