// 在ProjectWindow::loadMapView()中，添加桩号定位功能
// 替换原有的connect(stakeLocateBtn, &QPushButton::clicked, ...）

// 导入MileageDAO
#include "../database/MileageDAO.h"

// 在loadMapView()函数中，桩号定位按钮的连接修改为：
connect(stakeLocateBtn, &QPushButton::clicked, [this, mapWidget, project]() {
    QString stakeMark = stakeInput->text().trimmed();
    if (stakeMark.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入桩号");
        return;
    }
    
    // 从数据库查询桩号对应的坐标
    MileageDAO mileageDAO;
    MileageDAO::MileagePoint point = mileageDAO.getMileagePointByStake(project.getProjectId(), stakeMark);
    
    if (point.id < 0) {
        QMessageBox::warning(this, "未找到", QString("未找到桩号 %1 对应的坐标数据").arg(stakeMark));
        return;
    }
    
    // 定位到桩号位置
    mapWidget->setCenter(point.latitude, point.longitude);
    mapWidget->setZoomLevel(16);
    
    // 更新坐标输入框
    coordsInput->setText(QString("%1,%2").arg(point.longitude, 0, 'f', 6).arg(point.latitude, 0, 'f', 6));
    
    QMessageBox::information(this, "定位成功", 
        QString("已定位到桩号 %1\n坐标: (%.6f, %.6f)\n高程: %.2f m")
        .arg(stakeMark)
        .arg(point.longitude)
        .arg(point.latitude)
        .arg(point.elevation));
});

// 修改showPositioningDialog()函数
void ProjectWindow::showPositioningDialog()
{
    // 获取项目ID
    ProjectDAO projectDAO;
    Project project = projectDAO.getProjectByName(projectName);
    
    if (!project.isValid()) {
        QMessageBox::warning(this, "错误", "无法获取项目信息");
        return;
    }
    
    // 创建并显示定位校准对话框
    PositioningDialog dialog(project.getProjectId(), this);
    
    if (dialog.exec() == QDialog::Accepted) {
        PositioningDialog::ShieldPosition position = dialog.getShieldPosition();
        
        // 显示定位结果
        QString message;
        if (position.positioningMethod == 0) {
            message = "GPS定位成功";
        } else if (position.positioningMethod == 1) {
            message = QString("坐标定位成功:\n"
                            "前盾: (%.6f, %.6f)\n"
                            "盾尾: (%.6f, %.6f)\n"
                            "深度: %.2f m\n"
                            "倾角: %.2f°")
                        .arg(position.frontLongitude)
                        .arg(position.frontLatitude)
                        .arg(position.rearLongitude)
                        .arg(position.rearLatitude)
                        .arg(position.depth)
                        .arg(position.inclination);
        } else {
            message = QString("桩号定位成功:\n"
                            "前盾: %1 (%.6f, %.6f)\n"
                            "盾尾: %2 (%.6f, %.6f)\n"
                            "深度: %.2f m\n"
                            "倾角: %.2f°")
                        .arg(position.frontStakeMark)
                        .arg(position.frontLongitude)
                        .arg(position.frontLatitude)
                        .arg(position.rearStakeMark)
                        .arg(position.rearLongitude)
                        .arg(position.rearLatitude)
                        .arg(position.depth)
                        .arg(position.inclination);
        }
        
        QMessageBox::information(this, "定位校准完成", message);
    }
}

// 在loadMapView()中添加项目进度显示
// 在标题标签后添加：
QLabel *progressLabel = new QLabel(QString("施工进度: %1%").arg(project.getProgress(), 0, 'f', 1), mainContent);
progressLabel->setStyleSheet("font-size: 14px; color: #666; margin-left: 20px;");

// 当前桩号显示（如果有）
MileageDAO mileageDAO;
QList<MileageDAO::MileagePoint> mileagePoints = mileageDAO.getMileagePointsByProject(project.getProjectId());
if (!mileagePoints.isEmpty()) {
    // 根据进度计算当前桩号
    int currentIndex = static_cast<int>(mileagePoints.size() * project.getProgress() / 100.0);
    if (currentIndex >= mileagePoints.size()) {
        currentIndex = mileagePoints.size() - 1;
    }
    
    if (currentIndex >= 0 && currentIndex < mileagePoints.size()) {
        QString currentStake = mileagePoints[currentIndex].stakeMark;
        QLabel *stakeLabel = new QLabel(QString("当前桩号: %1").arg(currentStake), mainContent);
        stakeLabel->setStyleSheet("font-size: 14px; color: #666; margin-left: 20px;");
        
        // 添加到布局（在progressLabel之后）
    }
}
