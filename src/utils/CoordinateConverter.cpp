#include "CoordinateConverter.h"
#include <cmath>
#include <QRegularExpression>

void CoordinateConverter::gaussKrugerToWGS84(double x, double y, int zone, double &lat, double &lon)
{
    // 简化转换：假设项目坐标是CGCS2000 3度带第41带（中央子午线123°）
    // 对于胶州湾地区，实际应该使用更精确的转换参数
    
    // 3度带中央子午线计算：L0 = 3 * zone
    double L0 = 3.0 * zone;  // 中央子午线经度
    
    // 移除带号和东偏500km
    double X = x - 500000.0;
    double Y = y;
    
    // 椭球参数
    double a = SEMI_MAJOR_AXIS;
    double e2 = ECCENTRICITY_SQ;
    double e = std::sqrt(e2);
    
    // 计算辅助参数
    double e_prime2 = e2 / (1 - e2);
    
    // 计算底点纬度Bf (迭代法)
    double Bf = Y / a;
    double N, M;
    for (int i = 0; i < 5; i++) {
        M = a * (1 - e2) / std::pow(1 - e2 * std::sin(Bf) * std::sin(Bf), 1.5);
        N = a / std::sqrt(1 - e2 * std::sin(Bf) * std::sin(Bf));
        Bf = Y / M + Bf;
    }
    
    double tf = std::tan(Bf);
    double Cf = e_prime2 * std::cos(Bf) * std::cos(Bf);
    double Nf = N;
    double Mf = M;
    
    // 计算纬度
    lat = Bf - (tf / (2 * Mf * Nf)) * X * X
          + (tf / (24 * Mf * std::pow(Nf, 3))) * (5 + 3 * tf * tf + Cf - 9 * Cf * tf * tf) * std::pow(X, 4);
    
    // 计算经度
    lon = L0 + (X / (Nf * std::cos(Bf))) * RAD_TO_DEG
          - (std::pow(X, 3) / (6 * std::pow(Nf, 3) * std::cos(Bf))) 
          * (1 + 2 * tf * tf + Cf) * RAD_TO_DEG;
    
    lat *= RAD_TO_DEG;
}

void CoordinateConverter::wgs84ToGaussKruger(double lat, double lon, int zone, double &x, double &y)
{
    // 3度带中央子午线
    double L0 = 3.0 * zone;
    double dL = (lon - L0) * DEG_TO_RAD;
    double B = lat * DEG_TO_RAD;
    
    // 椭球参数
    double a = SEMI_MAJOR_AXIS;
    double e2 = ECCENTRICITY_SQ;
    
    double sinB = std::sin(B);
    double cosB = std::cos(B);
    double tanB = std::tan(B);
    
    double N = a / std::sqrt(1 - e2 * sinB * sinB);
    double M = a * (1 - e2) / std::pow(1 - e2 * sinB * sinB, 1.5);
    
    // 计算辅助参数
    double t = tanB;
    double eta2 = (e2 / (1 - e2)) * cosB * cosB;
    
    // 计算Y（纬度方向）
    double A0 = 1 + 3.0/4.0 * e2 + 45.0/64.0 * e2 * e2;
    double A2 = 3.0/4.0 * e2 + 15.0/16.0 * e2 * e2;
    double A4 = 15.0/64.0 * e2 * e2;
    
    y = a * (A0 * B - A2 * std::sin(2*B) + A4 * std::sin(4*B));
    
    // 计算X（经度方向）
    x = N * cosB * dL
        + N * cosB * std::pow(dL, 3) * (1 - t*t + eta2) / 6.0
        + N * cosB * std::pow(dL, 5) * (5 - 18*t*t + t*t*t*t) / 120.0;
    
    // 加上带号和东偏500km
    x += 500000.0;
}

double CoordinateConverter::calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    // 使用Haversine公式计算球面距离
    double dLat = (lat2 - lat1) * DEG_TO_RAD;
    double dLon = (lon2 - lon1) * DEG_TO_RAD;
    double lat1Rad = lat1 * DEG_TO_RAD;
    double lat2Rad = lat2 * DEG_TO_RAD;
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
               std::cos(lat1Rad) * std::cos(lat2Rad) *
               std::sin(dLon/2) * std::sin(dLon/2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    
    return SEMI_MAJOR_AXIS * c;  // 返回米
}

QString CoordinateConverter::formatMileage(double mileage)
{
    int km = static_cast<int>(mileage / 1000);
    double meters = mileage - km * 1000;
    return QString("K%1+%2").arg(km).arg(meters, 0, 'f', 2);
}

double CoordinateConverter::parseMileage(const QString &mileageStr)
{
    // 支持格式：K2+850、K2+850.5、2850、2850.5
    QRegularExpression re(R"(K?(\d+)\+?(\d+(?:\.\d+)?)?)");
    QRegularExpressionMatch match = re.match(mileageStr);
    
    if (match.hasMatch()) {
        int km = match.captured(1).toInt();
        double meters = 0;
        if (!match.captured(2).isEmpty()) {
            meters = match.captured(2).toDouble();
        }
        return km * 1000.0 + meters;
    }
    
    // 尝试直接解析为数字
    bool ok;
    double value = mileageStr.toDouble(&ok);
    if (ok) {
        return value;
    }
    
    return 0.0;
}
