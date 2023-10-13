#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_DEFAULT_RENAME "regist default:用户重名！"

enum ENUM_MSG_TYPE    //利用枚举类型，设置消息的类型
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,
    ENUM_MSG_TYPE_REGIST_RESPOND,
    ENUM_MSG_TYPE_MAX=0x00ffffff
};

struct PDU
{
    uint uiPDULen;   //总的协议数据单元大小
    uint uiMsgType;   //消息类型、
    char caData[64];
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //消息内容
};

PDU *mkPDU(uint uiMsgLen);  //用于创建一个协议数据单元


#endif // PROTOCOL_H