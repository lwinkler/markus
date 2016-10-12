
add_library(tools
translate_config.cpp
)

add_executable(translate_config translate_config.cpp)
target_link_libraries(translate_config ${MK_DEPS} ${MK_DEPS} ${MK_EXT_DEPS}) # double the dependencies to avoid linking error
