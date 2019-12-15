#TODO windows

configure_file(run_with_fixture.sh.cmake run_with_fixture.sh @ONLY)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/run_with_fixture.sh)

configure_file(start_server.sh.cmake start_server.sh @ONLY)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/start_server.sh)

configure_file(kill_server.sh.cmake kill_server.sh @ONLY)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/kill_server.sh)
