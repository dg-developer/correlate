#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Class initialization
    ui->setupUi(this);

    // Connect signals and slots
//    formConnections();
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::formConnections() {
    connect(ui->c1Lower, SIGNAL(sliderMoved(int)), this, SLOT(onC1LowerSliderMoved(int)));
    connect(ui->c1Upper, SIGNAL(sliderMoved(int)), this, SLOT(onC1UpperSliderMoved(int)));
    connect(ui->c2Lower, SIGNAL(sliderMoved(int)), this, SLOT(onC2LowerSliderMoved(int)));
    connect(ui->c2Upper, SIGNAL(sliderMoved(int)), this, SLOT(onC2UpperSliderMoved(int)));
    connect(ui->c3Lower, SIGNAL(sliderMoved(int)), this, SLOT(onC3LowerSliderMoved(int)));
    connect(ui->c3Upper, SIGNAL(sliderMoved(int)), this, SLOT(onC3UpperSliderMoved(int)));
    connect(this, SIGNAL(rethreshold()), this, SLOT(updateThresheldImages()));
}


void MainWindow::initializeMainWindow(QApplication *a) {

    // Handle inputs from the command line
    QStringList commands = a->arguments();
    parseCommandLine(commands, &(c.image_sequence), &(c.enable_step), &(c.enable_debug));

    // Determine image sequence
    loadImageSequence(c.image_sequence, pathNames, fileNames);

    // Initialize correlation object
    c.initObject();

    // Initialize tabs
    initializeThresholdingTab();
//    initializeCorrelationTab(a);
}


void MainWindow::onC1LowerSliderMoved(int val) {

    // Store updated value
    cout << "value = " << val << endl;
    c.R_lower = val;

    // Threshold image
    emit rethreshold();
}


void MainWindow::onC1UpperSliderMoved(int val) {
    c.R_upper = val;
    emit rethreshold();
}


void MainWindow::onC2LowerSliderMoved(int val) {
    c.G_lower = val;
    emit rethreshold();
}


void MainWindow::onC2UpperSliderMoved(int val) {
   c.G_upper = val;
   emit rethreshold();
}


void MainWindow::onC3LowerSliderMoved(int val) {
   c.B_lower = val;
   emit rethreshold();
}


void MainWindow::onC3UpperSliderMoved(int val) {
    c.B_upper = val;
    emit rethreshold();
}


void MainWindow::onColorSpaceSliderMoved(int val) {
}


void MainWindow::updateThresheldImages() {

    // Extract ROI
    c.subsetImage(imgInFull, imgIn);
    c.subsetImage(imgInFull0, imgIn0);
    c.subsetImage(imgInFull1, imgIn1);
    c.subsetImage(imgInFull2, imgIn2);
    c.subsetImage(imgInFull3, imgIn3);

    // Filter (currently based on static thresholding)
    c.colorFilter(imgIn0.imgp, &bw0.imgp);
    c.colorFilter(imgIn1.imgp, &bw1.imgp);
    c.colorFilter(imgIn2.imgp, &bw2.imgp);
    c.colorFilter(imgIn3.imgp, &bw3.imgp);

    /*
//     Run blob analysis
//    c.filterImage(imgThresh0_ROI.imgp);
//    c.blobAnalysis();
//    c.blobProbability();
//    c.identifyDots();
//    c.outputImage(false, true);
//    cvShowImage("Image 1", c.imgProc.imgp);
*/

    // Get qImages
    qImgIn = ipl2QImage(imgIn.imgp);
    qBw0 = ipl2QImageBW(bw0.imgp);
    qBw1 = ipl2QImageBW(bw1.imgp);
    qBw2 = ipl2QImageBW(bw2.imgp);
    qBw3 = ipl2QImageBW(bw3.imgp);

    // Show images
    cout << "updating displays" << endl;
    ui->inputImage->setPixmap(QPixmap::fromImage(qImgIn));
    ui->image0->setPixmap(QPixmap::fromImage(qBw0));
    ui->image1->setPixmap(QPixmap::fromImage(qBw1));
    ui->image2->setPixmap(QPixmap::fromImage(qBw2));
    ui->image3->setPixmap(QPixmap::fromImage(qBw3));

    ui->inputImage->setScaledContents(true);
    ui->image0->setScaledContents(true);
    ui->image1->setScaledContents(true);
    ui->image2->setScaledContents(true);
    ui->image3->setScaledContents(true);
}


void MainWindow::initializeThresholdingTab() {

    // Determine image sequence
    //loadImageSequence
    //changeImageDepth()

    // Display first image
    emit rethreshold();


//    onMouse(CV_EVENT_LBUTTONUP, c.ROI.width + c.ROI.x, c.ROI.height + c.ROI.y, 0, 0);  // This draws a rect;
//                                                                                       // does not mod ROI
//    cvShowImage("Input Image", imgInFull);

}


//void initializeCorrelationTab(QApplication *a) {

//}


/*
  Parse input commands from the command-line
  */
void MainWindow::parseCommandLine(QStringList commands, int *image_sequence, bool *enable_step, int *enable_debug) {

    // Parse command-line input
    for(int i = 0; i < commands.size(); i++) {
        QString command_i = commands.at(i).toLower();
        if( QString::compare(command_i.left(4), "-is=") == 0 ) {
            *image_sequence = command_i.right( command_i.size() - 4 ).toInt();
        }
        if( QString::compare(command_i, "--step") == 0 ) {
            *enable_step = true;
        }
        if( QString::compare(command_i, "--enable-debug") == 0 ) {
            *enable_debug = 1;
        }
        if( QString::compare(command_i, "--enable-debug2") == 0 ) {
            *enable_debug = 2;
        }
    }

    return;
}


/*
  Load path locations and file names for the image sequence.

  @output pathNames =
  @output imgInFull = full input image for correlation process
  @output imgInFull0, imgInFull1, imgInFull2, imgInFull3 = full input images for thresholding tab
  */
void MainWindow::loadImageSequence(int image_sequence, string pathNames[], string fileNames[]) {

    // Initialize
    pathNames[0] = "/home/david/Documents/correlate/data/";
    int whichTest = 0;
    for(int i = 0; i < 1000; ++i) {
        fileNames[i] = "";
    }

    // Switch image sequence
    if(image_sequence == 1)
    {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/";
        fileNames[0] = "IMG_3907.jpg";
        fileNames[1] = "IMG_3907_shifted.jpg";
    } else if(image_sequence == 2) {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/6_17_11_compact_tension/";
        fileNames[0] = "IMG_4100.jpg";
        fileNames[1] = "IMG_4101.jpg";
        fileNames[2] = "IMG_4102.jpg";
        fileNames[3] = "IMG_4103.jpg";
        fileNames[4] = "IMG_4104.jpg";
        fileNames[5] = "IMG_4105.jpg";
        fileNames[6] = "IMG_4106.jpg";
        fileNames[7] = "IMG_4107.jpg";
        fileNames[8] = "IMG_4108.jpg";
        fileNames[9] = "IMG_4109.jpg";
        fileNames[10] = "IMG_4110.jpg";
        fileNames[11] = "IMG_4111.jpg";
        fileNames[12] = "IMG_4112.jpg";
        fileNames[13] = "IMG_4113.jpg";
        fileNames[14] = "IMG_4114.jpg";
        fileNames[15] = "IMG_4115.jpg";
        fileNames[16] = "IMG_4116.jpg";
        fileNames[17] = "IMG_4117.jpg";
        fileNames[18] = "IMG_4118.jpg";
        fileNames[19] = "IMG_4119.jpg";
        fileNames[20] = "IMG_4120.jpg";
        fileNames[21] = "IMG_4121.jpg";
        fileNames[22] = "IMG_4122.jpg";
        fileNames[23] = "IMG_4123.jpg";
        fileNames[24] = "IMG_4124.jpg";
        fileNames[25] = "IMG_4125.jpg";
        fileNames[26] = "IMG_4126.jpg";
        fileNames[27] = "IMG_4127.jpg";
        fileNames[28] = "IMG_4128.jpg";
    } else if(image_sequence == 3) {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/6_17_11_compact_tension/";
        fileNames[0] = "IMG_4125.jpg";
        fileNames[1] = "IMG_4126.jpg";
        fileNames[2] = "IMG_4127.jpg";
    } else if(image_sequence == 4) {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/8_11_11_sinusoidal_test_1/";
        fileNames[0] = "IMG_4601.JPG";
        fileNames[1] = "IMG_4602.JPG";
        fileNames[2] = "IMG_4603.JPG";
        fileNames[3] = "IMG_4604.JPG";
        fileNames[4] = "IMG_4605.JPG";
        fileNames[5] = "IMG_4606.JPG";
        fileNames[6] = "IMG_4607.JPG";
        fileNames[7] = "IMG_4608.JPG";
        fileNames[8] = "IMG_4609.JPG";
        fileNames[9] = "IMG_4610.JPG";
        fileNames[10] = "IMG_4611.JPG";
        fileNames[11] = "IMG_4612.JPG";
        fileNames[12] = "IMG_4613.JPG";
        fileNames[13] = "IMG_4614.JPG";
        fileNames[14] = "IMG_4615.JPG";
        fileNames[15] = "IMG_4616.JPG";
        fileNames[16] = "IMG_4617.JPG";
        fileNames[17] = "IMG_4618.JPG";
        fileNames[18] = "IMG_4619.JPG";
        fileNames[19] = "IMG_4620.JPG";
        fileNames[20] = "IMG_4621.JPG";
        fileNames[21] = "IMG_4622.JPG";
        fileNames[22] = "IMG_4623.JPG";
        fileNames[23] = "IMG_4624.JPG";
    } else if(image_sequence == 5) {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/8_11_11_break/";
        fileNames[0] = "IMG_4702.JPG";
        fileNames[1] = "IMG_4703.JPG";
        fileNames[2] = "IMG_4704.JPG";
        fileNames[3] = "IMG_4705.JPG";
        fileNames[4] = "IMG_4706.JPG";
        fileNames[5] = "IMG_4707.JPG";
        fileNames[6] = "IMG_4708.JPG";
        fileNames[7] = "IMG_4709.JPG";
        fileNames[8] = "IMG_4710.JPG";
        fileNames[9] = "IMG_4711.JPG";
        fileNames[10] = "IMG_4712.JPG";
        fileNames[11] = "IMG_4713.JPG";
        fileNames[12] = "IMG_4714.JPG";
        fileNames[13] = "IMG_4715.JPG";
        fileNames[14] = "IMG_4716.JPG";
        fileNames[15] = "IMG_4717.JPG";
        fileNames[16] = "IMG_4718.JPG";
        fileNames[17] = "IMG_4719.JPG";
        fileNames[18] = "IMG_4720.JPG";
        fileNames[19] = "IMG_4721.JPG";
        fileNames[20] = "IMG_4722.JPG";
    } else if(image_sequence == 6) {
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/6_17_11_compact_tension/";
        fileNames[0] = "IMG_4108.jpg";
        fileNames[1] = "IMG_4109.jpg";
        fileNames[2] = "IMG_4110.jpg";
        fileNames[3] = "IMG_4111.jpg";
        fileNames[4] = "IMG_4112.jpg";
        fileNames[5] = "IMG_4113.jpg";
        fileNames[6] = "IMG_4114.jpg";
        fileNames[7] = "IMG_4115.jpg";
        fileNames[8] = "IMG_4116.jpg";
        fileNames[9] = "IMG_4117.jpg";
        fileNames[10] = "IMG_4118.jpg";
        fileNames[11] = "IMG_4119.jpg";
        fileNames[12] = "IMG_4120.jpg";
        fileNames[13] = "IMG_4121.jpg";
        fileNames[14] = "IMG_4122.jpg";
        fileNames[15] = "IMG_4123.jpg";
        fileNames[16] = "IMG_4124.jpg";
        fileNames[17] = "IMG_4125.jpg";
        fileNames[18] = "IMG_4126.jpg";
        fileNames[19] = "IMG_4127.jpg";
        fileNames[20] = "IMG_4128.jpg";
    } else if(image_sequence >= 100) {

        // Load image sequences used specifically as test cases.
        //   Each test sequence is defined by a number, beginning with 100 and continuing
        whichTest = image_sequence - 100;

        pathNames[1] = "/home/david/Documents/test-correlate/unittestdata/";

        char fCharArray[40];
        for(int i = 0; i < 1000; ++i) {

            // Write the maximum number of file names which could exist. The algorithms will fail gracefully for
            //   non-existent files.
            fileNames[i] = "";
            sprintf(fCharArray, "test%uplt%u.bmp", whichTest, i);
            fileNames[i].append(fCharArray);
        }

    } else {
        // Default image sequence
        pathNames[1] = "/home/david/Documents/test-correlate-extra/testimages/6_17_11_compact_tension/";
        fileNames[0] = "IMG_4071.jpg";
        fileNames[1] = "IMG_4072.jpg";
        fileNames[2] = "IMG_4073.jpg";
        fileNames[3] = "IMG_4074.jpg";
        fileNames[4] = "IMG_4075.jpg";
        fileNames[5] = "IMG_4076.jpg";
        fileNames[6] = "IMG_4077.jpg";
        fileNames[7] = "IMG_4078.jpg";
        fileNames[8] = "IMG_4079.jpg";
        fileNames[9] = "IMG_4080.jpg";
        fileNames[10] = "IMG_4081.jpg";
        fileNames[11] = "IMG_4082.jpg";
    }

    // Load subset of images for thresholding procedure
    string sourcePathName = pathNames[1];
    string fileName = fileNames[0];
    string file = sourcePathName.append(fileName);
    imgInFull.imgp = cvLoadImage(file.c_str());

    // Determine number of images in sequence
    int s = -1;
    while(true) {

        ++s;

        // Attempt to open fileNames
        sourcePathName = pathNames[1];
        fileName = fileNames[(int)s];
        file = sourcePathName.append(fileName);

        imgInFull3.imgp = cvLoadImage(file.c_str());
        if( fileNames[s].empty() || !imgInFull3.imgp ) {
            break;
        }
    }
    s--;

    // Initialize ROI to a good starting point
    c.ROI.x = 0.15*imgInFull.imgp->width;
    c.ROI.y = 0.15*imgInFull.imgp->height;
    c.ROI.width = 0.70*imgInFull.imgp->width;
    c.ROI.height = 0.70*imgInFull.imgp->height;

    sourcePathName = pathNames[1];
    fileName = fileNames[0];
    file = sourcePathName.append(fileName);
    imgInFull0.imgp = cvLoadImage(file.c_str());

    sourcePathName = pathNames[1];
    fileName = fileNames[(int)s/3];
    file = sourcePathName.append(fileName);
    imgInFull1.imgp = cvLoadImage(file.c_str());

    sourcePathName = pathNames[1];
    fileName = fileNames[(int)2*s/3];
    file = sourcePathName.append(fileName);
    imgInFull2.imgp = cvLoadImage(file.c_str());

    sourcePathName = pathNames[1];
    fileName = fileNames[(int)s];
    file = sourcePathName.append(fileName);
    imgInFull3.imgp = cvLoadImage(file.c_str());

    cout << "loadImageSequence " << imgInFull.imgp->width << endl;
    cout << "loadImageSequence " << imgInFull0.imgp->width << endl;
    cout << "loadImageSequence " << imgInFull1.imgp->width << endl;
    cout << "loadImageSequence " << imgInFull2.imgp->width << endl;
    cout << "loadImageSequence " << imgInFull3.imgp->width << endl;

    return;
}


QImage MainWindow::ipl2QImage(IplImage *img) {
cout << "ipl2QImage " << img->depth << endl;
    RgbImage imgWrapper(img);
    QImage qImg(img->width, img->height, QImage::Format_RGB32);
    int i, j;
    QRgb col;
    for (i = 0; i < qImg.height(); ++i)
    {
        for (j = 0; j < qImg.width(); ++j)
        {
            col = qRgb( (int)(imgWrapper[i][j].c3), (int)(imgWrapper[i][j].c2), (int)(imgWrapper[i][j].c1) );
            qImg.setPixel(j, i, col);
        }
    }

    return qImg;
}


QImage MainWindow::ipl2QImageBW(IplImage *img) {

    GreyImage imgWrapper(img);
    QImage qImg(img->width, img->height, QImage::Format_RGB32);
    int i, j;
    QRgb col;
    for (i = 0; i < qImg.height(); ++i)
    {
        for (j = 0; j < qImg.width(); ++j)
        {
            col = qRgb( (int)(imgWrapper[i][j]), (int)(imgWrapper[i][j]), (int)(imgWrapper[i][j]) );
            qImg.setPixel(j, i, col);
        }
    }

    return qImg;
}


//void MainWindow::initTrainingGui();
//void MainWindow::initTrainingGui2();
//void MainWindow::initGui();













//*******************************************************









//void MainWindow::updateThresheldImages() {

//    // Threshold
//    c.filterImage(imgThresh0_ROI.imgp);
//    c.blobAnalysis();
//    c.blobProbability();
//    c.identifyDots();
//    c.outputImage(false, true);
//    cvShowImage("Image 1", c.imgProc.imgp);

//    imgThresh1_ROI = c.subsetImage(imgThresh1);
//    c.filterImage(imgThresh1_ROI.imgp);
//    c.blobAnalysis();
//    c.blobProbability();
//    c.identifyDots();
//    c.outputImage(false, true);
//    cvShowImage("Image 2", c.imgProc.imgp);

//    imgThresh2_ROI = c.subsetImage(imgThresh2);
//    c.filterImage(imgThresh2_ROI.imgp);
//    c.blobAnalysis();
//    c.blobProbability();
//    c.identifyDots();
//    c.outputImage(false, true);
//    cvShowImage("Image 3", c.imgProc.imgp);

//    imgThresh3_ROI = c.subsetImage(imgThresh3);
//    c.filterImage(imgThresh3_ROI.imgp);
//    c.blobAnalysis();
//    c.blobProbability();
//    c.identifyDots();
//    c.outputImage(false, true);
//    cvShowImage("Image 4", c.imgProc.imgp);



//    // Show images
//    qImgThresh0_ROI = ipl2QImage(imgThresh0_ROI.imgp);
//    qImgThresh1_ROI = ipl2QImage(imgThresh1_ROI.imgp);
//    qImgThresh2_ROI = ipl2QImage(imgThresh2_ROI.imgp);
//    qImgThresh3_ROI = ipl2QImage(imgThresh3_ROI.imgp);
//    ui->image0->setPixmap(QPixmap::fromImage(qImgThresh0_ROI));
//    ui->image1->setPixmap(QPixmap::fromImage(qImgThresh1_ROI));
//    ui->image2->setPixmap(QPixmap::fromImage(qImgThresh2_ROI));
//    ui->image3->setPixmap(QPixmap::fromImage(qImgThresh3_ROI));
//}
























///*
//  This function handles the file and user input and output.

//  This function calls the code which does analysis as needed. All analytical code exists as structured functions in
//    the correlation class. All data needed to analyze an image and compute the strain field are also managed by the
//    correlation class.
//  */
//void MainWindow::runCode(QApplication *a) {

//    // Declare variables
//    //   Notes:
//    //     -pathNames[0] is reserved for the path for all output files; change this location to change where output files
//    //        are stored
//    //     -pathNames[1] is the image source folder for all images
//    //     -Currently, the remaining elements of pathNames are not used; in the future, it is straightforward to allow
//    //        image sequences accross multiple directories (but why would anyone do this?!?)
//    string pathNames[1001];
//    string fileNames[1000];


//    // Determine image sequence
//    loadImageSequence(c.image_sequence, pathNames, fileNames);


//    // Initialize correlation object
//    c.initObject();

//    /* ----- Parameter Thresholding ------------------------------------------------------------------------------*/

//    // Loop through the two steps of training: (1) Color thresholding, and (2) Geometric thresholding
//    for(int i = 0; i < 2; ++i) {

//        if(i == 0) {

//            /* ----- Color Thresholding --------------------------------------------------------------------------*/

//            // Initialize GUI output
//            initTrainingGui();

//            // Display first image
//            imgInFull = cvCloneImage(c.imgInFull.imgp);
//            onMouse(CV_EVENT_LBUTTONUP, c.ROI.width + c.ROI.x, c.ROI.height + c.ROI.y, 0, 0);  // This draws a rect;
//                                                                                               // does not mod ROI
//            cvShowImage("Input Image", imgInFull);

//        } else if(i == 1) {

//            /* ----- Blob Thresholding ---------------------------------------------------------------------------*/

//            // Partially re-initialize the GUI for blob thresholding
//            initTrainingGui2();

//            // Display first image
//            imgInFull = cvCloneImage(c.imgInFull.imgp);
//            cvShowImage("Input Image", imgInFull);

//        }

//        // Allow user to adjust color thresholds to correctly extract the blobs
//        int counter = 0;
//        int key = -1;
//        while(key == -1) {

//            // Run code which thresholds the image
//            imgThresh0_ROI = c.subsetImage(imgThresh0);
//            c.filterImage(imgThresh0_ROI.imgp);
//            c.blobAnalysis();
//            c.blobProbability();
//            c.identifyDots();
//            c.outputImage(false, true);
//            cvShowImage("Image 1", c.imgProc.imgp);

//            imgThresh1_ROI = c.subsetImage(imgThresh1);
//            c.filterImage(imgThresh1_ROI.imgp);
//            c.blobAnalysis();
//            c.blobProbability();
//            c.identifyDots();
//            c.outputImage(false, true);
//            cvShowImage("Image 2", c.imgProc.imgp);

//            imgThresh2_ROI = c.subsetImage(imgThresh2);
//            c.filterImage(imgThresh2_ROI.imgp);
//            c.blobAnalysis();
//            c.blobProbability();
//            c.identifyDots();
//            c.outputImage(false, true);
//            cvShowImage("Image 3", c.imgProc.imgp);

//            imgThresh3_ROI = c.subsetImage(imgThresh3);
//            c.filterImage(imgThresh3_ROI.imgp);
//            c.blobAnalysis();
//            c.blobProbability();
//            c.identifyDots();
//            c.outputImage(false, true);
//            cvShowImage("Image 4", c.imgProc.imgp);

//            // Wait for a keypress
//            key = cvWaitKey(100);
//        }

//    }

//    // Terminate process of obtaining persistent parameters
//    cvReleaseImage(&imgThresh0.imgp);
//    cvReleaseImage(&imgThresh1.imgp);
//    cvReleaseImage(&imgThresh2.imgp);
//    cvReleaseImage(&imgThresh3.imgp);
//    cvReleaseImage(&imgThresh0_ROI.imgp);
//    cvReleaseImage(&imgThresh1_ROI.imgp);
//    cvReleaseImage(&imgThresh2_ROI.imgp);
//    cvReleaseImage(&imgThresh3_ROI.imgp);
//    c.savePersistentValues();


//    /* ----- Analyze Image Sequence ------------------------------------------------------------------------------*/

//    // Initialize GUI
//    initGui();


//    // Call analyzeImage() for the reference image; store reference position field
//    c.analyzeImage(pathNames[0], pathNames[1], fileNames[0], true);
//    c.storePositionField( pathNames[0], true );
//    c.storeStrainField(pathNames[0], true);


//    // If step, debug, or debug2 are enabled, output image
//    if(c.enable_step || c.enable_debug > 0) {
//        c.outputImage(true);
//        c.outputOverlay(true);
//    }


//    // Recursively call analyzeImage on each image; store displacement field (relative to reference position field)
//    int i = 1;  // Begin with the second image
//    while(fileNames[i] != "")
//    {
//        bool result = c.analyzeImage(pathNames[0], pathNames[1], fileNames[i], false);
//        if( !result ) {
//            ++i;
//            continue;
//        }
//        c.storePositionField(pathNames[0], false);
//        c.storeStrainField(pathNames[0], false);

//        // Update image and pause so user can see analysis process
//        if(c.enable_step || c.enable_debug == 1) {
//            c.outputImage(true);
//            c.outputOverlay(true);
//        }
//        if(c.enable_debug > 1) {
//            c.outputImage(true);
//            c.outputOverlay(true);
//            std::cout << "Processing " << fileNames[i] << endl;
//            std::cout << endl;
//            std::cout << "Press any key > ";
//            std::cout << endl;
//        }

//        ++i;
//    }


//    // Output image for display
//    c.outputImage(true);
//    c.outputOverlay(true);

//    cout << "Program finished." << endl;
//    return;
//}
























































//void MainWindow::initialize(QApplication *a) {

//    // Member initialization
//    prevPos = 0;

//    // Handle inputs from the command line
//    QStringList commands = a->arguments();
//    parseCommandLine(commands, &(c.image_sequence), &(c.enable_step), &(c.enable_debug));
//}















///*
//  Initialize GUI for the "training" or "thresholding" procedure
//  */
//void MainWindow::initTrainingGui() {

//    // Make windows
//    cvNamedWindow("Input Image", CV_WINDOW_NORMAL);
//    cvNamedWindow("Image 1", CV_WINDOW_NORMAL);
//    cvNamedWindow("Image 2", CV_WINDOW_NORMAL);
//    cvNamedWindow("Image 3", CV_WINDOW_NORMAL);
//    cvNamedWindow("Image 4", CV_WINDOW_NORMAL);

//    // Resize windows
//    int screenWidth(1280), screenHeight(800);
//    cvResizeWindow("Input Image", screenWidth/3, screenHeight);
//    cvResizeWindow("Image 1", screenWidth/3, screenHeight/2);
//    cvResizeWindow("Image 2", screenWidth/3, screenHeight/2);
//    cvResizeWindow("Image 3", screenWidth/3, screenHeight/2);
//    cvResizeWindow("Image 4", screenWidth/3, screenHeight/2);

//    // Move windows
//    cvMoveWindow("Input Image", 0, 0);
//    cvMoveWindow("Image 1", screenWidth/3, 0);
//    cvMoveWindow("Image 2", 2*screenWidth/3, 0);
//    cvMoveWindow("Image 3", screenWidth/3, screenHeight/2);
//    cvMoveWindow("Image 4", 2*screenWidth/3, screenHeight/2);

//    // Create trackbars
//    cvCreateTrackbar("C1 lower", "Input Image", &(c.R_lower), 256, trackbarHandler);
//    cvCreateTrackbar("C1 Upper", "Input Image", &(c.R_upper), 256, trackbarHandler);
//    cvCreateTrackbar("C2 Lower", "Input Image", &(c.G_lower), 256, trackbarHandler);
//    cvCreateTrackbar("C2 Upper", "Input Image", &(c.G_upper), 256, trackbarHandler);
//    cvCreateTrackbar("C3 Lower", "Input Image", &(c.B_lower), 256, trackbarHandler);
//    cvCreateTrackbar("C3 Upper", "Input Image", &(c.B_upper), 256, trackbarHandler);
//    cvCreateTrackbar("Color Space", "Input Image", &c.colorSpace, 1, trackbarHandlerSpecial);

//    // Set mouse callback
//    cvSetMouseCallback("Input Image", onMouse, 0);

//    return;
//}


///*
//  Initialize GUI for modifying internal parameters which have to do with dot identification based on dot geometry
//  */
//void MainWindow::initTrainingGui2() {

//    // Destroy and regenerate window
//    cvDestroyWindow("Input Image");
//    cvNamedWindow("Input Image", CV_WINDOW_NORMAL);

//    // Resize and move window
//    int screenWidth(1280), screenHeight(800);
//    cvResizeWindow("Input Image", screenWidth/3, screenHeight);
//    cvMoveWindow("Input Image", 0, 0);

//    // Create trackbars for blob thresholding
//    cvCreateTrackbar("Static Migration Threshold", "Input Image", &(c.blob_dist_thresh), 100, trackbarHandler);
//    cvCreateTrackbar("Area Threshold", "Input Image", &(c.area_thresh), 500, trackbarHandler);

//    return;
//}


///*
//  Initialize GUI for normal code execution
//  */
//void MainWindow::initGui() {

//    // Initialize
//    int screenWidth(1280), screenHeight(800);

//    // Make windows
//    cvNamedWindow("Processed Image", CV_WINDOW_NORMAL);
//    cvNamedWindow("XX Strain Field Overlay", CV_WINDOW_NORMAL);
//    cvNamedWindow("XY Strain Field Overlay", CV_WINDOW_NORMAL);
//    cvNamedWindow("YY Strain Field Overlay", CV_WINDOW_NORMAL);

//    // Resize windows
//    cvResizeWindow("Processed Image", screenWidth/3, screenHeight/2);
//    cvResizeWindow("XX Strain Field Overlay", screenWidth/3, screenHeight/3);
//    cvResizeWindow("XY Strain Field Overlay", screenWidth/3, screenHeight/3);
//    cvResizeWindow("YY Strain Field Overlay", screenWidth/3, screenHeight/3);

//    // Move windows
//    cvMoveWindow("Processed Image", 1*screenWidth/6, screenHeight/4);
//    cvMoveWindow("XX Strain Field Overlay", screenWidth/2, 0);
//    cvMoveWindow("XY Strain Field Overlay", screenWidth/2, screenHeight/3);
//    cvMoveWindow("YY Strain Field Overlay", screenWidth/2, 2*screenHeight/3);

//    return;
//}


///*
//  Trackbar handler
//  */
//void trackbarHandler(int pos)
//{
//    return;
//}


///*
//  Special trackbar handler for the trackbar which selects the color thresholds
//  */
//void trackbarHandlerSpecial(int pos) {

////    Correlation *cc = &(MainWindow::c);
////    int *prevPos = &(MainWindow::prevPos);

////    // Change color spaces if requested
////    if(pos != *prevPos) {

////        if(pos == 0) {

////            // Hold threshold values currently associated with trackbars
////            int c1LProxy = cc->H_lower;
////            int c1UProxy = cc->H_upper;
////            int c2LProxy = cc->S_lower;
////            int c2UProxy = cc->S_upper;
////            int c3LProxy = cc->V_lower;
////            int c3UProxy = cc->V_upper;

////            // Re-generate the trackbars, linking them with different pointers
////            cvCreateTrackbar("C1 lower", "Input Image", &(cc->R_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C1 Upper", "Input Image", &(cc->R_upper), 256, trackbarHandler);
////            cvCreateTrackbar("C2 Lower", "Input Image", &(cc->G_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C2 Upper", "Input Image", &(cc->G_upper), 256, trackbarHandler);
////            cvCreateTrackbar("C3 Lower", "Input Image", &(cc->B_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C3 Upper", "Input Image", &(cc->B_upper), 256, trackbarHandler);

////            // Set the held values into the object array
////            cc->H_lower = c1LProxy;
////            cc->H_upper = c1UProxy;
////            cc->S_lower = c2LProxy;
////            cc->S_upper = c2UProxy;
////            cc->V_lower = c3LProxy;
////            cc->V_upper = c3UProxy;

////        } else {

////            // Hold threshold values currently associated with trackbars
////            int c1LProxy = cc->R_lower;
////            int c1UProxy = cc->R_upper;
////            int c2LProxy = cc->G_lower;
////            int c2UProxy = cc->G_upper;
////            int c3LProxy = cc->B_lower;
////            int c3UProxy = cc->B_upper;

////            // Re-generate the trackbars, linking them with different pointers
////            cvCreateTrackbar("C1 lower", "Input Image", &(cc->H_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C1 Upper", "Input Image", &(cc->H_upper), 256, trackbarHandler);
////            cvCreateTrackbar("C2 Lower", "Input Image", &(cc->S_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C2 Upper", "Input Image", &(cc->S_upper), 256, trackbarHandler);
////            cvCreateTrackbar("C3 Lower", "Input Image", &(cc->V_lower), 256, trackbarHandler);
////            cvCreateTrackbar("C3 Upper", "Input Image", &(cc->V_upper), 256, trackbarHandler);

////            // Set the held values into the object array
////            cc->R_lower = c1LProxy;
////            cc->R_upper = c1UProxy;
////            cc->G_lower = c2LProxy;
////            cc->G_upper = c2UProxy;
////            cc->B_lower = c3LProxy;
////            cc->B_upper = c3UProxy;

////        }

////    }

////    // Track changes in trackbar position
////    *prevPos = pos;

//    return;
//}


///*
//  Handle mouse clicking
//  */
//void onMouse(int event, int x, int y, int flags, void *param) {

////    Correlation *cc = &(MainWindow::c);
////    IplImage **imgInFull = &(MainWindow::imgInFull);

////    // Handle down and up clicks, for click-and-drag ROI rectangle
////    if(event == CV_EVENT_LBUTTONDOWN) {

////        // Assign ROI location
////        cc->ROI.x = x;
////        cc->ROI.y = y;

////    } else if(event == CV_EVENT_LBUTTONUP) {

////        // Assign ROI dimensions
////        cc->ROI.width = x - cc->ROI.x;
////        cc->ROI.height = y - cc->ROI.y;

////        // Display ROI visually
////        *imgInFull = cvCloneImage(cc->imgInFull.imgp);
////        CvScalar col;
////        if((*imgInFull)->nChannels == 1) {
////            col = cvScalar(255);
////        } else {
////            col = cvScalar(0, 0, 255);
////        }
////        cvDrawRect(*imgInFull, cvPoint(cc->ROI.x, cc->ROI.y), cvPoint(cc->ROI.x + cc->ROI.width, cc->ROI.y + cc->ROI.height), col);
////        cvShowImage("Input Image", *imgInFull);
////    }

//    return;
//}





















///*
//  Load thresholding values from file into memory

//  @input sourcePathName = Absolute path where file is located
//  */
//void loadPersistentValues(string sourcePathName) {

//    // Look for previously saved threshold values; if they exist, then load them into memory
//    string fileName = "persistentValues.dat";
//    string file = sourcePathName.append(fileName);
//    ifstream persistFile( file.c_str(), ios::in );

//    if(persistFile.is_open()) {

//        // Load values for the persistent parameters
//        persistFile.seekg(0);

//        persistFile >> c.R_lower;
//        persistFile >> c.R_upper;
//        persistFile >> c.G_lower;
//        persistFile >> c.G_upper;
//        persistFile >> c.B_lower;
//        persistFile >> c.B_upper;

//        persistFile >> c.H_lower;
//        persistFile >> c.H_upper;
//        persistFile >> c.S_lower;
//        persistFile >> c.S_upper;
//        persistFile >> c.V_lower;
//        persistFile >> c.V_upper;

//        persistFile.close();
//    }


//    return;
//}










/*
  this code allows user to modify internal thresholds, etc.
//

// Confirm identified dots
//   This is the user feedback loop. The user examines the output image, which clearly shows identified dots. They then
//     have the ability to change all parameters which affect dot identification.
//   Note that dots are ONLY recognized if their probability is above a threshold.
dotsIdentified = "n";
while(dotsIdentified.compare("n") == 0)
{
    // Refresh image
    outputImage();

    // Request user confirmation that dots have been correctly identified
    std::cout << endl;
    std::cout << "Open an image viewer for the most updated version of the file " << destPathName << "outputImage.bmp" << endl;
    std::cout << endl;
    std::cout << "In this image, do the blobs appear to be correctly identified (y/n)? ";
    std::cin >> dotsIdentified;
    std::cout << endl;

    // Allow user to change all parameters affecting dot identification
    if(dotsIdentified.compare("n") == 0)
    {
        std::cout << "Please select an option: " << endl;
        std::cout << "  1. Area threshold" << endl;
        std::cout << "  2. Probability threshold" << endl;
        std::cout << endl;
        std::cin >> option;

        // Area threshold
        if(option.compare("1") == 0) {
            std::cout << "Current area threshold: " << area_thresh << endl;
            std::cout << "Minimum blob area: " << min_blob_area << endl;
            std::cout << "Maximum blob area: " << max_blob_area << endl;
            std::cout << "New area threshold: ";
            std::cin >> area_thresh;
            std::cout << endl;
        }

        // Probability threshold
        else if(option.compare("1") == 0) {
            std::cout << "Current probabilty threshold: " << dotProbabilityThresh << endl;
            std::cout << "New area threshold: ";
            std::cin >> dotProbabilityThresh;
            std::cout << endl;
        }
    }
    else {
        break;
    }

    // Attempt to identify the dots using the new threshold and logic values
    identifyDots();
}
*/


