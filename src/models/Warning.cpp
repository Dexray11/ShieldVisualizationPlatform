#include "Warning.h"

Warning::Warning()
    : warningId(0), projectId(0), warningNumber(0),
      latitude(0.0), longitude(0.0), depth(0.0),
      thresholdValue(0), distance(0.0)
{
}

Warning::Warning(int id, int projectId, int number, const QString &level,
                 const QString &type, double lat, double lon, double depth,
                 int threshold, double distance, const QDateTime &time)
    : warningId(id), projectId(projectId), warningNumber(number),
      warningLevel(level), warningType(type), latitude(lat), longitude(lon),
      depth(depth), thresholdValue(threshold), distance(distance), warningTime(time)
{
}
