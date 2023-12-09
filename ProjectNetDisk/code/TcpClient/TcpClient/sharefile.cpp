#include "sharefile.h"
#include "friend.h"

ShareFile::ShareFile(QWidget *parent)
    : QWidget{parent}
{



    m_pSelectAllPB=new QPushButton("全选");
    m_pCancelAllPB=new QPushButton("取消全选");    //取消全选

    m_pOKPB=new QPushButton("确定");
    m_pCancelPB=new QPushButton("取消");

    m_pSA=new QScrollArea;
    m_pFriendW=new QWidget;
    m_pButtonGrop=new QButtonGroup(m_pFriendW);//多项选择的管理控件
    m_pButtonGrop->setExclusive(false);//设置多选

    QHBoxLayout *pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelAllPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL=new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMainVBL=new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    p=new QVBoxLayout(m_pFriendW);//这个用于显示可分享的好友
    setLayout(pMainVBL);
    //test();

}

void ShareFile::test()
{
    // 清除QButtonGroup中的内容
    QList<QAbstractButton*> buttons = m_pButtonGrop->buttons();
    for (QAbstractButton* button : buttons) {
        m_pButtonGrop->removeButton(button);
        delete button; // 如果需要释放内存
    }

    QVBoxLayout *p=new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB=NULL;
    for(int i=0;i<15;i++)
    {
        pCB=new QCheckBox("jack");
        p->addWidget(pCB);
        m_pButtonGrop->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{

    // 清除QButtonGroup中的内容

    QList<QAbstractButton*> buttons = m_pButtonGrop->buttons();
    for (QAbstractButton* button : buttons) {
        m_pButtonGrop->removeButton(button);
        p->removeWidget(button);
        delete button; // 如果需要释放内存
    }
    delete p;
    p=new QVBoxLayout(m_pFriendW) ;

    //QVBoxLayout *p=new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB=NULL;
    for(int i=0;i<pFriendList->count();i++)
    {
        pCB=new QCheckBox(pFriendList->item(i)->text());
        p->addWidget(pCB);
        m_pButtonGrop->addButton(pCB);
    }
    m_pFriendW->resize(m_pFriendW->sizeHint());  //设置QVBoxLayout的大小，以确保其适应新内容。
    m_pSA->setWidget(m_pFriendW);
}
