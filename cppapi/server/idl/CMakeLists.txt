message("Remove tango.h, tangoSK.cpp and tangoDynSK.cpp from idl if exist")

execute_process(COMMAND rm tango.h tangoSK.cpp tangoDynSK.cpp
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message("Generate tango.h, tangoSK.cpp and tangoDynSK.cpp from idl")

message("Using OMNIIDL_PATH=${OMNIIDL_PATH}")
message("Using IDL=${IDL_PKG_INCLUDE_DIRS}")

MESSAGE( STATUS "OMNIIDL_PATH: " ${OMNIIDL_PATH} )

if(WIN32)
    set_source_files_properties(tangoDynSK.cpp PROPERTIES COMPILE_DEFINITIONS _OMNIORB_STUB_DLL)
    set_source_files_properties(tangoDynSK.cpp PROPERTIES COMPILE_DEFINITIONS __NT__)
    set_source_files_properties(tangoSK.cpp PROPERTIES COMPILE_DEFINITIONS _OMNIORB_STUB_DLL)
    set_source_files_properties(tangoSK.cpp PROPERTIES COMPILE_DEFINITIONS __NT__)
    execute_process(COMMAND ${OMNIIDL_PATH}/omniidl.exe -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE FAILED)
else(WIN32)
    execute_process(COMMAND ${OMNIIDL_PATH}omniidl -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE FAILED)
endif(WIN32)

if(${FAILED})
    message(SEND_ERROR " Failed to generate source files from idl. rv=${FAILED}")
endif()

FILE(GLOB ENHANCEMENTS Enhance*)

MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

foreach(ENHANCEMENT ${ENHANCEMENTS})
    message("Applying enhancement ${ENHANCEMENT}")
    execute_process(COMMAND sed -i -f ${ENHANCEMENT} tango.h
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FAILED)

        #non-zero
    if(${FAILED})
        message(SEND_ERROR " Failed to apply ${ENHANCEMENT}. rv=${FAILED}")
    endif()
endforeach(ENHANCEMENT)

set(SOURCES tangoSK.cpp
            tangoDynSK.cpp)


if(WIN32)
    add_library(idl_objects_dyn OBJECT ${SOURCES})
    add_library(idl_objects_sta OBJECT ${SOURCES})
    target_compile_definitions(idl_objects_dyn PRIVATE "${dyn_defs};__x86__;__NT__;__OSVERSION__=4;__WIN32__;_WIN32_WINNT=0x0400;")
    target_compile_definitions(idl_objects_sta PRIVATE "${static_defs};__x86__;__NT__;__OSVERSION__=4;__WIN32__;_WIN32_WINNT=0x0400;")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(idl_objects_sta PRIVATE "/MTd")
    else()
        target_compile_options(idl_objects_sta PRIVATE "/MT")
    endif()
    install(FILES tango.h DESTINATION include/idl)
else(WIN32)
    add_library(idl_objects OBJECT ${SOURCES} tango.h)
    target_compile_options(idl_objects PRIVATE -fPIC)
    target_compile_definitions(idl_objects PRIVATE OMNI_UNLOADABLE_STUBS)
    install(FILES tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/idl")
endif(WIN32)
