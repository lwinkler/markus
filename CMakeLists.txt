project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(markus_SRCS markus.cpp main.cpp)
qt4_automoc(${markus_SRCS})
add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
