#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <QDir>

class MyTcpSocket : public QTcpSocket  //包装一个TCPSocket用于区分谁发来的socket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
signals:
    void offline(MyTcpSocket *mysocket);//下线的信号
public slots:
    void recvMsg();//处理收到信息的行为
    void clientOffline();//处理客户端下线的行为

private:
    QString m_strName;
};

#endif // MYTCPSOCKET_H



//37:14
