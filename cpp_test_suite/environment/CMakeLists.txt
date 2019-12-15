#TODO windows

SET(SCRIPTS
    run_with_fixture.sh
    setup_database.sh
    setup_devices.sh
    start_server.sh
    kill_server.sh
)

foreach(SCRIPT ${SCRIPTS})
    configure_file(${SCRIPT}.cmake ${SCRIPT} @ONLY)
    execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT})
endforeach(SCRIPT)
