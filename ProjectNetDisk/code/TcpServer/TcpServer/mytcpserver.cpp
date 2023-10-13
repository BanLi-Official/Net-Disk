#include "mytcpserver.h"
#include "QDebug.h"

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;//私有的静态成员变量，用于保存唯一的实例
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)//重新定义TCP server中的protect的虚函数，用于当有请求接入时给予反应,一旦有客户端请求，则自动调用
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket=new MyTcpSocket;//新建一个MyTcpSocket，用于接收客户端连接而来的socket
    pTcpSocket->setSocketDescriptor(socketDescriptor);//将一个已有的套接字描述符与一个QTcpSocket对象关联起来，使得QTcpSocket对象可以使用该套接字进行网络通信。
    m_tcpSocketList.append(pTcpSocket);
}
