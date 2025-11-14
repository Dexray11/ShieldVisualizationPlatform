#ifndef NEWSDAO_H
#define NEWSDAO_H

#include "../models/News.h"
#include <QList>

/**
 * @brief 新闻数据访问对象
 * 
 * 负责新闻数据的增删改查操作
 */
class NewsDAO
{
public:
    NewsDAO();
    
    // 添加新闻
    bool addNews(const News &news);
    
    // 删除新闻
    bool deleteNews(int newsId);
    
    // 批量删除新闻
    bool deleteNewsList(const QList<int> &newsIds);
    
    // 更新新闻
    bool updateNews(const News &news);
    
    // 根据ID获取新闻
    News getNewsById(int newsId);
    
    // 获取所有新闻
    QList<News> getAllNews();
    
    // 获取最近N条新闻
    QList<News> getRecentNews(int count = 10);
    
    // 获取错误信息
    QString getLastError() const { return lastError; }

private:
    QString lastError;
};

#endif // NEWSDAO_H
