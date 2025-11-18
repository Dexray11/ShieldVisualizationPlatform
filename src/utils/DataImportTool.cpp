#include "DataImportTool.h"
#include "../database/BoreholeDAO.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

// Qt 6需要的编码支持
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QStringConverter>
#endif

DataImportTool::DataImportTool(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("钻孔数据导入工具");
    resize(700, 500);
}

DataImportTool::~DataImportTool()
{
}

void DataImportTool::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 标题
    QLabel *titleLabel = new QLabel("钻孔数据导入工具", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 文件选择区域
    QHBoxLayout *fileLayout = new QHBoxLayout();
    QLabel *fileLabel = new QLabel("Excel文件:", this);
    excelPathEdit = new QLineEdit(this);
    excelPathEdit->setReadOnly(true);
    excelPathEdit->setPlaceholderText("请选择 起大区间.xlsx 文件...");
    selectFileButton = new QPushButton("浏览...", this);
    
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(excelPathEdit, 1);
    fileLayout->addWidget(selectFileButton);
    mainLayout->addLayout(fileLayout);
    
    // 说明文字
    QLabel *infoLabel = new QLabel(
        "说明：本工具支持导入CSV格式的钻孔数据。\n"
        "      如果Excel文件是.xlsx格式，请先在Excel中另存为CSV格式。\n"
        "      文件格式：起大区间.csv", this);
    infoLabel->setStyleSheet("color: #666; padding: 10px; background-color: #f5f5f5; border-radius: 5px;");
    mainLayout->addWidget(infoLabel);
    
    // 日志区域
    QLabel *logLabel = new QLabel("导入日志:", this);
    mainLayout->addWidget(logLabel);
    
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setStyleSheet("font-family: 'Courier New', monospace; font-size: 9pt;");
    mainLayout->addWidget(logTextEdit);
    
    // 进度条
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    importButton = new QPushButton("开始导入", this);
    importButton->setEnabled(false);
    QPushButton *closeButton = new QPushButton("关闭", this);
    
    buttonLayout->addWidget(importButton);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(selectFileButton, &QPushButton::clicked, this, &DataImportTool::onSelectExcelFile);
    connect(importButton, &QPushButton::clicked, this, &DataImportTool::onImportData);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}

void DataImportTool::onSelectExcelFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择钻孔数据文件",
        "",
        "CSV文件 (*.csv);;所有文件 (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        excelPathEdit->setText(fileName);
        importButton->setEnabled(true);
        logMessage("已选择文件: " + fileName);
    }
}

void DataImportTool::onImportData()
{
    QString excelPath = excelPathEdit->text();
    if (excelPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要导入的文件！");
        return;
    }
    
    // 禁用按钮
    importButton->setEnabled(false);
    selectFileButton->setEnabled(false);
    progressBar->setVisible(true);
    
    logMessage("======================================");
    logMessage("开始导入数据...");
    logMessage("======================================");
    
    // 执行导入
    bool success = importFromExcel(excelPath, 1);
    
    // 恢复按钮
    importButton->setEnabled(true);
    selectFileButton->setEnabled(true);
    progressBar->setVisible(false);
    
    if (success) {
        logMessage("======================================", false);
        logMessage("✓ 数据导入成功！", false);
        logMessage("======================================", false);
        QMessageBox::information(this, "成功", "钻孔数据导入成功！\n可以重新运行程序查看数据。");
    } else {
        logMessage("======================================", true);
        logMessage("✗ 数据导入失败！", true);
        logMessage("======================================", true);
        QMessageBox::critical(this, "错误", "数据导入失败！\n请查看日志了解详情。");
    }
}

void DataImportTool::logMessage(const QString &message, bool isError)
{
    QString coloredMessage;
    if (isError) {
        coloredMessage = QString("<font color='red'>%1</font>").arg(message);
    } else if (message.startsWith("✓") || message.contains("成功")) {
        coloredMessage = QString("<font color='green'>%1</font>").arg(message);
    } else if (message.startsWith("⚠") || message.contains("警告")) {
        coloredMessage = QString("<font color='orange'>%1</font>").arg(message);
    } else {
        coloredMessage = message;
    }
    
    logTextEdit->append(coloredMessage);
    qDebug() << message;
}

bool DataImportTool::importFromExcel(const QString &excelPath, int projectId)
{
    QFile file(excelPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logMessage("✗ 无法打开文件: " + excelPath, true);
        return false;
    }
    
    logMessage("✓ 文件打开成功");
    
    BoreholeDAO dao;
    
    // 先删除旧数据
    logMessage("正在清空项目 " + QString::number(projectId) + " 的旧数据...");
    if (!dao.deleteBoreholesByProjectId(projectId)) {
        logMessage("⚠ 清空旧数据失败: " + dao.getLastError(), true);
    } else {
        logMessage("✓ 旧数据已清空");
    }
    
    QTextStream in(&file);
    // Qt 6默认使用UTF-8编码
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
    #endif
    // Qt 6无需设置编码，默认即为UTF-8
    
    QString line;
    int lineNumber = 0;
    int boreholeCount = 0;
    int layerCount = 0;
    
    BoreholeData currentBorehole;
    bool hasBorehole = false;
    
    // 跳过第一行标题
    if (!in.atEnd()) {
        line = in.readLine();
        lineNumber++;
        logMessage("跳过标题行: " + line.left(50) + "...");
    }
    
    while (!in.atEnd()) {
        line = in.readLine();
        lineNumber++;
        
        if (line.trimmed().isEmpty()) {
            continue;
        }
        
        // 解析CSV行（简单的逗号分隔，不处理引号内的逗号）
        QStringList fields = line.split(',');
        
        if (fields.size() < 14) {
            logMessage(QString("⚠ 行 %1: 字段数不足，跳过").arg(lineNumber));
            continue;
        }
        
        // 字段索引
        QString seriesNum = fields[0].trimmed();      // 序号
        QString boreholeCode = fields[2].trimmed();   // 勘探点编号
        QString xCoord = fields[3].trimmed();         // x坐标
        QString yCoord = fields[4].trimmed();         // y坐标
        QString surfaceElev = fields[5].trimmed();    // 孔口高程
        QString mileage = fields[6].trimmed();        // 里程
        QString layerCode = fields[7].trimmed();      // 地层编号
        QString eraGenesis = fields[8].trimmed();     // 时代成因
        QString rockName = fields[12].trimmed();      // 岩土名称
        QString bottomElev = fields[9].trimmed();     // 层底标高
        QString bottomDepth = fields[10].trimmed();   // 层底深度
        QString thickness = fields[11].trimmed();     // 分层厚度
        QString characteristics = fields[13].trimmed(); // 特征
        
        // 判断是否是新钻孔（序号不为空）
        bool isNewBorehole = !seriesNum.isEmpty() && !boreholeCode.isEmpty();
        
        if (isNewBorehole) {
            // 保存上一个钻孔
            if (hasBorehole) {
                int bhId = dao.insertBorehole(currentBorehole);
                if (bhId > 0) {
                    boreholeCount++;
                    layerCount += currentBorehole.layers.size();
                    logMessage(QString("  ✓ 钻孔 %1: %2 (高程: %3m, 里程: %4m, 地层数: %5)")
                              .arg(boreholeCount)
                              .arg(currentBorehole.boreholeCode)
                              .arg(currentBorehole.surfaceElevation, 0, 'f', 2)
                              .arg(currentBorehole.mileage, 0, 'f', 1)
                              .arg(currentBorehole.layers.size()));
                } else {
                    logMessage("✗ 插入钻孔失败: " + dao.getLastError(), true);
                }
            }
            
            // 创建新钻孔
            currentBorehole = BoreholeData();
            currentBorehole.projectId = projectId;
            currentBorehole.boreholeCode = boreholeCode;
            currentBorehole.x = xCoord.toDouble();
            currentBorehole.y = yCoord.toDouble();
            currentBorehole.surfaceElevation = surfaceElev.toDouble();
            currentBorehole.mileage = mileage.toDouble();
            hasBorehole = true;
        }
        
        // 添加地层数据（只要有地层编号和岩土名称）
        if (hasBorehole && !layerCode.isEmpty() && !rockName.isEmpty()) {
            BoreholeLayerData layer;
            layer.layerCode = layerCode;
            layer.eraGenesis = eraGenesis;
            layer.rockName = rockName;
            layer.bottomElevation = bottomElev.toDouble();
            layer.bottomDepth = bottomDepth.toDouble();
            layer.thickness = thickness.toDouble();
            layer.characteristics = characteristics;
            
            // 提取地层序号
            QRegularExpression re("\\d+");
            QRegularExpressionMatch match = re.match(layerCode);
            if (match.hasMatch()) {
                layer.layerNumber = match.captured(0).toInt();
            } else {
                layer.layerNumber = 0;
            }
            
            currentBorehole.layers.append(layer);
        }
    }
    
    // 保存最后一个钻孔
    if (hasBorehole) {
        int bhId = dao.insertBorehole(currentBorehole);
        if (bhId > 0) {
            boreholeCount++;
            layerCount += currentBorehole.layers.size();
            logMessage(QString("  ✓ 钻孔 %1: %2 (高程: %3m, 里程: %4m, 地层数: %5)")
                      .arg(boreholeCount)
                      .arg(currentBorehole.boreholeCode)
                      .arg(currentBorehole.surfaceElevation, 0, 'f', 2)
                      .arg(currentBorehole.mileage, 0, 'f', 1)
                      .arg(currentBorehole.layers.size()));
        } else {
            logMessage("✗ 插入钻孔失败: " + dao.getLastError(), true);
        }
    }
    
    file.close();
    
    logMessage("");
    logMessage(QString("导入完成！共导入 %1 个钻孔，%2 个地层").arg(boreholeCount).arg(layerCount));
    
    return boreholeCount > 0;
}
