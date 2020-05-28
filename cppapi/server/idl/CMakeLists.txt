message("Generate tango.h, tangoSK.cpp and tangoDynSK.cpp from idl")

message("Using OMNIIDL_PATH=${OMNIIDL_PATH}")
message("Using IDL=${IDL_PKG_INCLUDE_DIRS}")

find_program(OMNIIDL NAMES omniidl
                     PATHS ${OMNIIDL_PATH})

if(NOT OMNIIDL)
  message(FATAL_ERROR "Could not find omniidl")
endif()

execute_process(COMMAND ${OMNIIDL} -I${IDL_PKG_INCLUDE_DIRS} -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba ${IDL_PKG_INCLUDE_DIRS}/tango.idl
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                RESULT_VARIABLE OMNIIDL_RETURN_CODE)

if(OMNIIDL_RETURN_CODE)
    message(SEND_ERROR " Failed to generate source files from idl. rv=${OMNIIDL_RETURN_CODE}")
endif()

function(replace_in_file FILENAME SEARCH NEW_CONTENT)
    file(READ "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}" ORIGINAL_DATA)
    string(REPLACE "${SEARCH}" "${NEW_CONTENT}" REPLACED_DATA "${ORIGINAL_DATA}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}" "${REPLACED_DATA}")
endfunction()

function(add_after_in_file FILENAME SEARCH CONTENT)
    replace_in_file(${FILENAME} "${SEARCH}" "${SEARCH} ${CONTENT}")
endfunction()

add_after_in_file(tango.h "typedef _CORBA_ConstrType_Variable_Var<AttributeValue_4> _var_type;" "\n\n\
    //Added by Tango team\n\
    virtual ~AttributeValue_4() {if (mut_ptr != NULL)mut_ptr->unlock();}\n\
    AttributeValue_4() {mut_ptr=NULL;}\n\
    void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\n\
    void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\n\
    omni_mutex *get_attr_mutex() {return mut_ptr;}\n\
    omni_mutex *mut_ptr;\n")

add_after_in_file(tango.h "typedef _CORBA_ConstrType_Variable_Var<AttributeValue_5> _var_type;" "\n\n\
    //Added by Tango team\n\
    virtual ~AttributeValue_5() {if (mut_ptr != NULL)mut_ptr->unlock();}\n\
    AttributeValue_5() {mut_ptr=NULL;}\n\
    void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\n\
    void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\n\
    omni_mutex *get_attr_mutex() {return mut_ptr;}\n\
    omni_mutex *mut_ptr;\n")

add_after_in_file(tango.h "typedef\ _CORBA_ConstrType_Variable_Var<DevPipeData> _var_type;" "\n\n\
    //Added by Tango team\n\
    virtual ~DevPipeData() {if (mut_ptr != NULL)mut_ptr->unlock();}\n\
    DevPipeData() {mut_ptr=NULL;}\n\
    void set_pipe_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\n\
    void rel_pipe_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\n\
    omni_mutex *mut_ptr;\n")

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
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.h" DESTINATION include)
else()
    target_compile_options(idl_objects PRIVATE -fPIC)
    target_compile_definitions(idl_objects PRIVATE OMNI_UNLOADABLE_STUBS)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tango.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/tango/idl")
endif()
