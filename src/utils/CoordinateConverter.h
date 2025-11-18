#ifndef COORDINATECONVERTER_H
#define COORDINATECONVERTER_H

#include <QPointF>
#include <cmath>

/**
 * @brief 坐标转换工具类
 * 
 * 用于在投影坐标系（如高斯-克吕格投影）和WGS84经纬度坐标之间转换
 * 本项目使用的投影坐标系为CGCS2000 3度带第41带
 */
class CoordinateConverter
{
public:
    /**
     * @brief 高斯-克吕格投影坐标转WGS84经纬度
     * @param x 横坐标（东向）
     * @param y 纵坐标（北向）
     * @param zone 3度带带号（默认41）
     * @param lat 输出纬度
     * @param lon 输出经度
     */
    static void gaussKrugerToWGS84(double x, double y, int zone, double &lat, double &lon);
    
    /**
     * @brief WGS84经纬度转高斯-克吕格投影坐标
     * @param lat 纬度
     * @param lon 经度
     * @param zone 3度带带号
     * @param x 输出横坐标
     * @param y 输出纵坐标
     */
    static void wgs84ToGaussKruger(double lat, double lon, int zone, double &x, double &y);
    
    /**
     * @brief 计算两个WGS84坐标之间的距离（米）
     * @param lat1 点1纬度
     * @param lon1 点1经度
     * @param lat2 点2纬度
     * @param lon2 点2经度
     * @return 距离（米）
     */
    static double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    
    /**
     * @brief 将里程格式化为字符串（如：K2+850.5）
     * @param mileage 里程值（米）
     * @return 格式化后的字符串
     */
    static QString formatMileage(double mileage);
    
    /**
     * @brief 解析里程字符串为数值（米）
     * @param mileageStr 里程字符串（如：K2+850.5）
     * @return 里程值（米）
     */
    static double parseMileage(const QString &mileageStr);

private:
    // 椭球参数（CGCS2000）
    static constexpr double SEMI_MAJOR_AXIS = 6378137.0;       // 长半轴
    static constexpr double FLATTENING = 1.0 / 298.257222101;  // 扁率
    static constexpr double ECCENTRICITY_SQ = 2 * FLATTENING - FLATTENING * FLATTENING; // 第一偏心率平方
    
    // 常量
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double DEG_TO_RAD = PI / 180.0;
    static constexpr double RAD_TO_DEG = 180.0 / PI;
};

#endif // COORDINATECONVERTER_H
