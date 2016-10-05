message("Generate tango.h, tangoSK.cpp and tangoDybSK.cpp from idl")

execute_process(COMMAND ${OMNIIDL_PATH}omniidl -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FAILED)

if(${FAILED})
    message(SEND_ERROR " Failed to generate source files from idl. rv=${FAILED}")
endif()

FILE(GLOB ENHANCEMENTS Enhance*)

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
target_compile_options(idl_objects PRIVATE -fPIC)

install(FILES tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/idl")
