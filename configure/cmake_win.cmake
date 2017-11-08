
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
project(tangod)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
project(tango)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(CMAKE_CL_64)
add_definitions(-D_64BITS)
if(MSVC14)
add_definitions(-D_TIMERS_T_)
add_definitions(-DHAVE_STRUCT_TIMESPEC)
add_definitions(-D_TIMESPEC_DEFINED)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_SEC)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_NSEC)
endif(MSVC14)
endif(CMAKE_CL_64)

#include and link directories

include_directories(${INCLUDE_OBJECT_MS})
set(WIN32_LIBS "ws2_32.lib;mswsock.lib;advapi32.lib;comctl32.lib;odbc32.lib;")
set(PTHREAD_WIN32_LIBS "pthreadVC2.lib;")
if(CMAKE_CL_64)
link_directories($ENV{PTHREAD_WIN}/Pre-built.2/lib/x64/)
else(CMAKE_CL_64)
link_directories($ENV{PTHREAD_WIN}/Pre-built.2/lib/x86/)
endif(CMAKE_CL_64)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_library(tangod-static STATIC    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
add_library(tangod SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
                            
SET_TARGET_PROPERTIES(tangod-static PROPERTIES OUTPUT_NAME "tangod")
SET_TARGET_PROPERTIES(tangod-static PROPERTIES PREFIX "lib")

target_link_libraries(tangod PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_DYN} ${ZMQ_PKG_LIBRARIES_DYN} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})
target_link_libraries(tangod-static PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_STA} ${ZMQ_PKG_LIBRARIES_STA} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})

set_property(TARGET tangod PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tangod PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
set_property(TARGET tangod-static PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tangod-static PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})

target_include_directories(tangod PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_include_directories(tangod-static PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

target_compile_options(tangod PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tangod PRIVATE WIN32 DEBUG _WINDOWS _USRDLL LOG4TANGO_HAS_DLL _TANGO_LIB _CRT_SECURE_NO_DEPRECATE JPG_USE_ASM OMNI_UNLOADABLE_STUBS)

target_compile_options(tangod-static PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tangod-static PRIVATE WIN32 _LIB _DEBUG _WINDOWS _WINSTATIC _TANGO_LIB _MBCS _CRT_SECURE_NO_DEPRECATE JPG_USE_ASM STATIC_EXPORT OMNI_UNLOADABLE_STUBS ZMQ_STATIC)

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
add_library(tango-static STATIC    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
SET_TARGET_PROPERTIES(tango-static PROPERTIES OUTPUT_NAME "tango")
SET_TARGET_PROPERTIES(tango-static PROPERTIES PREFIX "lib")
                            
target_link_libraries(tango PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_DYN} ${ZMQ_PKG_LIBRARIES_DYN} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})
target_link_libraries(tango-static PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_STA} ${ZMQ_PKG_LIBRARIES_STA} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})

set_property(TARGET tango PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tango PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
set_property(TARGET tango-static PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tango-static PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_include_directories(tango-static PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tango PRIVATE WIN32 NDEBUG _WINDOWS _USRDLL LOG4TANGO_HAS_DLL _TANGO_LIB _CRT_SECURE_NO_DEPRECATE JPG_USE_ASM OMNI_UNLOADABLE_STUBS)
target_compile_options(tango-static PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tango-static PRIVATE WIN32 NDEBUG _WINDOWS _WINSTATIC _TANGO_LIB _CRT_SECURE_NO_DEPRECATE JPG_USE_ASM STATIC_EXPORT OMNI_UNLOADABLE_STUBS ZMQ_STATIC)

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

install(DIRECTORY log4tango/include/log4tango DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "log4tango/include/log4tango/CMakeFiles" EXCLUDE PATTERN "log4tango/include/log4tango/threading/" EXCLUDE)
install(DIRECTORY log4tango/include/log4tango/threading DESTINATION include/log4tango COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "log4tango/include/log4tango/threading/CMakeFiles/" EXCLUDE)
install(DIRECTORY cppapi/server/ DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "cppapi/server/server_objects.dir/" EXCLUDE PATTERN "cppapi/server/CMakeFiles/" EXCLUDE PATTERN "cppapi/server/idl/" EXCLUDE PATTERN "cppapi/server/jpeg/" EXCLUDE PATTERN "cppapi/server/jpeg_mmx/" EXCLUDE)
install(DIRECTORY cppapi/client/ DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "cppapi/client/client_objects.dir/" EXCLUDE PATTERN "cppapi/client/CMakeFiles/" EXCLUDE PATTERN "cppapi/client/helpers/" EXCLUDE)
install(DIRECTORY cppapi/client/helpers/ DESTINATION include/tango/client COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "cppapi/client/helpers/CMakeFiles/" EXCLUDE)
install(FILES cppapi/server/resource.h DESTINATION include COMPONENT headers)
install(FILES cppapi/server/tango.h DESTINATION include COMPONENT headers)
install(FILES cppapi/server/idl/tango.h DESTINATION include/idl COMPONENT headers)
install(DIRECTORY $ENV{OMNI_BASE}/include/COS DESTINATION include COMPONENT)
install(DIRECTORY $ENV{OMNI_BASE}/include/omniORB4 DESTINATION include COMPONENT headers)
install(DIRECTORY $ENV{OMNI_BASE}/include/omnithread DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.in" EXCLUDE)
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
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v90-mt-s-gd-4_0_5.lib DESTINATION lib COMPONENT static)
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
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v100-mt-s-gd-4_0_5.lib DESTINATION lib COMPONENT static)
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
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v120-mt-s-gd-4_0_5.lib DESTINATION lib COMPONENT static)
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
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v140-mt-s-gd-4_0_5.lib DESTINATION lib COMPONENT static)
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
if(CMAKE_CL_64)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/lib/x64/pthreadVC2.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/dll/x64/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
else(CMAKE_CL_64)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/lib/x86/pthreadVC2.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/dll/x86/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
endif(CMAKE_CL_64)


configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION include/pkgconfig COMPONENT headers)
