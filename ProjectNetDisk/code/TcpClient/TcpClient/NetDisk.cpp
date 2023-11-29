#include "NetDisk.h"
#include <QMessageBox>
#include "tcpclient.h"


NetDisk::NetDisk(QWidget *parent)
    : QWidget{parent}
{
    m_pBookListW=new QListWidget;
    m_pReturnPB=new QPushButton("返回");
    m_pCreateDirPB=new QPushButton("创建文件夹");
    m_pDelDirPB=new QPushButton("删除文件夹");
    m_pRenamePB=new QPushButton("重命名文件");
    m_pFlushDirPB=new QPushButton("刷新文件夹");

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
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);


    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(CreateDir()));
    connect(m_pFlushDirPB,SIGNAL(clicked(bool)),this,SLOT(FlushDir()));
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
