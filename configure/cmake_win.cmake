project(tango)

if(CMAKE_CL_64)
    add_definitions(-D_64BITS)
endif()

set(TANGO_LIBRARY_NAME tango)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TANGO_LIBRARY_NAME ${TANGO_LIBRARY_NAME}d)
endif()

# The name without the variant tag (i.e. -static)
set(TANGO_LIBRARY_OUTPUT_NAME ${TANGO_LIBRARY_NAME})

if(NOT BUILD_SHARED_LIBS)
    set(TANGO_LIBRARY_NAME ${TANGO_LIBRARY_NAME}-static)
endif()

message("Tango library is '${TANGO_LIBRARY_NAME}'")

#dump version into a file, later used by AppVeyor
file(WRITE ${PROJECT_BINARY_DIR}/VERSION ${LIBRARY_VERSION})

#include and link directories

include_directories(SYSTEM ${PTHREAD_WIN_PKG_INCLUDE_DIRS})
set(WIN32_LIBS "ws2_32.lib;mswsock.lib;advapi32.lib;comctl32.lib;odbc32.lib;")
if(PTHREAD_WIN)
    link_directories(${PTHREAD_WIN}/lib)
endif()

add_library(${TANGO_LIBRARY_NAME} $<TARGET_OBJECTS:log4tango_objects>
        $<TARGET_OBJECTS:idl_objects>
        $<TARGET_OBJECTS:client_objects>
        $<TARGET_OBJECTS:jpeg_objects>
        $<TARGET_OBJECTS:jpeg_mmx_objects>
        $<TARGET_OBJECTS:server_objects>)

set_target_properties(${TANGO_LIBRARY_NAME} PROPERTIES COMPILE_DEFINITIONS
        "${windows_defs}")

target_compile_options(${TANGO_LIBRARY_NAME} PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})

if(BUILD_SHARED_LIBS)
    target_link_libraries(${TANGO_LIBRARY_NAME} PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_DYN} ${ZMQ_PKG_LIBRARIES_DYN} ${PTHREAD_WIN_PKG_LIBRARIES_DYN} ${CMAKE_DL_LIBS})
else()
    target_link_libraries(${TANGO_LIBRARY_NAME} PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES_STA} ${ZMQ_PKG_LIBRARIES_STA} ${PTHREAD_WIN_PKG_LIBRARIES_STA} ${CMAKE_DL_LIBS})
    set_target_properties(${TANGO_LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${TANGO_LIBRARY_OUTPUT_NAME})
    set_target_properties(${TANGO_LIBRARY_NAME} PROPERTIES PREFIX "lib")
endif()

set_property(TARGET ${TANGO_LIBRARY_NAME} PROPERTY LINK_FLAGS "/force:multiple")
target_include_directories(${TANGO_LIBRARY_NAME} SYSTEM PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

set_target_properties(${TANGO_LIBRARY_NAME} PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_INSTALL_CONFIG_NAME Debug)
endif()

#install code

install(TARGETS ${TANGO_LIBRARY_NAME}
        ARCHIVE DESTINATION lib COMPONENT static
        RUNTIME DESTINATION bin COMPONENT dynamic)

install(DIRECTORY log4tango/include/log4tango DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "threading" EXCLUDE)
install(DIRECTORY log4tango/include/log4tango/threading DESTINATION include/log4tango COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE)
install(DIRECTORY cppapi/server/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "server_objects_sta.dir" EXCLUDE PATTERN "server_objects_dyn.dir" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "idl" EXCLUDE PATTERN "jpeg" EXCLUDE PATTERN "jpeg_mmx" EXCLUDE)
install(DIRECTORY cppapi/client/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "client_objects_sta.dir" EXCLUDE PATTERN "client_objects_dyn.dir" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "helpers" EXCLUDE)
install(DIRECTORY cppapi/client/helpers/ DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.tpp" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.cpp" EXCLUDE PATTERN "*.in" EXCLUDE PATTERN "*.am" EXCLUDE PATTERN "CMakeFiles" EXCLUDE)
install(FILES cppapi/server/resource.h DESTINATION include COMPONENT headers)
install(FILES cppapi/server/tango.h DESTINATION include COMPONENT headers)

if (TANGO_INSTALL_DEPENDENCIES)
    install(DIRECTORY ${OMNI_BASE}/include/COS DESTINATION include COMPONENT)
    install(DIRECTORY ${OMNI_BASE}/include/omniORB4 DESTINATION include COMPONENT headers)
    install(DIRECTORY ${OMNI_BASE}/include/omnithread DESTINATION include COMPONENT headers FILES_MATCHING PATTERN "*.h" PATTERN "*.in" EXCLUDE)
    install(DIRECTORY ${OMNI_BASE}/include/omniVms DESTINATION include COMPONENT headers)
    install(FILES ${OMNI_BASE}/include/omniconfig.h DESTINATION include COMPONENT headers)
    install(FILES ${OMNI_BASE}/include/omnithread.h DESTINATION include COMPONENT headers)
    install(FILES ${ZMQ_BASE}/include/zmq.h DESTINATION include COMPONENT headers)
    install(FILES ${ZMQ_BASE}/include/zmq_utils.h DESTINATION include COMPONENT headers)
    install(FILES ${CPPZMQ_BASE}/include/zmq.hpp DESTINATION include COMPONENT headers)
    install(FILES ${CPPZMQ_BASE}/include/zmq_addon.hpp DESTINATION include COMPONENT headers)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        #omniorb static lib
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic4d.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB4d.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic4d.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithreadd.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS4d.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB4_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic4_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithread_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS4_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB421_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic421_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithread40_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS421_rtd.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/msvcstubd.lib DESTINATION lib COMPONENT dynamic)

    else()

        #omniorb static lib
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB4.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic4.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithread.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS4.lib DESTINATION lib COMPONENT static)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB4_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic4_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithread_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS4_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniORB421_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omniDynamic421_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/omnithread40_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/COS421_rt.lib DESTINATION lib COMPONENT dynamic)
        install(FILES ${OMNI_BASE}/lib/x86_win32/msvcstub.lib DESTINATION lib COMPONENT dynamic)

    endif()

    if(CMAKE_VS_PLATFORM_TOOLSET STREQUAL "v140")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/COS421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${ZMQ_BASE}/lib/Debug/libzmq-v140-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/lib/Debug/libzmq-v140-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/bin/Debug/libzmq-v140-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
        else()
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/COS421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${ZMQ_BASE}/lib/Release/libzmq-v140-mt-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/lib/Release/libzmq-v140-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/bin/Release/libzmq-v140-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
        endif()
    endif()

    if(CMAKE_VS_PLATFORM_TOOLSET STREQUAL "v141")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniORB421_vc15_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniDynamic421_vc15_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omnithread40_vc15_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/COS421_vc15_rtd.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${ZMQ_BASE}/lib/Debug/libzmq-v141-mt-gd-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/lib/Debug/libzmq-v141-mt-sgd-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/bin/Debug/libzmq-v141-mt-gd-4_0_5.dll DESTINATION bin COMPONENT dynamic)
        else()
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniORB421_vc15_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omniDynamic421_vc15_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/omnithread40_vc15_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${OMNI_BASE}/bin/x86_win32/COS421_vc15_rt.dll DESTINATION bin COMPONENT dynamic)
            install(FILES ${ZMQ_BASE}/lib/Release/libzmq-v141-mt-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/lib/Release/libzmq-v141-mt-s-4_0_5.lib DESTINATION lib COMPONENT static)
            install(FILES ${ZMQ_BASE}/bin/Release/libzmq-v141-mt-4_0_5.dll DESTINATION bin COMPONENT dynamic)
        endif()
    endif()

    #pthreads
    if (PTHREAD_WIN)
        install(FILES ${PTHREAD_WIN}/lib/pthreadVC2.lib DESTINATION lib COMPONENT static)
        install(FILES ${PTHREAD_WIN}/lib/pthreadVC2-s.lib DESTINATION lib COMPONENT static)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2.pdb DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2.exp DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2.ilk DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/lib/pthreadVC2d.lib DESTINATION lib COMPONENT static)
        install(FILES ${PTHREAD_WIN}/lib/pthreadVC2-sd.lib DESTINATION lib COMPONENT static)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2d.dll DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2d.pdb DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2d.exp DESTINATION bin COMPONENT dynamic)
        install(FILES ${PTHREAD_WIN}/bin/pthreadVC2d.ilk DESTINATION bin COMPONENT dynamic)
    endif()
endif()

configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION include/pkgconfig COMPONENT headers)
