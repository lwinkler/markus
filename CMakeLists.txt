project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)



SET(CMAKE_CXX_FLAGS "-Wall -g")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")


include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR} 
bulk 
gui
modules
tools
tools/cvblobs
)


set(markus_SRCS 
main.cpp
)


add_subdirectory(bulk)
add_subdirectory(gui)
add_subdirectory(modules)
add_subdirectory(tools)


add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} bulk gui modules tools ${OpenCV_LIBS} tinyxml bulk)


add_custom_command(
	OUTPUT AllModules.h
	COMMAND ./generate_all_modules.sh > AllModules.h
)
add_custom_target(list_modules
DEPENDS AllModules.h)
