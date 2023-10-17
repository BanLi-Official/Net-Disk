/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Online
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QListWidget *online_listwidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *addNewFriend_pb;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *Online)
    {
        if (Online->objectName().isEmpty())
            Online->setObjectName("Online");
        Online->resize(616, 428);
        horizontalLayout_2 = new QHBoxLayout(Online);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        online_listwidget = new QListWidget(Online);
        new QListWidgetItem(online_listwidget);
        new QListWidgetItem(online_listwidget);
        new QListWidgetItem(online_listwidget);
        online_listwidget->setObjectName("online_listwidget");

        horizontalLayout->addWidget(online_listwidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        addNewFriend_pb = new QPushButton(Online);
        addNewFriend_pb->setObjectName("addNewFriend_pb");
        QFont font;
        font.setPointSize(16);
        addNewFriend_pb->setFont(font);

        verticalLayout->addWidget(addNewFriend_pb);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout);


        horizontalLayout_2->addLayout(horizontalLayout);


        retranslateUi(Online);

        QMetaObject::connectSlotsByName(Online);
    } // setupUi

    void retranslateUi(QWidget *Online)
    {
        Online->setWindowTitle(QCoreApplication::translate("Online", "Form", nullptr));

        const bool __sortingEnabled = online_listwidget->isSortingEnabled();
        online_listwidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = online_listwidget->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("Online", "1111", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = online_listwidget->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("Online", "111111", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = online_listwidget->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("Online", "2222", nullptr));
        online_listwidget->setSortingEnabled(__sortingEnabled);

        addNewFriend_pb->setText(QCoreApplication::translate("Online", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Online: public Ui_Online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H
