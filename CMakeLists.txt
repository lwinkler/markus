project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR} bulk gui
modules
inputs
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
bulk/Stream.cpp
bulk/StreamImage.cpp
bulk/StreamDebug.cpp
bulk/StreamRect.cpp
bulk/util.cpp
bulk/ConfigReader.cpp
tools/Tracker.cpp
tools/cvblobs/BlobContour.cpp
tools/cvblobs/BlobOperators.cpp
tools/cvblobs/BlobResult.cpp
tools/cvblobs/blob.cpp
tools/cvblobs/BlobProperties.cpp
tools/cvblobs/ComponentLabeling.cpp
tools/Detector.cpp
modules/SlitCam/SlitCam.cpp
modules/BackgroundExtractor/BackgroundExtractor.cpp
modules/SimpleTracker/SimpleTracker.cpp
modules/BlobSegmenter/BlobSegmenter.cpp
modules/CascadeDetector/CascadeDetector.cpp
)

qt4_automoc(${markus_SRCS})
add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBS} tinyxml)


SET(CMAKE_CXX_FLAGS "-Wall -g")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
