#include "Project.h"

Project::Project()
    : projectId(0), latitude(0.0), longitude(0.0), progress(0.0)
{
}

Project::Project(int id, const QString &name, const QString &brief,
                 double lat, double lon, const QString &unit,
                 const QString &startDate, double progress,
                 const QString &location, const QString &status)
    : projectId(id), projectName(name), brief(brief),
      latitude(lat), longitude(lon), constructionUnit(unit),
      startDate(startDate), progress(progress), location(location), status(status)
{
}
