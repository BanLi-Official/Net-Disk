#include "tcpclient.h"
#include "opewidget.h"
#include <QApplication>
#include "online.h"
#include "friend.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpClient::getInstance().show();//在这里TcpClient使用了单例模式


    return a.exec();
}





