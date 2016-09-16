add_executable(conf_devtest conf_devtest.cpp)
target_link_libraries(conf_devtest PUBLIC ${CMAKE_DL_LIBS} tango)
add_library(compare_test_object OBJECT compare_test.cpp compare_test.h)