#ifndef BOREHOLEDAO_H
#define BOREHOLEDAO_H

#include <QString>
#include <QVector>

/**
 * @brief 钻孔图层结构
 */
struct BoreholeLayerData {
    int layerId;
    int boreholeId;
    int layerNumber;
    QString layerCode;
    QString eraGenesis;
    QString rockName;
    double bottomElevation;
    double bottomDepth;
    double thickness;
    QString characteristics;
};

/**
 * @brief 钻孔数据结构
 */
struct BoreholeData {
    int boreholeId;
    int projectId;
    QString boreholeCode;
    double x;
    double y;
    double surfaceElevation;
    double mileage;
    QVector<BoreholeLayerData> layers;
};

/**
 * @brief 钻孔数据访问对象（Data Access Object）
 */
class BoreholeDAO
{
public:
    BoreholeDAO();
    
    /**
     * @brief 插入钻孔数据
     * @param borehole 钻孔数据
     * @return 成功返回钻孔ID，失败返回-1
     */
    int insertBorehole(const BoreholeData &borehole);
    
    /**
     * @brief 插入钻孔地层数据
     * @param layer 地层数据
     * @return 成功返回true
     */
    bool insertBoreholeLayer(const BoreholeLayerData &layer);
    
    /**
     * @brief 根据项目ID获取所有钻孔数据（包含地层）
     * @param projectId 项目ID
     * @return 钻孔数据列表
     */
    QVector<BoreholeData> getBoreholesByProjectId(int projectId);
    
    /**
     * @brief 根据钻孔ID获取钻孔数据（包含地层）
     * @param boreholeId 钻孔ID
     * @return 钻孔数据，如果未找到则返回空数据
     */
    BoreholeData getBoreholeById(int boreholeId);
    
    /**
     * @brief 删除钻孔及其所有地层数据
     * @param boreholeId 钻孔ID
     * @return 成功返回true
     */
    bool deleteBorehole(int boreholeId);
    
    /**
     * @brief 删除项目的所有钻孔数据
     * @param projectId 项目ID
     * @return 成功返回true
     */
    bool deleteBoreholesByProjectId(int projectId);
    
    /**
     * @brief 获取最后的错误信息
     */
    QString getLastError() const { return lastError; }
    
private:
    QString lastError;
    
    /**
     * @brief 根据钻孔ID加载地层数据
     * @param boreholeId 钻孔ID
     * @return 地层数据列表
     */
    QVector<BoreholeLayerData> loadLayersByBoreholeId(int boreholeId);
};

#endif // BOREHOLEDAO_H
