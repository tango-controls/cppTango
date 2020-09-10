find_package (Threads REQUIRED)
find_program(PYTHON_INTERPRETER NAMES python3)

if(NOT PYTHON_INTERPRETER)
  message(FATAL_ERROR "Could not find python 3.")
endif()

function(CXX_GENERATE_TEST name)
    message("Generate ${name}.cpp")
    set(GENERATED_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    execute_process(COMMAND "${PYTHON_INTERPRETER}" cxxtestgen.py
            --template=${CMAKE_CURRENT_SOURCE_DIR}/template/tango_template.tpl
            -o ${GENERATED_FILE}
            ../../new_tests/${name}.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
        RESULT_VARIABLE GENERATOR_OUTPUT)

    if(NOT EXISTS ${GENERATED_FILE})
      message(FATAL_ERROR " Failed to generate ${name}.cpp due to \"${GENERATOR_OUTPUT}\".")
    endif()

    add_executable(${name} $<TARGET_OBJECTS:compare_test_object> ${name}.cpp)
    target_include_directories(${name} PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(${name} PRIVATE tango ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(${name} PRIVATE ${COMMON_TEST_CXX_DEFS})
    if(USE_PCH)
        tango_target_test_pch(${name})
    endif()

    tango_add_test(NAME "CXX::${name}" COMMAND $<TARGET_FILE:${name}>
            --device1=${DEV1}
            --device2=${DEV2}
            --device3=${DEV3}
            --device20=${DEV20}
            --fwd_device=${FWD_DEV}
            --loop=1
            --fulldsname=${SERV_NAME}/${INST_NAME}
            --serverhost=${HOST_NAME}
            --clienthost=${HOST_NAME}
            --serverversion=5
            --docurl=http://www.tango-controls.org
            --devtype=TestDevice
            --dbserver=sys/database/2
            --outpath=/tmp/
            --refpath=${CMAKE_CURRENT_SOURCE_DIR}/../new_tests/out/
            --loglevel=0
            --dsloglevel=5
            --suiteloop=1
            --devicealias=${DEV1_ALIAS}
            --attributealias=${ATTR_ALIAS}
            --verbose=true)
endfunction()

CXX_GENERATE_TEST(cxx_syntax)
CXX_GENERATE_TEST(cxx_poll)
CXX_GENERATE_TEST(cxx_poll_admin)
CXX_GENERATE_TEST(cxx_dserver_cmd)
CXX_GENERATE_TEST(cxx_dserver_misc)
CXX_GENERATE_TEST(cxx_always_hook)
CXX_GENERATE_TEST(cxx_blackbox)
CXX_GENERATE_TEST(cxx_class_dev_signal)
CXX_GENERATE_TEST(cxx_class_signal)
CXX_GENERATE_TEST(cxx_attr_misc)
CXX_GENERATE_TEST(cxx_attrprop)
CXX_GENERATE_TEST(cxx_attr_write)
CXX_GENERATE_TEST(cxx_attr_conf)
CXX_GENERATE_TEST(cxx_attr)
CXX_GENERATE_TEST(cxx_misc_util)
CXX_GENERATE_TEST(cxx_templ_cmd)
CXX_GENERATE_TEST(cxx_exception)
CXX_GENERATE_TEST(cxx_cmd_query)
CXX_GENERATE_TEST(cxx_seq_vec)
CXX_GENERATE_TEST(cxx_signal)#TODO Windows
CXX_GENERATE_TEST(cxx_test_state_on)
CXX_GENERATE_TEST(cxx_misc)
CXX_GENERATE_TEST(cxx_cmd_types)
CXX_GENERATE_TEST(cxx_group)
CXX_GENERATE_TEST(cxx_encoded)
CXX_GENERATE_TEST(cxx_database)
CXX_GENERATE_TEST(cxx_mem_attr)
CXX_GENERATE_TEST(cxx_enum_att)
CXX_GENERATE_TEST(cxx_write_attr_hard)
CXX_GENERATE_TEST(cxx_fwd_att)
CXX_GENERATE_TEST(cxx_pipe_conf)
CXX_GENERATE_TEST(cxx_pipe)
CXX_GENERATE_TEST(cxx_z00_dyn_cmd)
CXX_GENERATE_TEST(cxx_old_poll)
CXX_GENERATE_TEST(cxx_server_event)
CXX_GENERATE_TEST(cxx_reconnection_zmq)
CXX_GENERATE_TEST(cxx_stateless_subscription)
CXX_GENERATE_TEST(cxx_nan_inf_in_prop)
CXX_GENERATE_TEST(cxx_asyn_reconnection)

configure_file(include/config.h.cmake include/config.h @ONLY)
