#include "CorrelationParent.h"


/*
  Constructor for all derived classes
  */
CorrelationParent::CorrelationParent() {

    /* ----- Initialize Local Variables ------------------------------------------------------------------------------ */

    // Image arrays
    colorSpace = 0;
    colorSpacePrev = 0;


    /* ----- Initialize Global Variables ----------------------------------------------------------------------------- */

    // Object re-initialization
    persistFileString = "/home/david/Documents/correlate/data/persistentValues.dat";

    // Runtime logic
    enable_debug = 0;  // Call function with --enable-debug to output spiffy stuff during analysis
    enable_step = false;  // Pause after outputting image every time step
    image_sequence = -1;  // Flag -is=### runs with pre-stored image sequence number ###

    // Image data
    h = -1;
    w = -1;
    ROI.x = 0;
    ROI.y = 0;
    ROI.width = 100;
    ROI.height = 100;

    // Image filter logic variables
    H_lower = 85;
    H_upper = 170;
    S_lower = 85;
    S_upper = 170;
    V_lower = 85;
    V_upper = 170;

    R_lower = 85;
    R_upper = 170;
    G_lower = 85;
    G_upper = 170;
    B_lower = 85;
    B_upper = 170;

    // Blob analysis logic variables
    num_blobs = 0;
    found = 0;
    min_blob_area = 0;
    max_blob_area = 10000;

    // Post process logic variables
    blob_dist_thresh = 50;  // Static migration distance threshold

    // Position field logic variables
    area_thresh = 25;
    num_dots = 0;
    migration_dist_thresh = 50;

    // Position field data

    // Variables used for generateGridMap()
//    min_direction_angle = 80*M_PI/180;  // Units are radians
//    max_direction_angle = 100*M_PI/180;
    min_direction_angle = 20*M_PI/180;  // Units are radians
    max_direction_angle = 160*M_PI/180;
    max_neighbor_dist = 10000;

    // Grid parameters
    gridROI = cvRect(0, 0, 0, 0);  // Rectangle encompassing entire grid in reference image
//    *gridElementROI = cvRect(0, 0, 0, 0);  // Sub-region of grid


    //
    tfm = {0, 0, 0, 0, 0, 0};


    //
    tolerance.cof = 0.00001;


    //tmp
    t = 0;
    dotProbabilityThresh = 0.9;

    for(int b = 0; b < max_num_blobs; ++b) {
        epsxx[b] = 0;
        epsxy[b] = 0;
        epsyx[b] = 0;
        epsyy[b] = 0;
        positionField_ref[0][b] = -1;
        positionField_ref[1][b] = -1;
        positionField[0][b] = -1;
        positionField[1][b] = -1;
    }

}
