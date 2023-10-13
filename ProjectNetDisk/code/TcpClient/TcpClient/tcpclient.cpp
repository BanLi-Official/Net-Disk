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
    resize(500,250);
    loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));//将连接发出的信息号与信号处理结合起来
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(resvMsg()));//将准备好读取信息与信息接收处理连接起来




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

void TcpClient::resvMsg()
{
    qDebug()<<"收到pdu长度："<<m_tcpSocket.bytesAvailable();//先输出接收到的数据的长度
    //接收数据
    PDU *pdu=mkPDU(0);
    uint uiPDULen=0;
    m_tcpSocket.read((char *)&uiPDULen,sizeof(uint));
    uint uiMsgLen=uiPDULen-sizeof(PDU);
    m_tcpSocket.read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    pdu->uiMsgLen=uiMsgLen;

    //qDebug()<<"pduLen="<<pdu->uiPDULen<<" MsgLen="<<pdu->uiMsgLen<<" caData="<<pdu->caData;


    //按照接收到的不同数据做出不同的反应：
    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
        if(0==strcmp(pdu->caData,REGIST_OK))
        {
            QMessageBox::information(this,"注册","注册成功！");
        }
        else if(0==strcmp(pdu->caData,REGIST_DEFAULT_RENAME))
        {
            QMessageBox::warning(this,"注册","注册失败，原因是：存在重名用户！");
        }
        else
        {
            QMessageBox::warning(this,"注册","注册失败，未知原因！");
        }
        break;

    default:
        break;
    }

    free(pdu);
    pdu=NULL;




}

#if 0
void TcpClient::on_send_pd_clicked()//发送按钮点击事件
{
    QString strMsg=ui->lineEdit->text();//获取输入的内容
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.size());//初始化一个协议数据单元
        pdu->uiMsgType=8888;//设置协议数据单元的类型
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//将数据放入数据内容中
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//通过socket发送到服务器
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"信息发送","信息发送失败");
    }

}
#endif


void TcpClient::on_login_pb_clicked()
{

}


void TcpClient::on_regist_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);//创建一个注册请求的协议单元
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//通过socket发送到服务器
        free(pdu);
        pdu=NULL;
    }
    else
    {
         QMessageBox::warning(this,"注册账户","账户注册失败：账号或密码不能为空！");
    }
}


void TcpClient::on_cancel_pb_clicked()
{

}

