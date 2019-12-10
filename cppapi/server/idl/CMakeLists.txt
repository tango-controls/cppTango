message("Generate tango.h, tangoSK.cpp and tangoDynSK.cpp from idl")

message("Using OMNIIDL_PATH=${OMNIIDL_PATH}")
message("Using IDL=${IDL_PKG_INCLUDE_DIRS}")

find_program(OMNIIDL NAMES omniidl
                     PATHS ${OMNIIDL_PATH})

if(${OMNIIDL-NOTFOUND})
  message(FATAL_ERROR "Could not find omniidl")
endif()

execute_process(COMMAND ${OMNIIDL} -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                RESULT_VARIABLE FAILED)

if(${FAILED})
    message(SEND_ERROR " Failed to generate source files from idl. rv=${FAILED}")
endif()

SET(ENHANCEMENTS
    EnhanceAttributeValue_4
    EnhanceAttributeValue_5
    EnhanceDevPipeData
)

foreach(ENHANCEMENT ${ENHANCEMENTS})
    message("Applying enhancement ${ENHANCEMENT}")
    execute_process(COMMAND sed -i -f ${CMAKE_CURRENT_SOURCE_DIR}/${ENHANCEMENT} tango.h
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    RESULT_VARIABLE FAILED)

    if(${FAILED})
        message(SEND_ERROR " Failed to apply ${ENHANCEMENT}. rv=${FAILED}")
    endif()
endforeach(ENHANCEMENT)

set(SOURCES ${CMAKE_CURRENT_BINARY_DIR}/tangoSK.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/tangoDynSK.cpp)

if(WIN32)
    foreach(file ${SOURCES})
        set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS _OMNIORB_STUB_DLL)
        set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS __NT__)
    endforeach(file)

    add_library(idl_objects_dyn OBJECT ${SOURCES})
    add_library(idl_objects_sta OBJECT ${SOURCES})
    target_compile_definitions(idl_objects_dyn PRIVATE "${dyn_defs};__x86__;__NT__;__OSVERSION__=4;__WIN32__;_WIN32_WINNT=0x0400;")
    target_compile_definitions(idl_objects_sta PRIVATE "${static_defs};__x86__;__NT__;__OSVERSION__=4;__WIN32__;_WIN32_WINNT=0x0400;")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(idl_objects_sta PRIVATE "/MTd")
    else()
        target_compile_options(idl_objects_sta PRIVATE "/MT")
    endif()
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tango.h DESTINATION include/idl)
else(WIN32)
    add_library(idl_objects OBJECT ${SOURCES} tango.h)
    target_compile_options(idl_objects PRIVATE -fPIC)
    target_compile_definitions(idl_objects PRIVATE OMNI_UNLOADABLE_STUBS)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/idl")
endif(WIN32)
