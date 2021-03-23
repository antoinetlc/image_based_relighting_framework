QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IBR_Framework
TEMPLATE = app

win32:{
    #Dlib
    INCLUDEPATH += C:\Libraries\dlib-18.16

    #OpenCV
    CONFIG(debug, debug|release)
    {
         INCLUDEPATH += "C:\\OpenCV2411\\build\\include"

         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_core2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_highgui2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_imgproc2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_features2d2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_calib3d2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_contrib2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_flann2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_gpu2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_legacy2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ml2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_nonfree2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_objdetect2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ocl2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_photo2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_stitching2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_superres2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ts2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_video2411.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_videostab2411.lib"

    }
    CONFIG(release, debug|release)
    {
         INCLUDEPATH += "C:\\OpenCV2411\\build\\include"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_core2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_highgui2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_imgproc2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_features2d2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_calib3d2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_contrib2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_flann2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_gpu2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_legacy2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ml2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_nonfree2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_objdetect2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ocl2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_photo2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_stitching2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_superres2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_ts2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_video2411d.lib"
         LIBS += "C:\\OpenCV2411\\build\\x64\\vc12\\lib\\opencv_videostab2411d.lib"
    }
}
else:unix{
#Dlib
    INCLUDEPATH += /Users/Libraries/dlib-19.0

#OpenCV
    INCLUDEPATH += /usr/local/include/
    LIBS += -L/usr/local/lib
    LIBS += -lopencv_core
    LIBS += -lopencv_imgproc
    LIBS += -lopencv_highgui
    LIBS += -lopencv_ml
    LIBS += -lopencv_video
    LIBS += -lopencv_features2d
    LIBS += -lopencv_calib3d
    LIBS += -lopencv_objdetect
    LIBS += -lopencv_contrib
    LIBS += -lopencv_legacy
    LIBS += -lopencv_flann
}

SOURCES += \
    main.cpp \
    mathsFunctions.cpp \
    LightingBasis.cpp \
    imageProcessing.cpp \
    mainWindow.cpp \
    relighting.cpp \
    progressWindow.cpp \
    voronoi.cpp \
    optimisation.cpp \
    lightStageRelighting.cpp \
    officeRoomRelighting.cpp \
    freeformlightstage.cpp \
    manualSelection.cpp \
    PFMReadWrite.cpp \
    loadFiles.cpp

HEADERS  += \
    PFMReadWrite.h \
    freeformlightstage.h \
    imageProcessing.h \
    LightingBasis.h \
    lightStageRelighting.h \
    loadFiles.h \
    mainWindow.h \
    manualSelection.h \
    mathsFunctions.h \
    officeRoomRelighting.h \
    optimisation.h \
    progressWindow.h \
    voronoi.h \
    relighting.h

