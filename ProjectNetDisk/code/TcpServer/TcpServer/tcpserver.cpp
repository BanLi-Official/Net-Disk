#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include "QByteArray"
#include "QDebug"
#include "QMessageBox"
#include "QHostAddress"
#include "QFile"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();//加载资源的函数
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);//调用一个Tcpserver实例来进行监听,当有一个客户端请求时，自动调用MyTcpServer::incomingConnection()
}

TcpServer::~TcpServer()
{
    delete ui;
}


void TcpServer::loadConfig()//加载资源的函数
{
    QFile file(":/Server.config");//定义ip与端口的资源文件
    if(file.open(QIODevice::ReadOnly))//以只读的方式打开文件
    {
        QByteArray baData= file.readAll();//返回一个QByteArray的内容，需要转化为QString
        QString strData=baData.toStdString().c_str();
        //qDebug()<<strData;//没有问题则打印一下
        file.close();

        //对strData进行分割
        //先替换\t\n
        strData.replace("\r\n"," ");
        //按照“ ”进行分割，得到一个string数组
        QStringList strList=strData.split(" ");
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        //qDebug()<<"IP地址："<<m_strIP<<"   "<<"端口号："<<m_usPort;

    }else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

