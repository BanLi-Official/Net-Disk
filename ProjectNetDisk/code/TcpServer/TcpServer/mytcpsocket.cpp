#include "mytcpsocket.h"
#include <QDebug>
#include "opedb.h"
#include "protocol.h"
#include "mytcpserver.h"
#include "tools.h"
#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <QTimer>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    this->m_bUpload=false;
    timer=new QTimer;
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));//将读信号和recvMsg函数绑定在一起
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));//将用户下线的信号和clientOffline()函数绑定在一起
    connect(timer,SIGNAL(timeout()),this,SLOT(TransData()));


}

QString MyTcpSocket::getName()
{
    return m_strName;
}

QString MyTcpSocket::getNetDiskRoot()
{
    return this->Root;
}

void MyTcpSocket::recvMsg()//当有读信号出来的时候，就调用这个函数，用来读取信息
{

    if(!m_bUpload)
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
            char *CurrentPath=new char[pdu->uiMsgLen];  //动态内存分配
            //char CurrentPath[pdu->uiMsgLen];  静态内存分配，大小写死了
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



            delete[] CurrentPath;
            CurrentPath=NULL;

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


            delete[] curPath;
            curPath=NULL;

            break;
        }
        case ENUM_MSG_TYPE_INTO_FILE_REQUEST:
        {
            //qDebug()<<"收到客户端发来的请求，内容如下";
            //Tools::getInstance().ShowPDU(pdu);
            char DirName[32];  //文件夹名字
            char *curPath=new char[pdu->uiMsgLen];  //当前的位置
            strncpy(DirName,pdu->caData,32);
            memcpy(curPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath=QString("%1/%2").arg(curPath).arg(DirName);//拼成选择的文件的位置
            //qDebug()<<"strPath="<<strPath;

            QFileInfo mydir(strPath);//把这个文件（夹）提取信息，用来判断是否是文件夹
            if(mydir.isDir())  //是文件夹
            {
                QDir dir(strPath); //将这个文件夹放入Dir准备打开
                QFileInfoList myFileInfo=dir.entryInfoList(); //找出该文件夹的内容
                PDU *respdu=mkPDU((myFileInfo.size()-2)*sizeof(FileInfo)+1);  //回复的pdu
                respdu->uiMsgType=ENUM_MSG_TYPE_INTO_FILE_RESPOND;
                strcpy(respdu->caData,INTO_FILE_SUCESS); //打开成功的标志放在这里
                FileInfo *fileInfoIndex=(FileInfo *)respdu->caMsg; //按照FileInfo的长度定位
                int undo=0;
                for(int i=0;i<myFileInfo.size();i++)
                {
                    if(QString(".")==myFileInfo.at(i).fileName()||QString("..")==myFileInfo.at(i).fileName())//先跳过前面的“.”  “..”
                    {
                        undo++;
                        continue;
                    }
                    fileInfoIndex=(FileInfo *)respdu->caMsg+i-undo;  //跳一个FileInfo的距离
                    //放文件的名字
                    memcpy(fileInfoIndex->caFileName,myFileInfo.at(i).fileName().toStdString().c_str(),myFileInfo.at(i).fileName().toUtf8().size());
                    if(myFileInfo[i].isDir()) //放名字的类型
                    {
                        fileInfoIndex->iFileType=0;//文件夹类型
                    }
                    else
                    {
                        fileInfoIndex->iFileType=1; //文件类型
                    }
                }
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;

            }
            else //不是文件夹
            {
                PDU *respdu=mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_INTO_FILE_RESPOND;
                strcpy(respdu->caData,INTO_FILE_FALIED);//进入文件夹失败
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }


            delete[] curPath;
            curPath=NULL;
            break;
        }
        case ENUM_MSG_TYPE_RETURN_FILE_REQUEST:
        {
            char *CurPath=new char[pdu->uiMsgLen];
            memcpy(CurPath,pdu->caMsg,pdu->uiMsgLen);
            QString str_CurPath=QString(CurPath);
            //qDebug()<<"裁减前：   "<<str_CurPath;
            int index=str_CurPath.lastIndexOf("/");//找到最后一个反斜杠的索引
            str_CurPath=str_CurPath.left(index);
            //qDebug()<<"裁减后：   "<<str_CurPath;



            //找文件夹中的内容
            QFileInfo fileInfo(str_CurPath);
            if(fileInfo.isDir())
            {
                QDir dir;
                dir.setPath(str_CurPath);
                QFileInfoList infoList=dir.entryInfoList();

                //定义返回的pdu，存储基础信息
                PDU *respdu=mkPDU(sizeof(FileInfo)*(infoList.size()-2)+1);
                respdu->uiMsgType=ENUM_MSG_TYPE_RETURN_FILE_RESPOND;
                strcpy(respdu->caData,RETURN_FILE_SUCESS);

                //定义FileInfo指针
                FileInfo *FileIndex=(FileInfo *)respdu->caMsg;
                int undo=0;
                for(int i=0;i<infoList.size();i++)
                {

                    if(QString(".")==infoList.at(i).fileName()||QString("..")==infoList.at(i).fileName())//先跳过前面的“.”  “..”
                    {
                        undo++;
                        continue;
                    }
                    FileIndex=(FileInfo *)respdu->caMsg+i-undo;
                    memcpy(FileIndex->caFileName,infoList.at(i).fileName().toStdString().c_str(),infoList.at(i).fileName().toUtf8().size());
                    if(infoList.at(i).isDir())
                    {
                        FileIndex->iFileType=0;
                    }
                    else
                    {
                        FileIndex->iFileType=1;
                    }

                }
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }
            else
            {
                //返回失败
                PDU *respdu=mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_RETURN_FILE_RESPOND;
                strcpy(respdu->caData,RETURN_FILE_FALIED);//进入文件夹失败
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }

            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            //qDebug()<<"收到了来自客户端的pdu";
            //Tools::getInstance().ShowPDU(pdu);
            char FileName[32];
            qint64 FileSize=0;
            sscanf(pdu->caData,"%s %lld",FileName,&FileSize);
            char *CurPath=new char[pdu->uiMsgLen];
            memcpy(CurPath,pdu->caMsg,pdu->uiMsgLen);
            QString filePath=QString("%1/%2").arg(CurPath).arg(FileName);
            //qDebug()<<"拼接成的地址为： "<<filePath;
            free(CurPath);
            CurPath=NULL;

            m_file.setFileName(filePath);
            if(m_file.open(QIODevice::WriteOnly))
            {
                m_bUpload=true;
                m_iTotal=FileSize;
                m_iRecved=0;
            }

            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        {
            //qDebug()<<"收到了来自客户端的pdu";
            //Tools::getInstance().ShowPDU(pdu);
            char FileName[32];
            strcpy(FileName,pdu->caData);
            char *CurPath=new char[pdu->uiMsgLen];
            memcpy(CurPath,pdu->caMsg,pdu->uiMsgLen);
            QString filePath=QString("%1/%2").arg(CurPath).arg(FileName);
            //qDebug()<<"拼接成的地址为： "<<filePath;
            free(CurPath);
            CurPath=NULL;

            qint64 FileSize=0;
            QFileInfo fileInfo(filePath);
            FileSize=fileInfo.size();

            //回复的pdu，告诉客户端它应该接收多大的文件
            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            sprintf(respdu->caData,"%s %lld",FileName,FileSize);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;

            m_file.setFileName(filePath);

            m_file.open(QIODevice::ReadOnly);
            //qDebug()<<"打开："<<filePath;
            timer->start(1000);
            //qDebug()<<"打开开始了："<<filePath;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        {
            char caFromName[32];
            int friend_num;
            sscanf(pdu->caData,"%s%d",caFromName,&friend_num);
            int FilePathSize=pdu->uiPDULen-32*friend_num;
            //制作待转发的分享请求
            PDU *respdu=mkPDU(FilePathSize);
            respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData,caFromName);
            memcpy(respdu->caMsg,(char *)pdu->caMsg+32*friend_num,FilePathSize);

            //找到转发对象并进行转发
            for(int i=0;i<friend_num;i++)
            {
                char caToName[32];
                memcpy(caToName,(char *)pdu->caMsg+i*32,32);
                MyTcpServer::getInstance().resend(caToName,respdu);
            }

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
        {
            //qDebug()<<"收到了来自客户端的pdu";
            //Tools::getInstance().ShowPDU(pdu);
            char ToName[32];
            strcpy(ToName,pdu->caData);
            char *FilePath=new char[pdu->uiMsgLen];
            QString StrFilePath;

            memcpy(FilePath,pdu->caMsg,pdu->uiMsgLen);
            QString ToNameStr(ToName);

            //获取文件名称
            //int index_xie=QString.lastIndexOf(FilePath,"/");
            char* lastSlash = strrchr(FilePath, '/');//得到反斜杠在字符串中的地址
            int index = lastSlash - FilePath; //地址相减 获取反斜杠在字符串中的索引位置

            char *FileName=&FilePath[index+1];//获取反斜杠之后的文件名称

            QString ToPath=getNetDiskRoot()+ToNameStr+"/"+FileName;//组合目的地址
            qDebug()<<"strFromPath="<<StrFilePath;
            qDebug()<<"FromPath="<<FilePath;
            qDebug()<<"ToPath="<<ToPath;


            //QFile file(FilePath);
            //QFile DestFile()
            QFile sourceFile(FilePath);
            QFile destFile(ToPath);

            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_RESULT;


            if (!sourceFile.exists()) {
                qDebug() << "源文件不存在";
                strcpy(respdu->caData,SHARE_FILE_NOEXISTS);
            }else if (destFile.exists()) {
                qDebug() << "目标文件已存在";
                strcpy(respdu->caData,SHARE_FILE_EXISTS);
            }else if (QFile::copy(FilePath, ToPath)) {
                qDebug() << "文件复制成功";
                strcpy(respdu->caData,SHARE_FILE_SUCESS);
            } else {
                qDebug() << "文件复制失败";
                strcpy(respdu->caData,SHARE_FILE_FALIED);
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
    else
    {
        PDU *respdu=NULL;
        respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

        QByteArray buff=readAll();
        m_file.write(buff);
        m_iRecved +=buff.size();
        if(m_iRecved==m_iTotal)
        {
            m_file.close();
            m_bUpload=false;

            strcpy(respdu->caData,UPLOAD_FILE_SUCESS);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(m_iRecved>m_iTotal)
        {
            m_file.close();
            m_bUpload=false;

            strcpy(respdu->caData,UPLOAD_FILE_FALIED);
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;

        }

    }


}

void MyTcpSocket::clientOffline()
{

    OpeDB::getInstance().handleSetOffline(m_strName);//将用户状态设置为离线状态
    emit offline(this);  //发送信号 this为MyTcpSocket对象的地址
}

void MyTcpSocket::TransData()
{
    timer->stop();
    qint64 ret=0;
    char *buffer=new char[4096];

    while(true)
    {

        ret=m_file.read(buffer,4096);
        if(ret>0&&ret<=4096)
        {
            write(buffer,ret);

        }
        else if(ret==0)
        {
            m_file.close();
            break;
        }
        else if(ret<0)
        {
            m_file.close();
            qDebug()<<"传输过程中，读取文件出现错误";
            break;
        }

    }
    //qDebug()<<"一次处理";

    delete[] buffer;
    buffer=NULL;
}
