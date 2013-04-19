set REL_DIR=..\..\..\..\
%REL_DIR%cxxtest\bin\cxxtestgen.py --template=%REL_DIR%cxxtest\template\tango_template.tpl -o runner.cpp ^
%REL_DIR%new_tests\cxx_syntax.cpp ^
%REL_DIR%new_tests\cxx_poll.cpp ^
%REL_DIR%new_tests\cxx_poll_admin.cpp ^
%REL_DIR%new_tests\cxx_dserver_cmd.cpp ^
%REL_DIR%new_tests\cxx_dserver_misc.cpp ^
%REL_DIR%new_tests\cxx_always_hook.cpp ^
%REL_DIR%new_tests\cxx_blackbox.cpp ^
%REL_DIR%new_tests\cxx_attr_misc.cpp ^
%REL_DIR%new_tests\cxx_attrprop.cpp ^
%REL_DIR%new_tests\cxx_attr_write.cpp ^
%REL_DIR%new_tests\cxx_attr_conf.cpp ^
%REL_DIR%new_tests\cxx_attr.cpp ^
%REL_DIR%new_tests\cxx_misc_util.cpp ^
%REL_DIR%new_tests\cxx_templ_cmd.cpp ^
%REL_DIR%new_tests\cxx_exception.cpp ^
%REL_DIR%new_tests\cxx_cmd_query.cpp ^
%REL_DIR%new_tests\cxx_seq_vec.cpp ^
%REL_DIR%new_tests\cxx_test_state_on.cpp ^
%REL_DIR%new_tests\cxx_misc.cpp ^
%REL_DIR%new_tests\cxx_cmd_types.cpp ^
%REL_DIR%new_tests\cxx_group.cpp ^
%REL_DIR%new_tests\cxx_encoded.cpp ^
%REL_DIR%new_tests\cxx_database.cpp ^
%REL_DIR%new_tests\cxx_mem_attr.cpp ^
%REL_DIR%new_tests\cxx_write_attr_hard.cpp

rem %REL_DIR%new_tests\cxx_signal.cpp
rem %REL_DIR%new_tests\cxx_dserver_cmd.cpp
