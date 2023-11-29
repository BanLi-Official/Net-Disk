#ifndef FRIEND_H
#define FRIEND_H


#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void ShowAllOnlineUsr(PDU *pdu);
    void updateFriendList(PDU *pdu);//更新好友列表
    void updateM_pShowMsgTE(PDU *pdu);//更新群聊消息


    QString m_strName;//临时存储的名字

signals:

public slots:
    void ShowOnline();//展示好友界面
    void SearchUser();//查找用户
    void flushFriend();//刷新好友列表
    void delFriend();//删除好友列表
    void privateChat();//好友私聊
    void groupChat();//好友群聊

private:
    QTextEdit *m_pShowMsgTE;//用于展示信息
    QListWidget *m_pFriendListWidget;//用于展示好友信息
    QLineEdit *m_pInputMsgLE;//用于输入信息

    QPushButton *m_pDelFriendPB;//删除好友按钮
    QPushButton *m_pFlushFriendPB;//刷新好友信息按钮
    QPushButton *m_pShowOnlineUsrPB;//展示好友信息按钮
    QPushButton *m_pSearchUsrPB;//查找用户按钮
    QPushButton *m_pMsgSendPB;//查找用户按钮
    QPushButton *m_pPrivateChatPB;//用户私聊按钮

    Online *online;//在线用户

};

#endif // FRIEND_H
