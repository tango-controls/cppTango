set REL_DIR=..\..\..\..\
%REL_DIR%cxxtest\bin\cxxtestgen.py --template=%REL_DIR%cxxtest\template\tango_template.tpl -o runner.cpp ^
%REL_DIR%test_suite\cxx_syntax.cpp ^
%REL_DIR%test_suite\cxx_poll.cpp ^
%REL_DIR%test_suite\cxx_poll_admin.cpp ^
%REL_DIR%test_suite\cxx_dserver_misc.cpp ^
%REL_DIR%test_suite\cxx_always_hook.cpp ^
%REL_DIR%test_suite\cxx_blackbox.cpp ^
%REL_DIR%test_suite\cxx_attr_misc.cpp ^
%REL_DIR%test_suite\cxx_attr_write.cpp ^
%REL_DIR%test_suite\cxx_attr_conf.cpp ^
%REL_DIR%test_suite\cxx_attr.cpp ^
%REL_DIR%test_suite\cxx_misc_util.cpp ^
%REL_DIR%test_suite\cxx_templ_cmd.cpp ^
%REL_DIR%test_suite\cxx_exception.cpp ^
%REL_DIR%test_suite\cxx_cmd_query.cpp ^
%REL_DIR%test_suite\cxx_seq_vec.cpp ^
%REL_DIR%test_suite\cxx_test_state_on.cpp ^
%REL_DIR%test_suite\cxx_misc.cpp ^
%REL_DIR%test_suite\cxx_cmd_types.cpp
%REL_DIR%test_suite\cxx_group.cpp

rem %REL_DIR%test_suite\cxx_signal.cpp
rem %REL_DIR%test_suite\cxx_dserver_cmd.cpp
