project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
FIND_PACKAGE( OpenCV REQUIRED )

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}
modules/SlitCam
tools
)


set(markus_SRCS 
markus.cpp 
main.cpp 
Module.cpp
Manager.cpp
util.cpp
tools/Tracker.cpp
tools/cvblobs/BlobExtraction.cpp
tools/cvblobs/BlobResult.cpp
tools/cvblobs/Blob.cpp
tools/Detector.cpp
ConfigReader.cpp
modules/SlitCam/SlitCam.cpp
)

qt4_automoc(${markus_SRCS})
add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBS} tinyxml)

