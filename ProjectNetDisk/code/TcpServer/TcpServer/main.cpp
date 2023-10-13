#include "tcpserver.h"

#include <QApplication>

#include "opedb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OpeDB::getInstance().init();//初始化服务器端的数据库

    TcpServer w;
    w.show();
    return a.exec();
}
