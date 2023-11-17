#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

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
    QStringList handleAllOnline();//找出所有的在线用户
    int handlSearchUser(const char *name);//查找指定用户
    int handlAddFriend(const char *FriendName,const char *LoginName);//添加好友操作
    bool handlAddFriendToDataset(const char *FromName,const char *toName);//添加交好友信息到数据库

signals:

private:
    QSqlDatabase m_db;   //连接数据库

};

#endif // OPEDB_H
