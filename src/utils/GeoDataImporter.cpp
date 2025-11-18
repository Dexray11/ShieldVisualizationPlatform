#include "GeoDataImporter.h"
#include "../database/BoreholeDAO.h"
#include "../database/TunnelProfileDAO.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

GeoDataImporter::GeoDataImporter(int projectId, const QString &projectName, QWidget *parent)
    : QWidget(parent)
    , projectId(projectId)
    , projectName(projectName)
{
    setupUI();
    setWindowTitle("导入地质数据");
    setWindowModality(Qt::ApplicationModal);
    resize(700, 650);
}

GeoDataImporter::~GeoDataImporter()
{
}

void GeoDataImporter::setupUI()
{
    // 设置窗口样式
    setStyleSheet(
        "QWidget { background-color: white; }"
        "QLabel { color: #333333; }"
        "QPushButton { "
        "    background-color: #2196F3; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 4px; "
        "    padding: 8px 16px; "
        "    font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
        "QPushButton:pressed { background-color: #0D47A1; }"
        "QPushButton:disabled { background-color: #BDBDBD; }"
        "QTextEdit { "
        "    background-color: #FAFAFA; "
        "    border: 1px solid #E0E0E0; "
        "    border-radius: 4px; "
        "    padding: 8px; "
        "}"
        "QProgressBar { "
        "    border: 1px solid #E0E0E0; "
        "    border-radius: 4px; "
        "    text-align: center; "
        "    background-color: #F5F5F5; "
        "}"
        "QProgressBar::chunk { "
        "    background-color: #4CAF50; "
        "    border-radius: 3px; "
        "}"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 标题
    titleLabel = new QLabel("地质数据导入工具", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 项目信息
    projectLabel = new QLabel(QString("项目: %1 (ID: %2)").arg(projectName).arg(projectId), this);
    projectLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(projectLabel);
    
    // 重要提示
    QLabel *tipLabel = new QLabel(this);
    tipLabel->setText(QString::fromUtf8(
        "💡 <b>使用提示：</b><br>"
        "• 支持Excel文件（.xlsx）和CSV文件（.csv）<br>"
        "• <b>如果Excel无法直接导入</b>，请在Excel中将文件另存为\"CSV UTF-8（逗号分隔）\"格式<br>"
        "• 确保文件编码为UTF-8，避免中文乱码"
    ));
    tipLabel->setWordWrap(true);
    tipLabel->setStyleSheet(
        "background-color: #E3F2FD; "
        "color: #1976D2; "
        "padding: 10px; "
        "border-left: 4px solid #2196F3; "
        "border-radius: 4px;"
    );
    mainLayout->addWidget(tipLabel);
    
    mainLayout->addSpacing(10);
    
    // 钻孔数据文件选择
    QHBoxLayout *boreholeLayout = new QHBoxLayout();
    selectBoreholeButton = new QPushButton("📁 选择钻孔数据文件", this);
    selectBoreholeButton->setFixedHeight(40);
    boreholeFileLabel = new QLabel("未选择文件", this);
    boreholeFileLabel->setStyleSheet("color: gray;");
    boreholeLayout->addWidget(selectBoreholeButton);
    boreholeLayout->addWidget(boreholeFileLabel, 1);
    mainLayout->addLayout(boreholeLayout);
    
    // 隧道轮廓文件选择
    QHBoxLayout *tunnelLayout = new QHBoxLayout();
    selectTunnelButton = new QPushButton("📁 选择隧道轮廓文件", this);
    selectTunnelButton->setFixedHeight(40);
    tunnelFileLabel = new QLabel("未选择文件", this);
    tunnelFileLabel->setStyleSheet("color: gray;");
    tunnelLayout->addWidget(selectTunnelButton);
    tunnelLayout->addWidget(tunnelFileLabel, 1);
    mainLayout->addLayout(tunnelLayout);
    
    mainLayout->addSpacing(10);
    
    // 进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(progressBar);
    
    // 日志区域
    QLabel *logLabel = new QLabel("导入日志:", this);
    mainLayout->addWidget(logLabel);
    
    logText = new QTextEdit(this);
    logText->setReadOnly(true);
    logText->setMinimumHeight(200);
    mainLayout->addWidget(logText);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    importButton = new QPushButton(QString::fromUtf8("🚀 开始导入"), this);
    importButton->setFixedSize(120, 40);
    importButton->setStyleSheet("QPushButton { background-color: #4CAF50; } QPushButton:hover { background-color: #45a049; }");
    importButton->setEnabled(false);  // 初始禁用，直到选择文件
    
    cancelButton = new QPushButton(QString::fromUtf8("取消"), this);
    cancelButton->setFixedSize(100, 40);
    cancelButton->setStyleSheet("QPushButton { background-color: #757575; }");
    
    buttonLayout->addWidget(importButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(selectBoreholeButton, &QPushButton::clicked, this, &GeoDataImporter::onSelectBoreholeFile);
    connect(selectTunnelButton, &QPushButton::clicked, this, &GeoDataImporter::onSelectTunnelProfileFile);
    connect(importButton, &QPushButton::clicked, this, &GeoDataImporter::onStartImport);
    connect(cancelButton, &QPushButton::clicked, this, &GeoDataImporter::onCancel);
    
    addLog(QString::fromUtf8("欢迎使用地质数据导入工具！"), "success");
    addLog(QString::fromUtf8("请选择要导入的Excel文件..."));
}

void GeoDataImporter::onSelectBoreholeFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择钻孔数据文件",
        "",
        "数据文件 (*.xlsx *.xls *.csv);;Excel Files (*.xlsx *.xls);;CSV Files (*.csv);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        boreholeFilePath = fileName;
        QFileInfo fileInfo(fileName);
        boreholeFileLabel->setText(fileInfo.fileName());
        boreholeFileLabel->setStyleSheet("color: green; font-weight: bold;");
        addLog(QString("✓ 已选择钻孔数据文件: %1").arg(fileInfo.fileName()), "success");
        
        // 检查是否可以启用导入按钮
        if (!boreholeFilePath.isEmpty() || !tunnelProfileFilePath.isEmpty()) {
            importButton->setEnabled(true);
        }
    }
}

void GeoDataImporter::onSelectTunnelProfileFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择隧道轮廓文件",
        "",
        "数据文件 (*.xlsx *.xls *.csv);;Excel Files (*.xlsx *.xls);;CSV Files (*.csv);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        tunnelProfileFilePath = fileName;
        QFileInfo fileInfo(fileName);
        tunnelFileLabel->setText(fileInfo.fileName());
        tunnelFileLabel->setStyleSheet("color: green; font-weight: bold;");
        addLog(QString("✓ 已选择隧道轮廓文件: %1").arg(fileInfo.fileName()), "success");
        
        // 检查是否可以启用导入按钮
        if (!boreholeFilePath.isEmpty() || !tunnelProfileFilePath.isEmpty()) {
            importButton->setEnabled(true);
        }
    }
}

void GeoDataImporter::onStartImport()
{
    // 询问是否清除已有数据
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(QString::fromUtf8("确认导入"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString::fromUtf8("是否清除该项目的已有地质数据？"));
    
    QString infoText;
    infoText += QString::fromUtf8("选择【是】：清除旧数据后导入新数据（推荐）\n");
    infoText += QString::fromUtf8("选择【否】：在已有数据基础上追加导入");
    msgBox.setInformativeText(infoText);
    
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
    
    int reply = msgBox.exec();
    
    if (reply == QMessageBox::Cancel) {
        return;
    }
    
    bool clearOldData = (reply == QMessageBox::Yes);
    
    // 禁用按钮防止重复点击
    importButton->setEnabled(false);
    selectBoreholeButton->setEnabled(false);
    selectTunnelButton->setEnabled(false);
    
    addLog(QString::fromUtf8("========== 开始导入数据 =========="), "info");
    progressBar->setValue(0);
    
    // 如果选择清除旧数据
    if (clearOldData) {
        addLog(QString::fromUtf8("正在清除已有数据..."), "info");
        
        BoreholeDAO boreholeDao;
        TunnelProfileDAO profileDao;
        
        if (boreholeDao.deleteBoreholesByProjectId(projectId)) {
            addLog(QString::fromUtf8("✓ 已清除旧的钻孔数据"), "success");
        }
        
        if (profileDao.deleteProfilesByProjectId(projectId)) {
            addLog(QString::fromUtf8("✓ 已清除旧的隧道轮廓数据"), "success");
        }
    }
    
    bool success = true;
    int totalSteps = 0;
    int currentStep = 0;
    
    if (!boreholeFilePath.isEmpty()) totalSteps++;
    if (!tunnelProfileFilePath.isEmpty()) totalSteps++;
    
    // 导入钻孔数据
    if (!boreholeFilePath.isEmpty()) {
        addLog(QString::fromUtf8("正在导入钻孔数据..."), "info");
        if (importBoreholeData(boreholeFilePath)) {
            currentStep++;
            progressBar->setValue((currentStep * 100) / totalSteps);
            addLog(QString::fromUtf8("✓ 钻孔数据导入成功！"), "success");
        } else {
            addLog(QString::fromUtf8("✗ 钻孔数据导入失败！"), "error");
            success = false;
        }
    }
    
    // 导入隧道轮廓数据
    if (!tunnelProfileFilePath.isEmpty() && success) {
        addLog(QString::fromUtf8("正在导入隧道轮廓数据..."), "info");
        if (importTunnelProfileData(tunnelProfileFilePath)) {
            currentStep++;
            progressBar->setValue((currentStep * 100) / totalSteps);
            addLog(QString::fromUtf8("✓ 隧道轮廓数据导入成功！"), "success");
        } else {
            addLog(QString::fromUtf8("✗ 隧道轮廓数据导入失败！"), "error");
            success = false;
        }
    }
    
    if (success) {
        progressBar->setValue(100);
        addLog(QString::fromUtf8("========== 导入完成！ =========="), "success");
        
        QMessageBox msgBox2(this);
        msgBox2.setWindowTitle(QString::fromUtf8("导入成功"));
        msgBox2.setIcon(QMessageBox::Information);
        msgBox2.setText(QString::fromUtf8("地质数据已成功导入数据库！"));
        msgBox2.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox2.exec();
        
        emit importCompleted();
        close();
    } else {
        addLog(QString::fromUtf8("========== 导入失败 =========="), "error");
        
        QMessageBox msgBox3(this);
        msgBox3.setWindowTitle(QString::fromUtf8("导入失败"));
        msgBox3.setIcon(QMessageBox::Warning);
        msgBox3.setText(QString::fromUtf8("部分数据导入失败，请查看日志了解详情。"));
        msgBox3.setStyleSheet("QMessageBox { background-color: white; } QLabel { color: black; }");
        msgBox3.exec();
        
        // 重新启用按钮
        importButton->setEnabled(true);
        selectBoreholeButton->setEnabled(true);
        selectTunnelButton->setEnabled(true);
    }
}

void GeoDataImporter::onCancel()
{
    emit importCancelled();
    close();
}

void GeoDataImporter::addLog(const QString &message, const QString &type)
{
    QString color = "black";
    if (type == "success") color = "green";
    else if (type == "error") color = "red";
    else if (type == "warning") color = "orange";
    
    QString html = QString("<span style='color: %1;'>%2</span>").arg(color, message);
    logText->append(html);
    
    // 滚动到底部
    QTextCursor cursor = logText->textCursor();
    cursor.movePosition(QTextCursor::End);
    logText->setTextCursor(cursor);
    
    QApplication::processEvents();
}

bool GeoDataImporter::importBoreholeData(const QString &filePath)
{
    try {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            addLog(QString("无法打开文件: %1").arg(file.errorString()), "error");
            return false;
        }
        
        QTextStream in(&file);
        
        // 设置编码为UTF-8
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif
        // Qt 6默认使用UTF-8编码
        
        // 读取第一行并处理UTF-8 BOM
        QString firstLine = in.readLine();
        if (firstLine.startsWith("\xEF\xBB\xBF") || firstLine.startsWith(QChar(0xFEFF))) {
            // 移除BOM标记
            if (firstLine.startsWith("\xEF\xBB\xBF")) {
                firstLine = firstLine.mid(3);
            } else {
                firstLine = firstLine.mid(1);
            }
        }
        addLog(QString::fromUtf8("表头: ") + firstLine.left(50) + "...");
        
        // 检查下一行是否为空行（只包含逗号）
        qint64 pos = in.pos();  // 保存当前位置
        QString nextLine = in.readLine();
        
        // 如果下一行是空行或全是逗号，跳过它
        if (nextLine.trimmed().isEmpty() || 
            nextLine.remove(QRegularExpression("[,\\s]")).isEmpty()) {
            addLog(QString::fromUtf8("跳过空行"));
        } else {
            // 不是空行，回退到保存的位置
            in.seek(pos);
            addLog(QString::fromUtf8("没有空行，直接读取数据"));
        }
        
        addLog(QString::fromUtf8("正在读取钻孔数据..."));
        
        BoreholeDAO dao;
        
        int totalRows = 0;
        int importedBoreholes = 0;
        int importedLayers = 0;
        
        // 用于跟踪当前钻孔
        QString currentBoreholeCode;
        int currentBoreholeId = -1;
        BoreholeData currentBorehole;
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            
            if (fields.count() < 14) {
                continue;  // 跳过不完整的行
            }
            
            totalRows++;
            
            // 检查序号列（第0列），判断是否是新钻孔
            QString seqStr = fields[0].trimmed();
            QString boreholeCode = fields[2].trimmed();
            
            // 如果序号不为空且钻孔编号不为空，说明是新钻孔
            if (!seqStr.isEmpty() && !boreholeCode.isEmpty()) {
                // 保存前一个钻孔的地层
                if (currentBoreholeId != -1 && !currentBorehole.layers.isEmpty()) {
                    for (const auto &layer : currentBorehole.layers) {
                        if (dao.insertBoreholeLayer(layer)) {
                            importedLayers++;
                        }
                    }
                }
                
                // 开始新钻孔
                currentBorehole = BoreholeData();
                currentBorehole.projectId = projectId;
                currentBorehole.boreholeCode = boreholeCode;
                
                // 解析数字字段，trimmed()去除空格
                currentBorehole.x = fields[3].trimmed().toDouble();
                currentBorehole.y = fields[4].trimmed().toDouble();
                currentBorehole.surfaceElevation = fields[5].trimmed().toDouble();
                
                // 解析里程字段，处理可能的桩号前缀（如"YCK02+540.400"）
                QString mileageStr = fields[6].trimmed();
                if (mileageStr.contains('+')) {
                    // 格式：YCK##+offset，需要计算绝对里程
                    QStringList parts = mileageStr.split('+');
                    if (parts.size() >= 2) {
                        QString stakePrefix = parts[0].trimmed();  // "YCK02"
                        double offset = parts[1].trimmed().toDouble();  // 540.400
                        
                        // 提取桩号数字部分（YCK02 → 02 → 2）
                        QString stakeNumStr = stakePrefix;
                        stakeNumStr.remove(QRegularExpression("[^0-9]"));  // 移除非数字字符
                        int stakeNum = stakeNumStr.toInt();
                        
                        // 计算绝对里程：桩号×1000 + 偏移
                        // YCK02+540.400 = 2×1000 + 540.400 = 2540.400
                        currentBorehole.mileage = stakeNum * 1000.0 + offset;
                    } else {
                        currentBorehole.mileage = mileageStr.toDouble();
                    }
                } else {
                    // 没有'+'，直接是绝对里程
                    currentBorehole.mileage = mileageStr.toDouble();
                }
                
                currentBoreholeId = dao.insertBorehole(currentBorehole);
                if (currentBoreholeId > 0) {
                    importedBoreholes++;
                    currentBoreholeCode = boreholeCode;
                    currentBorehole.layers.clear();
                }
            }
            
            // 读取地层数据
            if (currentBoreholeId > 0) {
                BoreholeLayerData layer;
                layer.boreholeId = currentBoreholeId;
                layer.layerCode = fields[7].trimmed();
                layer.eraGenesis = fields[8].trimmed();
                // 数字字段也需要trim掉空格
                layer.bottomElevation = fields[9].trimmed().toDouble();
                layer.bottomDepth = fields[10].trimmed().toDouble();
                layer.thickness = fields[11].trimmed().toDouble();
                layer.rockName = fields[12].trimmed();
                layer.characteristics = fields[13].trimmed();
                
                currentBorehole.layers.append(layer);
            }
            
            // 更新进度
            if (totalRows % 10 == 0) {
                QApplication::processEvents();
            }
        }
        
        // 保存最后一个钻孔的地层
        if (currentBoreholeId != -1 && !currentBorehole.layers.isEmpty()) {
            for (const auto &layer : currentBorehole.layers) {
                if (dao.insertBoreholeLayer(layer)) {
                    importedLayers++;
                }
            }
        }
        
        file.close();
        
        addLog(QString("成功导入 %1 个钻孔, %2 个地层").arg(importedBoreholes).arg(importedLayers), "success");
        return true;
        
    } catch (const std::exception &e) {
        addLog(QString("导入钻孔数据时发生异常: %1").arg(e.what()), "error");
        return false;
    }
}

bool GeoDataImporter::importTunnelProfileData(const QString &filePath)
{
    try {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            addLog(QString("无法打开文件: %1").arg(file.errorString()), "error");
            return false;
        }
        
        QTextStream in(&file);
        
        // 设置编码为UTF-8
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif
        // Qt 6默认使用UTF-8编码
        
        TunnelProfileDAO dao;
        
        // 读取第一行并处理UTF-8 BOM
        QString firstLine = in.readLine();
        if (firstLine.startsWith("\xEF\xBB\xBF") || firstLine.startsWith(QChar(0xFEFF))) {
            // 移除BOM标记
            if (firstLine.startsWith("\xEF\xBB\xBF")) {
                firstLine = firstLine.mid(3);
            } else {
                firstLine = firstLine.mid(1);
            }
        }
        addLog(QString::fromUtf8("表头: ") + firstLine.left(50) + "...");
        
        // 检查下一行是否为空行（只包含逗号）
        qint64 pos = in.pos();  // 保存当前位置
        QString nextLine = in.readLine();
        
        // 如果下一行是空行或全是逗号，跳过它
        if (nextLine.trimmed().isEmpty() || 
            nextLine.remove(QRegularExpression("[,\\s]")).isEmpty()) {
            addLog(QString::fromUtf8("跳过空行"));
        } else {
            // 不是空行，回退到保存的位置
            in.seek(pos);
            addLog(QString::fromUtf8("没有空行，直接读取数据"));
        }
        
        addLog(QString::fromUtf8("正在读取隧道轮廓数据..."));
        
        int importedProfiles = 0;
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            
            if (fields.count() < 18) {
                continue;  // 跳过不完整的行
            }
            
            TunnelProfileData profile;
            profile.projectId = projectId;
            
            // 读取数据（根据CSV结构），trim去除空格
            profile.nearBorehole = fields[1].trimmed();
            profile.topLeftX = fields[2].trimmed().toDouble();
            profile.topLeftY = fields[3].trimmed().toDouble();
            profile.topLeftZ = fields[4].trimmed().toDouble();
            profile.bottomLeftX = fields[6].trimmed().toDouble();
            profile.bottomLeftY = fields[7].trimmed().toDouble();
            profile.bottomLeftZ = fields[8].trimmed().toDouble();
            profile.topRightX = fields[10].trimmed().toDouble();
            profile.topRightY = fields[11].trimmed().toDouble();
            profile.topRightZ = fields[12].trimmed().toDouble();
            profile.bottomRightX = fields[14].trimmed().toDouble();
            profile.bottomRightY = fields[15].trimmed().toDouble();
            profile.bottomRightZ = fields[16].trimmed().toDouble();
            
            // 计算里程（使用顶部中心点的Y坐标作为里程近似值）
            profile.mileage = (profile.topLeftY + profile.topRightY) / 2.0;
            
            if (dao.insertProfile(profile) > 0) {
                importedProfiles++;
            }
            
            // 更新进度
            if (importedProfiles % 5 == 0) {
                QApplication::processEvents();
            }
        }
        
        file.close();
        
        addLog(QString("成功导入 %1 个隧道断面").arg(importedProfiles), "success");
        return true;
        
    } catch (const std::exception &e) {
        addLog(QString("导入隧道轮廓数据时发生异常: %1").arg(e.what()), "error");
        return false;
    }
}
