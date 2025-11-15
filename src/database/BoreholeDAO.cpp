#include "BoreholeDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

BoreholeDAO::BoreholeDAO()
{
}

int BoreholeDAO::insertBorehole(const BoreholeData &borehole)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO boreholes (project_id, borehole_code, x_coordinate, "
                  "y_coordinate, surface_elevation, mileage) "
                  "VALUES (:project_id, :code, :x, :y, :elevation, :mileage)");
    
    query.bindValue(":project_id", borehole.projectId);
    query.bindValue(":code", borehole.boreholeCode);
    query.bindValue(":x", borehole.x);
    query.bindValue(":y", borehole.y);
    query.bindValue(":elevation", borehole.surfaceElevation);
    query.bindValue(":mileage", borehole.mileage);
    
    if (!query.exec()) {
        lastError = "插入钻孔失败: " + query.lastError().text();
        qCritical() << lastError;
        return -1;
    }
    
    int boreholeId = query.lastInsertId().toInt();
    
    // 插入地层数据
    for (const auto &layer : borehole.layers) {
        BoreholeLayerData layerCopy = layer;
        layerCopy.boreholeId = boreholeId;
        if (!insertBoreholeLayer(layerCopy)) {
            qWarning() << "插入地层失败:" << lastError;
        }
    }
    
    return boreholeId;
}

bool BoreholeDAO::insertBoreholeLayer(const BoreholeLayerData &layer)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO borehole_layers (borehole_id, layer_number, layer_code, "
                  "era_genesis, rock_name, bottom_elevation, bottom_depth, thickness, characteristics) "
                  "VALUES (:borehole_id, :layer_num, :code, :era, :rock_name, "
                  ":bottom_elev, :bottom_depth, :thickness, :chars)");
    
    query.bindValue(":borehole_id", layer.boreholeId);
    query.bindValue(":layer_num", layer.layerNumber);
    query.bindValue(":code", layer.layerCode);
    query.bindValue(":era", layer.eraGenesis);
    query.bindValue(":rock_name", layer.rockName);
    query.bindValue(":bottom_elev", layer.bottomElevation);
    query.bindValue(":bottom_depth", layer.bottomDepth);
    query.bindValue(":thickness", layer.thickness);
    query.bindValue(":chars", layer.characteristics);
    
    if (!query.exec()) {
        lastError = "插入钻孔地层失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

QVector<BoreholeData> BoreholeDAO::getBoreholesByProjectId(int projectId)
{
    QVector<BoreholeData> boreholes;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT borehole_id, project_id, borehole_code, x_coordinate, "
                  "y_coordinate, surface_elevation, mileage "
                  "FROM boreholes WHERE project_id = :project_id "
                  "ORDER BY mileage");
    query.bindValue(":project_id", projectId);
    
    if (!query.exec()) {
        lastError = "查询钻孔失败: " + query.lastError().text();
        qCritical() << lastError;
        return boreholes;
    }
    
    while (query.next()) {
        BoreholeData borehole;
        borehole.boreholeId = query.value("borehole_id").toInt();
        borehole.projectId = query.value("project_id").toInt();
        borehole.boreholeCode = query.value("borehole_code").toString();
        borehole.x = query.value("x_coordinate").toDouble();
        borehole.y = query.value("y_coordinate").toDouble();
        borehole.surfaceElevation = query.value("surface_elevation").toDouble();
        borehole.mileage = query.value("mileage").toDouble();
        
        // 加载地层数据
        borehole.layers = loadLayersByBoreholeId(borehole.boreholeId);
        
        boreholes.append(borehole);
    }
    
    return boreholes;
}

BoreholeData BoreholeDAO::getBoreholeById(int boreholeId)
{
    BoreholeData borehole;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT borehole_id, project_id, borehole_code, x_coordinate, "
                  "y_coordinate, surface_elevation, mileage "
                  "FROM boreholes WHERE borehole_id = :borehole_id");
    query.bindValue(":borehole_id", boreholeId);
    
    if (!query.exec()) {
        lastError = "查询钻孔失败: " + query.lastError().text();
        qCritical() << lastError;
        return borehole;
    }
    
    if (query.next()) {
        borehole.boreholeId = query.value("borehole_id").toInt();
        borehole.projectId = query.value("project_id").toInt();
        borehole.boreholeCode = query.value("borehole_code").toString();
        borehole.x = query.value("x_coordinate").toDouble();
        borehole.y = query.value("y_coordinate").toDouble();
        borehole.surfaceElevation = query.value("surface_elevation").toDouble();
        borehole.mileage = query.value("mileage").toDouble();
        
        // 加载地层数据
        borehole.layers = loadLayersByBoreholeId(borehole.boreholeId);
    }
    
    return borehole;
}

bool BoreholeDAO::deleteBorehole(int boreholeId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    // 删除地层数据
    query.prepare("DELETE FROM borehole_layers WHERE borehole_id = :borehole_id");
    query.bindValue(":borehole_id", boreholeId);
    
    if (!query.exec()) {
        lastError = "删除钻孔地层失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    // 删除钻孔
    query.prepare("DELETE FROM boreholes WHERE borehole_id = :borehole_id");
    query.bindValue(":borehole_id", boreholeId);
    
    if (!query.exec()) {
        lastError = "删除钻孔失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool BoreholeDAO::deleteBoreholesByProjectId(int projectId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    // 先获取所有钻孔ID
    query.prepare("SELECT borehole_id FROM boreholes WHERE project_id = :project_id");
    query.bindValue(":project_id", projectId);
    
    if (!query.exec()) {
        lastError = "查询钻孔失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    QVector<int> boreholeIds;
    while (query.next()) {
        boreholeIds.append(query.value("borehole_id").toInt());
    }
    
    // 删除所有钻孔的地层数据
    for (int id : boreholeIds) {
        if (!deleteBorehole(id)) {
            return false;
        }
    }
    
    return true;
}

QVector<BoreholeLayerData> BoreholeDAO::loadLayersByBoreholeId(int boreholeId)
{
    QVector<BoreholeLayerData> layers;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT layer_id, borehole_id, layer_number, layer_code, era_genesis, "
                  "rock_name, bottom_elevation, bottom_depth, thickness, characteristics "
                  "FROM borehole_layers WHERE borehole_id = :borehole_id "
                  "ORDER BY layer_number");
    query.bindValue(":borehole_id", boreholeId);
    
    if (!query.exec()) {
        lastError = "查询钻孔地层失败: " + query.lastError().text();
        qCritical() << lastError;
        return layers;
    }
    
    while (query.next()) {
        BoreholeLayerData layer;
        layer.layerId = query.value("layer_id").toInt();
        layer.boreholeId = query.value("borehole_id").toInt();
        layer.layerNumber = query.value("layer_number").toInt();
        layer.layerCode = query.value("layer_code").toString();
        layer.eraGenesis = query.value("era_genesis").toString();
        layer.rockName = query.value("rock_name").toString();
        layer.bottomElevation = query.value("bottom_elevation").toDouble();
        layer.bottomDepth = query.value("bottom_depth").toDouble();
        layer.thickness = query.value("thickness").toDouble();
        layer.characteristics = query.value("characteristics").toString();
        
        layers.append(layer);
    }
    
    return layers;
}
