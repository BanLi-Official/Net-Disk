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

int OpeDB::handlAddFriend(const char *FriendName, const char *LoginName)
{
    if(FriendName==NULL || LoginName==NULL)
    {
        return -1;
    }
    QString sql=QString("select * from friend where (id=(select id from userInfo where name='%1') and friendid=(select id from userInfo where name='%2')) "
                          "or (id=(select id from userInfo where name='%3') and friendid=(select id from userInfo where name='%4'))").arg(FriendName).arg(LoginName).arg(LoginName).arg(FriendName);
    qDebug()<<sql;
    QSqlQuery query;
    query.exec(sql);
    if(query.next())
    {
        return 0;//已经是好友了
    }
    else
    {
        QString getString=QString("select * from userInfo where name='%1'").arg(FriendName);
        qDebug()<<"查找特定用户的SQL语句："<<getString;
        QSqlQuery query;
        query.exec(getString);
        if(query.next())
        {
            int state=query.value(3).toInt();
            if(state==1)
            {
                return 1;//用户在线
            }
            else
            {
                return 2;//用户不在线
            }

        }
        else
        {
            qDebug()<<"用户在线状态未知";
            return 3;
        }
    }
}

bool OpeDB::handlAddFriendToDataset(const char *FromName, const char *toName)
{
    QString sql=QString("insert into friend (id, friendid) values ((select id from userInfo where name='%1'),(select id from userInfo where name='%2'));").arg(FromName).arg(toName);
    QSqlQuery query;
    bool ret=query.exec(sql);
    return ret;

}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(name==NULL)
    {
        return strFriendList;
    }
    QString sql=QString("select name from userInfo where id in(select friendid from friend where id=(select id from userInfo where name='%1')) and online='1'").arg(name);
    QSqlQuery query;
    query.exec(sql);
    //qDebug()<<"在线的好友列表";
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        //qDebug()<<query.value(0).toString();

    }

    QString sql2=QString("select name from userInfo where id in(select id from friend where friendid=(select id from userInfo where name='%1')) and online='1'").arg(name);
    QSqlQuery query2;
    query2.exec(sql2);
    while(query2.next())
    {
        strFriendList.append(query2.value(0).toString());
        //qDebug()<<query2.value(0).toString();

    }

    return strFriendList;

}
