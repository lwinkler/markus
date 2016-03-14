cmake_minimum_required(VERSION 2.8)

appendModules(modules_src)

add_library(modules_gen
${modules_src}
)



