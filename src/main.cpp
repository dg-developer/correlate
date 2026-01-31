//#include <QtCore/QCoreApplication>
//#include <QImage>
//#include <QColor>
//#include <QString>
//#include <stdio.h>
//#include <iostream>
//#include <iomanip>
//#include <fstream>
//#include <cmath>
//#include <math.h>
//#include <stdlib.h>
//#include <QApplication>
//#include <QtCore>
//#include <string>


#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QDebug>




int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MainWindow w;
    w.formConnections();
    w.initializeMainWindow(&a);
    w.show();

    return a.exec();
}



