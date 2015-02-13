/********************************************************************************
** Form generated from reading UI file 'dlprogress.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLPROGRESS_H
#define UI_DLPROGRESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DLProgress
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labelWatchUrl;
    QHBoxLayout *horizontalLayout_2;
    QProgressBar *progressBar;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QLabel *label;

    void setupUi(QWidget *DLProgress)
    {
        if (DLProgress->objectName().isEmpty())
            DLProgress->setObjectName(QStringLiteral("DLProgress"));
        DLProgress->resize(400, 100);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DLProgress->sizePolicy().hasHeightForWidth());
        DLProgress->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(DLProgress);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelWatchUrl = new QLabel(DLProgress);
        labelWatchUrl->setObjectName(QStringLiteral("labelWatchUrl"));

        verticalLayout->addWidget(labelWatchUrl);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        progressBar = new QProgressBar(DLProgress);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        horizontalLayout_2->addWidget(progressBar);

        pushButton_3 = new QPushButton(DLProgress);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout_2->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(DLProgress);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        horizontalLayout_2->addWidget(pushButton_4);


        verticalLayout->addLayout(horizontalLayout_2);

        label = new QLabel(DLProgress);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(DLProgress);

        QMetaObject::connectSlotsByName(DLProgress);
    } // setupUi

    void retranslateUi(QWidget *DLProgress)
    {
        DLProgress->setWindowTitle(QApplication::translate("DLProgress", "Form", 0));
        labelWatchUrl->setText(QApplication::translate("DLProgress", "lblWatchUrl", 0));
        pushButton_3->setText(QApplication::translate("DLProgress", "PushButton", 0));
        pushButton_4->setText(QApplication::translate("DLProgress", "PushButton", 0));
        label->setText(QApplication::translate("DLProgress", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class DLProgress: public Ui_DLProgress {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLPROGRESS_H
