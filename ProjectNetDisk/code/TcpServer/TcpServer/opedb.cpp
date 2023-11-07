#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");//创建一个SQLite数据库连接，并将其保存在m_db变量中。标明使用的数据库为SQLite3

}

OpeDB &OpeDB::getInstance()//单例模式，只有一个数据库操作类
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()  //数据库连接的初始化函数
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("../Database/cloud.db");
    if(m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from userInfo");
        while(query.next())
        {
            QString data=QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)//将注册信息传入数据库
{
    if(NULL==name||NULL==pwd)
    {
        return false;
    }
    QString insertQuery=QString("insert into userInfo (name,pwd) values('%1','%2')").arg(name).arg(pwd);//编写sql代码
    //qDebug()<<insertQuery;
    QSqlQuery query;
    return query.exec(insertQuery);//运行插入代码并返回运行结果
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(NULL==name||NULL==pwd)
    {
        return false;
    }
    QString selectQuery=QString("select * from userInfo where name='%1' and pwd='%2' and online=0").arg(name).arg(pwd);//编写sql代码
    //qDebug()<<selectQuery;
    QSqlQuery query;
    query.exec(selectQuery);
    //return query.next();

    if(query.next())  //防止重复登录，将数据库中的online字段设置为1
    {
        bool ret=OpeDB::handleSetOnline(name,pwd);
        return ret;
    }
    else
    {
        return false;
    }
}

bool OpeDB::handleSetOnline(const char *name, const char *pwd)//将用户的状态设置为在线
{
    QString setString=QString("update userInfo set online=1 where name='%1' and pwd='%2'").arg(name).arg(pwd);//sql代码
    qDebug()<<setString;
    QSqlQuery query;
    bool ret=query.exec(setString);
    if(ret)
    {
        qDebug()<<"用户状态已经成功修改为在线";
    }
    else
    {
        qDebug()<<"修改失败！";
    }
    return ret;

}

bool OpeDB::handleSetOffline(const QString name)
{
    QString setString=QString("update userInfo set online=0 where name='%1' ").arg(name);//sql代码
    qDebug()<<setString;
    QSqlQuery query;
    bool ret=query.exec(setString);
    if(ret)
    {
        qDebug()<<"用户状态已经成功修改为离线";
    }
    else
    {
        qDebug()<<"修改离线失败！";
    }
    return ret;
}

QStringList OpeDB::handleAllOnline()
{
    QString getString=QString("select * from userInfo where online='1'");//sql代码
    qDebug()<<getString;
    QSqlQuery query;
    query.exec(getString);

    QStringList allOnline;

    while(query.next())
    {
        allOnline.append(query.value(2).toString());
    }

    return allOnline;

}

int OpeDB::handlSearchUser(const char *name)
{
    QString getString=QString("select * from userInfo where name='%1'").arg(name);
    qDebug()<<"查找特定用户的SQL语句："<<getString;
    QSqlQuery query;
    query.exec(getString);
    if(query.next())
    {
        int state=query.value(3).toInt();
        qDebug()<<"用户在线状态："<<state;
        return state;
    }
    else
    {
        qDebug()<<"用户在线状态未知";
        return -1;
    }



}
