#include "protocol.h"


PDU *mkPDU(uint uiMsgLen)
{
    PDU *pdu=(PDU*)malloc(sizeof(PDU)+uiMsgLen);//利用动态结构体创建一个pdu,动态体现在长度按照后面实际的数据长度变化
    if(NULL==pdu)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,sizeof(PDU)+uiMsgLen);//将指针pdu所指向的内存空间中的前sizeof(PDU)+uiMsgLen个字节全部设置为0。
    pdu->uiPDULen=sizeof(PDU)+uiMsgLen;
    pdu->uiMsgLen=uiMsgLen;
    return pdu;
}
