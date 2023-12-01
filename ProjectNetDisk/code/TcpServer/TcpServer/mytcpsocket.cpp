#include "mytcpsocket.h"
#include <QDebug>
#include "opedb.h"
#include "protocol.h"
#include "mytcpserver.h"
#include "tools.h"
#include <QDir>
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));//将读信号和recvMsg函数绑定在一起
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));//将用户下线的信号和clientOffline()函数绑定在一起
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()//当有读信号出来的时候，就调用这个函数，用来读取信息
{
    //先获取消息
   // qDebug()<<this->bytesAvailable();//这段代码使用Qt框架中的qDebug函数输出当前对象可读取的字节数。
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
            QDir dir;
            dir.mkdir(QString("./UserFile/%1").arg(name));
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
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        //将名字和密码提取出来，运行并插入数据库
        char name[32];
        char pwd[32];
        strncpy(name,pdu->caData,32);
        strncpy(pwd,pdu->caData+32,32);
        qDebug()<<name<<pwd<<pdu->uiMsgType;
        bool ret=OpeDB::getInstance().handleLogin(name,pwd);

        //生成一个返回pdu,将运行结果告诉客户端
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
        if(ret)
        {
            strcpy(respdu->caData,LOGIN_OK);
            this->m_strName=name;
            qDebug()<<"登录成功！";
        }
        else
        {
            strcpy(respdu->caData,LOGIN_FAILED);
            qDebug()<<"登录失败！";
        }

        write((char *)respdu,respdu->uiPDULen);//将消息发送给客户端
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_All_ONLINE_REQUEST:
    {
        qDebug()<<"检测到显示所有用户的请求";
        //去数据库里面找
        QStringList allOnline= OpeDB::getInstance().handleAllOnline();
        //将数据装包并发送
        uint uiMsgLen=allOnline.size()*32;
        qDebug()<<"uiMsgLen="<<uiMsgLen;
        PDU *retPdu=mkPDU(uiMsgLen);

        retPdu->uiMsgType=ENUM_MSG_TYPE_All_ONLINE_RESPOND;

        for(int i=0;i<allOnline.size();i++)
        {
            memcpy((char *)(retPdu->caMsg)+i*32,allOnline[i].toStdString().c_str(),allOnline[i].size());
            qDebug()<<allOnline[i];

        }
        qDebug()<<"retPdu->uiPDULen="<<retPdu->uiPDULen;
        write((char *)retPdu,retPdu->uiPDULen);
        free(retPdu);
        retPdu=NULL;
        qDebug()<<"在线好友查找结果已返回";

        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
    {
        char name[32];
        strncpy(name,pdu->caData,32);
        int state=OpeDB::getInstance().handlSearchUser(name);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
        if(state==1)
        {
            strcpy(respdu->caData,SEARCH_USER_ONLINE);
        }
        else if(state==0)
        {
            strcpy(respdu->caData,SEARCH_USER_OFFLINE);
        }
        else
        {
            strcpy(respdu->caData,SEARCH_USER_NO);
        }
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;


        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char FriendName[32];
        char LoginName[32];
        strncpy(FriendName,pdu->caData,32);
        strncpy(LoginName,pdu->caData+32,32);
        int ret=OpeDB::getInstance().handlAddFriend(FriendName,LoginName);
        //PDU *respdu=NULL;
        if(ret==-1)
        {
            qDebug()<<UNKNOW_ERROR;
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOW_ERROR);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(ret==0)
        {
            qDebug()<<EXISTED_FRIEND;
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(ret==1)
        {
            qDebug()<<"用户在线";
            MyTcpServer::getInstance().resend(FriendName,pdu);
        }
        else if(ret==2)
        {
            qDebug()<<ADD_FRIENDS_OFFLINE;
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIENDS_OFFLINE);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(ret==3)
        {
            qDebug()<<ADD_FRIEND_NO_EXSIT;
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NO_EXSIT);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else
        {
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOW_ERROR);
            qDebug()<<"未知错误";
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
    {
        char fromName[32];
        char toName[32];
        memcpy(fromName,pdu->caData,32);
        memcpy(toName,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handlAddFriendToDataset(fromName,toName);
        if(ret)
        {
            qDebug()<<"好友添加成功";
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_SUCESS;
            strncpy(respdu->caData,toName,32);//告诉申请者谁同意了
            MyTcpServer::getInstance().resend(fromName,respdu);
            free(respdu);
            respdu=NULL;


        }
        else
        {
            qDebug()<<"好友添加失败";
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {

        qDebug()<<"对象拒绝了好友请求";
        char fromName[32];
        char toName[32];
        memcpy(fromName,pdu->caData,32);
        memcpy(toName,pdu->caData+32,32);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        strncpy(respdu->caData,toName,32);//告诉申请者谁拒绝了
        MyTcpServer::getInstance().resend(fromName,respdu);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        char name[32];
        memcpy(name,pdu->caData,32);
        QStringList strFriendList=OpeDB::getInstance().handleFlushFriend(name);
        //qDebug()<<"strFriendList="<<strFriendList;
        uint uiMsgLen=strFriendList.size()*32;
        PDU *resPdu=mkPDU(uiMsgLen);
        resPdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for(int i=0;i<strFriendList.size();i++)
        {
            memcpy((char *)(resPdu->caMsg)+i*32,strFriendList.at(i).toStdString().c_str(),strFriendList.at(i).size());
        }
        write((char *)resPdu,resPdu->uiPDULen);
        free(resPdu);
        resPdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char myName[32];
        char FriendName[32];
        memcpy(myName,pdu->caData,32);
        memcpy(FriendName,pdu->caData+32,32);



        bool ret=OpeDB::getInstance().handlDelFriend(myName,FriendName);
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        if(ret)
        {
            strcpy(pdu->caData,DELETE_FRIEND_OK);
        }
        else
        {
            strcpy(pdu->caData,DELETE_FRIEND_FAILED);
        }
        write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;


        bool is_online=OpeDB::getInstance().handlCheckOnline(FriendName);
        if(!is_online)
        {

            //不在线，不发送
        }
        else //在线 发送通知
        {
            PDU *pdu=mkPDU(0);
            pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_IMFORE;
            memcpy(pdu->caData,myName,32);
            MyTcpServer::getInstance().resend(FriendName,pdu);
            free(pdu);
            pdu=NULL;


        }


        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        char m_ChatName[32];
        memcpy(m_ChatName,pdu->caData,32);
        MyTcpServer::getInstance().resend(m_ChatName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_QUN_CHAT_REQUEST:
    {
        //qDebug()<<"收到了群聊请求";
        //for(找对象){转发}
        char FromName[32];
        memcpy(FromName,pdu->caData,32);
        QStringList allFriend=OpeDB::getInstance().handleFlushFriend(FromName);
        for(int i=0;i<allFriend.size();i++)
        {
            MyTcpServer::getInstance().resend(allFriend.at(i).toStdString().c_str(),pdu);
            //qDebug()<<"已转发给用户："<<allFriend.at(i);
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FILE_REQUEST:
    {
        //qDebug()<<"收到了来自客户端的新建文件夹请求";
        //Tools::getInstance().ShowPDU(pdu);
        QString curPath=QString("%1").arg((char *)(pdu->caMsg));
        QDir dirOpe;
        bool ret=dirOpe.exists(curPath);
        //qDebug()<<"curPath="<<curPath;
        PDU *resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_CREATE_FILE_RESPOND;
        if(ret)//该地址存在
        {
            char caNewDir[32];
            memcpy(caNewDir,pdu->caData+32,32);
            QString newPath=curPath+"/"+QString(caNewDir);
            qDebug()<<"需要创建的新的地址为："<<newPath;
            ret=dirOpe.exists(newPath);
            if(ret)//需要创建的新地址已经存在了
            {
                strcpy(resPdu->caData,CREATE_FILE_EXISTED);
            }
            else//开始创建文件夹
            {
                ret=dirOpe.mkdir(newPath);
                if(ret)//注册成功
                {
                    strcpy(resPdu->caData,CREATE_FILE_SUCESS);
                }
                else//注册失败
                {
                    strcpy(resPdu->caData,CREATE_FILE_FAILED_UNKNOWN);
                }
            }
        }
        else//该地址不存在
        {
            strcpy(resPdu->caData,CREATE_FILE_NO_PATH);
        }

        write((char *)resPdu,resPdu->uiPDULen);
        free(resPdu);
        resPdu=NULL;


        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
    {
        //qDebug()<<"收到了来自客户端的请求：";
        //Tools::getInstance().ShowPDU(pdu);
        char *curPath=new char[pdu->uiMsgLen];
        memcpy(curPath,pdu->caMsg,pdu->uiMsgLen);
        QDir dir(curPath);
        QFileInfoList fileInfoList=dir.entryInfoList();
        int FileSize=fileInfoList.size();
        PDU *respdu=mkPDU(sizeof(FileInfo)*(FileSize-2));
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
        FileInfo *pFileInfo=NULL;
        QString strFileName;
        int undo=0;
        for(int i=0;i<fileInfoList.size();i++)
        {

            if(QString(".")==fileInfoList.at(i).fileName()||QString("..")==fileInfoList.at(i).fileName())
            {
                undo++;
                continue;
            }
            pFileInfo=(FileInfo*)(respdu->caMsg)+i-undo;
            strFileName=fileInfoList.at(i).fileName();
            memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.toUtf8().size());
            if(fileInfoList[i].isDir())
            {
                pFileInfo->iFileType=0;//文件夹类型
            }
            else
            {
                pFileInfo->iFileType=1; //文件类型

            }

        }
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:
    {
        //qDebug()<<"收到客户端发来的请求，内容如下";
        //Tools::getInstance().ShowPDU(pdu);
        char FileName[32];
        strcpy(FileName,pdu->caData);
        //char *CurrentPath=new char[pdu->uiMsgLen];
        char CurrentPath[pdu->uiMsgLen];
        memcpy(CurrentPath,pdu->caMsg,pdu->uiMsgLen);
        QString DetPath=QString("%1/%2").arg(CurrentPath).arg(FileName);
        //qDebug()<<"DetPath="<<DetPath;
        QFileInfo fileInfo(DetPath);
        //qDebug()<<fileInfo;

        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
        if(fileInfo.isDir())
        {
            QDir dir;
            dir.setPath(DetPath);
            bool ret=dir.removeRecursively();
            if(ret)
            {
                //qDebug()<<"删除成功";
                strcpy(respdu->caData,DELETE_DIR_SUCESS);
            }
            else
            {
                //qDebug()<<"删除失败";
                strcpy(respdu->caData,DELETE_DIR_FALIED);
            }
        }
        else //if(fileInfo.isFile())
        {
            QFile file(DetPath);
            bool ret=file.remove();
            if(ret)
            {
                //qDebug()<<"删除文件成功";
                strcpy(respdu->caData,DELETE_FILE_SUCESS);
            }
            else
            {
                //qDebug()<<"删除文件失败";
                strcpy(respdu->caData,DELETE_FILE_FALIED);
            }

        }


        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
    {
        char oldName[32];
        char newName[32];
        char *curPath=new char[pdu->uiMsgLen];
        strncpy(oldName,pdu->caData,32);
        strncpy(newName,pdu->caData+32,32);
        memcpy(curPath,pdu->caMsg,pdu->uiMsgLen);
        QString oldPath=QString("%1/%2").arg(curPath).arg(oldName);
        QString newPath=QString("%1/%2").arg(curPath).arg(newName);
        QDir dir;
        bool ret=dir.rename(oldPath,newPath);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
        if(ret)
        {
            strcpy(respdu->caData,RENAME_FILE_SUCESS);
        }
        else
        {
            strcpy(respdu->caData,RENAME_FILE_FALIED);
        }
        write((char *)respdu,respdu->uiPDULen);
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

void MyTcpSocket::clientOffline()
{

    OpeDB::getInstance().handleSetOffline(m_strName);//将用户状态设置为离线状态
    emit offline(this);  //发送信号 this为MyTcpSocket对象的地址
}
