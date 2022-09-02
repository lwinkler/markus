
appendModules(modules_src)

# message("Found these source files in modules/: ${modules_src}")

add_library(modules
${modules_src}
)

