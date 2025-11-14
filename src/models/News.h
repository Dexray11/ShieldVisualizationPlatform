#ifndef NEWS_H
#define NEWS_H

#include <QString>
#include <QDateTime>

/**
 * @brief 新闻模型类
 * 
 * 表示一条新闻的完整信息
 */
class News
{
public:
    News();
    News(int id, const QString &content, const QDateTime &publishTime, int createdBy);
    
    // Getters
    int getNewsId() const { return newsId; }
    QString getNewsContent() const { return newsContent; }
    QDateTime getPublishTime() const { return publishTime; }
    int getCreatedBy() const { return createdBy; }
    
    // Setters
    void setNewsId(int id) { newsId = id; }
    void setNewsContent(const QString &content) { newsContent = content; }
    void setPublishTime(const QDateTime &time) { publishTime = time; }
    void setCreatedBy(int userId) { createdBy = userId; }
    
    // 检查新闻是否有效
    bool isValid() const { return newsId > 0; }

private:
    int newsId;
    QString newsContent;
    QDateTime publishTime;
    int createdBy;
};

#endif // NEWS_H
