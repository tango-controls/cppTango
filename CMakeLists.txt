cmake_minimum_required(VERSION 2.8)
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

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()


include_directories("log4tango/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/log4tango/include")
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
