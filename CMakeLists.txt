project(markus)
cmake_minimum_required(VERSION 2.6)

# Options to be set with ccmake or cmake-gui
set(CMAKE_BUILD_TYPE       "Debug" CACHE BOOL "Mode of compilation")
set(Markus_NO_GUI          false   CACHE BOOL "Compile Markus without a graphical user interface")
set(Markus_USE_QT5         false   CACHE BOOL "Use QT 5 instead of 4")
set(Markus_USE_PYTHON_LIBS false   CACHE BOOL "Compile with Python libs: only used in some modules")
set(Markus_UNIT_TESTING    false   CACHE BOOL "Enable unit testing")

# Required packages
find_package(OpenCV REQUIRED)
find_package(CURL QUIET)
find_package(PythonLibs 2 QUIET)

find_library(CPPUNIT_LIBS cppunit)
find_library(LOG4CXX_LIBS log4cxx)
find_library(TINYXML_LIBS tinyxml)

if("${CURL_LIBRARIES}" STREQUAL "")
	message(WARNING "Curl library not found on your system")
endif()
if("${CPPUNIT_LIBS}" STREQUAL "")
	message(WARNING "CppUnit library not found on your system")
endif()
if("${PYTHON_LIBRARIES}" STREQUAL "")
	message(WARNING "Python library not found on your system")
endif()
if("${LOG4CXX_LIBS}" STREQUAL "")
	message(FATAL_ERROR "Log4CXX library not found on your system")
endif()
if("${TINYXML_LIBS}" STREQUAL "")
	message(FATAL_ERROR "TinyXML library not found on your system")
endif()

if(Markus_USE_QT5)
	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5WebKit REQUIRED)
	find_package(Qt5WebKitWidgets REQUIRED)
	# find_package(Qt5Core REQUIRED) # already found via Qt5Widgets
ELSE()
	find_package(Qt4 REQUIRED)
endif()

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DMARKUS_DEBUG_STREAMS")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG  "-O2 -g -DMARKUS_DEBUG_STREAMS")

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(bulk modules)


set(markus_SRCS main.cpp)

#-------------------------------------------------------------------------------- 
if(Markus_NO_GUI)
	add_definitions(-DMARKUS_NO_GUI)
	set(DEPS bulk modules)
else()
	set(DEPS bulk gui editor modules)
	include_directories(gui editor)
endif()

if(Markus_UNIT_TESTING)
	add_definitions(-DMARKUS_UNIT_TESTING)
	set(DEPS ${DEPS} ${CPPUNIT_LIBS})
	include_directories(testing)
endif()

set(DEPS ${DEPS} ${DEPS} ${TINYXML_LIBS} ${LOG4CXX_LIBS} ${OpenCV_LIBS} ${PYTHON_LIBRARIES} ${CURL_LIBRARIES}) # double the dependencies to avoid linking error
if(Markus_USE_QT5)
	include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5WebKit_INCLUDE_DIRS} ${Qt5WebKitWidgets_INCLUDE_DIRS})
	set(DEPS ${DEPS} ${Qt5Widgets_LIBRARIES} ${Qt5WebKit_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES}) 
	# Executables fail to build with Qt 5 in the default configuration
	# without -fPIE. We add that here.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5WebKit_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5WebKit_DEFINITIONS})
else()
	include_directories(${QT_INCLUDES})
	set(DEPS ${DEPS} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})



#-------------------------------------------------------------------------------- 

add_subdirectory(bulk)
add_subdirectory(modules)
add_subdirectory(tools)
if(NOT Markus_NO_GUI)
	add_subdirectory(gui)
	add_subdirectory(editor)
endif()
if(Markus_UNIT_TESTING)
	add_subdirectory(testing)
endif()


add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${DEPS})




add_custom_target(lint
	COMMENT "Check C++ lint rules"
	COMMAND find . -name \"*.h\" -print -o -name \"*.cpp\" -print | xargs tools/cpplint.py --filter=-whitespace,-legal,-build,-runtime/references,-runtime/explicit,-readability/todo,-readability/streams,-readability/function
)

add_custom_target(update_modules_list
	COMMENT "Update the list of modules"
	COMMAND modules/AllModules.h.sh > modules/AllModules.h
	COMMAND editor/js/all_modules.js.sh > editor/js/all_modules.js
#	COMMAND make clean
	COMMAND +make
	COMMAND ./markus -d
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
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --long --tags --dirty RESULT_VARIABLE res_var OUTPUT_VARIABLE GIT_COM_ID )
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --long --tags --dirty RESULT_VARIABLE res_var OUTPUT_VARIABLE GIT_COM_ID2 WORKING_DIRECTORY vp-detection )
	execute_process(COMMAND uname -a OUTPUT_VARIABLE GIT_BUILD_HOST )
	if( NOT ${res_var} EQUAL 0 )
		set( GIT_COMMIT_ID "git commit id unknown")
		message( WARNING "Git failed (not a repo, or no tags). Build will not contain git revision info." )
	endif()
	string( REPLACE "\n" "" GIT_COMMIT_ID ${GIT_COM_ID} )
	string( REPLACE "\n" "" GIT_COMMIT_ID2 ${GIT_COM_ID2} )
	string( REPLACE "\n" "" GIT_BUILD_HOST ${GIT_BUILD_HOST} )
else()
	set( GIT_COMMIT_ID "unknown (git not found!)")
	message( WARNING "Git not found. Build will not contain git revision info." )
endif()

set( vstring "//version.h - written by cmake. changes will be lost!\n"
	"#ifndef MARKUS_VERSION_H\n"
	"#define MARKUS_VERSION_H\n"
	"const char * VERSION_STRING = \"${GIT_COMMIT_ID}\"\;\n"
	"const char * VERSION_STRING2 = \"${GIT_COMMIT_ID2}\"\;\n"
	"const char * VERSION_BUILD_HOST = \"${GIT_BUILD_HOST}\"\;\n"
	"#endif")

file(WRITE version.h.txt ${vstring} )
# copy the file to the final header only if the version changes
# reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different version.h.txt ${CMAKE_CURRENT_BINARY_DIR}/bulk/version.h)
