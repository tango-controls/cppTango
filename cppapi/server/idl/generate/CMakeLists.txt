message("Generate tango.h, tangoSK.cpp and tangoDybSK.cpp from idl")
message("Using OMNIIDL_PATH=${OMNIIDL_PATH}")
message("Using IDL=${IDL_PKG_INCLUDE_DIRS}")

execute_process(COMMAND ${OMNIIDL_PATH}omniidl -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                RESULT_VARIABLE FAILED)

if(${FAILED}) 
	message(SEND_ERROR " Failed to generate source files from idl. rv=${FAILED}")
endif()

message("Using PATCHES_SOURCE_DIR=${PATCHES_SOURCE_DIR}")

FILE(GLOB ENHANCEMENTS ${PATCHES_SOURCE_DIR}/Enhance*)

foreach(ENHANCEMENT ${ENHANCEMENTS})
	message("Applying enhancement ${ENHANCEMENT}")
	execute_process(COMMAND sed -i -f ${ENHANCEMENT} tango.h
		            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		            RESULT_VARIABLE FAILED)
    #non-zero
    if(${FAILED})
       	message(SEND_ERROR " Failed to apply ${ENHANCEMENT}. rv=${FAILED}")
    endif()
endforeach(ENHANCEMENT)

