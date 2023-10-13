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
