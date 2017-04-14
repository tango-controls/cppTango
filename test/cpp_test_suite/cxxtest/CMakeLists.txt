find_package (Threads REQUIRED)

macro(CXX_GENERATE_TEST name)
    message("Generate ${name}.cpp")
    execute_process(COMMAND python cxxtestgen.py --template=${CMAKE_CURRENT_SOURCE_DIR}/template/tango_template.tpl
            -o ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp
            ../../new_tests/${name}.cpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
            RESULT_VARIABLE FAILED)

    if(${FAILED})
        message(SEND_ERROR " Failed to generate ${name}.cpp. rv=${FAILED}")
    endif()

    add_executable(${name} $<TARGET_OBJECTS:compare_test_object> ${name}.cpp)
    target_include_directories(${name} PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(${name} PRIVATE tango ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(${name} PRIVATE "-DVALGRIND -D_PTHREADS -D_REENTRANT")

    add_test(NAME "CXX::${name}" COMMAND $<TARGET_FILE:${name}>
            --device1=${DEV1}
            --device2=${DEV2}
            --device3=${DEV3}
            --device20=${DEV20}
            --fwd_device=${FWD_DEV}
            --loop=1
            --fulldsname=${SERV_NAME}/${INST_NAME}
            --serverhost=${HOST_NAME}
            --clienthost=${HOST_NAME}
            --serverversion=6
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
endmacro()

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
CXX_GENERATE_TEST(cxx_cmd_inout_enum)
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


#utilities
configure_file(bin/start_server.sh.cmake    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/start_server.sh @ONLY)
configure_file(bin/kill_server.sh.cmake     ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/kill_server.sh @ONLY)

# now copy the temporary into the final destination, setting the permissions
file(COPY ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/start_server.sh
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
file(COPY ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/kill_server.sh
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

configure_file(include/config.h.cmake include/config.h @ONLY)
