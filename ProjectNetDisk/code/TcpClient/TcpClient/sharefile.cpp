#include "sharefile.h"
#include "friend.h"
#include "tcpclient.h"
#include "NetDisk.h"

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

    connect(m_pCancelAllPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(OKShare()));
    connect(m_pCancelPB,SIGNAL(clicked(bool)),this,SLOT(cancelShare()));

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

void ShareFile::cancelSelect()
{
    QList<QAbstractButton *> cbList=m_pButtonGrop->buttons();
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton *> cbList=m_pButtonGrop->buttons();
    for(int i=0;i<cbList.size();i++)
    {
        if(!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }

    return;
}

void ShareFile::OKShare()
{
    //pdu中存储信息的格式为：
    //caData：分享者名称+被分享的人数量
    //caMsg：分享者名称*n+分享文件的地址
    QString strName=TcpClient::getInstance().getLoginName();//获取分享者的名称
    QString strCurrPath=TcpClient::getInstance().getCurrentPath();//获取当前位置
    QString strShareFileName=OpeWidget::getInstance().getNetDisk()->getShareFileName();//获取被分享的文件
    uint num=0;

    QString strPath=strCurrPath+"/"+strShareFileName;  //拼接文件的位置


    QList<QAbstractButton *> cbList=m_pButtonGrop->buttons();  //遍历所有好友，判断是否被选中
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            num++;  //统计被选中的好友的数量
        }
    }

    PDU *pdu=mkPDU(32*num+strPath.toUtf8().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);  //存储分享者的名称和被分享的人的数量
    int j=0;
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            memcpy((char *)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),32);//存储被分享者的人的名字
            j++;
        }
    }

    memcpy((char *)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.toUtf8().size());//补上被分享的文件的地址
    qDebug()<<"strPath="<<strPath;
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

    this->hide();




}

void ShareFile::cancelShare()
{
    this->hide();
}
