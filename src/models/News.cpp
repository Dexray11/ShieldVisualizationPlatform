#include "News.h"

News::News()
    : newsId(0), createdBy(0)
{
}

News::News(int id, const QString &content, const QDateTime &publishTime, int createdBy)
    : newsId(id), newsContent(content), publishTime(publishTime), createdBy(createdBy)
{
}
