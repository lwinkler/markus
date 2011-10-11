project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}
modules/SlitCam
modules/ObjectTracker
tools
tools/cvblobs
)


set(markus_SRCS 
markus.cpp
QOpenCVWidget.cpp 
main.cpp 
Module.cpp
Manager.cpp
Parameter.cpp
OutputStream.cpp
util.cpp
tools/Tracker.cpp
tools/cvblobs/BlobExtraction.cpp
tools/cvblobs/BlobResult.cpp
tools/cvblobs/Blob.cpp
tools/Detector.cpp
ConfigReader.cpp
modules/SlitCam/SlitCam.cpp
modules/ObjectTracker/ObjectTracker.cpp
)

qt4_automoc(${markus_SRCS})
add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBS} tinyxml)

