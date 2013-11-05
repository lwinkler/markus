project(markus)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)
# find_package(tinyxml REQUIRED) # TODO: How to add this dependancy ?



SET(CMAKE_CXX_FLAGS "-Wall -g")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
# SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
# SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
# SET(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g")


include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR} 
bulk 
modules
lib
lib/cvblobs
)

## Python libs
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})  # TODO: Only use for one module

set(markus_SRCS 
main.cpp
)


#-------------------------------------------------------------------------------- 
# with GUI
add_subdirectory(gui)
include_directories(gui)
SET(DEPS bulk gui modules tools)

# without GUI
#add_definitions(-DMARKUS_NO_GUI)
#SET(DEPS bulk modules tools)

SET(DEPS ${DEPS} ${DEPS} ) # double the dependancies to avoid linking error
SET(DEPS ${DEPS} tinyxml ${OpenCV_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
SET(DEPS ${DEPS} ${PYTHON_LIBRARIES} )

# enable debug streams
add_definitions(-DMARKUS_DEBUG_STREAMS)

#-------------------------------------------------------------------------------- 

add_subdirectory(bulk)
add_subdirectory(modules)
add_subdirectory(lib)
add_subdirectory(tools)


add_executable(markus ${markus_SRCS})
# target_link_libraries(markus ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} bulk gui modules tools ${OpenCV_LIBS} tinyxml bulk)
target_link_libraries(markus ${DEPS})


add_custom_target(update_modules_list
	COMMAND modules/AllModules.h.sh > modules/AllModules.h
	COMMAND ./config_each_module.xml.sh > config_each_module.xml
	COMMAND editor/js/all_modules.js.sh > editor/js/all_modules.js
	COMMAND make
	COMMAND ./markus -d config_each_module.xml
)

add_custom_target(update_projects_list
	COMMAND editor/js/all_projects.js.sh > editor/js/all_projects.js
)
