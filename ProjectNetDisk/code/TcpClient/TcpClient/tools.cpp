#include "tools.h"
#include <QDebug>


//struct PDU   //协议数据单元的结构体
//{
//    uint uiPDULen;   //总的协议数据单元大小
//    uint uiMsgType;   //消息类型、
//    char caData[64];
//    uint uiMsgLen;  //实际消息长度
//    int caMsg[];    //消息内容
//};



Tools::Tools()
{

}

void Tools::ShowPDU(PDU *pdu)
{
    qDebug()<<"              总的协议数据单元大小 pdu->uiPDULen:"<<pdu->uiPDULen;
    qDebug()<<"              消息类型           pdu->uiMsgType:"<<pdu->uiMsgType;
    QString str = QString::fromUtf8(pdu->caData, 64);
    str.replace(QChar('\u0000'), QChar(' '));
    qDebug()<<"              caData            pdu->caData:"<<str;
    char *msg=new char[pdu->uiMsgLen];
    memcpy(msg,pdu->caMsg,pdu->uiMsgLen);
    qDebug()<<"              实际消息长度        pdu->uiMsgLen:"<<pdu->uiMsgLen;
    QString str2(msg);  // 将char*类型的字符串转换为QString
    QByteArray unicodeData = str2.toUtf8();
    qDebug() << "              消息内容 pdu->caMsg:" << unicodeData.data()<<"///";

}

Tools Tools::getInstance()
{
    static Tools Instances;
    return Instances;
}
