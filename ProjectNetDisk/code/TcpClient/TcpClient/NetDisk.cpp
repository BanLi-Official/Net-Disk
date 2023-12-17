#include "NetDisk.h"
#include <QMessageBox>
#include "tcpclient.h"
#include <QFileDialog>
#include <QTimer>
#include "opewidget.h"
#include "sharefile.h"
#include <QThread>


NetDisk::NetDisk(QWidget *parent)
    : QWidget{parent}
{
    Timer=new QTimer;
    Timer2=new QTimer;
    is_DownLoading=false;

    m_pBookListW=new QListWidget;
    m_pReturnPB=new QPushButton("返回");
    m_pCreateDirPB=new QPushButton("创建文件夹");
    m_pDelDirPB=new QPushButton("删除文件/夹");
    m_pRenamePB=new QPushButton("重命名文件");
    m_pFlushDirPB=new QPushButton("刷新文件夹");
    m_pMoveFilePB=new QPushButton("移动文件");
    m_pSelectPathPB=new QPushButton("放置文件");

    m_pSelectPathPB->setEnabled(false);//在没有选择文件之前先关闭掉选择路径的按钮

    QVBoxLayout *pDirVBL=new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushDirPB);

    m_pUploadPB=new QPushButton("上传文件");
    m_pDownLoadPB=new QPushButton("下载文件");
    m_pDelFilePB=new QPushButton("删除文件");
    m_pShareFilePB=new QPushButton("共享文件");


    QVBoxLayout *pFileVBL=new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    //pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectPathPB);


    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(CreateDir()));
    connect(m_pFlushDirPB,SIGNAL(clicked(bool)),this,SLOT(FlushDir()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(DeleteDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(RenameDir()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(IntoDir()));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(RetDir()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(UploadFile()));
    connect(Timer,SIGNAL(timeout()),this,SLOT(UploadFileData()));
    connect(m_pDownLoadPB,SIGNAL(clicked(bool)),this,SLOT(DownLoadData()));
    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(ShareFile_func()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),this,SLOT(moveFile()));
    connect(m_pSelectPathPB,SIGNAL(clicked(bool)),this,SLOT(SetPathAndSent()));


}

void NetDisk::updateFileList(const PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    m_pBookListW->clear();
    FileInfo *pFileInfo=NULL;
    int iCount=pdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<iCount;i++)
    {
        pFileInfo=(FileInfo *)(pdu->caMsg)+i;
        qDebug()<<pFileInfo->caFileName<<"    "<<pFileInfo->iFileType;
        QListWidgetItem *pItem=new QListWidgetItem;

        if(0==pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/img/dir.jpeg")));
        }
        else
        {
            pItem->setIcon(QIcon(QPixmap(":/img/file.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

NetDisk &NetDisk::getinstance()
{
    static NetDisk instance;
    return instance;
}




void NetDisk::Flush()
{
    this->FlushDir();
}

void NetDisk::setIs_DownLoading(bool status)
{
    this->is_DownLoading=status;
}

void NetDisk::setITotla(qint64 size)
{
    this->iTotal=size;
}

void NetDisk::setIReceved(qint64 size)
{
    this->iReceved=size;
}

QString NetDisk::getSavePath()
{
    return this->SavePath;
}

bool NetDisk::getIsDownLoading()
{
    return this->is_DownLoading;
}

QString NetDisk::getShareFileName()
{
    return this->m_strShareFileName;
}





void NetDisk::CreateDir()
{
    QString newFileName=QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if(!newFileName.isEmpty())
    {
        if(newFileName.toUtf8().size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","文件夹名字过长！");
        }
        else
        {
            QString UserName=TcpClient::getInstance().getLoginName();
            QString CurrPath=TcpClient::getInstance().getCurrentPath();
            PDU *pdu=mkPDU(CurrPath.toUtf8().size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_FILE_REQUEST;
            strncpy(pdu->caData,UserName.toStdString().c_str(),32);
            strncpy(pdu->caData+32,newFileName.toStdString().c_str(),32);
            memcpy(pdu->caMsg,CurrPath.toStdString().c_str(),CurrPath.toUtf8().size());
            TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }

    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","文件夹名字请勿为空！");
    }

}

void NetDisk::FlushDir()
{
    QString curPath=QString("%1").arg(TcpClient::getInstance().getCurrentPath());
    PDU *pdu=mkPDU(curPath.toUtf8().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strcpy((char *)pdu->caMsg,curPath.toStdString().c_str());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void NetDisk::DeleteDir()
{
    QString strCurrentPath=TcpClient::getInstance().getCurrentPath();
    //qDebug()<<"当前文件夹位置："<<strCurrentPath;
    QListWidgetItem *itemCurrent=m_pBookListW->currentItem();
    if(itemCurrent==NULL)
    {
        QMessageBox::warning(this,"删除文件/夹","请选择需要删除的内容");
    }
    else
    {
        QString caName=itemCurrent->text();
        //QString path=QString("%1/%2").arg(strCurrentPath).arg(caName);
        PDU *pdu=mkPDU(strCurrentPath.toUtf8().size());
        pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        strncpy(pdu->caData,caName.toStdString().c_str(),caName.toUtf8().size());
        memcpy(pdu->caMsg,strCurrentPath.toStdString().c_str(),strCurrentPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }
}

void NetDisk::RenameDir()
{
    QString strCurrentPath=TcpClient::getInstance().getCurrentPath();
    QListWidgetItem *itemCurrent=m_pBookListW->currentItem();
    if(itemCurrent==NULL)
    {
        QMessageBox::warning(this,"重命名文件夹","请选择需要重命名的文件夹");
    }
    else
    {
        QString caName=itemCurrent->text();
        PDU *pdu=mkPDU(strCurrentPath.toUtf8().size());
        pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
        QString newName=QInputDialog::getText(this,"重命名文件夹","新名字");
        strncpy(pdu->caData,caName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,newName.toStdString().c_str(),32);
        memcpy(pdu->caMsg,strCurrentPath.toStdString().c_str(),strCurrentPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void NetDisk::IntoDir()
{
    QString strCurrentPath=TcpClient::getInstance().getCurrentPath();
    QListWidgetItem *itemCurrent=m_pBookListW->currentItem();

    if(itemCurrent==NULL)
    {
        QMessageBox::warning(this,"重命名文件夹","请选择需要重命名的文件夹");
    }
    else
    {
        QString caName=itemCurrent->text();
        TcpClient::getInstance().setCurrentChoose(caName);
        PDU *pdu=mkPDU(strCurrentPath.toUtf8().size());
        pdu->uiMsgType=ENUM_MSG_TYPE_INTO_FILE_REQUEST;
        strncpy(pdu->caData,caName.toStdString().c_str(),32);
        memcpy(pdu->caMsg,strCurrentPath.toStdString().c_str(),strCurrentPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void NetDisk::RetDir()
{
    QString CurPath=TcpClient::getInstance().getCurrentPath();
    //qDebug()<<"当前位置："<<CurPath;
    QString TarPath=QString("./UserFile/%1").arg(TcpClient::getInstance().getLoginName());
    //qDebug()<<"目标位置："<<TarPath;
    if(CurPath==TarPath)
    {
        QMessageBox::warning(this,"返回上一级","已到最后一层，无法再返回");
        return;
    }

    PDU *pdu=mkPDU(CurPath.toUtf8().size());
    pdu->uiMsgType=ENUM_MSG_TYPE_RETURN_FILE_REQUEST;
    memcpy(pdu->caMsg,CurPath.toStdString().c_str(),CurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

}

void NetDisk::UploadFile()
{
    QString CurPath=TcpClient::getInstance().getCurrentPath();
    QString FilePath=QFileDialog::getOpenFileName();
    this->OpenFilePath=FilePath;
    if(FilePath.isEmpty())
    {
        QMessageBox::information(this,"选择文件","文件输入不能为空！");
    }
    else
    {
        int index_of_xieGang=FilePath.lastIndexOf("/");
        QString FileName=FilePath.right(FilePath.size()-index_of_xieGang-1);
        //qDebug()<<"得到的文件名称为："<<FileName;

        QFile File(FilePath);
        qint64 FileSize=File.size();//获取文件大小

        PDU *pdu=mkPDU(CurPath.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,CurPath.toStdString().c_str(),CurPath.toUtf8().size());
        sprintf(pdu->caData,"%s %lld",FileName.toStdString().c_str(),FileSize);

        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

        Timer->start(1000);
    }
}

void NetDisk::UploadFileData()
{
    Timer->stop();
    QFile File(OpenFilePath);
    if(!File.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"传输文件","打开文件失败");
        return;
    }

    char *pBuffer=new char[4096];
    qint64 ret=0;
    while(true)
    {
        ret = File.read(pBuffer,4096);
        if(ret<=4096&&ret>0)//传输这个buffer的内容
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }
        else if(ret==0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this,"传输文件","传输文件失败");
        }
    }
    File.close();
    delete []pBuffer;
    pBuffer=NULL;
}

void NetDisk::DownLoadData()
{
    //获取文件名称
    QListWidgetItem *CurItem=m_pBookListW->currentItem();

    if(CurItem==NULL)
    {
        QMessageBox::warning(this,"下载文件","请选择要下载的文件");
        return;
    }
    else
    {
        //获取当前选项
        QString FileName=CurItem->text();
        //设置存储位置
        QString savePath=QFileDialog::getSaveFileName();

        if(savePath.isEmpty())
        {
            QMessageBox::warning(this,"下载文件","请选择保存文件的位置");
            SavePath.clear();//将NetDisk中的位置清零，和上面的那个不是同一个
            return;
        }
        else
        {
            SavePath=savePath;
            TcpClient::getInstance().setSavePath(savePath);
            //QMessageBox::information(this,"下载文件","地址已经存入:"+TcpClient::getInstance().getSavePath());
            //qDebug()<<"NetDisk地址："<<this;
        }



        //获取当前位置
        QString CurPath=TcpClient::getInstance().getCurrentPath();
        PDU *pdu=mkPDU(CurPath.toUtf8().size());

        //装pdu并发送
        strcpy(pdu->caData,FileName.toStdString().c_str());
        memcpy(pdu->caMsg,CurPath.toStdString().c_str(),pdu->uiMsgLen);
        pdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);

        free(pdu);
        pdu=NULL;

    }

    //设置读取状态与存储位置
}

void NetDisk::ShareFile_func()
{
    //获取文件名称
    QListWidgetItem *CurItem=m_pBookListW->currentItem();

    if(CurItem==NULL)
    {
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }
    else
    {
        m_strShareFileName=CurItem->text();
    }


    Friend *pFriend=OpeWidget::getInstance().getFriend();
    QListWidget *pfriendList=pFriend->getFriendList();
    //qDebug()<<"pfriendList="<<pfriendList;

    if(pfriendList->count()==0)
    {
        //qDebug()<<"好友为空";
        //OpeWidget::getInstance().getFriend()->flushFriend();
        //Timer2->start(1000);
        //QThread::sleep(1);
        QMessageBox::warning(this,"好友分享","请先在好友界面获取在线好友信息");
        return;

    }

    ShareFile::getInstance().updateFriend(pfriendList);
    if(ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

void NetDisk::moveFile()
{
    QListWidgetItem *item=m_pBookListW->currentItem();
    if(item!=NULL)
    {
        this->m_strMoveSelectFileName=item->text().toUtf8();
        QString currPath=TcpClient::getInstance().getCurrentPath();
        this->m_strMoveSelectFilePath=currPath+"/"+m_strMoveSelectFileName;

        this->m_pSelectPathPB->setEnabled(true);
        //qDebug()<<"m_strMoveSelectFilePath="<<m_strMoveSelectFilePath;
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}

void NetDisk::SetPathAndSent()
{
    QListWidgetItem *item=m_pBookListW->currentItem();
    int sourcePathSize;//文件源地址的长度
    int destPathSize;//文件目的地址的长度
    if(item!=NULL)
    {
        this->m_strMoveSelectDirName=item->text().toUtf8();
        QString currPath=TcpClient::getInstance().getCurrentPath();
        this->m_strMoveSelectDirPath=currPath+"/"+m_strMoveSelectDirName;
        //qDebug()<<"m_strMoveSelectDirPath="<<m_strMoveSelectDirPath;
    }
    else
    {
        QString currPath=TcpClient::getInstance().getCurrentPath();
        this->m_strMoveSelectDirPath=currPath;
        //qDebug()<<"m_strMoveSelectDirPath="<<m_strMoveSelectDirPath;

    }
    this->m_pSelectPathPB->setEnabled(false);
    sourcePathSize=m_strMoveSelectFilePath.toUtf8().size();
    destPathSize=m_strMoveSelectDirPath.toUtf8().size();

    //qDebug()<<"sourcePathSize="<<sourcePathSize;
    //qDebug()<<"destPathSize="<<destPathSize;
    qDebug()<<"m_strMoveSelectFilePath="<<m_strMoveSelectFilePath;
    qDebug()<<"m_strMoveSelectDirPath="<<m_strMoveSelectDirPath;

    //发送pdu
    PDU *pdu=mkPDU(sourcePathSize+destPathSize+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    sprintf(pdu->caData,"%d %d",sourcePathSize,destPathSize);
    QString  temp=m_strMoveSelectFilePath+m_strMoveSelectDirPath;
    memcpy(pdu->caMsg,temp.toStdString().c_str(),sourcePathSize+destPathSize);
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

}













