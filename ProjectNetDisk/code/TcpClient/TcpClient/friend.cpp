#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include<QInputDialog>
#include <QDebug>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE=new QTextEdit;//用于展示信息
    m_pFriendListWidget=new QListWidget;//用于展示好友信息
    m_pInputMsgLE=new QLineEdit;//用于输入信息

    m_pDelFriendPB=new QPushButton("删除好友");//删除好友按钮
    m_pFlushFriendPB=new QPushButton("刷新好友");//刷新好友信息按钮
    m_pShowOnlineUsrPB=new QPushButton("显示在线好友");//展示好友信息按钮
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

}

void Friend::ShowAllOnlineUsr(PDU *pdu)
{
    if(NULL==pdu)
    {
        return ;
    }

    online->ShowUser(pdu);
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
