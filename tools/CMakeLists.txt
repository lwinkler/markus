
add_executable(test_file_input test_file_input.cpp)
target_link_libraries(test_file_input ${OpenCV_LIBS})

add_executable(test_file_output test_file_output.cpp)
target_link_libraries(test_file_output ${OpenCV_LIBS})

