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
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
execute_process(COMMAND ${OMNIIDL_PATH}/omniidl.exe -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FAILED)
        else()
execute_process(COMMAND ${OMNIIDL_PATH}/omniidl.exe -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FAILED)
        endif()
    else()
execute_process(COMMAND ${OMNIIDL_PATH}omniidl -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FAILED)
    endif()

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

add_library(idl_objects OBJECT ${SOURCES} tango.h)

IF(WIN32)
install(FILES tango.h DESTINATION include/idl)
ELSE()
target_compile_options(idl_objects PRIVATE -fPIC)
install(FILES tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/idl")
ENDIF()
