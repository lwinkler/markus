file(GLOB modules_src
    "../modules*/*/*.cpp"
)

add_library(modules
${modules_src}
)



