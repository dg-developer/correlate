#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QPixmap>
#include "Correlation.h"


namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{

    Q_OBJECT


public:

    // Class functions
    explicit MainWindow(QWidget *parent=0);
    ~MainWindow();
    void formConnections();

    // Correlation functions
    void initializeMainWindow(QApplication *a);


public slots:

    // Slots
    void onC1LowerSliderMoved(int);
    void onC1UpperSliderMoved(int);
    void onC2LowerSliderMoved(int);
    void onC2UpperSliderMoved(int);
    void onC3LowerSliderMoved(int);
    void onC3UpperSliderMoved(int);
    void onColorSpaceSliderMoved(int);
    void updateThresheldImages();


signals:

    // Signals
    void rethreshold();


public:

    // Class helper functions
    void initializeThresholdingTab();
    void initializeCorrelationTab(QApplication *a);

    void parseCommandLine(QStringList commands, int *image_sequence, bool *enable_step, int *enable_debug);
    void loadImageSequence(int image_sequence, string pathNames[], string fileNames[]);

    QImage ipl2QImage(IplImage *img);
    QImage ipl2QImageBW(IplImage *img);



    // Correlation helper functions
//    void initialize(QApplication *a);
//    void initTrainingGui();
//    void initTrainingGui2();
//    void initGui();


public:

    // Class members
    Ui::MainWindow *ui;

    // Correlation members
    //   Notes:
    //     -pathNames[0] is reserved for the path for all output files; change this location to change where output files
    //        are stored
    //     -pathNames[1] is the image source folder for all images
    //     -Currently, the remaining elements of pathNames are not used; in the future, it is straightforward to allow
    //        image sequences accross multiple directories (but why would anyone do this?!?)
    string pathNames[1001];
    string fileNames[1000];
    Correlation c;

    // Raw input image - all subsequent images are the size of ROI
    RgbImage imgInFull;  // imgInFull -> imgIn -> bw -> imgProc

    // Image ROI
    RgbImage imgIn;

    // Reference image
    RgbImage imgRef;

    // Thresheld image
    GreyImage bw;

    // Image after processing
    RgbImage imgProc;

    // Set of images for display
    RgbImage imgInFull0, imgInFull1, imgInFull2, imgInFull3;
    RgbImage imgIn0, imgIn1, imgIn2, imgIn3;
    GreyImage bw0, bw1, bw2, bw3;  // imgInFull0 -> imgIn0 -> bw0 -> qBw0
    QImage qImgIn, qBw0, qBw1, qBw2, qBw3;

    int prevPos;

};


#endif // MAINWINDOW_H
