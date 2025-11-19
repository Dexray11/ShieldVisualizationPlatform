#ifndef POSITIONINGDIALOG_H
#define POSITIONINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>

/**
 * @brief 定位校准对话框
 * 
 * 用于确定盾构机位置，支持三种方式：
 * 1. 链接GPS定位装置
 * 2. 输入坐标确定盾构机位置
 * 3. 输入桩号确定盾构机位置
 */
class PositioningDialog : public QDialog
{
    Q_OBJECT

public:
    struct ShieldPosition {
        // 前盾位置
        double frontLatitude;
        double frontLongitude;
        QString frontStakeMark;
        
        // 盾尾位置
        double rearLatitude;
        double rearLongitude;
        QString rearStakeMark;
        
        // 其他参数
        double depth;        // 深度（米）
        double inclination;  // 倾角（度）
        
        // 定位方式：0-GPS, 1-坐标, 2-桩号
        int positioningMethod;
    };

    explicit PositioningDialog(int projectId, QWidget *parent = nullptr);
    ~PositioningDialog();
    
    // 获取定位结果
    ShieldPosition getShieldPosition() const { return position; }
    
    // 设置是否有GPS设备可用
    void setGPSAvailable(bool available);
    
    // 设置当前盾构机位置（用于显示当前位置）
    void setCurrentPosition(const ShieldPosition &pos);

private slots:
    void onPositioningMethodChanged();
    void onGPSDeviceSelected(int index);
    void onConfirmClicked();
    void onCancelClicked();
    void onLocateByCoords();
    void onLocateByStake();
    void onResetPosition();

private:
    void setupUI();
    void updateInputFields();
    bool validateInputs();
    void calculatePositionFromCoords();
    void calculatePositionFromStake();
    
    int projectId;
    ShieldPosition position;
    ShieldPosition lastPosition;     // 保存上一次的位置（暂不使用）
    ShieldPosition originalPosition; // 打开对话框时的原始位置（暂不使用）
    ShieldPosition resetTargetPosition; // 复位目标：上次点击"确认保存"前的位置
    bool positionChanged;            // 标记位置是否已更改
    
    // UI组件
    QButtonGroup *methodGroup;
    QRadioButton *gpsMethodRadio;
    QRadioButton *coordsMethodRadio;
    QRadioButton *stakeMethodRadio;
    
    // GPS方式组件
    QWidget *gpsWidget;
    QComboBox *gpsDeviceCombo;
    QLabel *gpsStatusLabel;
    
    // 坐标方式组件
    QWidget *coordsWidget;
    QLineEdit *frontCoordsEdit;    // 前盾坐标
    QLineEdit *rearCoordsEdit;     // 盾尾坐标
    QLineEdit *coordsDepthEdit;    // 深度
    QLineEdit *coordsInclEdit;     // 倾角
    QPushButton *locateByCoordsBtn;
    
    // 桩号方式组件
    QWidget *stakeWidget;
    QLineEdit *frontStakeEdit;     // 前盾桩号
    QLineEdit *rearStakeEdit;      // 盾尾桩号
    QLineEdit *stakeDepthEdit;     // 深度
    QLineEdit *stakeInclEdit;      // 倾角
    QPushButton *locateByStakeBtn;
    
    // 复位按钮
    QPushButton *resetBtn;
    
    // 确认/取消按钮
    QPushButton *confirmBtn;
    QPushButton *cancelBtn;
};

#endif // POSITIONINGDIALOG_H
