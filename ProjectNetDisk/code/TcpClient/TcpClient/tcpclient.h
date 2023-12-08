#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"

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

    static TcpClient &getInstance();//用单例模式获取tcpClient使其能够在任何时候调用其中的内容，比如：m_tcpSocket

    QTcpSocket &getTcpSocket();//将TCPSocket返回出去，方便调用，同时增强代码安全性
    QString getLoginName();
    QString getCurrentPath();//获取当前位置
    QString getSavePath();
    bool getIsDownLoading();
    void setCurrentPath(QString str);//修改当前位置
    void setCurrentChoose(QString str);//修改当前选择位置
    void setSavePath(QString str);
    void setIsDownLoading(bool a);



public slots://在Qt中，槽函数（Slot）是一种特殊的成员函数，用于响应信号（Signal）的发出。
             //槽函数可以被连接到一个或多个信号上，当信号被发出时，与之连接的槽函数会被自动调用，以响应信号的事件。
    void showConnect();//显示连接的状态
    void resvMsg();//接收数据





private slots:
    //void on_send_pd_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP; //服务器IP地址
    quint16 m_usPort;//服务器端口
    QTcpSocket m_tcpSocket; //网络连接对象，通过它连接服务器，同服务器进行数据交互
    QString strLoginName;//存储登录的账户名称
    QString str_CurPath;//用户当前的位置
    QString str_CurChoose;//用户当前选择的位置

    QString fileSavePath;
    bool is_downLoading=false;


};
#endif // TCPCLIENT_H
