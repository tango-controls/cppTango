cmake_minimum_required(VERSION 2.8)
if(WIN32)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
project(tangod)
else()
project(tango)
endif()
endif()
include(CTest)

enable_testing()
#need to define the version of the library
set(MAJOR_VERSION "10")
set(MINOR_VERSION "2")
set(PATCH_VERSION "2")

IF (WIN32)
add_definitions(-D_WIN32)
add_definitions(-D__OSVERSION__=4)
add_definitions(-D__POSIX_NT__)
add_definitions(-D__x86__)
#add_definitions(-DLOG4TANGO_USE_MSTHREADS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DOMNI_UNLOADABLE_STUBS)
add_definitions(-D_TANGO_LIB)
add_definitions(-DHAVE_STRUCT_TIMESPEC)
add_definitions(-D_TIMESPEC_DEFINED)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_SEC)
add_definitions(-DSTRUCT_TIMESPEC_HAS_TV_NSEC)
IF(CMAKE_CL_64)
add_definitions(-D_64BITS)
IF (MSVC14)
add_definitions(-D_TIMERS_T_)
ENDIF()
ELSE()
add_definitions(-DJPG_USE_ASM)
ENDIF()
ENDIF()

#convenient versions
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "${MAJOR_VERSION}")


message("System TANGO_HOST=$ENV{TANGO_HOST}")
set(TANGO_HOST $ENV{TANGO_HOST})
include(configure/CMakeLists.txt)

include_directories(generated)
#required for generated config.h
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(include)
add_subdirectory(doxygen)
#source code
add_subdirectory(generated/idl)
include_directories(include)
add_subdirectory(src)
#add_subdirectory(test)
IF(WIN32)
include_directories(${INCLUDE_OBJECT_MS})
set(WIN32_LIBS "ws2_32.lib;mswsock.lib;advapi32.lib;comctl32.lib;odbc32.lib;")
set(PTHREAD_WIN32_LIBS "pthreadVC2.lib;")
IF(CMAKE_CL_64)
link_directories($ENV{PTHREAD_WIN}/Pre-built.2/lib/x64/)
ELSE()
link_directories($ENV{PTHREAD_WIN}/Pre-built.2/lib/x86/)
ENDIF()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
add_library(tangod SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
target_link_libraries(tangod PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES} ${ZMQ_PKG_LIBRARIES} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})
set_property(TARGET tangod PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tangod PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
target_include_directories(tangod PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

target_compile_options(tangod PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tangod PRIVATE OMNI_UNLOADABLE_STUBS _TANGO_LIB)

set_target_properties(tangod PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
SET(CMAKE_INSTALL_CONFIG_NAME Debug)
else()
add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>)
target_link_libraries(tango PUBLIC ${WIN32_LIBS} ${OMNIORB_PKG_LIBRARIES} ${ZMQ_PKG_LIBRARIES} ${PTHREAD_WIN32_LIBS} ${CMAKE_DL_LIBS})
set_property(TARGET tango PROPERTY LINK_FLAGS "/force:multiple")
set_property(TARGET tango PROPERTY PUBLIC_HEADER ${INCLUDE_OBJECT_MS})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tango PRIVATE OMNI_UNLOADABLE_STUBS _TANGO_LIB)

set_target_properties(tango PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
endif()
ELSE()
add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>
                            $<TARGET_OBJECTS:include_objects>)
target_link_libraries(tango PUBLIC ${ZMQ_PKG_LIBRARIES} ${OMNIORB_PKG_LIBRARIES} ${OMNICOS_PKG_LIBRARIES} ${OMNIDYN_PKG_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_options(tango PRIVATE -fPIC)
target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_definitions(tango PRIVATE OMNI_UNLOADABLE_STUBS _TANGO_LIB)

set_target_properties(tango PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})
ENDIF()

#install code
IF (WIN32)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
install(TARGETS tangod
        ARCHIVE DESTINATION lib COMPONENT static
        RUNTIME DESTINATION bin COMPONENT dynamic
        CONFIGURATIONS Debug)
else()
install(TARGETS tango
        ARCHIVE DESTINATION lib COMPONENT static
        RUNTIME DESTINATION bin COMPONENT dynamic)
endif()
install(DIRECTORY include/log4tango DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE)
install(DIRECTORY include/tango DESTINATION include COMPONENT headers PATTERN "*.h" PATTERN "*.txt" EXCLUDE PATTERN "*.vcproj" EXCLUDE PATTERN "*.cmake" EXCLUDE)
install(FILES include/resource.h DESTINATION include COMPONENT headers)
install(FILES include/tango.h DESTINATION include COMPONENT headers)
install(FILES generated/idl/tango.h DESTINATION include COMPONENT headers)
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
else()
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
#omniorb dynamic + zmq
endif()
IF(MSVC90)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc9_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v90-mt-gd-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v90-mt-gd-4_1_7.dll DESTINATION bin COMPONENT dynamic)
else()
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc9_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v90-mt-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v90-mt-4_1_7.dll DESTINATION bin COMPONENT dynamic)
endif()
ENDIF()
IF(MSVC10)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc10_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v100-mt-gd-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v100-mt-gd-4_1_7.dll DESTINATION bin COMPONENT dynamic)
else()
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc10_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v100-mt-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v100-mt-4_1_7.dll DESTINATION bin COMPONENT dynamic)
endif()
ENDIF()
IF(MSVC12)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc13_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v120-mt-gd-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v120-mt-gd-4_1_7.dll DESTINATION bin COMPONENT dynamic)
else()
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc13_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v120-mt-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v120-mt-4_1_7.dll DESTINATION bin COMPONENT dynamic)
endif()
ENDIF()
IF(MSVC14)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc14_rtd.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Debug/libzmq-v140-mt-gd-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Debug/libzmq-v140-mt-gd-4_1_7.dll DESTINATION bin COMPONENT dynamic)
else()
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniORB421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omniDynamic421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/omnithread40_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{OMNI_BASE}/bin/x86_win32/COS421_vc14_rt.dll DESTINATION bin COMPONENT dynamic)
install(FILES $ENV{ZMQ_BASE}/lib/Release/libzmq-v140-mt-4_1_7.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{ZMQ_BASE}/bin/Release/libzmq-v140-mt-4_1_7.dll DESTINATION bin COMPONENT dynamic)
endif()
ENDIF()
#pthreads
IF(CMAKE_CL_64)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/lib/x64/pthreadVC2.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/dll/x64/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
ELSE()
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/lib/x86/pthreadVC2.lib DESTINATION lib COMPONENT static)
install(FILES $ENV{PTHREAD_WIN}/Pre-built.2/dll/x86/pthreadVC2.dll DESTINATION bin COMPONENT dynamic)
ENDIF()


configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION include/pkgconfig COMPONENT headers)
ELSE()
install(TARGETS tango LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")
ENDIF()

IF (WIN32)
    IF(MSVC90)
      if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        IF(CMAKE_CL_64)
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x64_debug)
        ELSE()
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x86_debug)
        ENDIF()
      else()
        IF(CMAKE_CL_64)
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x64)
        ELSE()
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_v90_x86)
        ENDIF()
      endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ELSE()
      IF(CMAKE_CL_64)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")  
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x64_debug)
        else()
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x64)
        endif()
      ELSE()
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x86_debug)
        else()
          set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_${CMAKE_VS_PLATFORM_TOOLSET}_x86)
        endif()
      ENDIF()
    ENDIF(MSVC90)
  #
  # CPACK macros below here
  #
  set (CPACK_PACKAGE_NAME "Tango")
  set (CPACK_PACKAGE_VENDOR "www.tango-controls.org")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tango - Connecting Things Together")
  set (CPACK_PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
  set (CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
  set (CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
  set (CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})

  # Define components and their display names
  set (CPACK_COMPONENTS_ALL static dynamic headers)

  # Human readable component descriptions
  set (CPACK_COMPONENT_STATIC_DISPLAY_NAME
    "Tango.lib")
  set (CPACK_COMPONENT_DYNAMIC_DISPLAY_NAME
    "Tango.dll")
  set (CPACK_COMPONENT_HEADERS_DISPLAY_NAME
    "c++ Headers")
    set(CPACK_COMPONENT_STATIC_DESCRIPTION 
     "Tango.lib - for building projects")
   set(CPACK_COMPONENT_DYNAMIC_DESCRIPTION
     "Tango.dll - for running Tango executables")
   set(CPACK_COMPONENT_HEADERS_DESCRIPTION
     "C/C++ header files for use with Tango")

  # Define NSIS installation types
  set(CPACK_ALL_INSTALL_TYPES Development Runtime)
  set(CPACK_COMPONENT_STATIC_GROUP "Development" )
  set(CPACK_COMPONENT_DYNAMIC_GROUP "Runtime")
  set(CPACK_COMPONENT_HEADERS_GROUP "Development")

  set(CPACK_COMPONENT_STATIC_INSTALL_TYPES Development)
  set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Development)
  set(CPACK_COMPONENT_DYNAMIC_INSTALL_TYPES Development Runtime)

  set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
     "All of the tools you'll ever need to develop Tango software")
  set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION
     "All of the required libraries to launch Tango software")

  set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_BINARY_DIR}/LICENSE)
  set(CPACK_NSIS_HELP_LINK "http://www.tango-controls.org")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.tango-controls.org")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set(CPACK_NSIS_MENU_LINKS
      "http://tango-controls.readthedocs.io/en/latest/" "Tango Doc")
  set(CPACK_NSIS_MUI_ICON ${PROJECT_BINARY_DIR}/appveyor/tango.ico)
  set(CPACK_NSIS_MUI_UNIICON ${PROJECT_BINARY_DIR}/appveyor/tango.ico)
ELSE()
  SET(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_NAME libtango-dev)
  set(CPACK_DEBIAN_PACKAGE_VERSION ${LIBRARY_VERSION})
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcos4-dev, libzmq5-dev | libzmq3-dev")
  set(CPACK_DEBIAN_PACKAGE_CONFLICTS "libtango7-dev, libtango8-dev, libtango9-dev")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "TANGO team <tango@esrf.fr>") #required
ENDIF(WIN32)
include(CPack)
