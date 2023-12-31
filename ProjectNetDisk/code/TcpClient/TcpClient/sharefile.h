#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    void test();
    static ShareFile &getInstance();//单例模式
    void updateFriend(QListWidget *pFriendList);

signals:

public slots:
    void cancelSelect();//取消选择的函数
    void selectAll();//全选函数
    void OKShare();//分享函数
    void cancelShare();//取消分享函数

private:
    QPushButton *m_pSelectAllPB;  //选择所有好友的按钮
    QPushButton *m_pCancelAllPB;    //取消全选

    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QButtonGroup *m_pButtonGrop;
    QVBoxLayout *p;


};

#endif // SHAREFILE_H
