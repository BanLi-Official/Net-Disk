#ifndef NETDISK_H
#define NETDISK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include "protocol.h"


class NetDisk : public QWidget
{
    Q_OBJECT
public:
    explicit NetDisk(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    static NetDisk &getinstance();
    void Flush();//调用 FlushDir();刷新文件夹展示

signals:


private slots:
    void CreateDir();//创建一个文件夹
    void FlushDir();//刷新文件夹展示
    void DeleteDir();//删除文件/夹
    void RenameDir();//重命名文件夹





private:
    QListWidget *m_pBookListW; //文件展示界面
    QPushButton *m_pReturnPB;       //返回
    QPushButton *m_pCreateDirPB;  //创建文件夹
    QPushButton *m_pDelDirPB;   //删除文件夹
    QPushButton *m_pRenamePB;   //重命名文件
    QPushButton *m_pFlushDirPB; //刷新文件夹
    QPushButton *m_pUploadPB; //上传文件
    QPushButton *m_pDownLoadPB;  //删除文件
    QPushButton *m_pDelFilePB;   //下载文件
    QPushButton *m_pShareFilePB;   //分享文件


};

#endif // NETDISK_H
