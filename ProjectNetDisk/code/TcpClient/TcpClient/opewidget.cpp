#include "opewidget.h"
#include "tcpclient.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{
    m_pListW=new QListWidget(this);//旁边的选择列表
    m_pListW->addItem("好友");
    m_pListW->addItem("网盘");

    m_pFriend=new Friend;  //展示界面
    m_pNetDisk =new NetDisk;

    m_pSW=new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pNetDisk);

    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    setWindowTitle(TcpClient::getInstance().getLoginName());

    connect(m_pListW,SIGNAL(currentRowChanged(int)),
            m_pSW,SLOT(setCurrentIndex(int)));//将m_pListW中发出的行选择信息（currentRowChanged(int)）与setCurrentIndex(int)函数绑定起来，m_pListW发信息，m_pSW做出反应
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend()
{
    return this->m_pFriend;
}

NetDisk *OpeWidget::getNetDisk()
{
    return this->m_pNetDisk;
}
