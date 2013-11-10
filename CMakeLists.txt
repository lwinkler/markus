project(markus)
cmake_minimum_required(VERSION 2.6)

# Options to be set with ccmake or cmake-gui
set(Markus_NO_GUI          false CACHE PATH "Compile Markus without a graphical user interface")
set(Markus_USE_QT5         false CACHE PATH "Use QT 5 instead of 4")
set(Markus_USE_PYTHON_LIBS false CACHE PATH "Compile with Python libs: only used in some modules")

# Required packages
find_package(OpenCV REQUIRED)
# find_package(tinyxml REQUIRED) # TODO: How to add this dependancy ?
IF(Markus_USE_QT5)
	find_package(Qt5Gui REQUIRED)
	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5Core REQUIRED)
ELSE(Markus_USE_QT5)
	find_package(Qt4 REQUIRED)
ENDIF(Markus_USE_QT5)


SET(CMAKE_CXX_FLAGS "-Wall -g") #  -fPIE") # -fPIE flag required by Qt5
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
# SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
# SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
# SET(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g")

# Tell CMake to run moc when necessary: # TODO QT5 only ?
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(bulk modules lib lib/cvblobs )


set(markus_SRCS 
main.cpp
)


#-------------------------------------------------------------------------------- 
IF(Markus_NO_GUI)
	add_definitions(-DMARKUS_NO_GUI)
	SET(DEPS bulk modules tools)
ELSE(Markus_NO_GUI)
	SET(DEPS bulk gui modules tools)
	include_directories(gui)
ENDIF(Markus_NO_GUI)

SET(DEPS ${DEPS} ${DEPS} tinyxml ${OpenCV_LIBS}) # double the dependancies to avoid linking error
IF(Markus_USE_QT5)
	include_directories(${QT_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})
	SET(DEPS ${DEPS} {Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
ELSE(Markus_USE_QT5)
	include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})
	SET(DEPS ${DEPS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
	# Executables fail to build with Qt 5 in the default configuration
	# without -fPIE. We add that here.
	set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(${Qt5Widgets_DEFINITIONS})
ENDIF(Markus_USE_QT5)

## Python libs
IF(Markus_USE_PYTHON_LIBS)
	find_package(PythonLibs REQUIRED)
	include_directories(${PYTHON_INCLUDE_DIRS})
	SET(DEPS ${DEPS} ${PYTHON_LIBRARIES} )
ENDIF(Markus_USE_PYTHON_LIBS)


# enable debug streams
IF(CMAKE_BUILD_TYPE EQUAL "Debug")
	add_definitions(-DMARKUS_DEBUG_STREAMS)
ENDIF(CMAKE_BUILD_TYPE EQUAL "Debug")


#-------------------------------------------------------------------------------- 

add_subdirectory(bulk)
add_subdirectory(modules)
add_subdirectory(lib)
add_subdirectory(tools)
IF(NOT Markus_NO_GUI)
	add_subdirectory(gui)
ENDIF(NOT Markus_NO_GUI)


add_executable(markus ${markus_SRCS})
# target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} bulk gui modules tools ${OpenCV_LIBS} tinyxml bulk)
target_link_libraries(markus ${DEPS})


add_custom_target(update_modules_list
	COMMAND modules/AllModules.h.sh > modules/AllModules.h
	COMMAND ./config_each_module.xml.sh > config_each_module.xml
	COMMAND editor/js/all_modules.js.sh > editor/js/all_modules.js
#	COMMAND make clean
	COMMAND make
	COMMAND ./markus -d config_each_module.xml
)

add_custom_target(update_projects_list
	COMMAND editor/js/all_projects.js.sh > editor/js/all_projects.js
)
