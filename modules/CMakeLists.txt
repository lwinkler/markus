cmake_minimum_required(VERSION 2.8)

file(GLOB modules_src
    "../modules*/*/*.cpp"
)

add_library(modules
${modules_src}
AllModules.cpp
)



