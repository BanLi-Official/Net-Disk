#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT
public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();//加载资源的函数

private:
    Ui::TcpServer *ui;
    QString m_strIP; //服务器IP地址
    quint16 m_usPort;//服务器端口
};
#endif // TCPSERVER_H
