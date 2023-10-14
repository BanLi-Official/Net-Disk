#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();  //单例模式，只有一个数据库操作类
    void init();//数据库连接的初始化函数
    ~OpeDB();

    bool handleRegist(const char *name,const char *pwd);//处理注册的数据库操作
    bool handleLogin(const char *name,const char *pwd);//处理登录的数据库操作
    bool handleSetOnline(const char *name,const char *pwd);//将用户的状态设置为在线
    bool handleSetOffline(const QString name);//将用户的状态设置为离线

signals:

private:
    QSqlDatabase m_db;   //连接数据库

};

#endif // OPEDB_H
