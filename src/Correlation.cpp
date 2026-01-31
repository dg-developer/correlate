#include "Correlation.h"


/*
  */
Correlation::Correlation() :
    CorrelationParent()
{
}


/*
  Load thresholding values from file into memory. This file in the future may include values which similarly must be
  stored in a peresistent manner.
  */
void Correlation::initObject() {

    // Load persistent values from a file
    loadPersistentValues();

    return;
}


/*
  Input an image and analyze that image for dots and compute strain field

  @input sourcePathName = Absolute path of image file
  @input fileName = Name of image file
  @input makeReferenceImage = Use true for the first image in a sequence, false otherwise. If true, the position
    field is initialized, and generateGridMap() is run.
  */
bool Correlation::analyzeImage(RgbImage src, RgbImage dst, RgbImage refImage, GreyImage dstBW, RgbImage dstProc, string destPathName, string sourcePathName, string fileName, bool makeReferenceImage)
{
    debugHandle(0, "analyzeImage()");

    // Run inputImage()
    if( !inputImage(src, dst, dstBW, sourcePathName.append(fileName)) ) {
        return false;
    }

    // THESE ARE ASSOCIATED WITH THE BLOB ANALYSIS APPROACH
    if(analysisApproach == BLOB_ANALYSIS) {

        // Run blobAnalysis() to determine the core blob arrays
        blobAnalysis(dstBW);

        // Run extractPostionField()
        //   -> This function will simply transform blob locations into dot locations, and these into a position field
        //      standard form
        //   -> (ie, this is the function which robustly looks at possible dot locations and decides which ones are real)
        extractPositionField(makeReferenceImage);


        if(makeReferenceImage) {

            // For the reference image only, generate a map of nearest neighbors to use for computing the displacement
            //   gradients
            generateGridMap();

        } else {

            // Assuming a locally bilinear displacement field, calculate the strain field at each point
            computeStrain();

        }
    } else if (analysisApproach == DIGIAL_CORRELATION) {

        // Run extractDisplacementField()
        extractDisplacementField(src, dst, dstProc, refImage, makeReferenceImage);
        //   At this stage, the positionField_ref AND positionField arrays have been populated, for all values of
        //     makeReferenceImage.
        //   For simplicity the displacement field is added to the reference pf and stored in positionField
        //     (inefficient).

        // Assuming a locally bilinear displacement field, calculate the strain field at each point
        computeStrain();

    }

    // At this point, a position field array for the current image has been populated; this array will be overwritten
    //   next time this function is called

    debugHandle(1, "analyzeImage()");

    return true;
}


/*
  Store current position field to a file

  @input path = Absolute path to directory for file storage
  @input isFirstRun = Use true for first image in a sequence, false otherwise. If true, the position field file is
    overwritten with gridMap, followed by the reference position field. If false, the strain field is appended
    to the position file.
  */
void Correlation::storePositionField(string path, bool isFirstRun)
{
    debugHandle(0, "storePositionField()");

    // Notify entry to function if debug2 is enabled
    if(enable_debug == 2) {
        std::cout << "Running storePositionField()" << endl;
    }

    // Declare file output mode
    ios_base::openmode outputMode = ios::app;

    if(isFirstRun)
    {
        // Specify file output mode
        outputMode = ios::out;

        // Store reference positionField_ref in positionField
        for(int p = 0; p < num_dots; p++)
        {
            positionField[0][p] = positionField_ref[0][p];
            positionField[1][p] = positionField_ref[1][p];
        }
    }

    // Open file for output to store reference image data
    ofstream positionFile( path.append("positionField.txt").c_str(), outputMode );

    // Check for correct file opening
    if(!positionFile.is_open())
    {
        std::cout << "Error opening positionField.txt." << endl;
    }

    if(isFirstRun)
    {
        // Store number of points in a way that MATLAB understands
        positionFile << max_num_blobs << " ";
        for(int n = 0; n < max_neighbor_count; ++n) {
            positionFile << "0 ";
        }
        positionFile << endl;

        // Store gridMap at beginning of file
        for(int p = 0; p < max_num_blobs; p++)
        {
            for(int n = 0; n < max_neighbor_count; n++)
            {
                positionFile << gridMap[n][p] << " ";
            }
            positionFile << endl;
        }
        positionFile << endl;
    }

    // Store position field to file
    for(int p = 0; p < max_num_blobs; p++)
    {
        positionFile << positionField[0][p] << " " << positionField[1][p] << endl;
    }
    positionFile << endl;

    // Close file
    positionFile.close();

    debugHandle(1, "storePositionField()");
    return;
}


/*
  Store current strain field to file

  @input path = Absolute path to file for strain field
  @input isFirstRun = Use true for first image in a sequence, false otherwise. If true, strain field file is
    overwritten with the reference strain field (zeros by definition). If false, strain file is appended to.
  */
void Correlation::storeStrainField(string path, bool isFirstRun)
{
    debugHandle(0, "storeStrainField()");

    // Declare file output mode
    ios_base::openmode outputMode = ios::app;

    if(isFirstRun)
    {
        // Specify file output mode
        outputMode = ios::out;

        // Store zero strain field
        for(int p = 0; p < num_dots; p++)
        {
            epsxx[p] = 0;
            epsxy[p] = 0;
            epsyx[p] = 0;
            epsyy[p] = 0;
        }
    }

    // Open file for output to store reference strain field data
    ofstream strainFile ( path.append("strainField.txt").c_str(), outputMode );

    // Check for correct file opening
    if(!strainFile.is_open())
    {
        std::cout << "Error opening strainField.txt." << endl;
    }

    // Store strain field to file
    for(int p = 0; p < max_num_blobs; p++)
    {
        strainFile << epsxx[p] << " " << epsxy[p] << " " << epsyx[p] << " " << epsyy[p] << endl;
    }
    strainFile << endl;

    // Close file
    strainFile.close();

    debugHandle(1, "storeStrainField()");
    return;
}


///*
//  Write useful information to imgProc.imgp, and optionally output to a window

//  @input showInWindow = Use true to output imgProc.imgp to a default window ("Processed Image"). Use false to
//    manually output to an arbitrary window with cvShowImage().
//  @input switchIsDot = default false displays information about all blobs, true only shows blobs which have been
//    identified as dots
//  */
//void Correlation::outputImage(bool showInWindow, bool switchIsDot) {

//    debugHandle(0, "outputImage()");

//    // Offloaded drawing functions, and display image, and possibly cvWaitKey();
//    drawCurrentState(showInWindow, switchIsDot);

//    debugHandle(1, "outputImage()");
//    return;
//}


/*
  Show current image with strain field overlayed in a way which visually represents the relative computed strain
    values. This function modifies imgProc.imgp and outputs to a default window, "Strain Field Overlay."

    @input showInWindow = Use true to output in a default window. Use false to manually show image later with
      cvShowImage().
  */
void Correlation::outputOverlay(RgbImage src, RgbImage dst) {

    // Initialize scaling parameters
    int baseIntensity = 100;  // Maximum intensity of background image
    int minIntensity = 150;  // Minimum strain intensity
    int maxIntensity = 255;  // Maximum strain intensity

    // Put imgIn into imgProc
    if(!dst.imgp || dst.imgp->width != src.imgp->width || dst.imgp->height != src.imgp->height || dst.imgp->depth != src.imgp->depth) {
        dst.imgp = cvCreateImage(cvSize(src.imgp->width, src.imgp->height), src.imgp->depth, 3);
    }
    dst.imgp = cvCloneImage(src.imgp);

    // Split processed image into HSV components
    cvCvtColor(dst.imgp, dst.imgp, CV_BGR2HSV);
    GreyImage H, S, Vxx, Vxy, Vyy;
    H.imgp = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 1);
    S.imgp = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 1);
    Vxx.imgp = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 1);
    Vxy.imgp = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 1);
    Vyy.imgp = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 1);
    cvSplit(dst.imgp, H.imgp, S.imgp, Vxx.imgp, NULL);

    // Reduce overall image intensity
    int maxBackgroundIntensity = -1;
    for(int i = 0; i < Vxx.imgp->height; ++i) {
        for(int j = 0; j < Vxx.imgp->width; ++j) {
            maxBackgroundIntensity = maxIgnore(maxBackgroundIntensity, (int)Vxx[i][j]);
        }
    }
    cvConvertScale(Vxx.imgp, Vxx.imgp, (double)baseIntensity/(double)maxBackgroundIntensity);
    Vxy.imgp = cvCloneImage(Vxx.imgp);
    Vyy.imgp = cvCloneImage(Vxx.imgp);

    // Iterate over points
    int width = (int)2*sqrt( (min_blob_area + max_blob_area)/2 );
    int height = width;
    for(int p = 0; p < num_dots; ++p) {

        // Ignore invalid points
        if(positionField[0][p] == -1) {
            continue;
        }

        // Alter image "intensity" (no alpha channel so reducing color component values is a shortcut) at
        //   regions of interest
        int x = (int)( positionField[0][p] - width/2 );
        int y = (int)( positionField[1][p] - width/2 );

        CvScalar intensity = cvScalar( (double)minIntensity + ( epsxx[p] - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );  //        CvScalar intensity = cvScalar( (double)minIntensity + ( epsxx[p] - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );
        cvSetImageROI(Vxx.imgp, cvRect( x, y, width, height ));
        cvSet(Vxx.imgp, intensity, NULL);

        intensity = cvScalar( (double)minIntensity + ( epsxy[p] - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );
        cvSetImageROI(Vxy.imgp, cvRect( x, y, width, height ));
        cvSet(Vxy.imgp, intensity, NULL);

        intensity = cvScalar( (double)minIntensity + ( epsyy[p] - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );
        cvSetImageROI(Vyy.imgp, cvRect( x, y, width, height ));
        cvSet(Vyy.imgp, intensity, NULL);



//        double avgStrain = ( epsxx[p] + epsxy[p] + epsyx[p] + epsyy[p] )/4;
//        CvScalar intensity = cvScalar( (double)minIntensity + ( avgStrain - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );//        CvScalar intensity = cvScalar( (double)minIntensity + ( epsxx[p] - globalMinStrain )*( (double)maxIntensity - (double)minIntensity )/( globalMaxStrain - globalMinStrain ) );
//        cvSet(V.imgp, intensity, NULL);
    }

    // Initialize XY, YY images
    IplImage *imgXY = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 3);
    IplImage *imgYY = cvCreateImage(cvSize(dst.imgp->width, dst.imgp->height), dst.imgp->depth, 3);

    // Recombine color components
    cvResetImageROI(Vxx.imgp);
    cvResetImageROI(Vxy.imgp);
    cvResetImageROI(Vyy.imgp);
    cvMerge(H.imgp, S.imgp, Vxx.imgp, NULL, dst.imgp);
    cvMerge(H.imgp, S.imgp, Vxy.imgp, NULL, imgXY);
    cvMerge(H.imgp, S.imgp, Vyy.imgp, NULL, imgYY);
    cvCvtColor(dst.imgp, dst.imgp, CV_HSV2BGR);
    cvCvtColor(imgXY, imgXY, CV_HSV2BGR);
    cvCvtColor(imgYY, imgYY, CV_HSV2BGR);

//    // Show imgProc
//    if(showInWindow) {
//        cvShowImage("XX Strain Field Overlay", imgProc.imgp);
//        cvShowImage("XY Strain Field Overlay", imgXY);
//        cvShowImage("YY Strain Field Overlay", imgYY);
////        cvShowImage("Strain Field Overlay", imgProc.imgp);
//    }

    // Clear memory
    cvReleaseImage(&H.imgp);
    cvReleaseImage(&S.imgp);
    cvReleaseImage(&Vxx.imgp);
    cvReleaseImage(&Vxy.imgp);
    cvReleaseImage(&Vyy.imgp);
    cvReleaseImage(&imgXY);
    cvReleaseImage(&imgYY);

    // Wait for user to press key
    int key = -1;
    while(key == -1) {
        key = cvWaitKey(10);
    }

    return;
}


/*
  Load persistent values from a file
  */
void Correlation::loadPersistentValues() {

    // Open file
    ifstream persistFile( persistFileString.c_str(), ios::in );

    if(persistFile.is_open()) {

        // Load values for the persistent parameters
        persistFile.seekg(0);

        persistFile >> R_lower;
        persistFile >> R_upper;
        persistFile >> G_lower;
        persistFile >> G_upper;
        persistFile >> B_lower;
        persistFile >> B_upper;

        persistFile >> H_lower;
        persistFile >> H_upper;
        persistFile >> S_lower;
        persistFile >> S_upper;
        persistFile >> V_lower;
        persistFile >> V_upper;

        persistFile.close();

    } else {

        cout << "File failed to open to load persistent values." << endl;
    }

    return;
}


/*
  Save persistent values to a file for later recall
  */
void Correlation::savePersistentValues() {

    // Open file for output
    ofstream persistFile( persistFileString.c_str(), ios::out );

    if(persistFile.is_open()) {

        // Write values to file
        persistFile << R_lower << endl;
        persistFile << R_upper << endl;
        persistFile << G_lower << endl;
        persistFile << G_upper << endl;
        persistFile << B_lower << endl;
        persistFile << B_upper << endl;

        persistFile << H_lower << endl;
        persistFile << H_upper << endl;
        persistFile << S_lower << endl;
        persistFile << S_upper << endl;
        persistFile << V_lower << endl;
        persistFile << V_upper << endl;

        // Close file
        persistFile.close();

    } else {

        cout << "File failed to open to save persistent values." << endl;
    }

    return;
}


/*
  Input the file into the image struct imgInFull.imgp, then filter to form bw.imgp array. After running this function,
    the image data is stored in imgInFull, data in the ROI is in imgIn, and the binary image is in bw. See Image.h for
    information on how image data is stored.

  @input file = the image file to be opened

  @output imgInFull = raw pixel data (ignored)
  @output imgIn = raw pixel data, ROI only
  @output bw = array of true/false values representing pixels of the points to be tracked
  */
bool Correlation::inputImage(RgbImage src, RgbImage dst, GreyImage dstBW, string file)
{
    debugHandle(0, "inputImage()");

    // Read image from file
    src.imgp = cvLoadImage(file.c_str());
    if(!src.imgp) {
        if(enable_debug) { cout << "Failed to load image in inputImage()." << endl; }
        return false;
    }

    // Initialize structs
    if(!dst.imgp || dst.imgp->width != ROI.width || dst.imgp->height != ROI.height || dst.imgp->depth != dst.imgp->depth) {
        dst.imgp = cvCreateImage(cvSize(ROI.width, ROI.height), src.imgp->depth, 3);
    }
//    if(!bw.imgp || bw.imgp->width != ROI.width || bw.imgp->height != ROI.height || bw.imgp->depth != IPL_DEPTH_8U) {
//        bw.imgp = cvCreateImage(cvSize(ROI.width, ROI.height), IPL_DEPTH_8U, 1);
//    }

    // Determine image dimensions
    h = dst.imgp->height;
    w = dst.imgp->width;

    // Populate imgIn as a subset of dst, using the ROI specified by {{x0,y0}, {x1,y1}}
    subsetImage(src, dst);

    // Filter RGB array, and determine bw
    colorFilter(dst.imgp, &dstBW.imgp);

    debugHandle(1, "inputImage()");
    return true;
}


/*
  Extract the displacement field using digital image correlation
  */
void Correlation::extractDisplacementField(RgbImage src, RgbImage dst, RgbImage dstProc, RgbImage refImage, bool isFirstRun) {


    // If isFirstRun, generate a grid to arbitrarily assign a reference pf
    if(isFirstRun) {


        // Initialize grid ROI
        double offsetRatio = 0.8;  //this parameter needs to be moved somewhere else
        gridROI = cvRect( (int)(.5*(1-offsetRatio)*(double)src.imgp->width), \
                                (int)(.5*(1-offsetRatio)*(double)src.imgp->height), \
                                (int)(offsetRatio*(double)src.imgp->width), \
                                (int)(offsetRatio*(double)src.imgp->height) );


        // Set grid element width and height
        gridElementHeight = gridROI.height/gridContainerHeight;
        gridElementWidth = gridROI.width/gridContainerWidth;


        // Initialize gridContainer, pfRef, and pf
        int b = 0;
        for(int row = 0; row < gridContainerHeight; ++row) {
            for(int col = 0; col < gridContainerWidth; ++col) {


                // Initialize grid container
                *gridElementROI = cvRect( gridROI.x + col*gridElementWidth, \
                                         gridROI.y + row*gridElementHeight, \
                                         gridElementWidth, gridElementHeight );

                gridContainer[row][col] = *gridElementROI;


                // Initialize pfRef with grid element centers
                if(b < gridContainerWidth*gridContainerHeight && b < max_num_blobs) {
                    positionField_ref[0][b] = (double)(gridElementROI->x) + .5*(double)(gridElementROI->width);
                    positionField_ref[1][b] = (double)(gridElementROI->y) + .5*(double)(gridElementROI->height);
                } else {
                    positionField_ref[0][b] = -1;
                    positionField_ref[1][b] = -1;
                }


                // Initialize pf
                positionField[0][b] = positionField_ref[0][b];
                positionField[1][b] = positionField_ref[1][b];


                // Increment b
                ++b;

            }
        }


        // Set gridElementROI to point to gridContainer[0][0]
        gridElementROI = &gridContainer[0][0];


        // Store current image as reference image
        refImage.imgp = cvCloneImage(src.imgp);


    } else {


        // Initialize gridElementImg
        gridElementImgRef.imgp = cvCreateImage( cvSize(gridElementWidth, gridElementHeight), IPL_DEPTH_32F, 1 );
        gridElementImg.imgp = cvCreateImage( cvSize(gridElementWidth, gridElementHeight), IPL_DEPTH_32F, 1 );


        // Iterate over all grid sub-regions in the gridContainer
        int b = 0;
        for(int row = 0; row < gridContainerHeight; ++row) {
            for(int col = 0; col < gridContainerWidth; ++col) {


                // Specify initial transformation
                tfm = defaulTrasformation;


                // Point to current reference sub-image
                gridElementROI = &(gridContainer[row][col]);


                // Extract sub-image from the reference and deformed images
                gridElementImgRef.imgp = cvCloneImage( (extractSubImage(dst, gridElementROI)).imgp );
                gridElementImg.imgp = cvCloneImage( (extractSubImage(src, gridElementROI, tfm)).imgp );


                // Beginning at a "best guess," optimize the grey value pattern G(x,y), which is computed under a
                //   transformed sub-region
                cofPrev = -4*tolerance.cof;
                cofCurrent = -2*tolerance.cof;
                while( abs(cofCurrent - cofPrev) > tolerance.cof ) {


                    // Track previous objective function value
                    cofPrev = cofCurrent;



                    // Compute gradient



                    // Increment tfm parameters
//                    tfm -= gradCOF(tfm)/hessianCOF(tfm);
                    Transformation gradC = gradCOF(tfm);
                    Transformation hessianC = hessianCOF(tfm);
                    tfm.u -= gradC.u/hessianC.u;
                    tfm.v -= gradC.v/hessianC.v;
                    tfm.ux -= gradC.ux/hessianC.ux;
                    tfm.uy -= gradC.uy/hessianC.uy;
                    tfm.vx -= gradC.vx/hessianC.vx;
                    tfm.vy -= gradC.vy/hessianC.vy;


                    // Extract sub-image from current image
                    gridElementImg.imgp = cvCloneImage( (extractSubImage(src, gridElementROI, tfm)).imgp );


                    // Evaluate objective function
                    cofCurrent = evaluateCOF(gridElementImgRef, gridElementImg);
//                    cout << gridElementImgRef.imgp->depth << "    " << gridElementImg.imgp->depth << endl;
                    cout << cofPrev << "      " << cofCurrent << endl;


                    // Show deformed region in deformed image
                    int x = gridElementROI->x + 0.5*gridElementROI->width;
                    int y = gridElementROI->y + 0.5*gridElementROI->height;
                    int dx = 0.5*gridElementROI->width;
                    int dy = 0.5*gridElementROI->height;
                    int xD = x + tfm.u + tfm.ux * dx + tfm.uy * dy;
                    int yD = y + tfm.v + tfm.vx * dx + tfm.vy * dy;

                    CvPoint pt1 = cvPoint( x + tfm.u + tfm.ux * dx + tfm.uy * dy, \
                                          y + tfm.v + tfm.vx * dx + tfm.vy * dy );

                    CvPoint pt2 = cvPoint( x + tfm.u - tfm.ux * dx + tfm.uy * dy, \
                                          y + tfm.v - tfm.vx * dx + tfm.vy * dy );

                    CvPoint pt3 = cvPoint( x + tfm.u - tfm.ux * dx - tfm.uy * dy, \
                                          y + tfm.v - tfm.vx * dx - tfm.vy * dy );

                    CvPoint pt4 = cvPoint( x + tfm.u + tfm.ux * dx - tfm.uy * dy, \
                                          y + tfm.v + tfm.vx * dx - tfm.vy * dy );

                    dstProc.imgp = cvCloneImage(src.imgp);
                    cvDrawLine(dstProc.imgp, pt1, pt2, cvScalar(0, 0, 255), 1);
                    cvDrawLine(dstProc.imgp, pt2, pt3, cvScalar(0, 0, 255), 1);
                    cvDrawLine(dstProc.imgp, pt3, pt4, cvScalar(0, 0, 255), 1);
                    cvDrawLine(dstProc.imgp, pt4, pt1, cvScalar(0, 0, 255), 1);
                }

            }
        }


    }


    return;
}

/*
  For the image currently loaded in memory in bw, fully populate the core blob analysis arrays.

  Here blobAnalysis() identifies valid blobs. Then extract_position_field() identifies valid dots.

    "Valid blob b" <- Indicated by blob_area[b] > 0
    "Valid dot b"  <- Indicated by is_dot[b] == true

  Note that blobAnalysis() identifies blobs which are STRICTLY touching (edge pixels are adjacent or
    immediately diagonal). Then postProcess() connects blobs which are not strictly touching, but
    realistically belong to the same blob.

  @input bw

  @output Core blob analysis arrays, fully populated
*/
void Correlation::blobAnalysis(GreyImage bw)
{
    debugHandle(0, "blobAnalysis()");

    // Re-initialize all arrays
    for(int b = 0; b < max_num_blobs; b++)
    {
        num_blobs = 0;
        min_blob_area = 10000;
        max_blob_area = -1;
        blob_bound[0][b] = 0;  // Left bound
        blob_bound[1][b] = 0;  // Right bound
        blob_bound[2][b] = 0;  // Row on which these bounds were identified
        blob_area[b] = 0;
        blob_eccentricity[b] = 0;
        blob_centroid[0][b] = 0;
        blob_centroid[1][b] = 0;
        bounding_box[0][b] = -1;
        bounding_box[1][b] = -1;
        bounding_box[2][b] = -1;
        bounding_box[3][b] = -1;
        major_axis[b] = 0;
        minor_axis[b] = 0;
        blob_probability[b] = 0;
        is_dot[b] = false;
    }

    // Loop through bw image line-by-line
    int blob_track = -1;
    for(int r = 0; r < ROI.height; r++)  // Iterate over rows
    {
        for(int c = 0; c < ROI.width; c++)  // Iterate over columns
        {
            if( (int)(bw[r][c]) > 0 )  // Handle "true" pixel
            {
                if(blob_track == -1)  // Case "not previously tracking"
                {
                    // Determine which blob from the previous line the current pixel belongs to
                    blob_track = matchBlob(c, r, 0);

                    if(blob_track == -1)  // No matching blob is identified
                    {
                        // Find first zero value in blob_area
                        for(int b = 0; b < max_num_blobs; b++)
                        {
                            if(blob_area[b] == 0)
                            {
                                blob_track = b;
                                break;
                            }
                        }
                        if(blob_track == -1)
                        {
                            blob_track = max_num_blobs;
                        }

                        // A new blob is being tracked
                        num_blobs++;

                        // Initialize core blob arrays to track a new blob
                        blob_bound[0][blob_track] = c;
                        blob_bound[1][blob_track] = c;
                        blob_bound[2][blob_track] = r;

                        blob_area[blob_track] = 1;

                        blob_centroid[0][blob_track] = c;
                        blob_centroid[1][blob_track] = r;

                        bounding_box[0][blob_track] = c;
                        bounding_box[1][blob_track] = c;
                        bounding_box[2][blob_track] = r;
                        bounding_box[3][blob_track] = r;
                    }
                    else  // Matching blob is identified
                    {
                        // An old blob is being tracked
                        blob_bound[1][blob_track] = c;
                        blob_bound[2][blob_track] = r;

                        blob_eccentricity[blob_track] = 0;

                        blob_area[blob_track]++;

                        blob_centroid[0][blob_track] = (  ( ( (double)blob_area[blob_track]/((double)blob_area[blob_track] + 1) )*(double)blob_centroid[0][blob_track] ) + ( (double)c/((double)blob_area[blob_track] + 1) )  );
                        blob_centroid[1][blob_track] = (  ( ( (double)blob_area[blob_track]/((double)blob_area[blob_track] + 1) )*(double)blob_centroid[1][blob_track] ) + ( (double)r/((double)blob_area[blob_track] + 1) )  );

                        updateBoundingBox(blob_track, c, r);
                    }
                }
                else  // Case "previously tracking"
                {
                    // Tally area, centroid, etc.
                    blob_bound[1][blob_track] = c;
                    blob_bound[2][blob_track] = r;

                    blob_area[blob_track]++;

                    blob_centroid[0][blob_track] = (  ( ( (double)blob_area[blob_track]/((double)blob_area[blob_track] + 1) )*(double)blob_centroid[0][blob_track] ) + ( (double)c/((double)blob_area[blob_track] + 1) )  );
                    blob_centroid[1][blob_track] = (  ( ( (double)blob_area[blob_track]/((double)blob_area[blob_track] + 1) )*(double)blob_centroid[1][blob_track] ) + ( (double)r/((double)blob_area[blob_track] + 1) )  );

                    updateBoundingBox(blob_track, c, r);

                    // If bounds of an existing blob are encountered, then merge the two blobs
                    found = matchBlob(c, r, 0);
                    if(found != -1)
                    {
                        mergeBlobs(blob_track, found);
                        blob_track = found;
                    }
                }
            }
            else  // Handle "false" pixel
            {
                blob_track = -1;
            }
        }
    }

    // Perform computations which require some blob vectors to be populated first
    postProcess();

    debugHandle(1, "blobAnalysis()");
    return;
}


/*
  Determine which "blobs" are valid "dots" and store their position in a sorted array.

  Pseudocode statement:
    1. If isFirstRun, populate the bottom of positionField_ref with blob centroids whose indices map to true values
         in is_dot
    2. If !isFirstRun,

  @input destPathName = file for analyzed image to be output to, for user feedback loop
  @input isFirstRun = switch which causes position field to be stored as the reference position field (only
    the first image)

  @output positionField_ref = reference position field
  @output positionField = position field of current image
  */
void Correlation::extractPositionField(bool isFirstRun) {

    debugHandle(0, "extractPositionField()");

    if(isFirstRun)
    {
        // Declare variables
        string dotsIdentified("n"), option("");
        min_blob_area = 10000;
        max_blob_area = -1;
        int old_b(0);
        bool found_dot = false;
        pfRefRectangle.x = 10000;
        pfRefRectangle.y = 10000;
        pfRefRectangle.width = -10001;
        pfRefRectangle.height = -10001;

        // Calculate probability that each blob is a dot
        blobProbability();

        // Identify each blob as a dot if the probability is above some threshold
        identifyDots();

        // Store dot locations in the positionField_ref array
        //   Note that this nested for-loop groups all valid points to the lowest array indices
        old_b = 0;
        for(int p = 0; p < max_num_blobs; p++)
        {
            found_dot = false;
            for(int b = old_b; b < max_num_blobs; b++)
            {
                if(is_dot[b])
                {

                    // Assign values to reference position field
                    positionField_ref[0][p] = blob_centroid[0][b];
                    positionField_ref[1][p] = blob_centroid[1][b];

                    // Keep track of the smallest and larges values
                    double x1 = pfRefRectangle.x + pfRefRectangle.width;
                    double y1 = pfRefRectangle.y + pfRefRectangle.height;
                    pfRefRectangle.x = minIgnore( pfRefRectangle.x, positionField_ref[0][p] );
                    pfRefRectangle.y = minIgnore( pfRefRectangle.y, positionField_ref[1][p] );
                    pfRefRectangle.width = -pfRefRectangle.x + maxIgnore( x1, positionField_ref[0][p] );
                    pfRefRectangle.height = -pfRefRectangle.y + maxIgnore( y1, positionField_ref[1][p] );


//                    pfRefRectangle.width = -pfRefRectangle.x + maxIgnore( pfRefRectangle.x + pfRefRectangle.width, positionField_ref[0][b] );
//                    pfRefRectangle.height = -pfRefRectangle.y + maxIgnore( pfRefRectangle.y + pfRefRectangle.height, positionField_ref[1][b] );
//                    pfRefRectangle.x = minIgnore( pfRefRectangle.x, positionField_ref[0][b] );
//                    pfRefRectangle.y = minIgnore( pfRefRectangle.y, positionField_ref[1][b] );

                    old_b = b + 1;
                    found_dot = true;
                    break;
                }
            }

            // Clear out all values not associated with points
            if(!found_dot)
            {
                positionField_ref[0][p] = -1;
                positionField_ref[1][p] = -1;
            }

            // Clear out all values in the position field array
            positionField[0][p] = -1;
            positionField[1][p] = -1;

        }
    } else {

        // Declare variables
        double min_dist(0);
        double dist(0);
        int blobIndex = -1;
        bool hasBeenFound[max_num_blobs] = {false};
        pfRectangle.x = 10000;
        pfRectangle.y = 10000;
        pfRectangle.width = -10001;
        pfRectangle.height = -10001;

        // Calculate probability that each blob is a dot
        //   Populate blobProbability
        blobProbability();

        // Identify each blob as a dot if the probability is above some threshold
        //   Populate isDot
        identifyDots();

        // Populate positionField with dots, by finding the dot which is closest to the point in either the previous
        //   image or, if the point has been lost,  ...........
        // Pseudocode statement: Populate (point p) with (blob b)'s location, where (blob b) is found by iterating
        //   over all blobs and identifying the one closest to the previous (point p) location
        //
        // Note: By design, indices of points in the reference field correspond to those for the deformed position
        //   field.
        for(int p = 0; p < num_dots; p++)
        {
            // Ignore points not considered to be dot locations
            if(positionField_ref[0][p] == -1) {
                positionField[0][p] = -1;
                positionField[1][p] = -1;
                continue;
            }

            min_dist = 10000;
            for(int b = 0; b < max_num_blobs; b++)
            {
                // Only consider blob locations that have been identified as dots
                if(!is_dot[b]) {
                    continue;
                }

                if(positionField[0][p] != -1) {  // Handle points which migrate between time steps

                    // Calculate distance
                    dist = sqrt( pow(blob_centroid[0][b] - positionField[0][p], 2) + pow(blob_centroid[1][b] - positionField[1][p], 2) );
//                    cout << " extractPF() dist:   " << dist;

                    // Store the dot location closest to the p'th position (from previous image)
                    if(dist < min_dist && dist <= migration_dist_thresh && !hasBeenFound[b])
                    {
//                        cout << "   success ";
                        // Store index of correct blob
                        blobIndex = b;

                        // Update smallest distance found
                        min_dist = dist;

                    }
//                    cout << endl;

                } else {  // Handle dropped points

                    //   Note that is_dot does not contain information about dropped points in previous time step

                    // Calculate distance
                    dist = sqrt( pow(blob_centroid[0][b] - positionField_ref[0][p], 2) + pow(blob_centroid[1][b] - positionField_ref[1][p], 2) );
//                    cout << " DROPPED POINT  " << p << " extractPF() dist:   " << dist << endl;

                    // Store the dot location closest to the p'th position (from reference image)
                    if(dist < min_dist && dist <= migration_dist_thresh && !hasBeenFound[b])
                    {
                        // Store index of correct blob
                        blobIndex = b;

                        // Update smallest distance found
                        min_dist = dist;
                    }
                }
            }

            // Handle failure to identify closest dot
            if(min_dist == 10000) {
                positionField[0][p] = -1;
                positionField[1][p] = -1;
                cout << "POINT " << p << " IS BEING DROPPED" << endl;
            } else {

                // Assign values to position field
                positionField[0][p] = blob_centroid[0][blobIndex];
                positionField[1][p] = blob_centroid[1][blobIndex];

//                cout << "extractPositionField() following point is valid: (area, x, y)    " << blob_area[blobIndex] << "    " << positionField[0][p] << " " << positionField[1][p] << endl;

                // Mechanism to ensure no more than one blob is stored as a point
                hasBeenFound[blobIndex] = true;

                // Keep track of the smallest and larges values
                double x1 = pfRectangle.x + pfRectangle.width;
                double y1 = pfRectangle.y + pfRectangle.height;
                pfRectangle.x = minIgnore( pfRectangle.x, positionField[0][p] );
                pfRectangle.y = minIgnore( pfRectangle.y, positionField[1][p] );
//  pseudocode statemtent           pfRectangle.x1 = maxIgnore( x1, positionField[0][b] );
                pfRectangle.width = -pfRectangle.x + maxIgnore( x1, positionField[0][p] );
                pfRectangle.height = -pfRectangle.y + maxIgnore( y1, positionField[1][p] );

//                        pfRectangle.width = -pfRectangle.x + maxIgnore( pfRectangle.x + pfRectangle.width, positionField[0][b] );
//                        pfRectangle.height = -pfRectangle.y + maxIgnore( pfRectangle.y + pfRectangle.height, positionField[1][b] );





//old algorithm
//                pfRectangle.width = -pfRectangle.x + maxIgnore( pfRectangle.x + pfRectangle.width, positionField[0][p] );
//                pfRectangle.height = -pfRectangle.y + maxIgnore( pfRectangle.y + pfRectangle.height, positionField[1][p] );
//                pfRectangle.x = minIgnore( pfRectangle.x, positionField[0][p] );
//                pfRectangle.y = minIgnore( pfRectangle.y, positionField[1][p] );


            }

        }




//        for(int b = 0; b < max_num_blobs; ++b) {
//            if(is_dot[b]) {
//                cout << "is_dot true, blob centroid :    " << blob_centroid[0][b] << " " << blob_centroid[1][b] << endl;
//            }
//        }



        // Analyze the distances as statistical values; eliminate outliers (assuming displacement field will generally
        //   be small), or simply threshold the distances

        // Result of this is a position field which is similar to the previous position field
    }









//    if(!isFirstRun) {
//        cout << "extractPF() -> num_dots = " << num_dots << endl;
//        for(int i = 0; i < 10; ++i) {
//            cout << positionField[0][i] << " " << positionField[1][i] << endl;
//        }
//        cout << endl;
//        cout << endl;
//    }



//    cout << "thresholding params" << endl;
//    cout << min_blob_area << " " << max_blob_area << endl;
//    cout << "blob_dist_thresh"	 << "  " << 	blob_dist_thresh	 << endl;
//    cout << "area_thresh"	 << "  " << 	area_thresh	 << endl;
//    cout << "migration_dist_thresh"	 << "  " << 	migration_dist_thresh	 << endl;
//    cout << "num_blobs"	 << "  " << 	num_blobs	 << endl;
//    cout << "blob_dist_thresh"	 << "  " << 	blob_dist_thresh	 << endl;
//    cout << "area_thresh"	 << "  " << 	area_thresh	 << endl;
//    cout << "num_dots"	 << "  " << 	num_dots	 << endl;
//    cout << "dotProbabilityThresh"	 << "  " << 	dotProbabilityThresh	 << endl;
//    cout << endl;

////    blob_probability[max_num_blobs]
////    is_dot[max_num_blobs];



//    int key = -1;
//    while(key == -1) { key = cvWaitKey(10); }


//    for(int p; p < num_dots; ++p) {
//        cout << positionField[0][p] << endl;
//    }
//    cout << endl;
//    for(int p; p < num_dots; ++p) {
//        cout << epsxx[p] << endl;
//    }
//    cout << endl;




    debugHandle(1, "extractPositionField()");
    return;
}


/*
  Generate a sorted list of points which may be used by each point for the nearest-neighbor strain calcuation

  @input positionField_ref = array of point locations in reference image

  @output gridMap = list of indices of neighboring points for each point, sorted by distance.

      Consider gridMap[n][p].
        Index p refers to point index p. This maps to points in positionField and strainField.
        Index n refers to the nth nearest neighbor to point p. N acts as a point index, and can act on positionField
          and strainField.
  */
void Correlation::generateGridMap()
{
    debugHandle(0, "generateGridMap()");

    // This function should generate a list of the max_neighbor_count neighboring points for each point, in order of
    //   proximity (closest to farthest). Note that the points stored in gridMap will be used selectively, not
    //   exhaustively, in the strain calculation.

    // Pseudocode statement: For the point p, find a neighboring point n.

    // Map each point p to the closest neighboring points n_match
    for(int p = 0; p < max_num_blobs; p++)
    {
        // Ignore invalid points, stored as the location (-1, -1)
        if(positionField_ref[0][p] == -1) {

            // Populate gridMap with -1 at level p to indicate unidentified points
            for(int n = 0; n < max_neighbor_count; n++) {
                gridMap[n][p] = -1;
            }
            continue;

        }

        // Populate only max_neighbor_count neighbors
        int n_matched = -1;
        bool valid_match(true);
        for(int n = 0; n < max_neighbor_count; n++)
        {
            // Find the next closest point, with index n_matched
            double min_dist = 10000;
            n_matched = -1;
            for(int n_match = 0; n_match < max_num_blobs; n_match++)
            {
                // Explicitly disallow a point to map onto itself (useless); disallow invalid points
                if(n_match == p || positionField_ref[0][n_match] == -1) {
                    continue;
                }

                // Calculate distance and find minimum distance
                double dist = sqrt( pow(positionField_ref[0][n_match] - positionField_ref[0][p], 2) + pow(positionField_ref[1][n_match] - positionField_ref[1][p], 2) );
                if(dist < min_dist && positionField_ref[0][n_match] != -1 && positionField_ref[0][p] != -1) {

                    // Ensure no duplicate points are found
                    valid_match = true;
                    for(int n_check = 0; n_check < n; n_check++) {
                        if(n_match == gridMap[n_check][p]) {
                            valid_match = false;
                        }
                    }

                    // Store closest neighbor match
                    if(valid_match) {
                        min_dist = dist;
                        n_matched = n_match;
                    }
                }
            }

            // Store this next closest point
            gridMap[n][p] = n_matched;
        }
    }

    debugHandle(1, "generateGridMap()");
    return;
}


/*
  Compute the strain from positionField and positionField_ref.

  @input positionField_ref = list of points in reference frame
  @input positionField = list of points in current frame
  */
void Correlation::computeStrain()
{
    debugHandle(0, "computeStrain()");

    // This function should compute the strain for every combination of neighboring points which satisfies the following
    //   conditions:
    //   1. The neighboring points are associated with the point in question via gridMap
    //   2. The connecting lines between the strain calculation point and its neighbors form an angle greater than some
    //      threshold value (single-calculation accuracy requirement)
    //   3. All combinations of points are the closest points possible
    // This function should then combine the results (multiple-calculation accuracy requirement), for example by a
    //   weighted average.

    // Initialize
    double pt[2][3];
    double ptStar[2][3];

    // Compute strain for each point p
    for(int p = 0; p < num_dots; p++) {

        // Zero-out the strain vectors
        epsxx[p] = 0;
        epsxy[p] = 0;
        epsyx[p] = 0;
        epsyy[p] = 0;

        // Do not compute the strain for invalid points
        if(positionField_ref[0][p] == -1) {
            continue;
        }

        // Clear out temporary strain array
        int count = 0;
        for(int c = 0; c < calculation_count; c++) {
            epsCollection[0][c] = 0;
            epsCollection[1][c] = 0;
            epsCollection[2][c] = 0;
            epsCollection[3][c] = 0;
        }

        // Employ every neighbor 2-combination to calculate the strain
        for(int n1 = 0; n1 < max_neighbor_count; n1++) {

            for(int n2 = 0; n2 < max_neighbor_count; n2++) {

                // Ensure selected neighbors are not the same neighbor
                if(n1 == n2) {   // || n1 == p || n2 == p) {
                    continue;
                }

                // Do not consider an invalid neighbor. Note that all values in gridMap not equal to -1 have been
                //   ensured to map to valid points in positionField_ref by the generateGridMap() function.
                if(gridMap[n1][p] == -1 || gridMap[n2][p] == -1) {
                    continue;
                }

                // Formulate the point set to be used in the strain calculation
                pt[0][0] = positionField_ref[0][p];
                pt[0][1] = positionField_ref[0][ gridMap[n1][p] ];
                pt[0][2] = positionField_ref[0][ gridMap[n2][p] ];
                pt[1][0] = positionField_ref[1][p];
                pt[1][1] = positionField_ref[1][ gridMap[n1][p] ];
                pt[1][2] = positionField_ref[1][ gridMap[n2][p] ];

                ptStar[0][0] = positionField[0][p];
                ptStar[0][1] = positionField[0][ gridMap[n1][p] ];
                ptStar[0][2] = positionField[0][ gridMap[n2][p] ];
                ptStar[1][0] = positionField[1][p];
                ptStar[1][1] = positionField[1][ gridMap[n1][p] ];
                ptStar[1][2] = positionField[1][ gridMap[n2][p] ];

                // Check that this set of points does not violate the min_direction_angle criterion
                if(!isValidPointSet(pt, ptStar, p, n1, n2)) {
                    continue;
                }



//                //debug:
//                // 1. Show vectors in imgProc.
//                // 2. wait for keypress
//                //
//                // note: window initialization done externally.
//                if(debugParam) {

//                    // Output image with generalize visualizations
//                    outputImage();

//                    // Place vectors in image
//                    CvPoint pt0 = cvPoint( (int)ptStar[0][0], (int)ptStar[1][0] );
//                    CvPoint pt1 = cvPoint( (int)ptStar[0][1], (int)ptStar[1][1] );
//                    CvPoint pt2 = cvPoint( (int)ptStar[0][2], (int)ptStar[1][2] );
//                    CvScalar col = cvScalar(255, 0, 0);
//                    cvDrawLine(imgProc.imgp, pt0, pt1, col, 1);
//                    cvDrawLine(imgProc.imgp, pt0, pt2, col, 1);
//                }


                // Calculate strain at point p using neighbors n1, n2. Store in temporary strain calculation array.
                //   Note: The rightmost array dimension maps the the inner-most elements
                localStrain(pt, ptStar, count, 0);

                // Ensure no more than calculation_count strains are computed for each point p
                count++;
                if(count >= calculation_count) {
                    count = calculation_count;
                    break;
                }

            }

            // Ensure no more than calculation_count strains are computed for each point p
            if(count >= calculation_count) {
                count = calculation_count;
                break;
            }

        }

        // Combine the results
        //   Minimum value
        epsxx[p] = epsCollection[0][0];
        epsxy[p] = epsCollection[1][0];
        epsyx[p] = epsCollection[2][0];
        epsyy[p] = epsCollection[3][0];
        for(int c = 1; c < count; ++c) {

            epsxx[p] = minAbs(epsxx[p], epsCollection[0][c]);
            epsxy[p] = minAbs(epsxy[p], epsCollection[1][c]);
            epsyx[p] = minAbs(epsyx[p], epsCollection[2][c]);
            epsyy[p] = minAbs(epsyy[p], epsCollection[3][c]);
        }
        //   Average
//        epsxx[p] /= count;
//        epsxy[p] /= count;
//        epsyx[p] /= count;
//        epsyy[p] /= count;
        //for(c)
        //            epsxx[p] += epsCollection[0][c];
        //            epsxy[p] += epsCollection[1][c];
        //            epsyx[p] += epsCollection[2][c];
        //            epsyy[p] += epsCollection[3][c];

        // Store global minimum strains
        globalMinStrain = minIgnore(globalMinStrain, epsxx[p]);
        globalMinStrain = minIgnore(globalMinStrain, epsxy[p]);
        globalMinStrain = minIgnore(globalMinStrain, epsyx[p]);
        globalMinStrain = minIgnore(globalMinStrain, epsyy[p]);

        // Store global maximum strains
        globalMaxStrain = maxIgnore(globalMaxStrain, epsxx[p]);
        globalMaxStrain = maxIgnore(globalMaxStrain, epsxy[p]);
        globalMaxStrain = maxIgnore(globalMaxStrain, epsyx[p]);
        globalMaxStrain = maxIgnore(globalMaxStrain, epsyy[p]);

    }






    //debug: display strain values for order-of-magnitude study
//    for(int p = 0; p < num_dots; ++p) {
//        cout << epsxx[p] << " " << epsxy[p] << " " << epsyx[p] << " " << epsyy[p] << endl;
//    }
//    cout << endl;

    debugHandle(1, "computeStrain()");
    return;
}


/*
  Draw representations of which blobs have been detected in the image. Draw in imgProc.

  @input showInWindow = Use true to show in a default window, "Processed Image." Use false to avoid this and manually
    display image later with cvShowImage().
  @input switchIsDot = default false displays information about all blobs, true only shows blobs which have been
    identified as dots
  */
void Correlation::drawCurrentState(GreyImage src, RgbImage dst, bool switchIsDot) {

    // Merge src into dst, so true pixels are white, false are black
    if(!dst.imgp || dst.imgp->width != src.imgp->width || dst.imgp->height != src.imgp->height || dst.imgp->depth != src.imgp->depth) {
        dst.imgp = cvCreateImage(cvSize(src.imgp->width, src.imgp->height), src.imgp->depth, 3);
    }
    cvMerge(src.imgp, src.imgp, src.imgp, NULL, dst.imgp);

    // Mark blob properties as appropriate
    for(int b = 0; b < max_num_blobs; ++b) {

        if(is_dot[b]) {
            if( !switchIsDot || (switchIsDot && is_dot[b]) ) {
                drawBlobProperty(dst, 7, b, 0, 255, 0);
                drawBlobProperty(dst, 6, b, 0, 0, 255);
            }
        }

    }

    return;
}


/*
  Subset an image using member CvRect ROI

  @input src = Source image

  @output RgbImage = Destination image, with dimensions (ROI.width, ROI.height)
  */
void Correlation::subsetImage(RgbImage& src, RgbImage& dst) {

    // Initialize
    dst.imgp = cvCreateImage(cvSize(ROI.width, ROI.height), src.imgp->depth, src.imgp->nChannels);
    int ii(0), jj(0);

    // Fill dst with ROI of src
    for(int i = ROI.y; i < ROI.y + ROI.height; ++i) {
        jj = 0;
        for(int j = ROI.x; j < ROI.x + ROI.width; ++j) {

            if(jj > dst.imgp->width) {
                cout << "INDEX OUT OF BOUNDS " << jj << " " << dst.imgp->width << endl;
                break;
            }

            // Push each pixel from full image into subset image
            dst[ii][jj].c1 = src[i][j].c1;
            dst[ii][jj].c2 = src[i][j].c2;
            dst[ii][jj].c3 = src[i][j].c3;

            jj++;
        }
        ii++;
    }

    return;
}


/*
  Threshold imgIn with all 12 thresholds to generate bw

  @input imgInLocal = Image to be filtered. Typically this should be member image imgIn, after this has been acquired
    from imgInFull by subsetImage().

  @output bw = black-and-white image after thresholding (single-channel)
  */
void Correlation::colorFilter(IplImage *src, IplImage **dst) {

    // Initialize bw if neccessary
//    if(!dst || dst->width != src->width || dst->height != src->height) {
        *dst = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
//    }


    // Initialize local structs
    IplImage *imgC1 = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
    IplImage *imgC2 = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
    IplImage *imgC3 = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
    IplImage *srcHSV = cvCreateImage(cvSize(src->width, src->height), src->depth, 3);

    // Split image into components
    cvSplit(src, imgC1, imgC2, imgC3, NULL);

    // Threshold on BGR
    cvInRangeS(imgC1, cvScalar(B_lower), cvScalar(B_upper), imgC1);
    cvInRangeS(imgC2, cvScalar(G_lower), cvScalar(G_upper), imgC2);
    cvInRangeS(imgC3, cvScalar(R_lower), cvScalar(R_upper), imgC3);
    cvAnd(imgC1, imgC2, *dst);
    cvAnd(imgC3, *dst, *dst);

    // Convert to HSV
    cvCvtColor(src, srcHSV, CV_BGR2HSV);

    // Split image into components
    cvSplit(srcHSV, imgC1, imgC2, imgC3, NULL);

    // Threshold on HSV
    cvInRangeS(imgC1, cvScalar(H_lower), cvScalar(H_upper), imgC1);
    cvInRangeS(imgC2, cvScalar(S_lower), cvScalar(S_upper), imgC2);
    cvInRangeS(imgC3, cvScalar(V_lower), cvScalar(V_upper), imgC3);
    cvAnd(*dst, imgC1, *dst);
    cvAnd(*dst, imgC2, *dst);
    cvAnd(*dst, imgC3, *dst);

    // Release local images to clear memory
    cvReleaseImage(&imgC1);
    cvReleaseImage(&imgC2);
    cvReleaseImage(&imgC3);
    cvReleaseImage(&srcHSV);

    return;
}


/*
  Extract subimage from img, under specified transformation

  Mathematically defined parameters:

      x, y       = coordinates in reference image imgRef (pixels)

      xD, yD     = coordinates in deformed image imgIn (pixels)

      tfm.u .v .ux .uy .vx .vy = transformation parameters (pixels)

  Subimage destination coordinates - iterate over these to populate the subimage;
  This requires algorithm to ignore the transformation tfm:

      xDst, yDst = coordinates in subimage, dst (pixels)

  @intput src = source image (expects RgbImage with .imgp pointing to IplImage with 8UC3 type)
  @input gridElementROI = pointer to CvRect with values of a rectangle in src, units of pixels
  @input tfm = transformation which defines how returned image should compare to gridElementROI

  @output GreyImageFloat = wrapped IplImage with type 32FC1, transormed from gridElementROI
  */
GreyImageFloat Correlation::extractSubImage(RgbImage src, CvRect *gridElementROI, Transformation tfm) {

    // Initialize
    GreyImageFloat dst;
    dst.imgp = cvCreateImage(cvSize( gridElementROI->width, gridElementROI->height ), IPL_DEPTH_32F, 1);

    int x = gridElementROI->x + 0.5*gridElementROI->width;
    int y = gridElementROI->y + 0.5*gridElementROI->height;

    // Iterate over all points in gridElementROI
    int yDst = 0;
    for(int dy = -0.5*gridElementROI->height; dy < 0.5*gridElementROI->height; ++dy) {
        int xDst = 0;
        for(int dx = -0.5*gridElementROI->width; dx < 0.5*gridElementROI->width; ++dx) {

            // Calculate deformed coordinates
            int xD = x + tfm.u + tfm.ux * dx + tfm.uy * dy;
            int yD = y + tfm.v + tfm.vx * dx + tfm.vy * dy;

            // Disallow out-of-bounds errors
            if( xDst >=0 && xDst < dst.imgp->width && yDst >= 0 && yDst < dst.imgp->height \
                    && xD >=0 && xD < src.imgp->width && yD >= 0 && yD < src.imgp->height ) {

                dst[yDst][xDst] = (float)pow( pow(scale8U232F*(float)src[yD][xD].c1, 2) + \
                                               pow(scale8U232F*(float)src[yD][xD].c2, 2) + \
                                               pow(scale8U232F*(float)src[yD][xD].c3, 2), 0.5 );
            } else {
                dst[yDst][xDst] = (float)0.0;
            }
            ++xDst;
        }
        ++yDst;
    }

    return dst;
}


/*
  */
Transformation Correlation::gradCOF(Transformation tfm) {

    Transformation tfmGradient;

    return tfmGradient;
}


/*
  */
Transformation Correlation::hessianCOF(Transformation tfm) {

    Transformation tfmHessian;

    return tfmHessian;
}


/*
  Compute correlation cofficient

  */
float Correlation::evaluateCOF(GreyImageFloat gridImgRef, GreyImageFloat gridImg, CofType cofType) {

    float cof = 0;

    for(int j = 0; j < gridImgRef.imgp->height; ++j) {
        for(int i = 0; i < gridImgRef.imgp->width; ++i) {

            if(cofType == SSD_SIMPLE) {
                cof += (float)pow( (double)gridImg[j][i] - (double)gridImgRef[j][i], 2 );
            } else {
                cof += (float)pow( (double)gridImg[j][i] - (double)gridImgRef[j][i], 2 );
            }

        }
    }

    return cof;
}


/*
  Determine if pixel (r,c) is connected to a blob which is already being tracked.

  @input r = row of pixel being considered
  @input c = column of pixel being considered
  @input criteria = specification of matching criteria (currently the only method implemented calls diagonally touching
    pixels part of the same blob)

  @output int = index of blob which pixel (r,c) is connected to, or -1 if no matching blob is found
  */
int Correlation::matchBlob(int c, int r, int criteria)
{
    debugHandle(0, "matchBlob()");

    // The first row can only contain new blobs
    if(r == 0)
    {
        return -1;
    }

    // Return -1 if no match is found; else, return the index of the match
    int returnInt = -1;

    if(criteria == 0)  // Matching algorithm 0
    {
        for(int b = 0; b < max_num_blobs; b++)
        {
            // Allow only pixels that touch or are diagonal
            if( c+1 >= blob_bound[0][b] && c-1 <= blob_bound[1][b] && r == blob_bound[2][b] + 1)
            {
                returnInt = b;
                break;
            }
        }
    }

    debugHandle(1, "matchBlob()");

    // Return value
    return returnInt;
}


/*
  Increase the bounding box for a blob to include pixel (r,c)

  @input blob_track = index of blob currently being considered
  @input r = row of pixel
  @input c = column of pixel

  @output bounding_box = updated blob array which encloses both the blob and the pixel
  */
void Correlation::updateBoundingBox(int blob_track, int c, int r)
{
    debugHandle(0, "updateBoundingBox()");

    bounding_box[0][blob_track] = minIgnore( (double)c, bounding_box[0][blob_track] );
    bounding_box[1][blob_track] = maxIgnore( (double)c, bounding_box[1][blob_track] );
    bounding_box[2][blob_track] = minIgnore( (double)r, bounding_box[2][blob_track] );
    bounding_box[3][blob_track] = maxIgnore( (double)r, bounding_box[3][blob_track] );

    debugHandle(1, "updateBoundingBox()");
    return;
}


/*
  Given two blobs, merge their information so that one blob is being tracked.

  @input fromBlob = index of blob to be eliminated
  @input toBlob = index of blob to store combined results into

  @output Core blob analysis arrays, which have been updated to include the merged blobs
  */
void Correlation::mergeBlobs(int fromBlob, int toBlob)
{
    debugHandle(0, "mergeBlobs()");

    // Update information for the toBlob
    num_blobs--;

    blob_bound[0][toBlob] = blob_bound[0][fromBlob];
    blob_bound[1][toBlob] = blob_bound[1][fromBlob];
    blob_bound[2][toBlob] = blob_bound[2][fromBlob];

    blob_eccentricity[toBlob] += blob_eccentricity[fromBlob];

    blob_centroid[0][toBlob] = ( (double)blob_centroid[0][fromBlob] * (double)blob_area[fromBlob] + (double)blob_centroid[0][toBlob] * (double)blob_area[toBlob] ) / ( (double)blob_area[fromBlob] + (double)blob_area[toBlob] );
    blob_centroid[1][toBlob] = ( (double)blob_centroid[1][fromBlob] * (double)blob_area[fromBlob] + (double)blob_centroid[1][toBlob] * (double)blob_area[toBlob] ) / ( (double)blob_area[fromBlob] + (double)blob_area[toBlob] );

    blob_area[toBlob] += blob_area[fromBlob];

    bounding_box[0][toBlob] = minIgnore( bounding_box[0][fromBlob], bounding_box[0][toBlob] );
    bounding_box[1][toBlob] = maxIgnore( bounding_box[1][fromBlob], bounding_box[1][toBlob] );
    bounding_box[2][toBlob] = minIgnore( bounding_box[2][fromBlob], bounding_box[2][toBlob] );
    bounding_box[3][toBlob] = maxIgnore( bounding_box[3][fromBlob], bounding_box[3][toBlob] );

    // Set information for fromBlob to zero
    blob_bound[0][fromBlob] = 0;
    blob_bound[1][fromBlob] = 0;
    blob_bound[2][fromBlob] = 0;
    blob_area[fromBlob] = 0;
    blob_eccentricity[fromBlob] = 0;
    blob_centroid[0][fromBlob] = 0;
    blob_centroid[1][fromBlob] = 0;
    bounding_box[0][fromBlob] = -1;
    bounding_box[1][fromBlob] = -1;
    bounding_box[2][fromBlob] = -1;
    bounding_box[3][fromBlob] = -1;

    debugHandle(1, "mergeBlobs()");
    return;
}


/*
  Perform calculations on core blob arrays which require entire image to be processed first.

  This function does modify the core blob arrays, and therefore can only be called iteratively with the understanding
    that information is lost.

  @input Core blob arrays

  @output Core blob arrays, fully populated
  */
void Correlation::postProcess()
{
    debugHandle(0, "postProcess()");

    // Notify entry to function if debug2 is enabled
    if(enable_debug == 2) {
        std::cout << "Running postProcess()" << endl;
    }

    // Initialize
    bool moreMerges = true;
    bool breakout = false;
    double dist = 0;

    // Combine blobs that are likely the same blob, by iteratively merging blobs whose centroids are within some
    //   distance of each other, given by blob_dist_thresh
    while(moreMerges)
    {
        moreMerges = false;
        breakout = false;
        for(int b1 = 0; b1 < max_num_blobs; b1++)
        {
            for(int b2 = 0; b2 < max_num_blobs; b2++)
            {
                if(b2 == b1) {
                    continue;
                }

                dist = sqrt( pow(blob_centroid[0][b2] - blob_centroid[0][b1],2) + pow(blob_centroid[1][b2] - blob_centroid[1][b1],2) );

                if(dist <= blob_dist_thresh && blob_area[b1] > 0 && blob_area[b2] > 0)
                {
                    mergeBlobs(b2, b1);
                    moreMerges = true;
                    breakout = true;
                }

                if(breakout) {
                    break;
                }
            }

            if(breakout) {
                break;
            }
        }
    }

    // Calculate quantities that require blobs to be fully identified
    for(int b = 0; b < max_num_blobs; b++)
    {
        // Store major and minor axes for each blob
        major_axis[b] = maxIgnore( bounding_box[1][b] - bounding_box[0][b], bounding_box[3][b] - bounding_box[2][b] );
        minor_axis[b] = minIgnore( bounding_box[1][b] - bounding_box[0][b], bounding_box[3][b] - bounding_box[2][b] );

        // Determine eccentricity for each blob
        if(major_axis[b] == 0)
        {
            blob_eccentricity[b] = 1;
        }
        blob_eccentricity[b] = (double)sqrt(  (double)1 - ( pow(.5*minor_axis[b], 2)/pow(.5*major_axis[b], 2) )  );

        // Determine minimum and maximum areas
        min_blob_area = 1 + minIgnore(min_blob_area-1, blob_area[b]-1);
        max_blob_area = 1 + maxIgnore(max_blob_area-1, blob_area[b]-1);
//        if(b <= num_blobs) {
////            cout << b << " CALCULATING MIN_BLOB_AREA " << min_blob_area  << " " << blob_area[b] << " " <<  endl;
//        }
    }

    debugHandle(1, "postProcess()");
    return;
}


/*
  Determine the probability that each blob is a dot. Takes into account area, should also
    take into account eccentricity.

  @input Core blob arrays

  @output blob_probability = blob array of probability values, ranging from 0.0 to 1.0
  */
void Correlation::blobProbability()
{
    debugHandle(0, "blobProbability()");


    for(int b = 0; b < max_num_blobs; b++)
    {
        if(blob_area[b] == 0) {
            continue;
        }

        if(blob_area[b] > area_thresh)
        {
            blob_probability[b] = 1;
        }
        else
        {
            blob_probability[b] = 0;
        }
    }

    debugHandle(1, "blobProbability()");
    return;
}


/*
  Definitively determine whether each blob is a dot. Take into account blob_probability and potentially other
  information.

  @input blob_probability = 1-D array of probabilities (0 to 1) that each blob is a dot

  @output is_dot = 1-D array of dot status which maps to all blob arrays
  */
void Correlation::identifyDots()
{
    debugHandle(0, "identifyDots()");


    // EDITING NOTE: Currently, the number of dots being tracked is updated every time step. This needs to be changed.


    // Set dot count to zero
    num_dots = 0;

    for(int b = 0; b < max_num_blobs; b++)
    {
        // Identify blob by a sufficiently high probability
        if(blob_probability[b] > dotProbabilityThresh) {
            is_dot[b] = true;
            num_dots++;
        } else {
            is_dot[b] = false;
        }
    }

    debugHandle(1, "identifyDots()");
    return;
}


/*
  Determine if the points in each frame work as valid 3-sets

  @input pt = point set in reference frame (pt[][0] is the point at which strain is being calculated)
  @input ptStar = point set in current frame
  */
bool Correlation::isValidPointSet(double pt[2][3], double ptStar[2][3], int p, int n1, int n2)
{
    debugHandle(0, "isValidPointSet()");

    // Disallow values of -1 (invalid point)
    for(int pi = 0; pi < 3; ++pi) {
        if(pt[0][pi] == -1 || ptStar[0][pi] == -1) {
            return false;
        }
    }

    // Assign pt and ptStar to variables for clarity
    double x0 = pt[0][0];
    double y0 = pt[1][0];
    double x1 = pt[0][1];
    double y1 = pt[1][1];
    double x2 = pt[0][2];
    double y2 = pt[1][2];

    double x0_star = ptStar[0][0];
    double y0_star = ptStar[1][0];
    double x1_star = ptStar[0][1];
    double y1_star = ptStar[1][1];
    double x2_star = ptStar[0][2];
    double y2_star = ptStar[1][2];

    // Calculate angle
    double theta = acos( ( (x1-x0)*(x2-x0) + (y1-y0)*(y2-y0) ) / ( sqrt( pow(x1-x0,2) + pow(y1-y0,2) )*sqrt( pow(x2-x0,2) + pow(y2-y0,2) ) ) );
    double theta_star = acos( ( (x1_star-x0_star)*(x2_star-x0_star) + (y1_star-y0_star)*(y2_star-y0_star) ) / ( sqrt( pow(x1_star-x0_star,2) + pow(y1_star-y0_star,2) )*sqrt( pow(x2_star-x0_star,2) + pow(y2_star-y0_star,2) ) ) );

    // Calculate distance
    double dist01 = sqrt( pow(x1-x0,2) + pow(y1-y0,2) );
    double dist02 = sqrt( pow(x2-x0,2) + pow(y2-y0,2) );
    double dist01_star = sqrt( pow(x1_star-x0_star,2) + pow(y1_star-y0_star,2) );
    double dist02_star = sqrt( pow(x2_star-x0_star,2) + pow(y2_star-y0_star,2) );

    debugHandle(1, "isValidPointSet()");

    // Check that angle in reference and deformed configurations are within tolerance
    if(theta > min_direction_angle && theta_star > min_direction_angle \
            && theta < max_direction_angle && theta_star < max_direction_angle \
            && dist01 < max_neighbor_dist && dist02 < max_neighbor_dist \
            && dist01_star < max_neighbor_dist && dist02_star < max_neighbor_dist) {
        return true;
    } else {

//        cout<< "isvalidpointset" << endl;
//        cout << min_direction_angle << " " << theta << " " << max_direction_angle << endl;
//        cout << min_direction_angle << " " << theta_star << " " << max_direction_angle << endl;
//        cout << dist01 << " " << max_neighbor_dist << endl;
//        cout << dist02 << " " << max_neighbor_dist << endl;
//        cout << dist01_star << " " << max_neighbor_dist << endl;
//        cout << dist02_star << " " << max_neighbor_dist << endl;
//        cout << endl;
        return false;
    }
}


/*
  Compute the strain at pt[][0], and store in temporary strain arrays

  @input pt = point in reference frame
  @input ptStar = point in current frame
  @input count = index of strain calculation in temporary arrays
  @input method = strain calculation method; currently only local bilinear displacement field calculation is implemented

  @output epsCollection = array of computed strain values - multiple values of each strain at each point
  */
void Correlation::localStrain(double pt[2][3], double ptStar[2][3], int count, int method)
{
    debugHandle(0, "localStrain()");

    // This function computes the strain at the first point in the pt[][] array, using the specified method; each
    //   method requires a different number of points.
    //
    // Point storage method: pt[][n] = {xn, yn}  -> Convention dictates that the rightmost index is the selector

    // Select method
    if(method == 0) {

        // Method 0: Assume local binlinear displacement field
        double x0 = pt[0][0];
        double y0 = pt[1][0];
        double x1 = pt[0][1];
        double y1 = pt[1][1];
        double x2 = pt[0][2];
        double y2 = pt[1][2];

        double b0 = ptStar[0][0] - pt[0][0];
        double b1 = ptStar[0][1] - pt[0][1];
        double b2 = ptStar[0][2] - pt[0][2];
        double b3 = ptStar[1][0] - pt[1][0];
        double b4 = ptStar[1][1] - pt[1][1];
        double b5 = ptStar[1][2] - pt[1][2];

        double a =  (b0*y1 - b1*y0 - b0*y2 + b2*y0 + b1*y2 - b2*y1)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);
        double b = -(b0*x1 - b1*x0 - b0*x2 + b2*x0 + b1*x2 - b2*x1)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);
        double c =  (b0*x1*y2 - b0*x2*y1 - b1*x0*y2 + b1*x2*y0 + b2*x0*y1 - b2*x1*y0)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);
        double d =  (b3*y1 - b4*y0 - b3*y2 + b5*y0 + b4*y2 - b5*y1)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);
        double e = -(b3*x1 - b4*x0 - b3*x2 + b5*x0 + b4*x2 - b5*x1)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);
        double f =  (b3*x1*y2 - b3*x2*y1 - b4*x0*y2 + b4*x2*y0 + b5*x0*y1 - b5*x1*y0)/(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1);

        epsCollection[0][count] = a;
        epsCollection[1][count] = .5*(d + b);
        epsCollection[2][count] = .5*(b + d);
        epsCollection[3][count] = e;

    } else {

        return;
    }

    debugHandle(1, "localStrain()");
    return;
}


/*
  Return the signed value whose magnitude is smaller
  */
double Correlation::minAbs(double a, double b) {

    if( abs(a) <= abs(b) ) {
        return a;
    } else {
        return b;
    }

    return 0;
}


/*
  Return the signed value whose magnitude is greater
  */
double Correlation::maxAbs(double a, double b) {

    if( abs(a) >= abs(b) ) {
        return a;
    } else {
        return b;
    }

    return 0;
}


/*
  Return the minimum of a and b, but never return -1

  @input a = first number
  @input b = second number

  @output double = min of a and b, excluding -1
  */
double Correlation::minIgnore(double a, double b)
{
    debugHandle(0, "minIgnore()");

    if(b != -1 && a == -1)
    {
        // If one number is == -1, then return the other number
        return b;
    }
    else if(a != -1 && b == -1)
    {
        return a;
    }
    else if(a == -1 && b == -1)
    {
        // If both numbers are == -1, then return 0
        return 0;
    }

    debugHandle(1, "minIgnore()");

    // If neither number is == -1, then return the minimum
    return min(a, b);
}


/*
  Return the maximum of a and b, but never return -1

  @input a = first number
  @input b = second number

  @output double = max of a and b, excluding -1
  */
double Correlation::maxIgnore(double a, double b)
{
    debugHandle(0, "maxIgnore()");

    if(b != -1 && a == -1)
    {
        // If one number is == -1, then return the other number
        return b;
    }
    if(a != -1 && b == -1)
    {
        return a;
    }
    if(a == -1 && b == -1)
    {
        // If both numbers are == -1, then return 0
        return 0;
    }

    debugHandle(1, "maxIgnore()");

    // If neither number is == -1, then return the minimum
    return max(a, b);
}


/*
  Visually represent a requested property for one specific blob in the image. Alters contents of imgProc.imgp.

  @input whichProperty = Integer selecting blob property to be displayed. See switch statement to determine which value
    to use.
  @input blob_track = Index of blob to be displayed.
  @input B, G, R = Color to be used for the appropriate draw function

  @output imgProc = Altered imgProc.imgp with requested blob properties
  */
void Correlation::drawBlobProperty(RgbImage img, int whichProperty, int blob_track, int B, int G, int R) {

    int c, r, cLeft, cRight;
    int b = blob_track;
    CvPoint pt1, pt2;

    switch(whichProperty) {
    case 0:
        cout << num_blobs << endl;
        break;
    case 1:
        cout << found << endl;
        break;
    case 2:
        cout << min_blob_area << " " << max_blob_area << endl;
        break;
    case 3:
        cLeft = blob_bound[0][b];
        cRight = blob_bound[1][b];
        r = blob_bound[2][b];
        img[r][c].c1 = B;
        img[r][c].c2 = G;
        img[r][c].c3 = R;
        img[r][c++].c1 = B;
        img[r][c].c2 = G;
        img[r][c].c3 = R;
        img[r++][c].c1 = B;
        img[r][c].c2 = G;
        img[r][c].c3 = R;
        break;
    case 4:
        cout << blob_area[b] << endl;
        break;
    case 5:
        cout << blob_eccentricity[b] << endl;
        break;
    case 6:

        // Draw blob centroid
        c = blob_centroid[0][b];
        r = blob_centroid[1][b];

        if(c-1 >= 0 && c+1 < img.imgp->width \
                && r-1 >= 0 && r+1 < img.imgp->height) {

            // Draw centroid as a plus sign
            int rr, cc(c);
            for(rr = r-1; rr <= r+1; ++rr) {
                img[rr][cc].c1 = B;
                img[rr][cc].c2 = G;
                img[rr][cc].c3 = R;
            }
            rr = r;
            for(cc = c-1; cc <= c+1; ++cc) {
                img[rr][cc].c1 = B;
                img[rr][cc].c2 = G;
                img[rr][cc].c3 = R;
            }

        } else {

            // Draw centroid as a pixel
            img[r][c].c1 = B;
            img[r][c].c2 = G;
            img[r][c].c3 = R;
        }

        break;
    case 7:
        c = bounding_box[0][b];
        r = bounding_box[2][b];
        pt1 = cvPoint(c, r);
        c = bounding_box[1][b];
        r = bounding_box[3][b];
        pt2 = cvPoint(c, r);
        cvDrawRect(img.imgp, pt1, pt2, cvScalar(B, G, R));
        break;
    case 8:
        cout << major_axis[b] << " " << minor_axis[b] << endl;
        break;
    case 9:
        cout << blob_probability[b] << endl;
        break;
    case 10:
        cout << is_dot[b] << endl;
        break;
    }

    return;
}


/*
  Draw a marker at a point in the image, such that the marker will be clearly visible. This function modifies imgProc.
    Note that no other work is done on imgProc.

  @input c, r = column, row indices to use in image
  @input B, G, R = color to use for the marker

  @output imgProc = Processed image with marker drawn at (r,c)
  */
void Correlation::drawCurrentPixel(RgbImage img, int c, int r, int B, int G, int R) {

    img[r][c].c1 = (unsigned char)B;
    img[r][c].c2 = (unsigned char)G;
    img[r][c].c3 = (unsigned char)R;

    return;
}


/*
  Handle global debug requests. This makes it easier to determine which function is failing.

  @input direction = Indicates whether function is being entered or returned from. Use 0 for entering, 1 for leaving.
    Use any other number to notify the command line that the function has been toggled.

  @input functionName = Name of function being tracked
  */
void Correlation::debugHandle(int direction, string functionName) {

    // Only print out debugging information when enable_debug > 0
    if(enable_debug < 1) {
        return;
    }

    // Display when a function is entered or left
    if(direction == 0) {
        cout << "Entered " << functionName << endl;
    } else if(direction == 1) {
        cout << "Leaving " << functionName << endl;
    } else {
        cout << "Toggling function " << functionName << endl;
    }

    return;
}


/*
  This function contains functionalized code to perform a debugging task - whatever is currently needed.

  This is not a permanent function.

  */
void Correlation::debugFunction() {

    // Show size of gridElementImg
//    cout << gridElementImg.imgp->width << " " << gridElementImg.imgp->height << endl;
//    cout << gridElementWidth << " " << gridElementHeight << endl;


    // Display cof
    cout << cofCurrent << endl;
    // Display contents of gridElementImgRef and gridElementImg
//    cvShowImage("debug0", gridElementImgRef.imgp);
//    cvShowImage("debug1", gridElementImg.imgp);
//    cvWaitKey();



//    // Display gridROI
//    CvPoint pt1 = cvPoint( gridROI.x, gridROI.y );
//    CvPoint pt2 = cvPoint( gridROI.x + gridROI.width, gridROI.y + gridROI.height );
//    CvPoint pt3, pt4;
//    cvDrawRect(imgIn.imgp, pt1, pt2, cvScalar(255, 255, 255), 2);

//    // Display all grid elements in gridContainer
//    for(int row = 0; row < gridContainerHeight; ++row) {
//        for(int col = 0; col < gridContainerWidth; ++col) {

//            gridElementROI = &gridContainer[row][col];

//            pt1 = cvPoint( gridElementROI->x, gridElementROI->y );
//            pt2 = cvPoint( gridElementROI->x + gridElementROI->width, gridElementROI->y + gridElementROI->height );
//            cvDrawRect(imgIn.imgp, pt1, pt2, cvScalar(0, 0, 255), 1);
//        }
//    }

//    // Display all valid points in pfRef
//    int targetOffset = 5;
//    for(int b = 0; b < max_num_blobs; ++b) {
//        if(positionField_ref[0][b] != -1) {
//            // valid point
//            pt1 = cvPoint( positionField_ref[0][b] - targetOffset,  positionField_ref[1][b] - targetOffset);
//            pt2 = cvPoint( positionField_ref[0][b] + targetOffset,  positionField_ref[1][b] + targetOffset);
//            pt3 = cvPoint( positionField_ref[0][b] - targetOffset,  positionField_ref[1][b] + targetOffset);
//            pt4 = cvPoint( positionField_ref[0][b] + targetOffset,  positionField_ref[1][b] - targetOffset);
//            cvDrawLine(imgIn.imgp, pt1, pt2, cvScalar(0, 255, 0), 1);
//            cvDrawLine(imgIn.imgp, pt3, pt4, cvScalar(0, 255, 0), 1);
//        }
//    }

//    // Display image
//    cvNamedWindow("debug", CV_WINDOW_NORMAL);
//    cvShowImage("debug", imgIn.imgp);
//    cvWaitKey();

//    // Cause segfault
//    IplImage *img1 = NULL;
//    cout << img1->width << endl;


    return;

}


/*
  Cause segmentation fault
  */
void Correlation::halt() {

    IplImage *haltImg = NULL;
    haltImg->depth = 1;

    return;
}
