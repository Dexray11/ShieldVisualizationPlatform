#include "NewsDAO.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

NewsDAO::NewsDAO()
{
}

bool NewsDAO::addNews(const News &news)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("INSERT INTO news (news_content, publish_time, created_by) "
                  "VALUES (:content, :time, :createdBy)");
    query.bindValue(":content", news.getNewsContent());
    query.bindValue(":time", news.getPublishTime());
    query.bindValue(":createdBy", news.getCreatedBy());
    
    if (!query.exec()) {
        lastError = "添加新闻失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool NewsDAO::deleteNews(int newsId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("DELETE FROM news WHERE news_id = :id");
    query.bindValue(":id", newsId);
    
    if (!query.exec()) {
        lastError = "删除新闻失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

bool NewsDAO::deleteNewsList(const QList<int> &newsIds)
{
    if (newsIds.isEmpty()) {
        lastError = "删除列表为空";
        return false;
    }
    
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    
    // 开始事务
    if (!db.transaction()) {
        lastError = "开始事务失败: " + db.lastError().text();
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM news WHERE news_id = :id");
    
    for (int newsId : newsIds) {
        query.bindValue(":id", newsId);
        if (!query.exec()) {
            lastError = "删除新闻失败: " + query.lastError().text();
            db.rollback();
            return false;
        }
    }
    
    // 提交事务
    if (!db.commit()) {
        lastError = "提交事务失败: " + db.lastError().text();
        db.rollback();
        return false;
    }
    
    return true;
}

bool NewsDAO::updateNews(const News &news)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("UPDATE news SET news_content = :content, "
                  "publish_time = :time, created_by = :createdBy "
                  "WHERE news_id = :id");
    query.bindValue(":content", news.getNewsContent());
    query.bindValue(":time", news.getPublishTime());
    query.bindValue(":createdBy", news.getCreatedBy());
    query.bindValue(":id", news.getNewsId());
    
    if (!query.exec()) {
        lastError = "更新新闻失败: " + query.lastError().text();
        qCritical() << lastError;
        return false;
    }
    
    return true;
}

News NewsDAO::getNewsById(int newsId)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT news_id, news_content, publish_time, created_by "
                  "FROM news WHERE news_id = :id");
    query.bindValue(":id", newsId);
    
    if (query.exec() && query.next()) {
        return News(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toDateTime(),
            query.value(3).toInt()
        );
    }
    
    lastError = "未找到新闻: " + query.lastError().text();
    return News();
}

QList<News> NewsDAO::getAllNews()
{
    QList<News> newsList;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT news_id, news_content, publish_time, created_by "
                  "FROM news ORDER BY publish_time DESC");
    
    if (!query.exec()) {
        lastError = "查询新闻失败: " + query.lastError().text();
        qCritical() << lastError;
        return newsList;
    }
    
    while (query.next()) {
        newsList.append(News(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toDateTime(),
            query.value(3).toInt()
        ));
    }
    
    return newsList;
}

QList<News> NewsDAO::getRecentNews(int count)
{
    QList<News> newsList;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    query.prepare("SELECT news_id, news_content, publish_time, created_by "
                  "FROM news ORDER BY publish_time DESC LIMIT :count");
    query.bindValue(":count", count);
    
    if (!query.exec()) {
        lastError = "查询新闻失败: " + query.lastError().text();
        qCritical() << lastError;
        return newsList;
    }
    
    while (query.next()) {
        newsList.append(News(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toDateTime(),
            query.value(3).toInt()
        ));
    }
    
    return newsList;
}
