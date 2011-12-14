project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR} bulk gui
modules/SlitCam
modules/ObjectTracker
modules/CascadeDetector
inputs/UsbCam
inputs/VideoFileReader
tools
tools/cvblobs
)


set(markus_SRCS 
gui/markus.cpp
gui/QModuleViewer.cpp 
main.cpp 
bulk/Module.cpp
bulk/ModuleAsync.cpp
bulk/Manager.cpp
bulk/Input.cpp
inputs/UsbCam/UsbCam.cpp
inputs/VideoFileReader/VideoFileReader.cpp
bulk/Parameter.cpp
bulk/OutputStream.cpp
bulk/StreamImage.cpp
bulk/util.cpp
bulk/ConfigReader.cpp
bulk/ImageProcessor.cpp
tools/Tracker.cpp
tools/cvblobs/BlobExtraction.cpp
tools/cvblobs/BlobResult.cpp
tools/cvblobs/Blob.cpp
tools/Detector.cpp
modules/SlitCam/SlitCam.cpp
modules/ObjectTracker/ObjectTracker.cpp
modules/CascadeDetector/CascadeDetector.cpp
)

qt4_automoc(${markus_SRCS})
add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBS} tinyxml)

