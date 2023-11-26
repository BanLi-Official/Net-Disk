#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::ShowUser(PDU *pdu)
{

    if(NULL== pdu)
    {
        return ;
    }
    ui->online_listwidget->clear();
    char caTmp[32];
    uint uisize=(pdu->uiMsgLen)/32;
    for (uint i=0;i<uisize;i++)
    {
        memcpy(caTmp,(char *)(pdu->caMsg)+i*32,32);
        //QString str = QString::fromUtf8(caTmp);  // 使用fromUtf8()将char数组转换为QString类型
        QByteArray byteArray = TcpClient::getInstance().getLoginName().toUtf8();
        const char* Name = byteArray.constData();
        if(0==strcmp(caTmp,Name))
        {
            continue;
        }
        ui->online_listwidget->addItem(caTmp);

    }

}

void Online::on_addNewFriend_pb_clicked()
{
    QListWidgetItem *pItem=ui->online_listwidget->currentItem();
    QString strFirendName=pItem->text();
    QString strLoginName=TcpClient::getInstance().getLoginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strFirendName.toStdString().c_str(),strFirendName.size());
    memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;




}

