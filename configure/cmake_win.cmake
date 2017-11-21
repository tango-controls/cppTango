
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
project(tangod)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
project(tango)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(CMAKE_CL_64)
add_definitions(-D_64BITS)
endif(CMAKE_CL_64)

#include and link directories

include_directories(${INCLUDE_OBJECT_MS})
include_directories(${PTHREAD_WIN_PKG_INCLUDE_DIRS})
set(WIN32_LIBS "ws2_32.lib;mswsock.lib;advapi32.lib;comctl32.lib;odbc32.lib;")
link_directories($ENV{PTHREAD_WIN}/lib)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_library(tangod-static STATIC    $<TARGET_OBJECTS:log4tango_objects_sta>
                            $<TARGET_OBJECTS:idl_objects_sta>
                            $<TARGET_OBJECTS:client_objects_sta>
                            $<TARGET_OBJECTS:jpeg_objects_sta>
                            $<TARGET_OBJECTS:jpeg_mmx_objects_sta>
                            $<TARGET_OBJECTS:server_objects_sta>)
add_library(tangod SHARED    $<TARGET_OBJECTS:log4tango_objects_dyn>
                            $<TARGET_OBJECTS:idl_objects_dyn>
                            $<TARGET_OBJECTS:client_objects_dyn>
                            $<TARGET_OBJECTS:jpeg_objects_dyn>
                            $<TARGET_OBJECTS:jpeg_mmx_objects_dyn>
                            $<TARGET_OBJECTS:server_objects_dyn>)

message("static_defs: ${static_defs}")

set_target_properties(tangod-static PROPERTIES COMPILE_DEFINITIONS
"${static_defs}")

message("dyn_defs: ${dyn_defs}")

set_target_properties(tangod PROPERTIES COMPILE_DEFINITIONS
"${dyn_defs}")

target_compile_options(tangod PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_options(tangod-static PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER} "/MTd")

target_link_libraries(tangod PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_DYN} ${ZMQ_PKG_LIBRARIES_DYN} ${PTHREAD_WIN_PKG_LIBRARIES_DYN} ${CMAKE_DL_LIBS})
target_link_libraries(tangod-static PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_STA} ${ZMQ_PKG_LIBRARIES_STA} ${PTHREAD_WIN_PKG_LIBRARIES_STA} ${CMAKE_DL_LIBS})

set_property(TARGET tangod PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tangod PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
set_property(TARGET tangod-static PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tangod-static PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})

target_include_directories(tangod PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_include_directories(tangod-static PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

SET_TARGET_PROPERTIES(tangod-static PROPERTIES OUTPUT_NAME "tangod")
SET_TARGET_PROPERTIES(tangod-static PROPERTIES PREFIX "lib")

set_target_properties(tangod PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
set_target_properties(tangod-static PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_INSTALL_CONFIG_NAME Debug)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_library(tango-static STATIC    $<TARGET_OBJECTS:log4tango_objects_sta>
                            $<TARGET_OBJECTS:idl_objects_sta>
                            $<TARGET_OBJECTS:client_objects_sta>
                            $<TARGET_OBJECTS:jpeg_objects_sta>
                            $<TARGET_OBJECTS:jpeg_mmx_objects_sta>
                            $<TARGET_OBJECTS:server_objects_sta>)
add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects_dyn>
                            $<TARGET_OBJECTS:idl_objects_dyn>
                            $<TARGET_OBJECTS:client_objects_dyn>
                            $<TARGET_OBJECTS:jpeg_objects_dyn>
                            $<TARGET_OBJECTS:jpeg_mmx_objects_dyn>
                            $<TARGET_OBJECTS:server_objects_dyn>)

message("static_defs: ${static_defs}")

set_target_properties(tango-static PROPERTIES COMPILE_DEFINITIONS
"${static_defs}")

message("dyn_defs: ${dyn_defs}")

set_target_properties(tango PROPERTIES COMPILE_DEFINITIONS
"${dyn_defs}")
                 
SET_TARGET_PROPERTIES(tango-static PROPERTIES OUTPUT_NAME "tango")
SET_TARGET_PROPERTIES(tango-static PROPERTIES PREFIX "lib")
 
target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_options(tango-static PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER} "/MT")

target_link_libraries(tango PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_DYN} ${ZMQ_PKG_LIBRARIES_DYN} ${PTHREAD_WIN_PKG_LIBRARIES_DYN} ${CMAKE_DL_LIBS})
target_link_libraries(tango-static PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_STA} ${ZMQ_PKG_LIBRARIES_STA} ${PTHREAD_WIN_PKG_LIBRARIES_STA} ${CMAKE_DL_LIBS})

set_property(TARGET tango PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tango PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
set_property(TARGET tango-static PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tango-static PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_include_directories(tango-static PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

set_target_properties(tango PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
set_target_properties(tango-static PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

#install code

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

install(TARGETS tangod-static
        ARCHIVE DESTINATION lib COMPONENT static CONFIGURATIONS Debug
        RUNTIME DESTINATION bin COMPONENT dynamic CONFIGURATIONS Debug)
        
install(TARGETS tangod
        ARCHIVE DESTINATION lib COMPONENT static CONFIGURATIONS Debug
        RUNTIME DESTINATION bin COMPONENT dynamic CONFIGURATIONS Debug)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")

install(TARGETS tango-static
        ARCHIVE DESTINATION lib COMPONENT static
        RUNTIME DESTINATION bin COMPONENT dynamic)
        
install(TARGETS tango
        ARCHIVE DESTINATION lib COMPONENT static
        RUNTIME DESTINATION bin COMPONENT dynamic)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

install(DIRECTORY log4tango/include/log4tango DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "threading" EXCLUDE)
install(DIRECTORY log4tango/include/log4tango/threading DESTINATION include/log4tango COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE)
install(DIRECTORY cppapi/server/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "server_objects.dir" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "idl" EXCLUDE PATTERN "jpeg" EXCLUDE PATTERN "jpeg_mmx" EXCLUDE)
install(DIRECTORY cppapi/client/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "client_objects.dir" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "helpers" EXCLUDE)
install(DIRECTORY cppapi/client/helpers/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE)
install(FILES cppapi/server/resource.h DESTINATION include COMPONENT headers)
install(FILES cppapi/server/tango.h DESTINATION include COMPONENT headers)
install(FILES cppapi/server/idl/tango.h DESTINATION include/idl COMPONENT headers)
install(DIRECTORY $ENV{OMNI_BASE}/include/COS DESTINATION include COMPONENT)
install(DIRECTORY $ENV{OMNI_BASE}/include/omniORB4 DESTINATION include COMPONENT headers)
install(DIRECTORY $ENV{OMNI_BASE}/include/omnithread DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.in" EXCLUDE)
install(DIRECTORY $ENV{OMNI_BASE}/include/omniVms DESTINATION include COMPONENT headers)
install(FILES $ENV{OMNI_BASE}/include/omniconfig.h DESTINATION include COMPONENT headers)
install(FILES $ENV{OMNI_BASE}/include/omnithread.h DESTINATION include COMPONENT headers)
install(FILES $ENV{ZMQ_BASE}/include/zmq.h DESTINATION include COMPONENT headers)
install(FILES $ENV{ZMQ_BASE}/include/zmq.hpp DESTINATION include COMPONENT headers)
install(FILES $ENV{ZMQ_BASE}/include/zmq_addon.hpp DESTINATION include COMPONENT headers)
install(FILES $ENV{ZMQ_BASE}/include/zmq_utils.h DESTINATION include COMPONENT headers)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

#adding tango since tere seems to be a bug
install(FILES Debug/tangod.dll DESTINATION bin COMPONENT dynamic)
install(FILES Debug/tangod.lib DESTINATION lib COMPONENT static)
install(FILES Debug/tangod.pdb DESTINATION bin COMPONENT dynamic)
install(FILES Debug/libtangod.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic4d.lib DESTINATION lib COMPONENT static)
#omniorb static lib
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB4d.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic4d.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithreadd.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS4d.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB4_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic4_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithread_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS4_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB421_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic421_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithread40_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS421_rtd.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/msvcstubd.lib DESTINATION bin COMPONENT dynamic)

else(CMAKE_BUILD_TYPE STREQUAL "Debug")

#omniorb static lib
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB4.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic4.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithread.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS4.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB4_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic4_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithread_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS4_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniORB421_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omniDynamic421_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/omnithread40_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/COS421_rt.lib DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/lib/x86_win32/msvcstub.lib DESTINATION bin COMPONENT dynamic)

endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

#omniorb dynamic + zmq
if(MSVC90)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v90-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v90-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v90-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v90-mt-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v90-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v90-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC90)

if(MSVC10)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v100-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v100-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v100-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v100-mt-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v100-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v100-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC10)

if(MSVC12)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v120-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v120-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v120-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v120-mt-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v120-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v120-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC12)

if(MSVC14)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v140-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v140-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v140-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v140-mt-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v140-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v140-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC14)
#pthreads
install(FILES $ENV{PTHREAD_WIN}/lib/pthreadVC2.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/lib/pthreadVC2-s.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2.pdb DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2.exp DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2.ilk DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/lib/pthreadVC2d.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/lib/pthreadVC2-sd.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2d.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2d.pdb DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2d.exp DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{PTHREAD_WIN}/bin/pthreadVC2d.ilk DESTINATION bin COMPONENT dynamic)


configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION include/pkgconfig COMPONENT headers)
