#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{
    m_pListW=new QListWidget(this);//旁边的选择列表
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");

    m_pFriend=new Friend;  //展示界面
    m_pBook =new Book;

    m_pSW=new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_pListW,SIGNAL(currentRowChanged(int)),
            m_pSW,SLOT(setCurrentIndex(int)));//将m_pListW中发出的行选择信息（currentRowChanged(int)）与setCurrentIndex(int)函数绑定起来
}
