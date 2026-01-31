/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue May 8 16:49:50 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_10;
    QHBoxLayout *horizontalLayout_4;
    QLabel *inputImage;
    QSpacerItem *horizontalSpacer_3;
    QLabel *image0;
    QSpacerItem *horizontalSpacer_2;
    QLabel *image1;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QSlider *c1Lower;
    QSpacerItem *verticalSpacer_5;
    QSlider *c1Upper;
    QSpacerItem *verticalSpacer_4;
    QSlider *c2Lower;
    QSpacerItem *verticalSpacer_6;
    QSlider *c2Upper;
    QSpacerItem *verticalSpacer_7;
    QSlider *c3Lower;
    QSpacerItem *verticalSpacer_8;
    QSlider *c3Upper;
    QSpacerItem *verticalSpacer_9;
    QSlider *colorSelect;
    QSpacerItem *horizontalSpacer_5;
    QLabel *image2;
    QSpacerItem *horizontalSpacer_4;
    QLabel *image3;
    QWidget *tab_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 800);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 1024, 768));
        tabWidget->setMinimumSize(QSize(1000, 600));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayoutWidget = new QWidget(tab);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 991, 547));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_10);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        inputImage = new QLabel(verticalLayoutWidget);
        inputImage->setObjectName(QString::fromUtf8("inputImage"));
        inputImage->setMinimumSize(QSize(320, 240));
        inputImage->setMaximumSize(QSize(320, 240));
        inputImage->setFrameShape(QFrame::Box);
        inputImage->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(inputImage);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        image0 = new QLabel(verticalLayoutWidget);
        image0->setObjectName(QString::fromUtf8("image0"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(image0->sizePolicy().hasHeightForWidth());
        image0->setSizePolicy(sizePolicy);
        image0->setMinimumSize(QSize(320, 240));
        image0->setMaximumSize(QSize(320, 240));
        image0->setFrameShape(QFrame::Box);
        image0->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(image0);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        image1 = new QLabel(verticalLayoutWidget);
        image1->setObjectName(QString::fromUtf8("image1"));
        image1->setMinimumSize(QSize(320, 240));
        image1->setMaximumSize(QSize(320, 240));
        image1->setFrameShape(QFrame::Box);
        image1->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(image1);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        c1Lower = new QSlider(verticalLayoutWidget);
        c1Lower->setObjectName(QString::fromUtf8("c1Lower"));
        c1Lower->setMinimumSize(QSize(320, 20));
        c1Lower->setMaximumSize(QSize(320, 20));
        c1Lower->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c1Lower);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_5);

        c1Upper = new QSlider(verticalLayoutWidget);
        c1Upper->setObjectName(QString::fromUtf8("c1Upper"));
        c1Upper->setMinimumSize(QSize(320, 20));
        c1Upper->setMaximumSize(QSize(320, 20));
        c1Upper->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c1Upper);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_4);

        c2Lower = new QSlider(verticalLayoutWidget);
        c2Lower->setObjectName(QString::fromUtf8("c2Lower"));
        c2Lower->setMinimumSize(QSize(320, 20));
        c2Lower->setMaximumSize(QSize(320, 20));
        c2Lower->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c2Lower);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_6);

        c2Upper = new QSlider(verticalLayoutWidget);
        c2Upper->setObjectName(QString::fromUtf8("c2Upper"));
        c2Upper->setMinimumSize(QSize(320, 20));
        c2Upper->setMaximumSize(QSize(320, 20));
        c2Upper->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c2Upper);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_7);

        c3Lower = new QSlider(verticalLayoutWidget);
        c3Lower->setObjectName(QString::fromUtf8("c3Lower"));
        c3Lower->setMinimumSize(QSize(320, 20));
        c3Lower->setMaximumSize(QSize(320, 20));
        c3Lower->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c3Lower);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_8);

        c3Upper = new QSlider(verticalLayoutWidget);
        c3Upper->setObjectName(QString::fromUtf8("c3Upper"));
        c3Upper->setMinimumSize(QSize(320, 20));
        c3Upper->setMaximumSize(QSize(320, 20));
        c3Upper->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(c3Upper);

        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_9);

        colorSelect = new QSlider(verticalLayoutWidget);
        colorSelect->setObjectName(QString::fromUtf8("colorSelect"));
        colorSelect->setMinimumSize(QSize(320, 20));
        colorSelect->setMaximumSize(QSize(320, 20));
        colorSelect->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(colorSelect);


        horizontalLayout_3->addLayout(verticalLayout_2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_5);

        image2 = new QLabel(verticalLayoutWidget);
        image2->setObjectName(QString::fromUtf8("image2"));
        image2->setMinimumSize(QSize(320, 240));
        image2->setMaximumSize(QSize(320, 240));
        image2->setFrameShape(QFrame::Box);
        image2->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(image2);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        image3 = new QLabel(verticalLayoutWidget);
        image3->setObjectName(QString::fromUtf8("image3"));
        image3->setMinimumSize(QSize(320, 240));
        image3->setMaximumSize(QSize(320, 240));
        image3->setFrameShape(QFrame::Box);
        image3->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(image3);


        verticalLayout->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        inputImage->setText(QApplication::translate("MainWindow", "InputImage", 0, QApplication::UnicodeUTF8));
        image0->setText(QApplication::translate("MainWindow", "Image0", 0, QApplication::UnicodeUTF8));
        image1->setText(QApplication::translate("MainWindow", "Image1", 0, QApplication::UnicodeUTF8));
        image2->setText(QApplication::translate("MainWindow", "Image2", 0, QApplication::UnicodeUTF8));
        image3->setText(QApplication::translate("MainWindow", "Image3", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Tab 2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
