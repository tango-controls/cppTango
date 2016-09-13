cmake_minimum_required(VERSION 3.0)
project(libtango)
include(CTest)

#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "2")
set(PATCH_VERSION "5")
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "2")


set (PACKAGE_URL "https://github.com/tango-controls/cppTango")
set (PACKAGE_BUGREPORT "https://github.com/tango-controls/cppTango/issues")
set (PACKAGE_STRING "${CMAKE_PROJECT_NAME} ${LIBRARY_VERSION}")

#source code
add_subdirectory("log4tango")
add_subdirectory("cppapi")
#add_subdirectory("cpp_test_ds")
#add_subdirectory("cpp_test_suite")

add_library(libtango SHARED $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
