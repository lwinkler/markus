cmake_minimum_required(VERSION 2.8)

file(GLOB modules_src
    "*/*.cpp"
)

#-------------------------------------------------------------------------------- 
# Auto generate AllModules.cpp (used at first run)
execute_process(COMMAND ./modules/AllModules.cpp.sh . OUTPUT_FILE modules/AllModules.cpp)

add_definitions(-fPIC -shared)

# TODO: Remove this ?
add_library(modules
# AllModules.cpp
)

foreach(infileName ${modules_src})
	get_filename_component(lib ${infileName} NAME_WE)
	get_filename_component(dir ${infileName} PATH)
	# message("Add library: ${lib}.so")
	# add_subdirectory(${dir})
	add_library(${lib} SHARED ${infileName})

endforeach(infileName)



