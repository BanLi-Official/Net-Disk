#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "QByteArray"
#include "QDebug"
#include "QMessageBox"
#include "QHostAddress"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));//将连接发出的信息号与信号处理结合起来
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);//连接到服务器


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()//加载资源的函数
{
    QFile file(":/Client.config");//定义ip与端口的资源文件
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

    }else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

void TcpClient::showConnect()//展示连接成功信息
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

