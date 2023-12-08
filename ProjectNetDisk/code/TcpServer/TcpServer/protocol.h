#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_DEFAULT_RENAME "regist default:用户重名！"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:账户或者密码错误！或者请勿重复登录"


#define SEARCH_USER_NO "用户不存在"
#define SEARCH_USER_ONLINE "用户在线"
#define SEARCH_USER_OFFLINE "用户不在线"

#define UNKNOW_ERROR "未知错误"
#define EXISTED_FRIEND "已经是好友了"
#define ADD_FRIENDS_OFFLINE "好友不在线"
#define ADD_FRIEND_NO_EXSIT "没有这个用户"

#define DELETE_FRIEND_OK "成功删除好友"
#define DELETE_FRIEND_FAILED "删除好友失败"
#define DELETE_FRIEND_NOT_ONLINE "删除好友不在线"


#define CREATE_FILE_SUCESS "创建文件夹成功"
#define CREATE_FILE_EXISTED "创建文件夹已存在"
#define CREATE_FILE_NO_PATH "当前文件夹路径不存在"
#define CREATE_FILE_FAILED_UNKNOWN "创建文件夹因未知原因失败"

#define DELETE_DIR_SUCESS "删除文件夹成功"
#define DELETE_DIR_FALIED "删除文件夹失败"
#define DELETE_FILE_SUCESS "删除文件成功"
#define DELETE_FILE_FALIED "删除文件失败"


#define RENAME_FILE_SUCESS "重命名文件夹成功"
#define RENAME_FILE_FALIED "重命名文件夹失败"

#define INTO_FILE_SUCESS "进入文件夹成功"
#define INTO_FILE_FALIED "进入文件夹失败"


#define RETURN_FILE_SUCESS "返回文件夹成功"
#define RETURN_FILE_FALIED "返回文件夹失败"


#define UPLOAD_FILE_SUCESS "传输文件成功"
#define UPLOAD_FILE_FALIED "传输文件失败"







enum ENUM_MSG_TYPE    //利用枚举类型，设置消息的类型
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,//注册恢复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登录恢复
    ENUM_MSG_TYPE_All_ONLINE_REQUEST,//显示所有在线用户请求
    ENUM_MSG_TYPE_All_ONLINE_RESPOND,//显示所有在线用户的回复
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,//搜索用户的回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,//同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_SUCESS,//成功添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_DEFAULT,//添加好友失败
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新好友的回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//删除好友的回复
    ENUM_MSG_TYPE_DELETE_FRIEND_IMFORE,//通知用户被删除好友
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//与好友私聊的请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,//与好友私聊的回复
    ENUM_MSG_TYPE_QUN_CHAT_REQUEST,//与好友群聊的请求
    ENUM_MSG_TYPE_QUN_CHAT_RESPOND,//与好友群聊的回复
    ENUM_MSG_TYPE_CREATE_FILE_REQUEST,//创建文件夹的请求
    ENUM_MSG_TYPE_CREATE_FILE_RESPOND,//创建文件夹的回复
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件夹的请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//刷新文件夹的回复
    ENUM_MSG_TYPE_DELETE_FILE_REQUEST,//删除文件夹的请求
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND,//删除文件夹的回复
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//重命名文件夹的请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//重命名文件夹的回复
    ENUM_MSG_TYPE_INTO_FILE_REQUEST,//进入文件夹的请求
    ENUM_MSG_TYPE_INTO_FILE_RESPOND,//进入文件夹的回复
    ENUM_MSG_TYPE_UPDATE_CURRENT_PATH,//修改当前文件夹的位置
    ENUM_MSG_TYPE_RETURN_FILE_REQUEST,//返回上一级文件夹的请求
    ENUM_MSG_TYPE_RETURN_FILE_RESPOND,//返回上一级文件夹的回复
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传文件的请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//上传文件的回复
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载文件的请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件的回复
    ENUM_MSG_TYPE_MAX=0x00ffffff
};


struct FileInfo
{
    char caFileName[32];//文件名字
    int iFileType;//文件类型
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
