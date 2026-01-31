/********************************************************************************
** Form generated from reading UI file 'thresholdingtab.ui'
**
** Created: Sat May 5 15:08:16 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THRESHOLDINGTAB_H
#define UI_THRESHOLDINGTAB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ThresholdingTab
{
public:
    QWidget *tab;
    QWidget *tab1;

    void setupUi(QTabWidget *ThresholdingTab)
    {
        if (ThresholdingTab->objectName().isEmpty())
            ThresholdingTab->setObjectName(QString::fromUtf8("ThresholdingTab"));
        ThresholdingTab->resize(400, 300);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        ThresholdingTab->addTab(tab, QString());
        tab1 = new QWidget();
        tab1->setObjectName(QString::fromUtf8("tab1"));
        ThresholdingTab->addTab(tab1, QString());

        retranslateUi(ThresholdingTab);

        ThresholdingTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ThresholdingTab);
    } // setupUi

    void retranslateUi(QTabWidget *ThresholdingTab)
    {
        ThresholdingTab->setWindowTitle(QApplication::translate("ThresholdingTab", "TabWidget", 0, QApplication::UnicodeUTF8));
        ThresholdingTab->setTabText(ThresholdingTab->indexOf(tab), QApplication::translate("ThresholdingTab", "Tab 1", 0, QApplication::UnicodeUTF8));
        ThresholdingTab->setTabText(ThresholdingTab->indexOf(tab1), QApplication::translate("ThresholdingTab", "Tab 2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ThresholdingTab: public Ui_ThresholdingTab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THRESHOLDINGTAB_H
