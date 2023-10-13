#include "mytcpsocket.h"
#include <QDebug>
#include "opedb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));//将读信号和recvMsg函数绑定在一起
}

void MyTcpSocket::recvMsg()//当有读信号出来的时候，就调用这个函数，用来读取信息
{
    //先获取消息
    qDebug()<<this->bytesAvailable();//这段代码使用Qt框架中的qDebug函数输出当前对象可读取的字节数。
    uint uiPDULen=0;
    this->read((char*) &uiPDULen,sizeof(uint));//从当前对象中读取一个unsigned int类型的数据，并将其存储在uiPDULen变量中。
    uint uiMsgLen=uiPDULen-sizeof(PDU);  //获取数据协议中实际数据的大小
    PDU *pdu=(PDU *)mkPDU(uiMsgLen);//初始化一个新的数据协议单元用于存储接下来读取的内容
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));//读取发过来UIPDULen后面的内容
    pdu->uiPDULen=uiPDULen;
//    qDebug()<<"发送过来的数据长度："<<pdu->uiPDULen<<"   发送过来的Data实际内容："<<(char*)(pdu->caData);
//    qDebug()<<"发送过来的数据类型："<<pdu->uiMsgType<<"   发送过来的数据实际内容："<<(char*)(pdu->caMsg);
//    char name[32];
//    char pwd[32];
//    strncpy(name,pdu->caData,32);
//    strncpy(pwd,pdu->caData+32,32);
//    qDebug()<<name<<pwd<<pdu->uiMsgType;


    //按照不同的消息内容进行不同的处理
    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        //将名字和密码提取出来，运行并插入数据库
        char name[32];
        char pwd[32];
        strncpy(name,pdu->caData,32);
        strncpy(pwd,pdu->caData+32,32);
        qDebug()<<name<<pwd<<pdu->uiMsgType;
        bool ret=OpeDB::getInstance().handleRegist(name,pwd);

        //生成一个返回pdu,将运行结果告诉客户端
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret)
        {
            //respdu->caData=REGIST_OK;  这样不安全，放入的REGIST_OK是地址常量，而不是将数据实际内容放入其中
            strcpy(respdu->caData,REGIST_OK);//这种赋值方法是将一个字符串常量的内容复制到respdu->caData指向的内存空间中，这样做是安全的，因为复制后的字符串是可写的，后续代码可以修改它。
            qDebug()<<"注册成功！";
        }
        else
        {
            //respdu->caData=REGIST_DEFAULT_RENAME;
            strcpy(respdu->caData,REGIST_DEFAULT_RENAME);
            qDebug()<<"注册失败，可能原因是：数据库中有重名用户！";
        }

        write((char *)respdu,respdu->uiPDULen);//将这个返回的信息发送到客户端
        free(respdu);
        respdu=NULL;
        break;

    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;

}
