#include "friend.h"

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


}

void Friend::ShowOnline()
{
    if(online->isHidden())
    {
        online->show();
    }
    else
    {
        online->hide();
    }
}
