#ifndef DATASIMULATOR_H
#define DATASIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QDateTime>

class DatabaseManager;

/**
 * @brief 数据模拟器
 * 在演示/测试模式下生成模拟的传感器数据
 */
class DataSimulator : public QObject
{
    Q_OBJECT

public:
    explicit DataSimulator(DatabaseManager* dbManager, QObject* parent = nullptr);
    ~DataSimulator();

    // 启动模拟
    void start(int projectId, int intervalMs = 5000);
    
    // 停止模拟
    void stop();
    
    // 获取模拟状态
    bool isRunning() const;
    
    // 设置模拟参数
    void setInterval(int ms);
    void setProjectId(int projectId);
    void setCurrentMileage(double mileage);
    
    // 设置地质条件（从CSV数据推断）
    void setGeologicalCondition(const QString& rockType, double hardness);

signals:
    // 生成新的掘进参数数据
    void excavationDataGenerated(int projectId, const QJsonObject& data);
    
    // 生成新的补勘数据
    void prospectingDataGenerated(int projectId, const QJsonObject& data);
    
    // 模拟状态改变
    void statusChanged(bool running);

private slots:
    void generateData();

private:
    // 生成掘进参数
    QJsonObject generateExcavationData();
    
    // 生成补勘数据
    QJsonObject generateProspectingData();
    
    // 根据岩层类型计算参数
    double calculateThrustForce(const QString& rockType);
    double calculateCutterTorque(const QString& rockType);
    double calculateExcavationSpeed(const QString& rockType);
    double calculatePWaveVelocity(const QString& rockType);
    double calculateSWaveVelocity(const QString& rockType);
    
    // 添加随机波动
    double addNoise(double value, double percentage = 5.0);

private:
    QTimer* m_timer;
    DatabaseManager* m_dbManager;
    int m_projectId;
    double m_currentMileage;
    int m_sequenceNumber;
    
    // 地质条件
    QString m_currentRockType;
    double m_rockHardness;
    
    // 模拟参数
    int m_intervalMs;
    bool m_isRunning;
};

#endif // DATASIMULATOR_H
