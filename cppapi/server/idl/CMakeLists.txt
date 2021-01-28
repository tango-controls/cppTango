message("Using OMNIIDL_PATH=${OMNIIDL_PATH}")
message("Using IDL=${IDL_PKG_INCLUDE_DIRS}")

find_program(OMNIIDL NAMES omniidl
                     PATHS ${OMNIIDL_PATH})

if(NOT OMNIIDL)
  message(FATAL_ERROR "Could not find omniidl")
endif()

add_custom_command(OUTPUT tango.h tangoSK.cpp tangoDynSK.cpp
                   COMMAND ${OMNIIDL} -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/apply-patches.cmake
                   COMMENT "Generate tango.h, tangoSK.cpp and tangoDynSK.cpp from idl"
                   DEPENDS ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                           ${CMAKE_CURRENT_SOURCE_DIR}/apply-patches.cmake
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES ${CMAKE_CURRENT_BINARY_DIR}/tangoSK.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/tangoDynSK.cpp)

if(WIN32)
    foreach(file ${SOURCES})
        set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS _OMNIORB_STUB_DLL)
        set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS __NT__)
    endforeach(file)
endif()

add_library(idl_objects OBJECT ${SOURCES} tango.h)

if(WIN32)
    target_compile_definitions(idl_objects PRIVATE "${windows_defs};__x86__;__NT__;__OSVERSION__=4;__WIN32__;_WIN32_WINNT=0x0400;")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.h" DESTINATION include/tango/idl)
else()
    target_compile_options(idl_objects PRIVATE -fPIC)
    target_compile_definitions(idl_objects PRIVATE OMNI_UNLOADABLE_STUBS)

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      target_compile_options(idl_objects PRIVATE -Wno-maybe-uninitialized -Wno-unused-parameter)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(idl_objects PRIVATE -Wno-sometimes-uninitialized -Wno-unused-variable -Wno-unused-parameter)
    endif()

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/idl")
endif()
