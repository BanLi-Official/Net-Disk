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

    bool handleRegist(const char *name,const char *pwd);

signals:

private:
    QSqlDatabase m_db;   //连接数据库

};

#endif // OPEDB_H
