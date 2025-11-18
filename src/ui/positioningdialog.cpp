#include "positioningdialog.h"
#include "../database/MileageDAO.h"
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
{
    setWindowTitle("定位校准");
    setMinimumSize(600, 500);
    
    // 初始化位置数据
    position.positioningMethod = 1; // 默认使用坐标方式
    position.depth = 15.0;
    position.inclination = 9.83;
    
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
    
    // 标题
    QLabel *titleLabel = new QLabel("定位校准界面：用于确定盾构机的具体位置，三选一即可", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);
    
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
    
    // 确认和取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelBtn = new QPushButton("取消", this);
    cancelBtn->setMinimumWidth(80);
    connect(cancelBtn, &QPushButton::clicked, this, &PositioningDialog::onCancelClicked);
    buttonLayout->addWidget(cancelBtn);
    
    confirmBtn = new QPushButton("确认", this);
    confirmBtn->setMinimumWidth(80);
    confirmBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
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
    
    accept();
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
    
    // 显示确认对话框
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("确认位置");
    confirmBox.setText(QString("确认更改盾构机位置吗?\n\n前盾: (%.6f, %.6f)\n盾尾: (%.6f, %.6f)\n深度: %.2f m\n倾角: %.2f°")
        .arg(frontLon).arg(frontLat)
        .arg(rearLon).arg(rearLat)
        .arg(depth).arg(incl));
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    if (confirmBox.exec() == QMessageBox::Yes) {
        // 保存位置数据
        position.frontLongitude = frontLon;
        position.frontLatitude = frontLat;
        position.rearLongitude = rearLon;
        position.rearLatitude = rearLat;
        position.depth = depth;
        position.inclination = incl;
        
        QMessageBox successBox(this);
        successBox.setWindowTitle("定位成功");
        successBox.setText("盾构机位置已更新");
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
    confirmBox.setText(QString("确认更改盾构机位置吗?\n\n前盾: %1 (%.6f, %.6f)\n盾尾: %2 (%.6f, %.6f)\n深度: %.2f m\n倾角: %.2f°")
        .arg(frontStake)
        .arg(frontPoint.longitude).arg(frontPoint.latitude)
        .arg(rearStake)
        .arg(rearPoint.longitude).arg(rearPoint.latitude)
        .arg(depth).arg(incl));
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    if (confirmBox.exec() == QMessageBox::Yes) {
        // 设置位置数据
        position.frontLatitude = frontPoint.latitude;
        position.frontLongitude = frontPoint.longitude;
        position.frontStakeMark = frontStake;
        
        position.rearLatitude = rearPoint.latitude;
        position.rearLongitude = rearPoint.longitude;
        position.rearStakeMark = rearStake;
        
        position.depth = depth;
        position.inclination = incl;
        
        QMessageBox successBox(this);
        successBox.setWindowTitle("定位成功");
        successBox.setText("盾构机位置已更新");
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
        // 坐标方式
        if (frontCoordsEdit->text().isEmpty() || rearCoordsEdit->text().isEmpty()) {
            QMessageBox::warning(this, "输入错误", "请先点击定位按钮进行定位");
            return false;
        }
    } else if (method == 2) {
        // 桩号方式
        if (frontStakeEdit->text().isEmpty() || rearStakeEdit->text().isEmpty()) {
            QMessageBox::warning(this, "输入错误", "请先点击定位按钮进行定位");
            return false;
        }
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
