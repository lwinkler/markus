project(markus)
cmake_minimum_required(VERSION 2.6)

# Options to be set with ccmake or cmake-gui
set(Markus_MODULE_DIRS          "modules;modules2"   CACHE STRING "List of directories containing modules")
set(Markus_PROJECTS_DIRS        "projects;projects2" CACHE STRING "List of directories containing XML projects")
set(Markus_MODULE_BLACKLIST     ""                   CACHE STRING "List of modules to blacklist")
set(Markus_NO_GUI          false   CACHE BOOL "Compile Markus without a graphical user interface")
set(Markus_USE_QT5         false   CACHE BOOL "Use QT 5 instead of 4")

# Create text files: dirs.txt contains all module directories and modules.txt contains all paths to modules
file(REMOVE dirs.txt)
file(REMOVE modules.txt)
foreach(iter IN LISTS Markus_MODULE_DIRS)
	file(APPEND dirs.txt "${iter}\n")
endforeach()

# Function used by module directories: create a list of all module directories and add all *.cpp files to modules_src
function(appendModules modules_src)
file(GLOB tmp "*")
foreach(mod IN LISTS tmp)
	if(IS_DIRECTORY ${mod})
		get_filename_component(name1 ${mod} NAME_WE)
		if(name1 STREQUAL "CMakeFiles")
		else()
			list(FIND Markus_MODULE_BLACKLIST ${name1} index)
			if(index STREQUAL "-1")
				# Include this module
				file(APPEND "${PROJECT_SOURCE_DIR}/modules.txt" "${mod}\n")
				file(GLOB modules_tmp "${mod}/*.cpp")
				list(APPEND modules_src2 ${modules_tmp})
			else()
				message("Blacklisted module ${name1}")
			endif()
		endif()
	endif()
endforeach(mod)
set(modules_src ${modules_src2} PARENT_SCOPE)
endfunction(appendModules)


add_definitions(-std=c++11 -Wextra -Wno-unused-parameter -Wno-deprecated-register)
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DMARKUS_DEBUG_STREAMS)
endif()

find_package( Boost 1.40 COMPONENTS system thread REQUIRED )
include_directories(${Boost_INCLUDE_DIR})

# Required packages
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

find_library(LOG4CXX_LIBS log4cxx)
find_library(TINYXML_LIBS tinyxml)
find_library(JSONCPP_LIBS jsoncpp)

if("${LOG4CXX_LIBS}" STREQUAL "")
	message(FATAL_ERROR "Log4CXX library not found on your system")
endif()
if("${TINYXML_LIBS}" STREQUAL "")
	message(FATAL_ERROR "TinyXML library not found on your system")
endif()
if("${JSONCPP_LIBS}" STREQUAL "")
	message(FATAL_ERROR "JsonCpp library not found on your system")
endif()

set(MK_EXT_DEPS ${MK_EXT_DEPS} ${LOG4CXX_LIBS})
set(MK_EXT_DEPS ${MK_EXT_DEPS} ${TINYXML_LIBS})
set(MK_EXT_DEPS ${MK_EXT_DEPS} ${JSONCPP_LIBS})
set(MK_EXT_DEPS ${MK_EXT_DEPS} ${OpenCV_LIBS})
set(MK_EXT_DEPS ${MK_EXT_DEPS} ${CMAKE_THREAD_LIBS_INIT})
set(MK_EXT_DEPS ${MK_EXT_DEPS} ${Boost_LIBRARIES})

if(NOT Markus_NO_GUI)
	if(Markus_USE_QT5)
		find_package(Qt5Widgets REQUIRED)
		find_package(Qt5WebKit REQUIRED)
		find_package(Qt5WebKitWidgets REQUIRED)
		# find_package(Qt5Core REQUIRED) # already found via Qt5Widgets
	else()
		find_package(Qt4 REQUIRED)
	endif()
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Search all directory named modules... (note: we need to preserve the order)
# file(GLOB modules_tmp
    # "modules" "modules2*" "modules3*" "modules4"
# )
# foreach(iter IN LISTS modules_tmp)
	# get_filename_component(ext1  ${iter} EXT)
	# get_filename_component(name1 ${iter} NAME_WE)
	# list(APPEND modules_dirs "${name1}${ext1}")
# endforeach(iter)
# message("Looking for modules in ${modules_dirs}")

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(util bulk ${Markus_MODULE_DIRS})

set(markus_SRCS main.cpp modules/AllModules.cpp)

#-------------------------------------------------------------------------------- 
if(Markus_NO_GUI)
	add_definitions(-DMARKUS_NO_GUI)
	set(MK_DEPS util bulk ${Markus_MODULE_DIRS})
else()
	set(MK_DEPS util bulk gui editor ${Markus_MODULE_DIRS})
	include_directories(gui editor)
endif()

if(NOT Markus_NO_GUI)
	if(Markus_USE_QT5)
		include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5WebKit_INCLUDE_DIRS} ${Qt5WebKitWidgets_INCLUDE_DIRS})
		set(MK_EXT_DEPS ${MK_EXT_DEPS} ${Qt5Widgets_LIBRARIES} ${Qt5WebKit_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES}) 
		# Executables fail to build with Qt 5 in the default configuration
		# without -fPIE. We add that here.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5WebKit_EXECUTABLE_COMPILE_FLAGS}")
		add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5WebKit_DEFINITIONS})
	else()
		include_directories(${QT_INCLUDES})
		set(MK_EXT_DEPS ${MK_EXT_DEPS} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
	endif()
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})

# trick: We need to include this here to compile
include_directories(/usr/local/mongo-cxx-driver/include)



#-------------------------------------------------------------------------------- 
# Include sub directories
add_subdirectory(util)
add_subdirectory(bulk)
foreach(iter IN LISTS Markus_MODULE_DIRS)
	add_subdirectory(${iter})
endforeach(iter)
if(NOT Markus_NO_GUI)
	add_subdirectory(gui)
	add_subdirectory(editor)
endif()
# Auto generate AllModules.cpp
execute_process(COMMAND ./modules/AllModules.cpp.sh . OUTPUT_FILE modules/AllModules.cpp)

add_subdirectory(tests)

add_executable(markus ${markus_SRCS})
target_link_libraries(markus ${MK_DEPS} ${MK_DEPS} ${MK_EXT_DEPS}) # double the dependencies to avoid linking error


add_custom_target(doc
	COMMENT "Build the project documentation"
	COMMAND doxygen
)


add_custom_target(lint
	COMMENT "Check C++ lint rules with the lint tool provided by Google"
	COMMAND tools/list_source_dirs.sh | xargs -I{} find -L {} -name \"*.h\" -print -o -name \"*.cpp\" ! -name \"moc_*.cpp\" -print | xargs tools/cpplint.py --filter=-whitespace,-legal,-build,-runtime/references,-runtime/explicit,-readability/todo,-readability/streams,-readability/function,-runtime/string
)

add_custom_target(check
	COMMENT "Check C++ code statically with cppcheck"
	COMMAND tools/list_source_dirs.sh | xargs cppcheck --force --enable=all -q --template "{file}:{line}: {severity} {id}: {message}" --inline-suppr
)

add_custom_target(todo
	COMMENT "List all todos"
	COMMAND tools/list_source_dirs.sh | xargs -I{} grep -rn TODO --exclude-dir=js {}
	# COMMAND git grep -n TODO
	# COMMAND cd modules2 && git grep -n TODO
)

add_custom_target(update_modules_list
	COMMENT "Update the list of modules for the editor"
	COMMAND ./markus -d
	DEPENDS  markus
)

add_custom_target(update_projects_list
	COMMENT "Update the list of projects for the editor"
	COMMAND editor/js/all_projects.js.sh > editor/js/all_projects.js
)

add_custom_target(tests
	COMMENT "Launch the set of unit tests"
	COMMAND tests/unitTests
)

add_custom_target(indent
	COMMENT "Indent all files by following the standart"
	COMMAND tools/list_source_dirs.sh | xargs -I{} find {} -name \"*.h\" -print -o -name \"*.cpp\" -print | xargs astyle -nToO --style=allman
)

add_custom_target(xmlindent
	COMMENT "Indent all files by following the standart"
	COMMAND find -L projects/ projects2/ -name \"*.xml\" -print | xargs -i{} xmlindent -wt {}
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
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different version.h.txt ${CMAKE_CURRENT_BINARY_DIR}/util/version.h)
