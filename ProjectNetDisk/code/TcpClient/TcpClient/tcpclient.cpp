#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "QByteArray"
#include "QDebug"
#include "QMessageBox"
#include "QHostAddress"
#include "protocol.h"
#include "privatechat.h"
#include "tools.h"
#include <QTimer>
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
    //is_downLoading=false;


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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}



QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getLoginName()
{
    return strLoginName;
}

QString TcpClient::getCurrentPath()
{
    return str_CurPath;
}

void TcpClient::setCurrentPath(QString str)
{
    this->str_CurPath=str;
    return ;
}

void TcpClient::setCurrentChoose(QString str)
{
    this->str_CurChoose=str;
    return;
}

QString TcpClient::getSavePath()
{
    return fileSavePath;
}

bool TcpClient::getIsDownLoading()
{
    return is_downLoading;
}

void TcpClient::setSavePath(QString str)
{
    fileSavePath=str;
}

void TcpClient::setIsDownLoading(bool a)
{
    is_downLoading=a;
}

void TcpClient::showConnect()//展示连接成功信息
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::resvMsg()
{
    qDebug()<<"getIsDownLoading()="<<TcpClient::getInstance().getIsDownLoading();
    if(TcpClient::getInstance().getIsDownLoading())//如果是正在下载文件，则只运行里面的东西
    {
        qDebug()<<"11111111111";
        QByteArray buffer=m_tcpSocket.readAll();
        NetDisk::getinstance().file.write(buffer);

        NetDisk::getinstance().iReceved+=buffer.size();
        if(NetDisk::getinstance().iReceved==NetDisk::getinstance().iTotal)
        {
            NetDisk::getinstance().file.close();
            NetDisk::getinstance().iTotal=0;
            NetDisk::getinstance().iReceved=0;
            //NetDisk::getinstance().setIs_DownLoading(false);
            setIsDownLoading(false);
            QMessageBox::information(this,"下载文件","文件下载成功！");

        }
        else if(NetDisk::getinstance().iReceved>NetDisk::getinstance().iTotal)
        {
            NetDisk::getinstance().file.close();
            NetDisk::getinstance().iTotal=0;
            NetDisk::getinstance().iReceved=0;
            //NetDisk::getinstance().setIs_DownLoading(false);
            setIsDownLoading(false);
            QMessageBox::information(this,"下载文件","文件下载失败了！");
        }
        qDebug()<<"22222";
        return;
    }

   // qDebug()<<"收到pdu长度："<<m_tcpSocket.bytesAvailable();//先输出接收到的数据的长度
    //接收数据
    uint uiPDULen=0;
    m_tcpSocket.read((char *)&uiPDULen,sizeof(uint));
    uint uiMsgLen=uiPDULen-sizeof(PDU);
    PDU *pdu=mkPDU(uiMsgLen);
    m_tcpSocket.read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    pdu->uiMsgLen=uiMsgLen;




    //按照接收到的不同数据做出不同的反应：
    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
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
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        if(0==strcmp(pdu->caData,LOGIN_OK))
        {
            str_CurPath=QString("./UserFile/%1").arg(TcpClient::getInstance().getLoginName());
            QMessageBox::information(this,"登录","登录成功！");
            OpeWidget::getInstance().show();
            this->hide();
        }
        else if(0==strcmp(pdu->caData,LOGIN_FAILED))
        {
            QMessageBox::warning(this,"登录","登录失败，原因是：用户名或者密码错误！或者请勿重复登录！");
        }
        break;
    }
    case ENUM_MSG_TYPE_All_ONLINE_RESPOND:
    {

        OpeWidget::getInstance().getFriend()->ShowAllOnlineUsr(pdu);
        //qDebug()<<(pdu);

        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:
    {
        char state[32];
        strncpy(state,pdu->caData,32);
        qDebug()<<state;
        if(0==strcmp(SEARCH_USER_NO,pdu->caData))
        {
            QMessageBox::information(this,"搜索",QString("%1:%2").arg(OpeWidget::getInstance().getFriend()->m_strName).arg(SEARCH_USER_NO));
        }
        else if(0==strcmp(SEARCH_USER_ONLINE,pdu->caData))
        {
            QMessageBox::information(this,"搜索",QString("%1:%2").arg(OpeWidget::getInstance().getFriend()->m_strName).arg(SEARCH_USER_ONLINE));
        }
        else if(0==strcmp(SEARCH_USER_OFFLINE,pdu->caData))
        {
            QMessageBox::information(this,"搜索",QString("%1:%2").arg(OpeWidget::getInstance().getFriend()->m_strName).arg(SEARCH_USER_OFFLINE));
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        qDebug()<<"收到了来自服务器转发而来的好友请求消息";
        char fromName[32];
        memcpy(fromName,pdu->caData+32,32);
        int ret=QMessageBox::information(this,"好友申请",QString("%1希望添加您为好友！").arg(fromName),QMessageBox::Yes,QMessageBox::No);
        if(ret==QMessageBox::Yes)
        {
            PDU *addFriendPdu=mkPDU(0);
            addFriendPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
            strncpy(addFriendPdu->caData,fromName,32);//请求人
            strncpy(addFriendPdu->caData+32,strLoginName.toStdString().c_str(),32);//回复人
            m_tcpSocket.write((char *)addFriendPdu,addFriendPdu->uiPDULen);
            free(addFriendPdu);
            addFriendPdu=NULL;
            qDebug()<<"同意信息已发送";
        }
        else
        {
            PDU *addFriendPdu=mkPDU(0);
            addFriendPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            strncpy(addFriendPdu->caData,fromName,32);
            strncpy(addFriendPdu->caData+32,strLoginName.toStdString().c_str(),32);//回复人
            m_tcpSocket.write((char *)addFriendPdu,addFriendPdu->uiPDULen);
            free(addFriendPdu);
            addFriendPdu=NULL;
            qDebug()<<"拒绝信息已发送";
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"添加好友",pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_SUCESS:
    {
        char toName[32];
        memcpy(toName,pdu->caData,32);
        QMessageBox::information(this,"添加好友",QString("%1同意了您的好友请求！").arg(toName));
        break;

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char toName[32];
        memcpy(toName,pdu->caData,32);
        QMessageBox::information(this,"添加好友",QString("%1拒绝了您的好友请求！").arg(toName));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
    {
        if(0==strcmp(pdu->caData,DELETE_FRIEND_OK))
        {
            QMessageBox::information(this,"提示","已成功解除好友关系！");
        }
        else if(0==strcmp(pdu->caData,DELETE_FRIEND_FAILED))
        {
            QMessageBox::information(this,"提示","删除好友失败");
        }
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_IMFORE:
    {
        char FriendName[32];
        strncpy(FriendName,pdu->caData,32);
        QString imfor=QString("%1解除了与您的好友关系").arg(FriendName);
        QMessageBox::information(this,"提示",imfor);
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        if(PrivateChat::getInstance().isHidden())
        {
            char FromName[32];
            memcpy(FromName,pdu->caData+32,32);
            QString StrFromName=FromName;
            PrivateChat::getInstance().setChatName(StrFromName);
            PrivateChat::getInstance().show();
        }
        PrivateChat::getInstance().updateMsg(pdu);


        break;
    }
    case ENUM_MSG_TYPE_QUN_CHAT_REQUEST:
    {
        OpeWidget::getInstance().getFriend()->updateM_pShowMsgTE(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        QString str=QString("%1").arg(pdu->caData);
        QMessageBox::information(this,"新建文件夹",str);
        NetDisk::getinstance().Flush();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        OpeWidget::getInstance().getNetDisk()->updateFileList(pdu);

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        QMessageBox::information(this,"删除文件/夹",pdu->caData);
        NetDisk::getinstance().Flush();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
    {
        QMessageBox::information(this,"重命名文件夹",pdu->caData);
        NetDisk::getinstance().Flush();
        break;
    }
    case ENUM_MSG_TYPE_INTO_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        if(0==strcmp(pdu->caData,INTO_FILE_SUCESS))
        {
            OpeWidget::getInstance().getNetDisk()->updateFileList(pdu);
            QString CurPathNew=QString("%1/%2").arg(getCurrentPath()).arg(str_CurChoose); //根据系统存的当前文件夹位置（点击前）和点击的文件夹，拼成一个新文件夹的地址
            setCurrentPath(CurPathNew);//设置新的当前文件夹地址

        }
        else
        {
            QMessageBox::warning(this,"打卡文件夹","无法打开文件夹：该项不是文件夹");
        }
        break;
    }
    case ENUM_MSG_TYPE_RETURN_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        if(0==strcmp(pdu->caData,RETURN_FILE_SUCESS))
        {
            OpeWidget::getInstance().getNetDisk()->updateFileList(pdu);
            QString str_CurPath=TcpClient::getInstance().getCurrentPath();
            //qDebug()<<"裁减前：   "<<str_CurPath;
            int index=str_CurPath.lastIndexOf("/");//找到最后一个反斜杠的索引
            QString CurPathNew=str_CurPath.left(index);
            //qDebug()<<"裁减后：   "<<str_CurPath;
            setCurrentPath(CurPathNew);//设置新的当前文件夹地址

        }
        else
        {
            QMessageBox::warning(this,"返回上一级","无法打开文件夹：该项不是文件夹");
        }
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
    {
        //qDebug()<<"收到了来自服务器的respdu：";
        //Tools::getInstance().ShowPDU(pdu);
        QMessageBox::information(this,"文件传输",pdu->caData);
        NetDisk::getinstance().Flush();
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
    {
        qDebug()<<"收到了来自服务器的respdu：";
        Tools::getInstance().ShowPDU(pdu);


        //检查获取文件名称和大小，为大小赋值
        char *FileName=new char[32];
        qint64 fileSize=0;

        sscanf(pdu->caData,"%s %lld",FileName,&fileSize);

        NetDisk::getinstance().setITotla(fileSize);
        NetDisk::getinstance().setIReceved(0);
        //NetDisk::getinstance().setIs_DownLoading(true);
        TcpClient::getInstance().setIsDownLoading(true);


        //打开文件，设置读取状态
        QTimer timer;
        //timer.start(1000);
        NetDisk::getinstance().file.setFileName(getSavePath());
        //qDebug()<<"savepath="<<NetDisk::getinstance().getSavePath();

        if(!NetDisk::getinstance().file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this,"下载文件","创建文件失败，位于ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND部分");
        }
        qDebug()<<"aaaaa";
        break;
    }
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
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        strLoginName=strName;
        PDU *pdu=mkPDU(0);//创建一个登录请求的协议单元
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//通过socket发送到服务器
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"登录账户","账户登录失败：账号或密码不能为空！");
    }
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

