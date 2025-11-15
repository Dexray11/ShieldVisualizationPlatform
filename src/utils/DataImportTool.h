#ifndef DATAIMPORTTOOL_H
#define DATAIMPORTTOOL_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QString>

/**
 * @brief 数据导入工具对话框
 * 
 * 用于从Excel文件导入钻孔数据到数据库
 * 不需要Python环境，直接使用Qt读取Excel
 */
class DataImportTool : public QDialog
{
    Q_OBJECT

public:
    explicit DataImportTool(QWidget *parent = nullptr);
    ~DataImportTool();

private slots:
    void onSelectExcelFile();
    void onImportData();

private:
    void setupUI();
    void logMessage(const QString &message, bool isError = false);
    bool importFromExcel(const QString &excelPath, int projectId = 1);
    
    // UI组件
    QLineEdit *excelPathEdit;
    QPushButton *selectFileButton;
    QPushButton *importButton;
    QTextEdit *logTextEdit;
    QProgressBar *progressBar;
};

#endif // DATAIMPORTTOOL_H
