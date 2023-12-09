#ifndef NETDISK_H
#define NETDISK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include "protocol.h"
#include <QFile>


class NetDisk : public QWidget
{
    Q_OBJECT
public:
    explicit NetDisk(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    static NetDisk& getinstance();
    void Flush();//调用 FlushDir();刷新文件夹展示
    void setIs_DownLoading(bool status);//设置状态
    void setITotla(qint64 size);//设置文件大小
    void setIReceved(qint64 size);//设置已接收的文件大小
    QString getSavePath();//获取保存文件的位置
    bool getIsDownLoading();//获取下载状态


    QFile file;  //当前操作的文件

    qint64 iTotal;//文件的总体大小
    qint64 iReceved;//文件接收的大小

signals:


private slots:
    void CreateDir();//创建一个文件夹
    void FlushDir();//刷新文件夹展示
    void DeleteDir();//删除文件/夹
    void RenameDir();//重命名文件夹
    void IntoDir();//进入文件夹
    void RetDir();//返回文件夹上一级
    void UploadFile();//上传文件
    void UploadFileData();//时间到就文件传输
    void DownLoadData();//下载文件
    void ShareFile_func();//分享






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


    QString OpenFilePath;//当前打开文件的地址
    QTimer *Timer;
    QTimer *Timer2;
    QString SavePath;//下载位置

    bool is_DownLoading;//下载状态






};

#endif // NETDISK_H
