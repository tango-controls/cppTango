set(SOURCES
        helpers.hxx
        helpers.cxx
        command.hxx
        add_obj_command.hxx
        add_obj_command.cxx
        rem_obj_command.cxx
        rem_obj_command.hxx
        rem_ext_trigger_command.cxx
        rem_ext_trigger_command.hxx
        add_trigger_command.cxx
        add_trigger_command.hxx
        rem_dev_command.cxx
        rem_dev_command.hxx
        update_poll_period_command.cxx
        update_poll_period_command.hxx
        start_polling_command.cxx
        start_polling_command.hxx
        stop_polling_command.cxx
        stop_polling_command.hxx
        exit_command.cxx
        exit_command.hxx)

add_library(polling_objects OBJECT ${SOURCES})
target_include_directories(polling_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(polling_objects PRIVATE -std=c++1y -fPIC)