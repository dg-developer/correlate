#-------------------------------------------------
#
# Project created by QtCreator 2011-09-25T03:58:47
#
#-------------------------------------------------

QT += core gui

TARGET = correlate

TEMPLATE = app

INCLUDEPATH += . .. $(CORRELATE_HOME)/include $(VISION_HOME)/include

HEADERS += $(VISION_HOME)/include/OpenCvInclude.h \
    $(CORRELATE_HOME)/include/CorrelationParent.h \
    $(CORRELATE_HOME)/include/Correlation.h \
    $(CORRELATE_HOME)/mainwindow.h
#    thresholdingtab.h

SOURCES += $(CORRELATE_HOME)/src/main.cpp \
    $(CORRELATE_HOME)/src/CorrelationParent.cpp \
    $(CORRELATE_HOME)/src/Correlation.cpp \
    $(CORRELATE_HOME)/mainwindow.cpp
#    thresholdingtab.cpp

FORMS += mainwindow.ui
#    thresholdingtab.ui

LIBS += -I/usr/include/opencv -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
