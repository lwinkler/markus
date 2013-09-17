file(GLOB modules_src
    "*/*.cpp"
)

add_library(modules
${modules_src}
)



