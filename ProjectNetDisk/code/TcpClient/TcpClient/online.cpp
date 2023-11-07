#include "online.h"
#include "ui_online.h"

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

    char caTmp[32];
    uint uisize=(pdu->uiMsgLen)/32;
    for (uint i=0;i<uisize;i++)
    {

        memcpy(caTmp,(char *)(pdu->caMsg)+i*32,32);
        ui->online_listwidget->addItem(caTmp);
    }

}
