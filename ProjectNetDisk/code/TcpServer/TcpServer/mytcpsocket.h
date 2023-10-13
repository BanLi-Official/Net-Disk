#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"

class MyTcpSocket : public QTcpSocket  //包装一个TCPSocket用于区分谁发来的socket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
public slots:
    void recvMsg();
};

#endif // MYTCPSOCKET_H



//37:14
