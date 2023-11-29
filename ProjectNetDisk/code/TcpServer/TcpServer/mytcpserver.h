#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "tcpserver.h"
#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT    //在Qt中，如果一个类需要使用Qt的信号和槽机制，就必须在类的声明中加上Q_OBJECT宏定义。
                //这个宏定义会自动为类生成一些必要的代码，包括信号和槽的声明、元对象信息的生成等。
public:
    MyTcpServer();
    //单例模式：它保证一个类只有一个实例，并提供一个全局访问点。
    static MyTcpServer &getInstance(); //公有的静态成员函数，用于获取唯一的实例
    void incomingConnection(qintptr socketDescriptor); //重新定义TCP server中的protect的虚函数，用于当有请求接入时给予反应
    void resend(const char *name,PDU *pdu);//转发消息给指定的用户




public slots:
    void deleteScoket(MyTcpSocket *mysocket);  //定义一个槽函数，对用户下线做出反应，将socekt从list中删除

private:
    QList<MyTcpSocket*> m_tcpSocketList;//创建一个socket的list
};

#endif // MYTCPSERVER_H
