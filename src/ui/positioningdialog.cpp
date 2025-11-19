#include "positioningdialog.h"
#include "../database/MileageDAO.h"
#include "../database/ShieldPositionDAO.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDebug>

PositioningDialog::PositioningDialog(int projectId, QWidget *parent)
    : QDialog(parent)
    , projectId(projectId)
    , positionChanged(false)
{
    setWindowTitle("定位校准");
    setMinimumSize(600, 550);
    
    // 设置对话框背景为白色
    setStyleSheet("QDialog { background-color: white; }"
                  "QGroupBox { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 5px; padding: 10px; margin-top: 10px; }"
                  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; }"
                  "QLabel { color: black; }"
                  "QLineEdit { background-color: white; color: black; border: 1px solid #ccc; padding: 5px; }"
                  "QRadioButton { color: black; }"
                  "QComboBox { background-color: white; color: black; border: 1px solid #ccc; padding: 5px; }");
    
    // 初始化位置数据
    position.positioningMethod = 1; // 默认使用坐标方式
    position.depth = 15.0;
    position.inclination = 9.83;
    position.frontLatitude = 0.0;
    position.frontLongitude = 0.0;
    position.rearLatitude = 0.0;
    position.rearLongitude = 0.0;
    
    // 从数据库加载当前位置
    ShieldPositionDAO shieldDAO;
    if (shieldDAO.hasPosition(projectId)) {
        ShieldPositionDAO::ShieldPosition dbPos = shieldDAO.getPosition(projectId);
        
        // 转换为对话框的位置格式
        position.frontLatitude = dbPos.frontLatitude;
        position.frontLongitude = dbPos.frontLongitude;
        position.frontStakeMark = dbPos.frontStakeMark;
        position.rearLatitude = dbPos.rearLatitude;
        position.rearLongitude = dbPos.rearLongitude;
        position.rearStakeMark = dbPos.rearStakeMark;
        position.depth = dbPos.depth;
        position.inclination = dbPos.inclination;
        position.positioningMethod = dbPos.positioningMethod;
        
        qDebug() << "已加载盾构机位置 - 前盾:" << position.frontStakeMark << "盾尾:" << position.rearStakeMark;
        
        // 加载previous位置作为复位目标
        resetTargetPosition.frontLatitude = dbPos.previousFrontLatitude;
        resetTargetPosition.frontLongitude = dbPos.previousFrontLongitude;
        resetTargetPosition.frontStakeMark = dbPos.previousFrontStakeMark;
        resetTargetPosition.rearLatitude = dbPos.previousRearLatitude;
        resetTargetPosition.rearLongitude = dbPos.previousRearLongitude;
        resetTargetPosition.rearStakeMark = dbPos.previousRearStakeMark;
        resetTargetPosition.depth = dbPos.previousDepth;
        resetTargetPosition.inclination = dbPos.previousInclination;
        resetTargetPosition.positioningMethod = dbPos.positioningMethod;
        
        if (!resetTargetPosition.frontStakeMark.isEmpty()) {
            qDebug() << "复位目标位置 - 前盾:" << resetTargetPosition.frontStakeMark 
                     << "盾尾:" << resetTargetPosition.rearStakeMark;
        } else {
            qDebug() << "无复位目标位置（首次使用或刚保存过）";
        }
    }
    
    // 保存初始位置（用于其他功能）
    lastPosition = position;
    originalPosition = position;
    
    setupUI();
    
    // 默认选择坐标方式
    coordsMethodRadio->setChecked(true);
    onPositioningMethodChanged();
}

PositioningDialog::~PositioningDialog()
{
}

void PositioningDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 标题和说明
    QLabel *titleLabel = new QLabel("定位校准 - 更新盾构机当前位置", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);
    
    // 添加功能说明
    QLabel *descLabel = new QLabel(
        "💡 功能说明：定期更新系统中的盾构机位置，使其与实际施工位置保持一致\n"
        "• 坐标方式：根据全站仪等测量设备获得的精确坐标\n"
        "• 桩号方式：根据已拼装的管片数量推算当前桩号\n"
        "• 更新后，二维视图、三维视图将显示新的盾构机位置\n"
        "• 复位功能：恢复到上次点击【确认保存】之前的位置", this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("QLabel { color: #555; font-size: 10pt; padding: 10px; "
                             "background-color: #f0f8ff; border-left: 3px solid #2196F3; }");
    mainLayout->addWidget(descLabel);
    
    // 定位方式选择
    QGroupBox *methodGroup = new QGroupBox("选择定位方式", this);
    QVBoxLayout *methodLayout = new QVBoxLayout();
    
    this->methodGroup = new QButtonGroup(this);
    
    gpsMethodRadio = new QRadioButton("链接GPS定位装置", this);
    coordsMethodRadio = new QRadioButton("输入坐标确定盾构机位置", this);
    stakeMethodRadio = new QRadioButton("输入桩号确定盾构机位置", this);
    
    this->methodGroup->addButton(gpsMethodRadio, 0);
    this->methodGroup->addButton(coordsMethodRadio, 1);
    this->methodGroup->addButton(stakeMethodRadio, 2);
    
    methodLayout->addWidget(gpsMethodRadio);
    methodLayout->addWidget(coordsMethodRadio);
    methodLayout->addWidget(stakeMethodRadio);
    methodGroup->setLayout(methodLayout);
    mainLayout->addWidget(methodGroup);
    
    // Qt 6中使用idClicked信号
    connect(this->methodGroup, &QButtonGroup::idClicked,
            [this](int) { onPositioningMethodChanged(); });
    
    // GPS方式界面
    gpsWidget = new QWidget(this);
    QVBoxLayout *gpsLayout = new QVBoxLayout(gpsWidget);
    
    QHBoxLayout *gpsDeviceLayout = new QHBoxLayout();
    gpsDeviceLayout->addWidget(new QLabel("GPS设备:", this));
    gpsDeviceCombo = new QComboBox(this);
    gpsDeviceCombo->addItem("演示装置1");
    gpsDeviceCombo->addItem("演示装置2");
    gpsDeviceLayout->addWidget(gpsDeviceCombo);
    gpsDeviceLayout->addStretch();
    gpsLayout->addLayout(gpsDeviceLayout);
    
    gpsStatusLabel = new QLabel("状态：等待GPS信号...", this);
    gpsLayout->addWidget(gpsStatusLabel);
    
    mainLayout->addWidget(gpsWidget);
    
    // 坐标方式界面
    coordsWidget = new QWidget(this);
    QVBoxLayout *coordsLayout = new QVBoxLayout(coordsWidget);
    
    QGridLayout *coordsGrid = new QGridLayout();
    coordsGrid->setSpacing(10);
    
    // 前盾坐标
    coordsGrid->addWidget(new QLabel("前盾坐标:", this), 0, 0);
    frontCoordsEdit = new QLineEdit(this);
    frontCoordsEdit->setPlaceholderText("经度,纬度 例如: 120.36,36.23");
    coordsGrid->addWidget(frontCoordsEdit, 0, 1);
    
    // 盾尾坐标
    coordsGrid->addWidget(new QLabel("盾尾坐标:", this), 1, 0);
    rearCoordsEdit = new QLineEdit(this);
    rearCoordsEdit->setPlaceholderText("经度,纬度 例如: 120.36,36.23");
    coordsGrid->addWidget(rearCoordsEdit, 1, 1);
    
    // 深度
    coordsGrid->addWidget(new QLabel("深度(m):", this), 2, 0);
    coordsDepthEdit = new QLineEdit("15", this);
    coordsGrid->addWidget(coordsDepthEdit, 2, 1);
    
    // 倾角
    coordsGrid->addWidget(new QLabel("倾角(度):", this), 3, 0);
    coordsInclEdit = new QLineEdit("9.83", this);
    coordsGrid->addWidget(coordsInclEdit, 3, 1);
    
    coordsLayout->addLayout(coordsGrid);
    
    locateByCoordsBtn = new QPushButton("定位", this);
    locateByCoordsBtn->setMaximumWidth(100);
    coordsLayout->addWidget(locateByCoordsBtn, 0, Qt::AlignRight);
    
    connect(locateByCoordsBtn, &QPushButton::clicked, 
            this, &PositioningDialog::onLocateByCoords);
    
    mainLayout->addWidget(coordsWidget);
    
    // 桩号方式界面
    stakeWidget = new QWidget(this);
    QVBoxLayout *stakeLayout = new QVBoxLayout(stakeWidget);
    
    QGridLayout *stakeGrid = new QGridLayout();
    stakeGrid->setSpacing(10);
    
    // 前盾桩号
    stakeGrid->addWidget(new QLabel("前盾桩号:", this), 0, 0);
    frontStakeEdit = new QLineEdit(this);
    frontStakeEdit->setPlaceholderText("例如: K1+190.265");
    stakeGrid->addWidget(frontStakeEdit, 0, 1);
    
    // 盾尾桩号
    stakeGrid->addWidget(new QLabel("盾尾桩号:", this), 1, 0);
    rearStakeEdit = new QLineEdit(this);
    rearStakeEdit->setPlaceholderText("例如: K1+210.265");
    stakeGrid->addWidget(rearStakeEdit, 1, 1);
    
    // 深度
    stakeGrid->addWidget(new QLabel("深度(m):", this), 2, 0);
    stakeDepthEdit = new QLineEdit("12", this);
    stakeGrid->addWidget(stakeDepthEdit, 2, 1);
    
    // 倾角
    stakeGrid->addWidget(new QLabel("倾角(度):", this), 3, 0);
    stakeInclEdit = new QLineEdit("11.60", this);
    stakeGrid->addWidget(stakeInclEdit, 3, 1);
    
    stakeLayout->addLayout(stakeGrid);
    
    locateByStakeBtn = new QPushButton("定位", this);
    locateByStakeBtn->setMaximumWidth(100);
    stakeLayout->addWidget(locateByStakeBtn, 0, Qt::AlignRight);
    
    connect(locateByStakeBtn, &QPushButton::clicked,
            this, &PositioningDialog::onLocateByStake);
    
    mainLayout->addWidget(stakeWidget);
    
    // 添加弹簧
    mainLayout->addStretch();
    
    // 确认、复位和取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // 复位按钮放在左侧
    resetBtn = new QPushButton("🔄 复位", this);
    resetBtn->setMinimumWidth(100);
    resetBtn->setToolTip("恢复到上次点击【确认保存】之前的位置");
    // 根据是否有复位目标位置来决定是否启用
    bool hasResetTarget = !resetTargetPosition.frontStakeMark.isEmpty();
    resetBtn->setEnabled(hasResetTarget);
    if (!hasResetTarget) {
        resetBtn->setToolTip("暂无可复位的位置（首次使用或刚保存过）");
    }
    connect(resetBtn, &QPushButton::clicked, this, &PositioningDialog::onResetPosition);
    buttonLayout->addWidget(resetBtn);
    
    buttonLayout->addStretch();
    
    cancelBtn = new QPushButton("取消", this);
    cancelBtn->setMinimumWidth(80);
    connect(cancelBtn, &QPushButton::clicked, this, &PositioningDialog::onCancelClicked);
    buttonLayout->addWidget(cancelBtn);
    
    confirmBtn = new QPushButton("✓ 确认保存", this);
    confirmBtn->setMinimumWidth(100);
    confirmBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    confirmBtn->setToolTip("将新位置保存到数据库（当前位置会成为新的复位目标）");
    connect(confirmBtn, &QPushButton::clicked, this, &PositioningDialog::onConfirmClicked);
    buttonLayout->addWidget(confirmBtn);
    
    mainLayout->addLayout(buttonLayout);
}

void PositioningDialog::onPositioningMethodChanged()
{
    int method = methodGroup->checkedId();
    
    // 隐藏所有输入界面
    gpsWidget->setVisible(method == 0);
    coordsWidget->setVisible(method == 1);
    stakeWidget->setVisible(method == 2);
    
    position.positioningMethod = method;
}

void PositioningDialog::onGPSDeviceSelected(int index)
{
    // 模拟GPS设备连接
    gpsStatusLabel->setText(QString("状态：已连接到 %1").arg(gpsDeviceCombo->currentText()));
    
    // 模拟获取GPS数据
    if (index == 0) {
        position.frontLatitude = 36.23;
        position.frontLongitude = 120.36;
        position.rearLatitude = 36.23;
        position.rearLongitude = 120.36;
        position.depth = 15.0;
        position.inclination = 9.83;
    } else {
        position.frontLatitude = 36.24;
        position.frontLongitude = 120.37;
        position.rearLatitude = 36.24;
        position.rearLongitude = 120.37;
        position.depth = 16.0;
        position.inclination = 10.5;
    }
}

void PositioningDialog::onConfirmClicked()
{
    if (!validateInputs()) {
        return;
    }
    
    // 保存盾构机位置到数据库
    ShieldPositionDAO shieldDAO;
    ShieldPositionDAO::ShieldPosition dbPosition;
    dbPosition.projectId = projectId;
    dbPosition.frontLatitude = position.frontLatitude;
    dbPosition.frontLongitude = position.frontLongitude;
    dbPosition.frontStakeMark = position.frontStakeMark;
    dbPosition.rearLatitude = position.rearLatitude;
    dbPosition.rearLongitude = position.rearLongitude;
    dbPosition.rearStakeMark = position.rearStakeMark;
    dbPosition.depth = position.depth;
    dbPosition.inclination = position.inclination;
    dbPosition.positioningMethod = position.positioningMethod;
    
    if (shieldDAO.savePosition(dbPosition)) {
        qDebug() << "盾构机位置已保存到数据库";
        accept();
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("保存失败");
        msgBox.setText("无法保存盾构机位置到数据库");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
    }
}

void PositioningDialog::onCancelClicked()
{
    reject();
}

void PositioningDialog::onLocateByCoords()
{
    // 解析坐标输入
    QString frontCoords = frontCoordsEdit->text().trimmed();
    QString rearCoords = rearCoordsEdit->text().trimmed();
    
    if (frontCoords.isEmpty() || rearCoords.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("请输入前盾和盾尾坐标");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    // 解析前盾坐标
    QStringList frontParts = frontCoords.split(',');
    if (frontParts.size() != 2) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("格式错误");
        msgBox.setText("前盾坐标格式错误，应为: 经度,纬度");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    // 解析盾尾坐标
    QStringList rearParts = rearCoords.split(',');
    if (rearParts.size() != 2) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("格式错误");
        msgBox.setText("盾尾坐标格式错误，应为: 经度,纬度");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    bool ok;
    double frontLon = frontParts[0].trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("前盾经度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double frontLat = frontParts[1].trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("前盾纬度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double rearLon = rearParts[0].trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("盾尾经度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double rearLat = rearParts[1].trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("盾尾纬度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double depth = coordsDepthEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("深度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double incl = coordsInclEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("倾角格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    // 根据坐标反向查找桩号
    MileageDAO mileageDAO;
    MileageDAO::MileagePoint frontPoint = mileageDAO.getNearestMileagePoint(projectId, frontLat, frontLon);
    MileageDAO::MileagePoint rearPoint = mileageDAO.getNearestMileagePoint(projectId, rearLat, rearLon);
    
    QString frontStakeText = frontPoint.stakeMark.isEmpty() ? 
        QString("坐标 (%1, %2)").arg(frontLon, 0, 'f', 6).arg(frontLat, 0, 'f', 6) : 
        frontPoint.stakeMark;
    QString rearStakeText = rearPoint.stakeMark.isEmpty() ? 
        QString("坐标 (%1, %2)").arg(rearLon, 0, 'f', 6).arg(rearLat, 0, 'f', 6) : 
        rearPoint.stakeMark;
    
    // 显示确认对话框
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("确认位置");
    QString confirmText = QString("确认更改盾构机位置吗?\n\n"
                                  "前盾: %1\n"
                                  "     坐标: (%2, %3)\n"
                                  "盾尾: %4\n"
                                  "     坐标: (%5, %6)\n"
                                  "深度: %7 m\n"
                                  "倾角: %8°")
                          .arg(frontStakeText)
                          .arg(frontLon, 0, 'f', 6).arg(frontLat, 0, 'f', 6)
                          .arg(rearStakeText)
                          .arg(rearLon, 0, 'f', 6).arg(rearLat, 0, 'f', 6)
                          .arg(depth, 0, 'f', 2)
                          .arg(incl, 0, 'f', 2);
    confirmBox.setText(confirmText);
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    if (confirmBox.exec() == QMessageBox::Yes) {
        // 保存当前位置为上次位置（用于复位）
        if (!positionChanged) {
            lastPosition = position;
        }
        
        // 保存新位置数据
        position.frontLongitude = frontLon;
        position.frontLatitude = frontLat;
        position.frontStakeMark = frontPoint.stakeMark;  // 保存计算出的桩号
        position.rearLongitude = rearLon;
        position.rearLatitude = rearLat;
        position.rearStakeMark = rearPoint.stakeMark;   // 保存计算出的桩号
        position.depth = depth;
        position.inclination = incl;
        
        positionChanged = true;
        resetBtn->setEnabled(true);  // 启用复位按钮
        
        qDebug() << "坐标定位成功 - 前盾:" << frontStakeText << "盾尾:" << rearStakeText;
        
        QMessageBox successBox(this);
        successBox.setWindowTitle("定位成功");
        successBox.setText(QString("盾构机位置已更新\n\n前盾: %1\n盾尾: %2\n\n提示：可使用复位按钮恢复到打开对话框时的位置")
                          .arg(frontStakeText).arg(rearStakeText));
        successBox.setIcon(QMessageBox::Information);
        successBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        successBox.exec();
    }
}

void PositioningDialog::onLocateByStake()
{
    QString frontStake = frontStakeEdit->text().trimmed();
    QString rearStake = rearStakeEdit->text().trimmed();
    
    if (frontStake.isEmpty() || rearStake.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("请输入前盾和盾尾桩号");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    // 从数据库查询桩号对应的坐标
    MileageDAO mileageDAO;
    
    MileageDAO::MileagePoint frontPoint = mileageDAO.getMileagePointByStake(projectId, frontStake);
    if (frontPoint.id < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("查询失败");
        msgBox.setText("未找到前盾桩号对应的坐标数据");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    MileageDAO::MileagePoint rearPoint = mileageDAO.getMileagePointByStake(projectId, rearStake);
    if (rearPoint.id < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("查询失败");
        msgBox.setText("未找到盾尾桩号对应的坐标数据");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    bool ok;
    double depth = stakeDepthEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("深度格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    double incl = stakeInclEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("倾角格式错误");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    // 显示确认对话框
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("确认位置");
    QString confirmText = QString("确认更改盾构机位置吗?\n\n"
                                  "前盾: %1\n"
                                  "     坐标: (%2, %3)\n"
                                  "盾尾: %4\n"
                                  "     坐标: (%5, %6)\n"
                                  "深度: %7 m\n"
                                  "倾角: %8°")
                          .arg(frontStake)
                          .arg(frontPoint.longitude, 0, 'f', 6).arg(frontPoint.latitude, 0, 'f', 6)
                          .arg(rearStake)
                          .arg(rearPoint.longitude, 0, 'f', 6).arg(rearPoint.latitude, 0, 'f', 6)
                          .arg(depth, 0, 'f', 2)
                          .arg(incl, 0, 'f', 2);
    confirmBox.setText(confirmText);
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    if (confirmBox.exec() == QMessageBox::Yes) {
        // 保存当前位置为上次位置（用于复位）
        if (!positionChanged) {
            lastPosition = position;
        }
        
        // 设置位置数据
        position.frontLatitude = frontPoint.latitude;
        position.frontLongitude = frontPoint.longitude;
        position.frontStakeMark = frontPoint.stakeMark;  // 使用实际匹配的桩号
        
        position.rearLatitude = rearPoint.latitude;
        position.rearLongitude = rearPoint.longitude;
        position.rearStakeMark = rearPoint.stakeMark;    // 使用实际匹配的桩号
        
        position.depth = depth;
        position.inclination = incl;
        
        positionChanged = true;
        resetBtn->setEnabled(true);  // 启用复位按钮
        
        qDebug() << "桩号定位成功 - 前盾:" << position.frontStakeMark << "盾尾:" << position.rearStakeMark;
        
        QMessageBox successBox(this);
        successBox.setWindowTitle("定位成功");
        successBox.setText(QString("盾构机位置已更新\n\n前盾: %1\n盾尾: %2\n\n提示：可使用复位按钮恢复到打开对话框时的位置")
                          .arg(position.frontStakeMark).arg(position.rearStakeMark));
        successBox.setIcon(QMessageBox::Information);
        successBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        successBox.exec();
    }
}

bool PositioningDialog::validateInputs()
{
    int method = position.positioningMethod;
    
    if (method == 0) {
        // GPS方式，已经从GPS设备获取数据
        return true;
    } else if (method == 1) {
        // 坐标方式 - 检查position对象是否有有效数据
        if (position.frontLongitude == 0.0 && position.frontLatitude == 0.0) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("输入错误");
            msgBox.setText("请先点击【定位】按钮进行坐标定位");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
            msgBox.exec();
            return false;
        }
    } else if (method == 2) {
        // 桩号方式 - 检查position对象是否有有效数据
        if (position.frontStakeMark.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("输入错误");
            msgBox.setText("请先点击【定位】按钮进行桩号定位");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
            msgBox.exec();
            return false;
        }
    }
    
    // 检查必要的字段是否已填充
    if (position.depth == 0.0 || position.inclination == 0.0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("输入错误");
        msgBox.setText("请确保深度和倾角已填写");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return false;
    }
    
    return true;
}

void PositioningDialog::setGPSAvailable(bool available)
{
    gpsMethodRadio->setEnabled(available);
    if (!available) {
        gpsMethodRadio->setToolTip("无可用的GPS设备");
    }
}

void PositioningDialog::setCurrentPosition(const ShieldPosition &pos)
{
    // 这个函数用于从外部设置当前位置
    // 但由于构造函数已经从数据库加载了位置，这个函数通常不需要调用
    // 保留它是为了兼容性
    
    position = pos;
    lastPosition = pos;
    originalPosition = pos;  // 更新原始位置
    positionChanged = false;
    
    // 复位按钮始终启用，无需动态调整
    
    // 如果位置有效，在输入框中显示
    if (pos.positioningMethod == 1 && pos.frontLongitude != 0.0) {
        // 坐标方式
        frontCoordsEdit->setText(QString("%1,%2").arg(pos.frontLongitude, 0, 'f', 6).arg(pos.frontLatitude, 0, 'f', 6));
        rearCoordsEdit->setText(QString("%1,%2").arg(pos.rearLongitude, 0, 'f', 6).arg(pos.rearLatitude, 0, 'f', 6));
        coordsDepthEdit->setText(QString::number(pos.depth, 'f', 2));
        coordsInclEdit->setText(QString::number(pos.inclination, 'f', 2));
    } else if (pos.positioningMethod == 2 && !pos.frontStakeMark.isEmpty()) {
        // 桩号方式
        frontStakeEdit->setText(pos.frontStakeMark);
        rearStakeEdit->setText(pos.rearStakeMark);
        stakeDepthEdit->setText(QString::number(pos.depth, 'f', 2));
        stakeInclEdit->setText(QString::number(pos.inclination, 'f', 2));
    }
}

void PositioningDialog::onResetPosition()
{
    // 检查是否有复位目标
    if (resetTargetPosition.frontStakeMark.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("无法复位");
        msgBox.setText("暂无可复位的位置\n\n"
                      "复位功能将恢复到上次点击【确认保存】之前的位置。\n"
                      "• 如果这是首次使用，还没有历史位置\n"
                      "• 如果刚保存过，上次保存前的位置已被当前位置替代");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox.exec();
        return;
    }
    
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("确认复位");
    confirmBox.setText(QString("确定要恢复到上次保存前的位置吗？\n\n"
                               "复位目标位置（上次点击【确认保存】前）：\n"
                               "  前盾: %1\n"
                               "  盾尾: %2\n"
                               "  深度: %3 m\n"
                               "  倾角: %4°\n\n"
                               "当前显示位置：\n"
                               "  前盾: %5\n"
                               "  盾尾: %6\n\n"
                               "💡 说明：\n"
                               "• 复位将清空所有输入框\n"
                               "• 恢复到复位目标位置\n"
                               "• 丢弃所有未保存的修改")
                      .arg(resetTargetPosition.frontStakeMark)
                      .arg(resetTargetPosition.rearStakeMark)
                      .arg(resetTargetPosition.depth, 0, 'f', 2)
                      .arg(resetTargetPosition.inclination, 0, 'f', 2)
                      .arg(position.frontStakeMark.isEmpty() ? "未设置" : position.frontStakeMark)
                      .arg(position.rearStakeMark.isEmpty() ? "未设置" : position.rearStakeMark));
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    if (confirmBox.exec() == QMessageBox::Yes) {
        // 恢复到复位目标位置
        position = resetTargetPosition;
        lastPosition = resetTargetPosition;
        positionChanged = false;
        
        // 清空所有输入框
        frontCoordsEdit->clear();
        rearCoordsEdit->clear();
        coordsDepthEdit->clear();
        coordsInclEdit->clear();
        frontStakeEdit->clear();
        rearStakeEdit->clear();
        stakeDepthEdit->clear();
        stakeInclEdit->clear();
        
        // 如果有保存的位置，显示在输入框中
        if (resetTargetPosition.positioningMethod == 1 && resetTargetPosition.frontLongitude != 0.0) {
            // 坐标方式
            frontCoordsEdit->setText(QString("%1,%2")
                .arg(resetTargetPosition.frontLongitude, 0, 'f', 6)
                .arg(resetTargetPosition.frontLatitude, 0, 'f', 6));
            rearCoordsEdit->setText(QString("%1,%2")
                .arg(resetTargetPosition.rearLongitude, 0, 'f', 6)
                .arg(resetTargetPosition.rearLatitude, 0, 'f', 6));
            coordsDepthEdit->setText(QString::number(resetTargetPosition.depth, 'f', 2));
            coordsInclEdit->setText(QString::number(resetTargetPosition.inclination, 'f', 2));
            
            // 切换到坐标方式
            coordsMethodRadio->setChecked(true);
            onPositioningMethodChanged();
        } else if (resetTargetPosition.positioningMethod == 2 && !resetTargetPosition.frontStakeMark.isEmpty()) {
            // 桩号方式
            frontStakeEdit->setText(resetTargetPosition.frontStakeMark);
            rearStakeEdit->setText(resetTargetPosition.rearStakeMark);
            stakeDepthEdit->setText(QString::number(resetTargetPosition.depth, 'f', 2));
            stakeInclEdit->setText(QString::number(resetTargetPosition.inclination, 'f', 2));
            
            // 切换到桩号方式
            stakeMethodRadio->setChecked(true);
            onPositioningMethodChanged();
        }
        
        QMessageBox successBox(this);
        successBox.setWindowTitle("复位成功");
        successBox.setText(QString("✅ 已恢复到上次保存前的位置\n\n"
                                  "前盾: %1\n"
                                  "盾尾: %2\n"
                                  "深度: %3 m\n"
                                  "倾角: %4°\n\n"
                                  "💡 提示：如需保存此位置，请点击【确认保存】")
                          .arg(resetTargetPosition.frontStakeMark)
                          .arg(resetTargetPosition.rearStakeMark)
                          .arg(resetTargetPosition.depth, 0, 'f', 2)
                          .arg(resetTargetPosition.inclination, 0, 'f', 2));
        successBox.setIcon(QMessageBox::Information);
        successBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        successBox.exec();
        
        qDebug() << "已复位到上次保存前的位置 - 前盾:" << resetTargetPosition.frontStakeMark 
                 << "盾尾:" << resetTargetPosition.rearStakeMark;
    }
}
