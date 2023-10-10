#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();//加载资源的函数

public slots://在Qt中，槽函数（Slot）是一种特殊的成员函数，用于响应信号（Signal）的发出。
             //槽函数可以被连接到一个或多个信号上，当信号被发出时，与之连接的槽函数会被自动调用，以响应信号的事件。
    void showConnect();//显示连接的状态




private:
    Ui::TcpClient *ui;
    QString m_strIP; //服务器IP地址
    quint16 m_usPort;//服务器端口
    QTcpSocket m_tcpSocket; //网络连接对象，通过它连接服务器，同服务器进行数据交互

};
#endif // TCPCLIENT_H
