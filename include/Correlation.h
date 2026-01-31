#ifndef CORRELATION_H
#define CORRELATION_H


#include "CorrelationParent.h"


/*
  Correlation class

  This class provides functions which can store and analyze an image for the digital image correlation procedure. The
  class is meant to be used in the context of a sequence of images taken by a camera in an inertial reference frame.

  Note: Dynamic memory allocation allows for larger arrays. This is because the stack has more memory available than
  the heap.

  Rule: cvPoint(x, y)
     x -> column
     y -> row

  Rule: bw[r][c]
     r -> row
     c -> column

  */
class Correlation : public CorrelationParent {


public:

    Correlation();


public:


    /* ----- Level 0 Functions --------------------------------------------------------------------------------------- */

    void initObject();
    bool analyzeImage(RgbImage src, RgbImage dst, RgbImage refImage, GreyImage dstBW, RgbImage dstProc, string destPathName, string sourcePathName, string fileName, bool makeReferenceImage=false);
    void storePositionField(string path, bool isFirstRun=false);
    void storeStrainField(string path, bool isFirstRun=false);
//    void outputImage(bool showInWindow=true, bool switchIsDot=false);
    void outputOverlay(RgbImage src, RgbImage dst);


    /* ----- Level 1 Functions --------------------------------------------------------------------------------------- */

    void loadPersistentValues();
    void savePersistentValues();
    bool inputImage(RgbImage src, RgbImage dst, GreyImage dstBW, string file);
    void extractDisplacementField(RgbImage src, RgbImage dst, RgbImage dstProc, RgbImage refImage, bool isFirstRun=false);
    void blobAnalysis(GreyImage bw);
    void extractPositionField(bool isFirstRun=false);
    void generateGridMap();
    void computeStrain();
    void drawCurrentState(GreyImage src, RgbImage dst, bool switchIsDot=false);


    /* ----- Level 2 Functions --------------------------------------------------------------------------------------- */

    void subsetImage(RgbImage& src, RgbImage& dst);
    void colorFilter(IplImage *src, IplImage **dst);
    GreyImageFloat extractSubImage(RgbImage src, CvRect *gridElementROI, Transformation tfm=defaulTrasformation);
    Transformation gradCOF(Transformation tfm);
    Transformation hessianCOF(Transformation tfm);
    float evaluateCOF(GreyImageFloat gridImgRef, GreyImageFloat gridImg, CofType cofType=SSD_SIMPLE);
    int matchBlob(int c, int r, int criteria);
    void updateBoundingBox(int blob_track, int c, int r);
    void mergeBlobs(int fromBlob, int toBlob);
    void postProcess();
    void blobProbability();
    void identifyDots();
    bool isValidPointSet(double pt[2][3], double pt_star[2][3], int p, int n1, int n2);
    void localStrain(double pt[2][3], double pt_star[2][3], int count, int method);
    double minAbs(double a, double b);
    double maxAbs(double a, double b);
    double minIgnore(double a, double b);
    double maxIgnore(double a, double b);
    void drawBlobProperty(RgbImage img, int whichProperty, int blob_track, int B, int G, int R);
    void drawCurrentPixel(RgbImage img, int c, int r, int B, int G, int R);
    void debugHandle(int direction=-1, string functionName="unknown function");
    void debugFunction();
    void halt();



public:

    bool debugParam;
};


#endif // CORRELATION_H
