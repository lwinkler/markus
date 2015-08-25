cmake_minimum_required(VERSION 2.8)

file(GLOB modules_src
    "../modules*/*/*.cpp"
)

#-------------------------------------------------------------------------------- 
# Auto generate AllModules.cpp (used at first run)
execute_process(COMMAND ./modules/AllModules.cpp.sh . OUTPUT_FILE modules/AllModules.cpp)

add_library(modules
${modules_src}
AllModules.cpp
)



