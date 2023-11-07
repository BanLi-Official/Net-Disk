#include "mytcpserver.h"
#include "QDebug.h"

MyTcpServer::MyTcpServer()
{
    //connect(this,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteScoket(MyTcpSocket*)));//将下线的信号与deleteScoket(MyTcpSocket *)函数关联起来
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
     connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteScoket(MyTcpSocket*)));//将下线的信号与deleteScoket(MyTcpSocket *)函数关联起来
}

void MyTcpServer::deleteScoket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket *>::iterator iter=m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();iter++)
    {
        if(mysocket==*iter)//如果找到了相等的元素，代码会先使用delete运算符删除该元素指向的对象，然后将该元素赋值为NULL，最后使用erase函数将该元素从m_tcpSocketList列表中移除。这样就完成了删除操作。
        {
            //delete *iter;//在客户端下线时调用了delete并销毁客户端，而服务器端的QTCPSocket对象还在处理相应的关闭连接操作，导致服务器端崩溃
            //*iter=NULL;
            (*iter)->deleteLater();//解决方法：延迟删除
            m_tcpSocketList.erase(iter);
            break;
        }
    }


    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<<m_tcpSocketList.at(i)->getName();
    }
    return;
}
