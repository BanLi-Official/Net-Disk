#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::setChatName(QString strName)
{
    this->m_strChatName=strName;
    this->m_strLoginName=TcpClient::getInstance().getLoginName();
    return ;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu==NULL)
    {
        return;
    }
    char FromName[32];
    memcpy(FromName,pdu->caData+32,32);
   // QString msg = QString::fromUtf8((char *)(pdu->caMsg), strlen((char *)(pdu->caMsg)) / 3);
    //QString msg = QString::fromUtf8((char *)pdu->caMsg);
    //qDebug()<<msg<<"      (char *)pdu->caMsg.size()=" ;

    //QString str=QString("%1说：\n %2").arg(FromName).arg((char *)pdu->caMsg);
    QString str=QString("%1说：\n %2").arg(FromName).arg(QString::fromUtf8((char *)pdu->caMsg));
    ui->ShowMsg_pc->append(str);
    return;
}

void PrivateChat::on_SendMsg_pb_clicked()
{
    QString strMsg=ui->InputMsg_le->text();
    qDebug()<<strMsg;
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.toUtf8().size()+1);
        qDebug()<<"strMsg.size()+1="<<strMsg.toUtf8().size()+1;
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData,this->m_strChatName.toStdString().c_str(),m_strChatName.size());
        memcpy(pdu->caData+32,this->m_strLoginName.toStdString().c_str(),m_strLoginName.size());
        strcpy((char *)pdu->caMsg,strMsg.toStdString().c_str());
        qDebug()<<"pdu->caMsg="<<(char *)(pdu->caMsg)<<"         strMsg.toUtf8().size()="<<strMsg.toUtf8().size();
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"警告","请勿发送空消息！");
    }
}







