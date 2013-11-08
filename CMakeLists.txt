project(markus)
cmake_minimum_required(VERSION 2.6)

# Options to be set with ccmake or cmake-gui
set(Markus_NO_GUI          false CACHE BOOL "Compile Markus without a graphical user interface")
set(Markus_USE_QT5         false CACHE BOOL "Use QT 5 instead of 4")
set(Markus_USE_PYTHON_LIBS false CACHE BOOL "Compile with Python libs: only used in some modules")

# Required packages
find_package(OpenCV REQUIRED)
# find_package(tinyxml REQUIRED) # TODO: How to add this dependancy ?
IF(Markus_USE_QT5)
	find_package(Qt5Widgets REQUIRED)
	# find_package(Qt5Core REQUIRED) # already found via Qt5Widgets
ELSE()
	find_package(Qt4 REQUIRED)
ENDIF()


SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O2 -g")
# SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
# SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
# SET(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g")

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(bulk modules lib lib/cvblobs )


set(markus_SRCS main.cpp)

#-------------------------------------------------------------------------------- 
IF(Markus_NO_GUI)
	add_definitions(-DMARKUS_NO_GUI)
	SET(DEPS bulk modules tools)
ELSE(Markus_NO_GUI)
	SET(DEPS bulk gui modules tools)
	include_directories(gui)
ENDIF(Markus_NO_GUI)

SET(DEPS ${DEPS} ${DEPS} tinyxml ${OpenCV_LIBS}) # double the dependencies to avoid linking error
IF(Markus_USE_QT5)
	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	SET(DEPS ${DEPS} ${Qt5Widgets_LIBRARIES})
	# Executables fail to build with Qt 5 in the default configuration
	# without -fPIE. We add that here.
	set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(${Qt5Widgets_DEFINITIONS})
ELSE()
	include_directories(${QT_INCLUDES})
	SET(DEPS ${DEPS} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
ENDIF()

include_directories(${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})

## Python libs
IF(Markus_USE_PYTHON_LIBS)
	find_package(PythonLibs REQUIRED)
	include_directories(${PYTHON_INCLUDE_DIRS})
	SET(DEPS ${DEPS} ${PYTHON_LIBRARIES} )
ENDIF()


# enable debug streams
IF(CMAKE_BUILD_TYPE EQUAL "Debug")
	add_definitions(-DMARKUS_DEBUG_STREAMS)
ENDIF()


#-------------------------------------------------------------------------------- 

add_subdirectory(bulk)
add_subdirectory(modules)
add_subdirectory(lib)
add_subdirectory(tools)
IF(NOT Markus_NO_GUI)
	add_subdirectory(gui)
ENDIF()


add_executable(markus ${markus_SRCS})
# target_link_libraries(markus AAA ${QT_QTCORE_LIBRARY} BBB ${QT_QTGUI_LIBRARY} bulk gui modules tools ${OpenCV_LIBS} tinyxml bulk)
target_link_libraries(markus ${DEPS})




add_custom_target(update_modules_list
	COMMENT "Update the list of modules"
	COMMAND modules/AllModules.h.sh > modules/AllModules.h
	COMMAND ./config_each_module.xml.sh > config_each_module.xml
	COMMAND editor/js/all_modules.js.sh > editor/js/all_modules.js
#	COMMAND make clean
	COMMAND make
	COMMAND ./markus -d config_each_module.xml
)

add_custom_target(update_projects_list
	COMMENT "Update the list of projects"
	COMMAND editor/js/all_projects.js.sh > editor/js/all_projects.js
)

#-------------------------------------------------------------------------------- 
#create a pretty commit id using git
#uses 'git describe --tags', so tags are required in the repo
#create a tag with 'git tag <name>' and 'git push --tags'

find_package(Git)
if(GIT_FOUND)
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --all --long RESULT_VARIABLE res_var OUTPUT_VARIABLE GIT_COM_ID )
	if( NOT ${res_var} EQUAL 0 )
		set( GIT_COMMIT_ID "git commit id unknown")
		message( WARNING "Git failed (not a repo, or no tags). Build will not contain git revision info." )
	endif()
	string( REPLACE "\n" "" GIT_COMMIT_ID ${GIT_COM_ID} )
else()
	set( GIT_COMMIT_ID "unknown (git not found!)")
	message( WARNING "Git not found. Build will not contain git revision info." )
endif()

set( vstring "//version.h - written by cmake. changes will be lost!\n"
	"#ifndef MARKUS_VERSION_H\n"
	"#define MARKUS_VERSION_H\n"
	"const char * VERSION_STRING = \"${GIT_COMMIT_ID}\"\;\n"
	"#endif")

file(WRITE version.h.txt ${vstring} )
# copy the file to the final header only if the version changes
# reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different version.h.txt ${CMAKE_CURRENT_BINARY_DIR}/bulk/version.h)
