#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include<QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include "privatechat.h"

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE=new QTextEdit;//用于展示信息
    m_pFriendListWidget=new QListWidget;//用于展示好友信息
    m_pInputMsgLE=new QLineEdit;//用于输入信息

    m_pDelFriendPB=new QPushButton("删除好友");//删除好友按钮
    m_pFlushFriendPB=new QPushButton("刷新好友");//刷新好友信息按钮
    m_pShowOnlineUsrPB=new QPushButton("显示在线用户");//展示好友信息按钮
    m_pSearchUsrPB=new QPushButton("查找用户");//查找用户按钮
    m_pMsgSendPB=new QPushButton("发送信息");//发送信息按钮
    m_pPrivateChatPB=new QPushButton("私聊好友");//私聊按钮

    QVBoxLayout *pRightPBVBL=new QVBoxLayout;//定义一个垂直分布，将这些按钮都放一起
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL=new QHBoxLayout;//定义一个横向分布
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL=new QHBoxLayout;//定义一个横向分布
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    online=new Online;

     QVBoxLayout *pMain=new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(online);
    online->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(ShowOnline()));//将展示在线用户的按钮与展示在线用户的函数关联起来
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(SearchUser()));//将用户搜索按钮与用户搜索功能关联起来
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));//将刷新好友列表与函数关联起来
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));//将删除好友函数与按钮关联起来
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));//关联私聊按钮
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));//关联群聊按钮

}

void Friend::ShowAllOnlineUsr(PDU *pdu)
{
    if(NULL==pdu)
    {
        return ;
    }

    online->ShowUser(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu==NULL)
    {
        return;
    }

    m_pFriendListWidget->clear();

    uint uiSize=pdu->uiMsgLen/32;
    char caName[32]={'\0'};
    for (uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char *)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }

}

void Friend::updateM_pShowMsgTE(PDU *pdu)
{
    char FromName[32];
    memcpy(FromName,pdu->caData,32);
    QString data_str=QString("%1对你说：\n %2 \n\n").arg(FromName).arg(QString::fromUtf8((char *)pdu->caMsg));
    m_pShowMsgTE->append(data_str);
}

void Friend::ShowOnline()
{
    qDebug()<<"friend";
    if(online->isHidden())
    {
        online->show();

        //传递显示所有用户的消息给服务器
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_All_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        online->hide();
    }
}

void Friend::SearchUser()
{
    m_strName=QInputDialog::getText(this,"搜索用户","用户名：");
    if(!m_strName.isEmpty())
    {
        qDebug()<<"用户输入的名称为："<<m_strName;
        PDU *pdu=mkPDU(0);
        strncpy(pdu->caData,m_strName.toStdString().c_str(),m_strName.size());
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;



    }
}

void Friend::flushFriend()
{
    QString strName=TcpClient::getInstance().getLoginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strFriendName=m_pFriendListWidget->currentItem()->text();
        QString strMyName=TcpClient::getInstance().getLoginName();
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        memcpy(pdu->caData,strMyName.toStdString().c_str(),strMyName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());

        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::information(this,"提示","请选择要删除的好友");
    }
}

void Friend::privateChat()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strChatName=m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::information(this,"提示","请选择要私聊的好友");
    }
}

void Friend::groupChat()
{
    QString str=m_pInputMsgLE->text();
    if(str.isEmpty())
    {
        QMessageBox::information(this,"提示","请输入你想发送的内容！");
    }
    else
    {
        QString FromName=TcpClient::getInstance().getLoginName();
        PDU *pdu=mkPDU(str.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_QUN_CHAT_REQUEST;
        memcpy(pdu->caData,FromName.toStdString().c_str(),32);
        strcpy((char *)pdu->caMsg,str.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}
