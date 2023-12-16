#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <QDir>
#include <QTimer>

class MyTcpSocket : public QTcpSocket  //包装一个TCPSocket用于区分谁发来的socket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
    QString getNetDiskRoot();
signals:
    void offline(MyTcpSocket *mysocket);//下线的信号
public slots:
    void recvMsg();//处理收到信息的行为
    void clientOffline();//处理客户端下线的行为
    void TransData();//开始传送数据

private:
    QString m_strName;
    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;

    QTimer *timer;

    QString Root=QString("F:/WORKS/c++/ProjectNetDisk/code/TcpServer/build-TcpServer-Desktop_Qt_6_6_0_MinGW_64_bit-Debug/UserFile/");
};

#endif // MYTCPSOCKET_H



//37:14
