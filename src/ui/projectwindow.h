#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>

class ProjectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectWindow(const QString &projectName, QWidget *parent = nullptr);
    ~ProjectWindow();

signals:
    void backToDashboard();  // 返回信号

private slots:
    void onBackClicked();
    void onMapViewClicked();
    void on2DViewClicked();
    void on3DViewClicked();
    void onExcavationParamsClicked();
    void onSupplementaryDataClicked();
    void onPositioningClicked();

private:
    void setupUI();
    void createTopBar();
    void createSidebar();
    void createMainContent();
    void loadMapView();
    void load2DView();
    void load3DView();
    void loadExcavationParams();
    void loadSupplementaryData();
    void showPositioningDialog();
    void clearMainContent();
    
    QString projectName;
    QWidget *centralWidget;
    QWidget *sidebar;
    QWidget *mainContent;
    QPushButton *backButton;
    
    // 侧边栏按钮
    QPushButton *mapViewButton;
    QPushButton *view2DButton;
    QPushButton *view3DButton;
    QPushButton *excavationButton;
    QPushButton *supplementaryButton;
    QPushButton *positioningButton;
    
    // 定位输入
    QLineEdit *coordsInput;
    QLineEdit *stakeInput;
};

#endif // PROJECTWINDOW_H
