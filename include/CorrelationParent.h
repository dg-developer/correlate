#ifndef CORRELATIONPARENT_H
#define CORRELATIONPARENT_H


//#include <iostream>
//#include <fstream>


#include <string>
#include "OpenCvInclude.h"


using namespace std;


/* ----- User-defined Types ----------------------------------------------------------------------------------------- */

// Global parameters which "hard-code" Correlation algorithms to work using either blob analysis or digital image
//   correlation
enum {
    analysisApproach = 2,  // Match this value to the desired method of analysis, below
    BLOB_ANALYSIS = 2,
    DIGIAL_CORRELATION = 3
};

// Correlation coefficient type
enum CofType {
    SSD_SIMPLE
};


/* ----- User-defined Types ----------------------------------------------------------------------------------------- */

typedef struct RectDouble
{
    double x;
    double y;
    double width;
    double height;
} RectDouble;

// Tolerances
typedef struct Tolerance {
    float cof;
} Tolerance;

// Bilinear displacement field transformation
typedef struct Transformation {
    float u, v;  // x- and y-displacement components
    float ux, uy;  // x-displacement gradients
    float vx, vy;  // y-displacement gradients
} Transformation;
const Transformation defaulTrasformation = {0, 0, 1, 0, 0, 1};


/* ----- Constants -------------------------------------------------------------------------------------------------- */

// Static array sizing
const int max_num_blobs = 1000;
const int max_neighbor_count = 36;  // Allow 10-degree increments
const int calculation_count = 3;

// Grid sizing
const int gridContainerWidth(5), gridContainerHeight(6);  // "Units" are indices

// Other constants
//const float scale8U232F = (float)pow( 3, 0.5 )/(float)pow( 3*pow(255, 2), 0.5 );
const float scale8U232F = 1.0/255.0;
//const float scale8U232F = pow( 3*pow(255, 2), -0.5 );


/*
  CorrelationParent class

  This class containing members and functions (all of which are abstract) is the parent of the Data, Correlation, and
  RunTest classes.

  Correlation class -  contains implementations of the member functions which perform tasks needed for the actual
    digital image correlation
  RunTest class - contains implementations of the functions which each performs a suite of tests on the corresponding
    function in the Correlation class
  Data class - contains implementations which load test data for each test suite to be performed

  */
class CorrelationParent {

public:

    /* ----- Class Implementation Functions ------------------------------------------------------------------------- */

    CorrelationParent();


    /* ----- Level 0 Functions -------------------------------------------------------------------------------------- */

    virtual void initObject() { return; }
    virtual bool analyzeImage(RgbImage src, RgbImage dst, RgbImage refImage, GreyImage dstBW, RgbImage dstProc, string destPathName, string sourcePathName, string fileName, bool makeReferenceImage) { return false; }
    virtual void storePositionField(string path, bool isFirstRun=false) { return; }
    virtual void storeStrainField(string path, bool isFirstRun=false) { return; }
//    virtual void outputImage(bool showInWindow=true, bool switchIsDot=false) { return; }
    virtual void outputOverlay(RgbImage src, RgbImage dst) { return; }


    /* ----- Level 1 Functions -------------------------------------------------------------------------------------- */

    virtual void loadPersistentValues() { return; }
    virtual void savePersistentValues() { return; }
    virtual bool inputImage(RgbImage src, RgbImage dst, GreyImage dstBW, string file) { return false; }
    virtual void extractDisplacementField(RgbImage src, RgbImage dst, RgbImage dstProc, RgbImage refImage, bool isFirstRun=false) { return; }
    virtual void blobAnalysis(GreyImage bw) { return; }
    virtual void extractPositionField(bool isFirstRun=false) { return; }
    virtual void generateGridMap() { return; }
    virtual void computeStrain(bool showDebug) { return; }
    virtual void drawCurrentState(GreyImage src, RgbImage dst, bool switchIsDot=false) { return; }


    /* ----- Level 2 Functions -------------------------------------------------------------------------------------- */

    virtual void subsetImage(RgbImage& src, RgbImage& dst) { return; }
    virtual void colorFilter(IplImage *src, IplImage **dst) { return; }
    virtual GreyImageFloat extractSubImage(RgbImage src, CvRect *gridElementROI, Transformation tfm=defaulTrasformation) { GreyImageFloat a; return a; }
    virtual Transformation gradCOF(Transformation tfm) { Transformation tfmGradient; return tfmGradient; }
    virtual Transformation hessianCOF(Transformation tfm) { Transformation tfmHessian; return tfmHessian; }
    virtual float evaluateCOF(GreyImageFloat gridImgRef, GreyImageFloat gridImg, CofType cofType=SSD_SIMPLE) { return 0; }
    virtual int matchBlob(int r, int c, int criteria) { return -1; }
    virtual void updateBoundingBox(int blob_track, int r, int c) { return; }
    virtual void mergeBlobs(int fromBlob, int toBlob) { return; }
    virtual void postProcess() { return; }
    virtual void blobProbability() { return; }
    virtual void identifyDots() { return; }
    virtual bool isValidPointSet(double pt[2][3], double pt_star[2][3], int p, int n1, int n2) { return false; }
    virtual void localStrain(double pt[2][3], double pt_star[2][3], int count, int method) { return; }
    virtual double minAbs(double a, double b) { return 0; }
    virtual double maxAbs(double a, double b) { return 0; }
    virtual double minIgnore(double a, double b) { return -1; }
    virtual double maxIgnore(double a, double b) { return -1; }
    virtual void drawBlobProperty(RgbImage img, int whichProperty, int blob_track, int B, int G, int R) { return; }
    virtual void drawCurrentPixel(RgbImage img, int r, int c, int B, int G, int R) { return; }
    virtual void debugHandle(int direction=-1, string functionName="unknown function") { return; }
    virtual void debugFunction() { return; }


public:

    string persistFileString;
    int enable_debug;
    bool enable_step;
    int image_sequence;
    int h, w;
    int colorSpace;
    int colorSpacePrev;


    /* ----- Images ------------------------------------------------------------------------------------------------- */

    // This class is no longer responsible for storing images

    // Region of imgInFull from which to extract imgIn
    CvRect ROI;


    /* ----- Blob Analysis Parameters ------------------------------------------------------------------------------- */

    // Static thresholds
    int R_lower, R_upper, G_lower, G_upper, B_lower, B_upper;
    int H_lower, H_upper, S_lower, S_upper, V_lower, V_upper;

    int num_blobs;
    int found;
    double min_blob_area, max_blob_area;
    int blob_bound[3][max_num_blobs];
    int blob_area[max_num_blobs];
    double blob_eccentricity[max_num_blobs];
    double blob_centroid[2][max_num_blobs];
    double bounding_box[4][max_num_blobs];
    double major_axis[max_num_blobs];
    double minor_axis[max_num_blobs];
    double blob_probability[max_num_blobs];
    bool is_dot[max_num_blobs];
    int blob_dist_thresh;
    int area_thresh;
    int num_dots;
    double migration_dist_thresh;
    RectDouble pfRefRectangle;
    RectDouble pfRectangle;
    double positionField_ref[2][max_num_blobs];
    double positionField[2][max_num_blobs];
    double min_direction_angle;
    double max_direction_angle;
    double max_neighbor_dist;
    int gridMap[max_neighbor_count][max_num_blobs];
    double epsxx[max_num_blobs];
    double epsxy[max_num_blobs];
    double epsyx[max_num_blobs];
    double epsyy[max_num_blobs];
    double epsCollection[4][calculation_count];
    double globalMinStrain, globalMaxStrain;
    int t;
    double dotProbabilityThresh;


    /* ----- Logic -------------------------------------------------------------------------------------------------- */

    Tolerance tolerance;


    /* ----- Digital Image Correlation Parameters ------------------------------------------------------------------- */

    float cofCurrent, cofPrev;


    /* ----- Grid --------------------------------------------------------------------------------------------------- */

    // Grid outer bounds (in pixels)
    CvRect gridROI;

    // Pointer to a grid element
    CvRect *gridElementROI;

    // Grid sub-image
    GreyImageFloat gridElementImgRef, gridElementImg;

    // Parameters
    int gridElementHeight, gridElementWidth;  // Units are pixels
    Transformation tfm;

    // Array of CvRect grid elements
    CvRect gridContainer[ gridContainerWidth ][ gridContainerHeight ];
    // Note: Static multi-dimensional array may not reserve memory correctly if elements are
    //   CvRect. This has caused THREE major errors in practice. Do not declare ANY variables of any type the line after
    //   a CvRect[][] array declaration.

};


#endif // CORRELATIONPARENT_H
