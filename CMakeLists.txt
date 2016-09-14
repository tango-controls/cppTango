cmake_minimum_required(VERSION 2.8)
project(libtango)
include(CTest)

enable_testing()
#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "2")
set(PATCH_VERSION "5")
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "2")


set (PACKAGE_URL "https://github.com/tango-controls/cppTango")
set (PACKAGE_BUGREPORT "https://github.com/tango-controls/cppTango/issues")
set (PACKAGE_STRING "${CMAKE_PROJECT_NAME} ${LIBRARY_VERSION}")

include(configure/CMakeLists.txt)

include_directories("cppapi/client")
include_directories("cppapi/client/helpers")
include_directories("cppapi/server")
include_directories("cppapi/server/idl")
include_directories("log4tango/include")
#required for generated config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}/log4tango/include")

#externals
include_directories(${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS})

#source code
add_subdirectory("log4tango")
add_subdirectory("cppapi")
#add_subdirectory("cpp_test_suite")
#add_subdirectory("cpp_test_ds")

add_library(tango SHARED $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
target_link_libraries(tango PUBLIC ${ZMQ_PKG_LIBRARIES} ${OMNIORB_PKG_LIBRARIES})
target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER})

#install
