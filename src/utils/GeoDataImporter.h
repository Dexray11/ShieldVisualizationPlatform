#ifndef GEODATAIMPORTER_H
#define GEODATAIMPORTER_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>

/**
 * @brief 地质数据导入器 - 用户友好的图形化导入工具
 * 
 * 功能：
 * 1. 直接读取Excel文件（钻孔数据 + 隧道轮廓）
 * 2. 图形化进度显示
 * 3. 实时日志反馈
 * 4. 一键导入所有数据
 */
class GeoDataImporter : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeoDataImporter(int projectId, const QString &projectName, QWidget *parent = nullptr);
    ~GeoDataImporter();
    
signals:
    void importCompleted();  // 导入完成信号
    void importCancelled();  // 取消导入信号
    
private slots:
    void onSelectBoreholeFile();       // 选择钻孔数据文件
    void onSelectTunnelProfileFile();  // 选择隧道轮廓文件
    void onStartImport();              // 开始导入
    void onCancel();                   // 取消
    
private:
    void setupUI();
    void addLog(const QString &message, const QString &type = "info");
    
    /**
     * @brief 导入钻孔数据
     * @param filePath Excel文件路径
     * @return 成功返回true
     */
    bool importBoreholeData(const QString &filePath);
    
    /**
     * @brief 导入隧道轮廓数据
     * @param filePath Excel文件路径
     * @return 成功返回true
     */
    bool importTunnelProfileData(const QString &filePath);
    
    int projectId;
    QString projectName;
    QString boreholeFilePath;
    QString tunnelProfileFilePath;
    
    // UI组件
    QLabel *titleLabel;
    QLabel *projectLabel;
    QPushButton *selectBoreholeButton;
    QPushButton *selectTunnelButton;
    QPushButton *importButton;
    QPushButton *cancelButton;
    QProgressBar *progressBar;
    QTextEdit *logText;
    
    QLabel *boreholeFileLabel;
    QLabel *tunnelFileLabel;
};

#endif // GEODATAIMPORTER_H
