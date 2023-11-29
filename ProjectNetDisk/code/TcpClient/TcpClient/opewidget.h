#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "NetDisk.h"
#include <QStackedWidget>


class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();
    Friend *getFriend();
    NetDisk *getNetDisk();

signals:

private:
    QListWidget *m_pListW;//创建一个list用于存放要显示的窗口
    Friend *m_pFriend;//好友界面
    NetDisk *m_pNetDisk;//网盘界面

    QStackedWidget *m_pSW;

};

#endif // OPEWIDGET_H
