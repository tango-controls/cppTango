/* Generated file, do not edit */

// -*- C++ -*-
// Tango main template
//

#define CXXTEST_HAVE_EH
#define CXXTEST_HAVE_STD
#define CXXTEST_ABORT_TEST_ON_FAIL
#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include <cxxtest/TangoPrinter.h>

int main( int argc, char **argv )
{
    return CxxTest::TangoPrinter().run( argc, argv );
}

// The CxxTest "world"
#include "../../../../new_tests/cxx_syntax.cpp"

static SyntaxTestSuite *suite_SyntaxTestSuite = 0;

static CxxTest::List Tests_SyntaxTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<SyntaxTestSuite> suiteDescription_SyntaxTestSuite( "../../../../new_tests/cxx_syntax.cpp", 17, "SyntaxTestSuite", Tests_SyntaxTestSuite, suite_SyntaxTestSuite, 96, 101 );

static class TestDescription_SyntaxTestSuite_test_check_device_name_syntax_when_creating_DeviceProxy_instance : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_check_device_name_syntax_when_creating_DeviceProxy_instance() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 156, "test_check_device_name_syntax_when_creating_DeviceProxy_instance" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_check_device_name_syntax_when_creating_DeviceProxy_instance(); }
} testDescription_SyntaxTestSuite_test_check_device_name_syntax_when_creating_DeviceProxy_instance;

static class TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_my_alias : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_my_alias() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 188, "test_connect_to_a_device_via_its_alias_as_my_alias" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_connect_to_a_device_via_its_alias_as_my_alias(); }
} testDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_my_alias;

static class TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_host_port_my_alias : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_host_port_my_alias() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 206, "test_connect_to_a_device_via_its_alias_as_host_port_my_alias" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_connect_to_a_device_via_its_alias_as_host_port_my_alias(); }
} testDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_alias_as_host_port_my_alias;

static class TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_classic_name : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_classic_name() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 225, "test_connect_to_a_device_via_its_classic_name" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_connect_to_a_device_via_its_classic_name(); }
} testDescription_SyntaxTestSuite_test_connect_to_a_device_via_its_classic_name;

static class TestDescription_SyntaxTestSuite_test_check_attribute_name_syntax_when_creating_AttributeProxy_instance : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_check_attribute_name_syntax_when_creating_AttributeProxy_instance() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 232, "test_check_attribute_name_syntax_when_creating_AttributeProxy_instance" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_check_attribute_name_syntax_when_creating_AttributeProxy_instance(); }
} testDescription_SyntaxTestSuite_test_check_attribute_name_syntax_when_creating_AttributeProxy_instance;

static class TestDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_attribute_alias : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_attribute_alias() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 265, "test_connect_to_an_attribute_via_its_alias_as_attribute_alias" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_connect_to_an_attribute_via_its_alias_as_attribute_alias(); }
} testDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_attribute_alias;

static class TestDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 276, "test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias(); }
} testDescription_SyntaxTestSuite_test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias;

static class TestDescription_SyntaxTestSuite_test_check_alias_call : public CxxTest::RealTestDescription {
public:
 TestDescription_SyntaxTestSuite_test_check_alias_call() : CxxTest::RealTestDescription( Tests_SyntaxTestSuite, suiteDescription_SyntaxTestSuite, 288, "test_check_alias_call" ) {}
 void runTest() { if ( suite_SyntaxTestSuite ) suite_SyntaxTestSuite->test_check_alias_call(); }
} testDescription_SyntaxTestSuite_test_check_alias_call;

#include "../../../../new_tests/cxx_poll.cpp"

static PollTestSuite__loop *suite_PollTestSuite__loop = 0;

static CxxTest::List Tests_PollTestSuite__loop = { 0, 0 };
CxxTest::DynamicSuiteDescription<PollTestSuite__loop> suiteDescription_PollTestSuite__loop( "../../../../new_tests/cxx_poll.cpp", 17, "PollTestSuite__loop", Tests_PollTestSuite__loop, suite_PollTestSuite__loop, 193, 198 );

static class TestDescription_PollTestSuite__loop_test_polling_exceptions : public CxxTest::RealTestDescription {
public:
 TestDescription_PollTestSuite__loop_test_polling_exceptions() : CxxTest::RealTestDescription( Tests_PollTestSuite__loop, suiteDescription_PollTestSuite__loop, 209, "test_polling_exceptions" ) {}
 void runTest() { if ( suite_PollTestSuite__loop ) suite_PollTestSuite__loop->test_polling_exceptions(); }
} testDescription_PollTestSuite__loop_test_polling_exceptions;

static class TestDescription_PollTestSuite__loop_test_commands_and_attributes_obtained_from_polling_buffer : public CxxTest::RealTestDescription {
public:
 TestDescription_PollTestSuite__loop_test_commands_and_attributes_obtained_from_polling_buffer() : CxxTest::RealTestDescription( Tests_PollTestSuite__loop, suiteDescription_PollTestSuite__loop, 290, "test_commands_and_attributes_obtained_from_polling_buffer" ) {}
 void runTest() { if ( suite_PollTestSuite__loop ) suite_PollTestSuite__loop->test_commands_and_attributes_obtained_from_polling_buffer(); }
} testDescription_PollTestSuite__loop_test_commands_and_attributes_obtained_from_polling_buffer;

static class TestDescription_PollTestSuite__loop_test_check_the_CACHE_DEV_source_parameter : public CxxTest::RealTestDescription {
public:
 TestDescription_PollTestSuite__loop_test_check_the_CACHE_DEV_source_parameter() : CxxTest::RealTestDescription( Tests_PollTestSuite__loop, suiteDescription_PollTestSuite__loop, 372, "test_check_the_CACHE_DEV_source_parameter" ) {}
 void runTest() { if ( suite_PollTestSuite__loop ) suite_PollTestSuite__loop->test_check_the_CACHE_DEV_source_parameter(); }
} testDescription_PollTestSuite__loop_test_check_the_CACHE_DEV_source_parameter;

#include "../../../../new_tests/cxx_poll_admin.cpp"

static PollAdminTestSuite__loop *suite_PollAdminTestSuite__loop = 0;

static CxxTest::List Tests_PollAdminTestSuite__loop = { 0, 0 };
CxxTest::DynamicSuiteDescription<PollAdminTestSuite__loop> suiteDescription_PollAdminTestSuite__loop( "../../../../new_tests/cxx_poll_admin.cpp", 17, "PollAdminTestSuite__loop", Tests_PollAdminTestSuite__loop, suite_PollAdminTestSuite__loop, 256, 261 );

static class TestDescription_PollAdminTestSuite__loop_test_Start_Stop_polling : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_Start_Stop_polling() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 272, "test_Start_Stop_polling" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_Start_Stop_polling(); }
} testDescription_PollAdminTestSuite__loop_test_Start_Stop_polling;

static class TestDescription_PollAdminTestSuite__loop_test_get_polling_status_for_a_non_polled_device : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_get_polling_status_for_a_non_polled_device() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 305, "test_get_polling_status_for_a_non_polled_device" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_get_polling_status_for_a_non_polled_device(); }
} testDescription_PollAdminTestSuite__loop_test_get_polling_status_for_a_non_polled_device;

static class TestDescription_PollAdminTestSuite__loop_test_start_polling_a_command : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_start_polling_a_command() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 326, "test_start_polling_a_command" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_start_polling_a_command(); }
} testDescription_PollAdminTestSuite__loop_test_start_polling_a_command;

static class TestDescription_PollAdminTestSuite__loop_test_check_if_command_polling_is_started : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_check_if_command_polling_is_started() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 345, "test_check_if_command_polling_is_started" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_check_if_command_polling_is_started(); }
} testDescription_PollAdminTestSuite__loop_test_check_if_command_polling_is_started;

static class TestDescription_PollAdminTestSuite__loop_test_update_command_polling_period : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_update_command_polling_period() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 371, "test_update_command_polling_period" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_update_command_polling_period(); }
} testDescription_PollAdminTestSuite__loop_test_update_command_polling_period;

static class TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_command : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_command() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 402, "test_stop_polling_the_command" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_stop_polling_the_command(); }
} testDescription_PollAdminTestSuite__loop_test_stop_polling_the_command;

static class TestDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 425, "test_start_polling_an_attribute" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_start_polling_an_attribute(); }
} testDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute;

static class TestDescription_PollAdminTestSuite__loop_test_check_if_attribute_polling_is_started : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_check_if_attribute_polling_is_started() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 444, "test_check_if_attribute_polling_is_started" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_check_if_attribute_polling_is_started(); }
} testDescription_PollAdminTestSuite__loop_test_check_if_attribute_polling_is_started;

static class TestDescription_PollAdminTestSuite__loop_test_update_attribute_polling_period : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_update_attribute_polling_period() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 470, "test_update_attribute_polling_period" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_update_attribute_polling_period(); }
} testDescription_PollAdminTestSuite__loop_test_update_attribute_polling_period;

static class TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 501, "test_stop_polling_the_attribute" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_stop_polling_the_attribute(); }
} testDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute;

static class TestDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute_and_a_command : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute_and_a_command() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 524, "test_start_polling_an_attribute_and_a_command" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_start_polling_an_attribute_and_a_command(); }
} testDescription_PollAdminTestSuite__loop_test_start_polling_an_attribute_and_a_command;

static class TestDescription_PollAdminTestSuite__loop_test_check_if_polling_is_started : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_check_if_polling_is_started() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 554, "test_check_if_polling_is_started" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_check_if_polling_is_started(); }
} testDescription_PollAdminTestSuite__loop_test_check_if_polling_is_started;

static class TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute_and_the_command : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute_and_the_command() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 584, "test_stop_polling_the_attribute_and_the_command" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_stop_polling_the_attribute_and_the_command(); }
} testDescription_PollAdminTestSuite__loop_test_stop_polling_the_attribute_and_the_command;

static class TestDescription_PollAdminTestSuite__loop_test_start_polling_for_several_devices : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_start_polling_for_several_devices() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 617, "test_start_polling_for_several_devices" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_start_polling_for_several_devices(); }
} testDescription_PollAdminTestSuite__loop_test_start_polling_for_several_devices;

static class TestDescription_PollAdminTestSuite__loop_test_check_if_polling_for_serveral_devices_is_started : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_check_if_polling_for_serveral_devices_is_started() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 665, "test_check_if_polling_for_serveral_devices_is_started" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_check_if_polling_for_serveral_devices_is_started(); }
} testDescription_PollAdminTestSuite__loop_test_check_if_polling_for_serveral_devices_is_started;

static class TestDescription_PollAdminTestSuite__loop_test_stop_polling_for_several_devices : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_stop_polling_for_several_devices() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 708, "test_stop_polling_for_several_devices" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_stop_polling_for_several_devices(); }
} testDescription_PollAdminTestSuite__loop_test_stop_polling_for_several_devices;

static class TestDescription_PollAdminTestSuite__loop_test_device_polling_after_a_restart : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_device_polling_after_a_restart() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 756, "test_device_polling_after_a_restart" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_device_polling_after_a_restart(); }
} testDescription_PollAdminTestSuite__loop_test_device_polling_after_a_restart;

static class TestDescription_PollAdminTestSuite__loop_test_poll_object_which_returns_an_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_poll_object_which_returns_an_exception() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 840, "test_poll_object_which_returns_an_exception" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_poll_object_which_returns_an_exception(); }
} testDescription_PollAdminTestSuite__loop_test_poll_object_which_returns_an_exception;

static class TestDescription_PollAdminTestSuite__loop_test_polling_for_several_devices_after_a_ServerRestart : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_polling_for_several_devices_after_a_ServerRestart() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 920, "test_polling_for_several_devices_after_a_ServerRestart" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_polling_for_several_devices_after_a_ServerRestart(); }
} testDescription_PollAdminTestSuite__loop_test_polling_for_several_devices_after_a_ServerRestart;

static class TestDescription_PollAdminTestSuite__loop_test_start_a_command_externally_triggered : public CxxTest::RealTestDescription {
public:
 TestDescription_PollAdminTestSuite__loop_test_start_a_command_externally_triggered() : CxxTest::RealTestDescription( Tests_PollAdminTestSuite__loop, suiteDescription_PollAdminTestSuite__loop, 1079, "test_start_a_command_externally_triggered" ) {}
 void runTest() { if ( suite_PollAdminTestSuite__loop ) suite_PollAdminTestSuite__loop->test_start_a_command_externally_triggered(); }
} testDescription_PollAdminTestSuite__loop_test_start_a_command_externally_triggered;

#include "../../../../new_tests/cxx_dserver_misc.cpp"

static DServerMiscTestSuite *suite_DServerMiscTestSuite = 0;

static CxxTest::List Tests_DServerMiscTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<DServerMiscTestSuite> suiteDescription_DServerMiscTestSuite( "../../../../new_tests/cxx_dserver_misc.cpp", 17, "DServerMiscTestSuite", Tests_DServerMiscTestSuite, suite_DServerMiscTestSuite, 120, 125 );

static class TestDescription_DServerMiscTestSuite_test_State_and_Status_commands : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_State_and_Status_commands() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 136, "test_State_and_Status_commands" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_State_and_Status_commands(); }
} testDescription_DServerMiscTestSuite_test_State_and_Status_commands;

static class TestDescription_DServerMiscTestSuite_test_DevRestart_command_on_the_dserver_device : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_DevRestart_command_on_the_dserver_device() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 153, "test_DevRestart_command_on_the_dserver_device" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_DevRestart_command_on_the_dserver_device(); }
} testDescription_DServerMiscTestSuite_test_DevRestart_command_on_the_dserver_device;

static class TestDescription_DServerMiscTestSuite_test_DevRestart_command_on_classical_device : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_DevRestart_command_on_classical_device() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 176, "test_DevRestart_command_on_classical_device" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_DevRestart_command_on_classical_device(); }
} testDescription_DServerMiscTestSuite_test_DevRestart_command_on_classical_device;

static class TestDescription_DServerMiscTestSuite_test_name_description_state_ans_status_CORBA_attributes : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_name_description_state_ans_status_CORBA_attributes() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 207, "test_name_description_state_ans_status_CORBA_attributes" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_name_description_state_ans_status_CORBA_attributes(); }
} testDescription_DServerMiscTestSuite_test_name_description_state_ans_status_CORBA_attributes;

static class TestDescription_DServerMiscTestSuite_test_ping_the_device : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_ping_the_device() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 228, "test_ping_the_device" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_ping_the_device(); }
} testDescription_DServerMiscTestSuite_test_ping_the_device;

static class TestDescription_DServerMiscTestSuite_test_info_call : public CxxTest::RealTestDescription {
public:
 TestDescription_DServerMiscTestSuite_test_info_call() : CxxTest::RealTestDescription( Tests_DServerMiscTestSuite, suiteDescription_DServerMiscTestSuite, 235, "test_info_call" ) {}
 void runTest() { if ( suite_DServerMiscTestSuite ) suite_DServerMiscTestSuite->test_info_call(); }
} testDescription_DServerMiscTestSuite_test_info_call;

#include "../../../../new_tests/cxx_always_hook.cpp"

static AlwaysHookTestSuite *suite_AlwaysHookTestSuite = 0;

static CxxTest::List Tests_AlwaysHookTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<AlwaysHookTestSuite> suiteDescription_AlwaysHookTestSuite( "../../../../new_tests/cxx_always_hook.cpp", 18, "AlwaysHookTestSuite", Tests_AlwaysHookTestSuite, suite_AlwaysHookTestSuite, 166, 171 );

static class TestDescription_AlwaysHookTestSuite_test_checking_OILong_command_preceding_the_test : public CxxTest::RealTestDescription {
public:
 TestDescription_AlwaysHookTestSuite_test_checking_OILong_command_preceding_the_test() : CxxTest::RealTestDescription( Tests_AlwaysHookTestSuite, suiteDescription_AlwaysHookTestSuite, 182, "test_checking_OILong_command_preceding_the_test" ) {}
 void runTest() { if ( suite_AlwaysHookTestSuite ) suite_AlwaysHookTestSuite->test_checking_OILong_command_preceding_the_test(); }
} testDescription_AlwaysHookTestSuite_test_checking_OILong_command_preceding_the_test;

static class TestDescription_AlwaysHookTestSuite_test_changing_logging_level : public CxxTest::RealTestDescription {
public:
 TestDescription_AlwaysHookTestSuite_test_changing_logging_level() : CxxTest::RealTestDescription( Tests_AlwaysHookTestSuite, suiteDescription_AlwaysHookTestSuite, 194, "test_changing_logging_level" ) {}
 void runTest() { if ( suite_AlwaysHookTestSuite ) suite_AlwaysHookTestSuite->test_changing_logging_level(); }
} testDescription_AlwaysHookTestSuite_test_changing_logging_level;

static class TestDescription_AlwaysHookTestSuite_test_comparing_input_with_output : public CxxTest::RealTestDescription {
public:
 TestDescription_AlwaysHookTestSuite_test_comparing_input_with_output() : CxxTest::RealTestDescription( Tests_AlwaysHookTestSuite, suiteDescription_AlwaysHookTestSuite, 248, "test_comparing_input_with_output" ) {}
 void runTest() { if ( suite_AlwaysHookTestSuite ) suite_AlwaysHookTestSuite->test_comparing_input_with_output(); }
} testDescription_AlwaysHookTestSuite_test_comparing_input_with_output;

#include "../../../../new_tests/cxx_blackbox.cpp"

static BlackboxTestSuite *suite_BlackboxTestSuite = 0;

static CxxTest::List Tests_BlackboxTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<BlackboxTestSuite> suiteDescription_BlackboxTestSuite( "../../../../new_tests/cxx_blackbox.cpp", 21, "BlackboxTestSuite", Tests_BlackboxTestSuite, suite_BlackboxTestSuite, 137, 142 );

static class TestDescription_BlackboxTestSuite_test_blackbox_device_feature : public CxxTest::RealTestDescription {
public:
 TestDescription_BlackboxTestSuite_test_blackbox_device_feature() : CxxTest::RealTestDescription( Tests_BlackboxTestSuite, suiteDescription_BlackboxTestSuite, 153, "test_blackbox_device_feature" ) {}
 void runTest() { if ( suite_BlackboxTestSuite ) suite_BlackboxTestSuite->test_blackbox_device_feature(); }
} testDescription_BlackboxTestSuite_test_blackbox_device_feature;

#include "../../../../new_tests/cxx_attr_misc.cpp"

static AttrMiscTestSuite *suite_AttrMiscTestSuite = 0;

static CxxTest::List Tests_AttrMiscTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<AttrMiscTestSuite> suiteDescription_AttrMiscTestSuite( "../../../../new_tests/cxx_attr_misc.cpp", 17, "AttrMiscTestSuite", Tests_AttrMiscTestSuite, suite_AttrMiscTestSuite, 154, 159 );

static class TestDescription_AttrMiscTestSuite_test_set_get_alarms : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_set_get_alarms() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 172, "test_set_get_alarms" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_set_get_alarms(); }
} testDescription_AttrMiscTestSuite_test_set_get_alarms;

static class TestDescription_AttrMiscTestSuite_test_set_get_ranges : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_set_get_ranges() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 238, "test_set_get_ranges" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_set_get_ranges(); }
} testDescription_AttrMiscTestSuite_test_set_get_ranges;

static class TestDescription_AttrMiscTestSuite_test_set_get_properties : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_set_get_properties() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 286, "test_set_get_properties" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_set_get_properties(); }
} testDescription_AttrMiscTestSuite_test_set_get_properties;

static class TestDescription_AttrMiscTestSuite_test_read_attribute_exceptions : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_read_attribute_exceptions() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 779, "test_read_attribute_exceptions" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_read_attribute_exceptions(); }
} testDescription_AttrMiscTestSuite_test_read_attribute_exceptions;

static class TestDescription_AttrMiscTestSuite_test_SCALAR_attribute_of_type_different_than_READ : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_SCALAR_attribute_of_type_different_than_READ() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 817, "test_SCALAR_attribute_of_type_different_than_READ" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_SCALAR_attribute_of_type_different_than_READ(); }
} testDescription_AttrMiscTestSuite_test_SCALAR_attribute_of_type_different_than_READ;

static class TestDescription_AttrMiscTestSuite_test_read_attribute_on_write_type_attribute : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_read_attribute_on_write_type_attribute() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 879, "test_read_attribute_on_write_type_attribute" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_read_attribute_on_write_type_attribute(); }
} testDescription_AttrMiscTestSuite_test_read_attribute_on_write_type_attribute;

static class TestDescription_AttrMiscTestSuite_test_read_attribute_on_initialised_write_type_attribute : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_read_attribute_on_initialised_write_type_attribute() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 922, "test_read_attribute_on_initialised_write_type_attribute" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_read_attribute_on_initialised_write_type_attribute(); }
} testDescription_AttrMiscTestSuite_test_read_attribute_on_initialised_write_type_attribute;

static class TestDescription_AttrMiscTestSuite_test_alarm_on_attribute : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrMiscTestSuite_test_alarm_on_attribute() : CxxTest::RealTestDescription( Tests_AttrMiscTestSuite, suiteDescription_AttrMiscTestSuite, 970, "test_alarm_on_attribute" ) {}
 void runTest() { if ( suite_AttrMiscTestSuite ) suite_AttrMiscTestSuite->test_alarm_on_attribute(); }
} testDescription_AttrMiscTestSuite_test_alarm_on_attribute;

#include "../../../../new_tests/cxx_attr_write.cpp"

static AttrWriteTestSuite *suite_AttrWriteTestSuite = 0;

static CxxTest::List Tests_AttrWriteTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<AttrWriteTestSuite> suiteDescription_AttrWriteTestSuite( "../../../../new_tests/cxx_attr_write.cpp", 17, "AttrWriteTestSuite", Tests_AttrWriteTestSuite, suite_AttrWriteTestSuite, 92, 97 );

static class TestDescription_AttrWriteTestSuite_test_some_exception_cases : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrWriteTestSuite_test_some_exception_cases() : CxxTest::RealTestDescription( Tests_AttrWriteTestSuite, suiteDescription_AttrWriteTestSuite, 108, "test_some_exception_cases" ) {}
 void runTest() { if ( suite_AttrWriteTestSuite ) suite_AttrWriteTestSuite->test_some_exception_cases(); }
} testDescription_AttrWriteTestSuite_test_some_exception_cases;

static class TestDescription_AttrWriteTestSuite_test_write_some_scalar_attributes : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrWriteTestSuite_test_write_some_scalar_attributes() : CxxTest::RealTestDescription( Tests_AttrWriteTestSuite, suiteDescription_AttrWriteTestSuite, 143, "test_write_some_scalar_attributes" ) {}
 void runTest() { if ( suite_AttrWriteTestSuite ) suite_AttrWriteTestSuite->test_write_some_scalar_attributes(); }
} testDescription_AttrWriteTestSuite_test_write_some_scalar_attributes;

static class TestDescription_AttrWriteTestSuite_test_memorized_attributes : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrWriteTestSuite_test_memorized_attributes() : CxxTest::RealTestDescription( Tests_AttrWriteTestSuite, suiteDescription_AttrWriteTestSuite, 212, "test_memorized_attributes" ) {}
 void runTest() { if ( suite_AttrWriteTestSuite ) suite_AttrWriteTestSuite->test_memorized_attributes(); }
} testDescription_AttrWriteTestSuite_test_memorized_attributes;

#include "../../../../new_tests/cxx_attr_conf.cpp"

static AttrConfTestSuite *suite_AttrConfTestSuite = 0;

static CxxTest::List Tests_AttrConfTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<AttrConfTestSuite> suiteDescription_AttrConfTestSuite( "../../../../new_tests/cxx_attr_conf.cpp", 17, "AttrConfTestSuite", Tests_AttrConfTestSuite, suite_AttrConfTestSuite, 97, 102 );

static class TestDescription_AttrConfTestSuite_test_some_basic_exception_cases : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_some_basic_exception_cases() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 113, "test_some_basic_exception_cases" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_some_basic_exception_cases(); }
} testDescription_AttrConfTestSuite_test_some_basic_exception_cases;

static class TestDescription_AttrConfTestSuite_test_get_a_full_lib_default_conf : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_get_a_full_lib_default_conf() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 123, "test_get_a_full_lib_default_conf" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_get_a_full_lib_default_conf(); }
} testDescription_AttrConfTestSuite_test_get_a_full_lib_default_conf;

static class TestDescription_AttrConfTestSuite_test_get_a_full_user_default_conf : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_get_a_full_user_default_conf() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 148, "test_get_a_full_user_default_conf" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_get_a_full_user_default_conf(); }
} testDescription_AttrConfTestSuite_test_get_a_full_user_default_conf;

static class TestDescription_AttrConfTestSuite_test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 183, "test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level(); }
} testDescription_AttrConfTestSuite_test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level;

static class TestDescription_AttrConfTestSuite_test_several_attributes_in_one_call : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_several_attributes_in_one_call() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 246, "test_several_attributes_in_one_call" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_several_attributes_in_one_call(); }
} testDescription_AttrConfTestSuite_test_several_attributes_in_one_call;

static class TestDescription_AttrConfTestSuite_test_add_one_attribute_on_the_fly_and_get_its_config : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrConfTestSuite_test_add_one_attribute_on_the_fly_and_get_its_config() : CxxTest::RealTestDescription( Tests_AttrConfTestSuite, suiteDescription_AttrConfTestSuite, 327, "test_add_one_attribute_on_the_fly_and_get_its_config" ) {}
 void runTest() { if ( suite_AttrConfTestSuite ) suite_AttrConfTestSuite->test_add_one_attribute_on_the_fly_and_get_its_config(); }
} testDescription_AttrConfTestSuite_test_add_one_attribute_on_the_fly_and_get_its_config;

#include "../../../../new_tests/cxx_attr.cpp"

static AttrTestSuite *suite_AttrTestSuite = 0;

static CxxTest::List Tests_AttrTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<AttrTestSuite> suiteDescription_AttrTestSuite( "../../../../new_tests/cxx_attr.cpp", 17, "AttrTestSuite", Tests_AttrTestSuite, suite_AttrTestSuite, 105, 110 );

static class TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SCALAR_types : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SCALAR_types() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 121, "test_one_attribute_at_a_time_for_all_SCALAR_types" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_one_attribute_at_a_time_for_all_SCALAR_types(); }
} testDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SCALAR_types;

static class TestDescription_AttrTestSuite_test_several_SCALAR_attributes_in_one_call : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_several_SCALAR_attributes_in_one_call() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 164, "test_several_SCALAR_attributes_in_one_call" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_several_SCALAR_attributes_in_one_call(); }
} testDescription_AttrTestSuite_test_several_SCALAR_attributes_in_one_call;

static class TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SPECTRUM_types : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SPECTRUM_types() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 235, "test_one_attribute_at_a_time_for_all_SPECTRUM_types" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_one_attribute_at_a_time_for_all_SPECTRUM_types(); }
} testDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_SPECTRUM_types;

static class TestDescription_AttrTestSuite_test_several_SPECTRUM_attributes_in_one_call : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_several_SPECTRUM_attributes_in_one_call() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 280, "test_several_SPECTRUM_attributes_in_one_call" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_several_SPECTRUM_attributes_in_one_call(); }
} testDescription_AttrTestSuite_test_several_SPECTRUM_attributes_in_one_call;

static class TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_IMAGE_types : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_IMAGE_types() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 312, "test_one_attribute_at_a_time_for_all_IMAGE_types" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_one_attribute_at_a_time_for_all_IMAGE_types(); }
} testDescription_AttrTestSuite_test_one_attribute_at_a_time_for_all_IMAGE_types;

static class TestDescription_AttrTestSuite_test_one_call_with_all_three_types : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_one_call_with_all_three_types() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 355, "test_one_call_with_all_three_types" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_one_call_with_all_three_types(); }
} testDescription_AttrTestSuite_test_one_call_with_all_three_types;

static class TestDescription_AttrTestSuite_test_attributes_written_using_the_set_value_date_and_quality_method : public CxxTest::RealTestDescription {
public:
 TestDescription_AttrTestSuite_test_attributes_written_using_the_set_value_date_and_quality_method() : CxxTest::RealTestDescription( Tests_AttrTestSuite, suiteDescription_AttrTestSuite, 396, "test_attributes_written_using_the_set_value_date_and_quality_method" ) {}
 void runTest() { if ( suite_AttrTestSuite ) suite_AttrTestSuite->test_attributes_written_using_the_set_value_date_and_quality_method(); }
} testDescription_AttrTestSuite_test_attributes_written_using_the_set_value_date_and_quality_method;

#include "../../../../new_tests/cxx_misc_util.cpp"

static MiscUtilTestSuite *suite_MiscUtilTestSuite = 0;

static CxxTest::List Tests_MiscUtilTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<MiscUtilTestSuite> suiteDescription_MiscUtilTestSuite( "../../../../new_tests/cxx_misc_util.cpp", 17, "MiscUtilTestSuite", Tests_MiscUtilTestSuite, suite_MiscUtilTestSuite, 88, 93 );

static class TestDescription_MiscUtilTestSuite_test_get_device_list_by_class : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscUtilTestSuite_test_get_device_list_by_class() : CxxTest::RealTestDescription( Tests_MiscUtilTestSuite, suiteDescription_MiscUtilTestSuite, 104, "test_get_device_list_by_class" ) {}
 void runTest() { if ( suite_MiscUtilTestSuite ) suite_MiscUtilTestSuite->test_get_device_list_by_class(); }
} testDescription_MiscUtilTestSuite_test_get_device_list_by_class;

static class TestDescription_MiscUtilTestSuite_test_get_device_list_by_class_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscUtilTestSuite_test_get_device_list_by_class_exception() : CxxTest::RealTestDescription( Tests_MiscUtilTestSuite, suiteDescription_MiscUtilTestSuite, 125, "test_get_device_list_by_class_exception" ) {}
 void runTest() { if ( suite_MiscUtilTestSuite ) suite_MiscUtilTestSuite->test_get_device_list_by_class_exception(); }
} testDescription_MiscUtilTestSuite_test_get_device_list_by_class_exception;

static class TestDescription_MiscUtilTestSuite_test_get_dserver_device : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscUtilTestSuite_test_get_dserver_device() : CxxTest::RealTestDescription( Tests_MiscUtilTestSuite, suiteDescription_MiscUtilTestSuite, 138, "test_get_dserver_device" ) {}
 void runTest() { if ( suite_MiscUtilTestSuite ) suite_MiscUtilTestSuite->test_get_dserver_device(); }
} testDescription_MiscUtilTestSuite_test_get_dserver_device;

static class TestDescription_MiscUtilTestSuite_test_get_device_by_name : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscUtilTestSuite_test_get_device_by_name() : CxxTest::RealTestDescription( Tests_MiscUtilTestSuite, suiteDescription_MiscUtilTestSuite, 151, "test_get_device_by_name" ) {}
 void runTest() { if ( suite_MiscUtilTestSuite ) suite_MiscUtilTestSuite->test_get_device_by_name(); }
} testDescription_MiscUtilTestSuite_test_get_device_by_name;

static class TestDescription_MiscUtilTestSuite_test_get_device_by_name_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscUtilTestSuite_test_get_device_by_name_exception() : CxxTest::RealTestDescription( Tests_MiscUtilTestSuite, suiteDescription_MiscUtilTestSuite, 163, "test_get_device_by_name_exception" ) {}
 void runTest() { if ( suite_MiscUtilTestSuite ) suite_MiscUtilTestSuite->test_get_device_by_name_exception(); }
} testDescription_MiscUtilTestSuite_test_get_device_by_name_exception;

#include "../../../../new_tests/cxx_templ_cmd.cpp"

static TemplateCmdTestSuite *suite_TemplateCmdTestSuite = 0;

static CxxTest::List Tests_TemplateCmdTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<TemplateCmdTestSuite> suiteDescription_TemplateCmdTestSuite( "../../../../new_tests/cxx_templ_cmd.cpp", 17, "TemplateCmdTestSuite", Tests_TemplateCmdTestSuite, suite_TemplateCmdTestSuite, 106, 111 );

static class TestDescription_TemplateCmdTestSuite_test_IOTempl : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTempl() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 122, "test_IOTempl" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTempl(); }
} testDescription_TemplateCmdTestSuite_test_IOTempl;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplState : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplState() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 129, "test_IOTemplState" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplState(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplState;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplState_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplState_exception() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 136, "test_IOTemplState_exception" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplState_exception(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplState_exception;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplIn : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplIn() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 156, "test_IOTemplIn" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplIn(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplIn;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplInState : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplInState() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 166, "test_IOTemplInState" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplInState(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplInState;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplInState_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplInState_exception() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 176, "test_IOTemplInState_exception" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplInState_exception(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplInState_exception;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplOut : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplOut() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 198, "test_IOTemplOut" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplOut(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplOut;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplOutState : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplOutState() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 212, "test_IOTemplOutState" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplOutState(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplOutState;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplOutState_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplOutState_exception() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 226, "test_IOTemplOutState_exception" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplOutState_exception(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplOutState_exception;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplInOut : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplInOut() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 246, "test_IOTemplInOut" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplInOut(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplInOut;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplInOutState : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplInOutState() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 260, "test_IOTemplInOutState" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplInOutState(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplInOutState;

static class TestDescription_TemplateCmdTestSuite_test_IOTemplInOutState_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_TemplateCmdTestSuite_test_IOTemplInOutState_exception() : CxxTest::RealTestDescription( Tests_TemplateCmdTestSuite, suiteDescription_TemplateCmdTestSuite, 274, "test_IOTemplInOutState_exception" ) {}
 void runTest() { if ( suite_TemplateCmdTestSuite ) suite_TemplateCmdTestSuite->test_IOTemplInOutState_exception(); }
} testDescription_TemplateCmdTestSuite_test_IOTemplInOutState_exception;

#include "../../../../new_tests/cxx_exception.cpp"

static ExceptionTestSuite *suite_ExceptionTestSuite = 0;

static CxxTest::List Tests_ExceptionTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<ExceptionTestSuite> suiteDescription_ExceptionTestSuite( "../../../../new_tests/cxx_exception.cpp", 17, "ExceptionTestSuite", Tests_ExceptionTestSuite, suite_ExceptionTestSuite, 105, 110 );

static class TestDescription_ExceptionTestSuite_test_throw_exception_command : public CxxTest::RealTestDescription {
public:
 TestDescription_ExceptionTestSuite_test_throw_exception_command() : CxxTest::RealTestDescription( Tests_ExceptionTestSuite, suiteDescription_ExceptionTestSuite, 121, "test_throw_exception_command" ) {}
 void runTest() { if ( suite_ExceptionTestSuite ) suite_ExceptionTestSuite->test_throw_exception_command(); }
} testDescription_ExceptionTestSuite_test_throw_exception_command;

static class TestDescription_ExceptionTestSuite_test_throw_exception_with_several_levels : public CxxTest::RealTestDescription {
public:
 TestDescription_ExceptionTestSuite_test_throw_exception_with_several_levels() : CxxTest::RealTestDescription( Tests_ExceptionTestSuite, suiteDescription_ExceptionTestSuite, 137, "test_throw_exception_with_several_levels" ) {}
 void runTest() { if ( suite_ExceptionTestSuite ) suite_ExceptionTestSuite->test_throw_exception_with_several_levels(); }
} testDescription_ExceptionTestSuite_test_throw_exception_with_several_levels;

static class TestDescription_ExceptionTestSuite_test_command_not_found_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_ExceptionTestSuite_test_command_not_found_exception() : CxxTest::RealTestDescription( Tests_ExceptionTestSuite, suiteDescription_ExceptionTestSuite, 161, "test_command_not_found_exception" ) {}
 void runTest() { if ( suite_ExceptionTestSuite ) suite_ExceptionTestSuite->test_command_not_found_exception(); }
} testDescription_ExceptionTestSuite_test_command_not_found_exception;

static class TestDescription_ExceptionTestSuite_test_command_not_allowed_in_this_state_exception : public CxxTest::RealTestDescription {
public:
 TestDescription_ExceptionTestSuite_test_command_not_allowed_in_this_state_exception() : CxxTest::RealTestDescription( Tests_ExceptionTestSuite, suiteDescription_ExceptionTestSuite, 170, "test_command_not_allowed_in_this_state_exception" ) {}
 void runTest() { if ( suite_ExceptionTestSuite ) suite_ExceptionTestSuite->test_command_not_allowed_in_this_state_exception(); }
} testDescription_ExceptionTestSuite_test_command_not_allowed_in_this_state_exception;

#include "../../../../new_tests/cxx_cmd_query.cpp"

static CmdQueryTestSuite *suite_CmdQueryTestSuite = 0;

static CxxTest::List Tests_CmdQueryTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<CmdQueryTestSuite> suiteDescription_CmdQueryTestSuite( "../../../../new_tests/cxx_cmd_query.cpp", 17, "CmdQueryTestSuite", Tests_CmdQueryTestSuite, suite_CmdQueryTestSuite, 93, 98 );

static class TestDescription_CmdQueryTestSuite_test_command_list_query : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 116, "test_command_list_query" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query(); }
} testDescription_CmdQueryTestSuite_test_command_list_query;

static class TestDescription_CmdQueryTestSuite_test_command_Status : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_Status() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 124, "test_command_Status" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_Status(); }
} testDescription_CmdQueryTestSuite_test_command_Status;

static class TestDescription_CmdQueryTestSuite_test_fake_command : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_fake_command() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 137, "test_fake_command" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_fake_command(); }
} testDescription_CmdQueryTestSuite_test_fake_command;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_AddLoggingTarget : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_AddLoggingTarget() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 146, "test_command_list_query_AddLoggingTarget" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_AddLoggingTarget(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_AddLoggingTarget;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_AddObjPolling : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_AddObjPolling() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 159, "test_command_list_query_AddObjPolling" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_AddObjPolling(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_AddObjPolling;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_DevLockStatus : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_DevLockStatus() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 172, "test_command_list_query_DevLockStatus" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_DevLockStatus(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_DevLockStatus;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_DevPollStatus : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_DevPollStatus() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 185, "test_command_list_query_DevPollStatus" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_DevPollStatus(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_DevPollStatus;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_DevRestart : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_DevRestart() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 198, "test_command_list_query_DevRestart" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_DevRestart(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_DevRestart;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_EventSubscriptionChange : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_EventSubscriptionChange() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 211, "test_command_list_query_EventSubscriptionChange" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_EventSubscriptionChange(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_EventSubscriptionChange;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingLevel : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingLevel() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 224, "test_command_list_query_GetLoggingLevel" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_GetLoggingLevel(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingLevel;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingTarget : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingTarget() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 237, "test_command_list_query_GetLoggingTarget" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_GetLoggingTarget(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_GetLoggingTarget;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_Init : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_Init() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 250, "test_command_list_query_Init" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_Init(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_Init;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_Kill : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_Kill() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 263, "test_command_list_query_Kill" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_Kill(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_Kill;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_LockDevice : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_LockDevice() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 276, "test_command_list_query_LockDevice" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_LockDevice(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_LockDevice;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_PolledDevice : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_PolledDevice() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 289, "test_command_list_query_PolledDevice" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_PolledDevice(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_PolledDevice;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_QueryClass : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_QueryClass() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 302, "test_command_list_query_QueryClass" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_QueryClass(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_QueryClass;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_QueryDevice : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_QueryDevice() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 315, "test_command_list_query_QueryDevice" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_QueryDevice(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_QueryDevice;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_QuerySubDevice : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_QuerySubDevice() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 328, "test_command_list_query_QuerySubDevice" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_QuerySubDevice(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_QuerySubDevice;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardClassProperty : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardClassProperty() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 341, "test_command_list_query_QueryWizardClassProperty" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_QueryWizardClassProperty(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardClassProperty;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardDevProperty : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardDevProperty() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 354, "test_command_list_query_QueryWizardDevProperty" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_QueryWizardDevProperty(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_QueryWizardDevProperty;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_ReLockDevices : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_ReLockDevices() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 367, "test_command_list_query_ReLockDevices" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_ReLockDevices(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_ReLockDevices;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_RemObjPolling : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_RemObjPolling() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 380, "test_command_list_query_RemObjPolling" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_RemObjPolling(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_RemObjPolling;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_RemoveLoggingTarget : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_RemoveLoggingTarget() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 393, "test_command_list_query_RemoveLoggingTarget" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_RemoveLoggingTarget(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_RemoveLoggingTarget;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_RestartServer : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_RestartServer() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 406, "test_command_list_query_RestartServer" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_RestartServer(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_RestartServer;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_SetLoggingLevel : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_SetLoggingLevel() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 419, "test_command_list_query_SetLoggingLevel" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_SetLoggingLevel(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_SetLoggingLevel;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_StartLogging : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_StartLogging() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 432, "test_command_list_query_StartLogging" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_StartLogging(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_StartLogging;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_StartPolling : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_StartPolling() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 445, "test_command_list_query_StartPolling" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_StartPolling(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_StartPolling;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_State : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_State() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 458, "test_command_list_query_State" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_State(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_State;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_Status : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_Status() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 471, "test_command_list_query_Status" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_Status(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_Status;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_StopLogging : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_StopLogging() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 484, "test_command_list_query_StopLogging" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_StopLogging(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_StopLogging;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_StopPolling : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_StopPolling() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 497, "test_command_list_query_StopPolling" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_StopPolling(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_StopPolling;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_UnLockDevice : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_UnLockDevice() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 510, "test_command_list_query_UnLockDevice" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_UnLockDevice(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_UnLockDevice;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_list_query_UpdObjPollingPeriod : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_list_query_UpdObjPollingPeriod() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 523, "test_command_list_query_list_query_UpdObjPollingPeriod" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_list_query_UpdObjPollingPeriod(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_list_query_UpdObjPollingPeriod;

static class TestDescription_CmdQueryTestSuite_test_command_list_query_ZMQEventSubscriptionChange : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdQueryTestSuite_test_command_list_query_ZMQEventSubscriptionChange() : CxxTest::RealTestDescription( Tests_CmdQueryTestSuite, suiteDescription_CmdQueryTestSuite, 536, "test_command_list_query_ZMQEventSubscriptionChange" ) {}
 void runTest() { if ( suite_CmdQueryTestSuite ) suite_CmdQueryTestSuite->test_command_list_query_ZMQEventSubscriptionChange(); }
} testDescription_CmdQueryTestSuite_test_command_list_query_ZMQEventSubscriptionChange;

#include "../../../../new_tests/cxx_seq_vec.cpp"

static SeqVecTestSuite *suite_SeqVecTestSuite = 0;

static CxxTest::List Tests_SeqVecTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<SeqVecTestSuite> suiteDescription_SeqVecTestSuite( "../../../../new_tests/cxx_seq_vec.cpp", 17, "SeqVecTestSuite", Tests_SeqVecTestSuite, suite_SeqVecTestSuite, 87, 92 );

static class TestDescription_SeqVecTestSuite_test_IOSeqVecChar : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecChar() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 103, "test_IOSeqVecChar" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecChar(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecChar;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecShort : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecShort() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 125, "test_IOSeqVecShort" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecShort(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecShort;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecLong : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecLong() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 148, "test_IOSeqVecLong" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecLong(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecLong;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecFloat : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecFloat() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 168, "test_IOSeqVecFloat" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecFloat(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecFloat;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecDouble : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecDouble() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 190, "test_IOSeqVecDouble" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecDouble(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecDouble;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecUShort : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecUShort() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 211, "test_IOSeqVecUShort" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecUShort(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecUShort;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecULong : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecULong() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 236, "test_IOSeqVecULong" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecULong(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecULong;

static class TestDescription_SeqVecTestSuite_test_IOSeqVecString : public CxxTest::RealTestDescription {
public:
 TestDescription_SeqVecTestSuite_test_IOSeqVecString() : CxxTest::RealTestDescription( Tests_SeqVecTestSuite, suiteDescription_SeqVecTestSuite, 257, "test_IOSeqVecString" ) {}
 void runTest() { if ( suite_SeqVecTestSuite ) suite_SeqVecTestSuite->test_IOSeqVecString(); }
} testDescription_SeqVecTestSuite_test_IOSeqVecString;

#include "../../../../new_tests/cxx_test_state_on.cpp"

static TestStateOnTestSuite *suite_TestStateOnTestSuite = 0;

static CxxTest::List Tests_TestStateOnTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<TestStateOnTestSuite> suiteDescription_TestStateOnTestSuite( "../../../../new_tests/cxx_test_state_on.cpp", 17, "TestStateOnTestSuite", Tests_TestStateOnTestSuite, suite_TestStateOnTestSuite, 87, 92 );

static class TestDescription_TestStateOnTestSuite_test_test_state_on : public CxxTest::RealTestDescription {
public:
 TestDescription_TestStateOnTestSuite_test_test_state_on() : CxxTest::RealTestDescription( Tests_TestStateOnTestSuite, suiteDescription_TestStateOnTestSuite, 103, "test_test_state_on" ) {}
 void runTest() { if ( suite_TestStateOnTestSuite ) suite_TestStateOnTestSuite->test_test_state_on(); }
} testDescription_TestStateOnTestSuite_test_test_state_on;

#include "../../../../new_tests/cxx_misc.cpp"

static MiscTestSuite *suite_MiscTestSuite = 0;

static CxxTest::List Tests_MiscTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<MiscTestSuite> suiteDescription_MiscTestSuite( "../../../../new_tests/cxx_misc.cpp", 17, "MiscTestSuite", Tests_MiscTestSuite, suite_MiscTestSuite, 125, 130 );

static class TestDescription_MiscTestSuite_test_DevState_and_DevStatus_commands : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscTestSuite_test_DevState_and_DevStatus_commands() : CxxTest::RealTestDescription( Tests_MiscTestSuite, suiteDescription_MiscTestSuite, 141, "test_DevState_and_DevStatus_commands" ) {}
 void runTest() { if ( suite_MiscTestSuite ) suite_MiscTestSuite->test_DevState_and_DevStatus_commands(); }
} testDescription_MiscTestSuite_test_DevState_and_DevStatus_commands;

static class TestDescription_MiscTestSuite_test_DevRestart_command : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscTestSuite_test_DevRestart_command() : CxxTest::RealTestDescription( Tests_MiscTestSuite, suiteDescription_MiscTestSuite, 157, "test_DevRestart_command" ) {}
 void runTest() { if ( suite_MiscTestSuite ) suite_MiscTestSuite->test_DevRestart_command(); }
} testDescription_MiscTestSuite_test_DevRestart_command;

static class TestDescription_MiscTestSuite_test_name_description_state_and_status_read_as_CORBA_attributes : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscTestSuite_test_name_description_state_and_status_read_as_CORBA_attributes() : CxxTest::RealTestDescription( Tests_MiscTestSuite, suiteDescription_MiscTestSuite, 177, "test_name_description_state_and_status_read_as_CORBA_attributes" ) {}
 void runTest() { if ( suite_MiscTestSuite ) suite_MiscTestSuite->test_name_description_state_and_status_read_as_CORBA_attributes(); }
} testDescription_MiscTestSuite_test_name_description_state_and_status_read_as_CORBA_attributes;

static class TestDescription_MiscTestSuite_test_ping_the_device : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscTestSuite_test_ping_the_device() : CxxTest::RealTestDescription( Tests_MiscTestSuite, suiteDescription_MiscTestSuite, 197, "test_ping_the_device" ) {}
 void runTest() { if ( suite_MiscTestSuite ) suite_MiscTestSuite->test_ping_the_device(); }
} testDescription_MiscTestSuite_test_ping_the_device;

static class TestDescription_MiscTestSuite_test_info_call : public CxxTest::RealTestDescription {
public:
 TestDescription_MiscTestSuite_test_info_call() : CxxTest::RealTestDescription( Tests_MiscTestSuite, suiteDescription_MiscTestSuite, 204, "test_info_call" ) {}
 void runTest() { if ( suite_MiscTestSuite ) suite_MiscTestSuite->test_info_call(); }
} testDescription_MiscTestSuite_test_info_call;

#include "../../../../new_tests/cxx_cmd_types.cpp"

static CmdTypesTestSuite *suite_CmdTypesTestSuite = 0;

static CxxTest::List Tests_CmdTypesTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<CmdTypesTestSuite> suiteDescription_CmdTypesTestSuite( "../../../../new_tests/cxx_cmd_types.cpp", 17, "CmdTypesTestSuite", Tests_CmdTypesTestSuite, suite_CmdTypesTestSuite, 92, 97 );

static class TestDescription_CmdTypesTestSuite_test_Scalar_Short : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Short() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 119, "test_Scalar_Short" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Short(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Short;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Short_DevEncoded_2__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Short_DevEncoded_2__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 158, "test_Scalar_Short_DevEncoded_2__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Short_DevEncoded_2__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Short_DevEncoded_2__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Long__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Long__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 188, "test_Scalar_Long__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Long__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Long__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Double__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Double__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 217, "test_Scalar_Double__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Double__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Double__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_String__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_String__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 236, "test_Scalar_String__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_String__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_String__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Float__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Float__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 255, "test_Scalar_Float__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Float__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Float__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Boolean__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Boolean__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 274, "test_Scalar_Boolean__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Boolean__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Boolean__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Short__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Short__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 293, "test_Scalar_Unsigned_Short__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Unsigned_Short__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Short__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Char__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Char__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 312, "test_Scalar_Unsigned_Char__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Unsigned_Char__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Char__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Long64__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Long64__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 331, "test_Scalar_Long64__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Long64__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Long64__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 357, "test_Scalar_Unsigned_Long__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Unsigned_Long__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long64__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long64__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 378, "test_Scalar_Unsigned_Long64__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_Unsigned_Long64__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_Unsigned_Long64__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_State__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_State__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 399, "test_Scalar_State__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_State__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_State__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_DevEncoded__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_DevEncoded__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 429, "test_Scalar_DevEncoded__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_DevEncoded__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_DevEncoded__loop;

static class TestDescription_CmdTypesTestSuite_test_Scalar_DevEncoded_JPEG__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Scalar_DevEncoded_JPEG__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 460, "test_Scalar_DevEncoded_JPEG__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Scalar_DevEncoded_JPEG__loop(); }
} testDescription_CmdTypesTestSuite_test_Scalar_DevEncoded_JPEG__loop;

static class TestDescription_CmdTypesTestSuite_test_Thirteen_in_one_call__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Thirteen_in_one_call__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 490, "test_Thirteen_in_one_call__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Thirteen_in_one_call__loop(); }
} testDescription_CmdTypesTestSuite_test_Thirteen_in_one_call__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Short__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Short__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 576, "test_Spectrum_Short__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Short__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Short__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Long__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Long__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 604, "test_Spectrum_Long__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Long__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Long__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Double__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Double__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 633, "test_Spectrum_Double__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Double__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Double__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_String__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_String__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 657, "test_Spectrum_String__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_String__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_String__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Float__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Float__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 679, "test_Spectrum_Float__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Float__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Float__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Boolean__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Boolean__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 702, "test_Spectrum_Boolean__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Boolean__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Boolean__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 731, "test_Spectrum_Unsigned_Short__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Short__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 753, "test_Spectrum_Unsigned_Char__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Char__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Long64__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Long64__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 779, "test_Spectrum_Long64__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Long64__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Long64__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 803, "test_Spectrum_Unsigned_Long__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Long__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 827, "test_Spectrum_Unsigned_Long64__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Long64__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_State__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_State__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 851, "test_Spectrum_State__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_State__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_State__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Short_DevVarShortArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Short_DevVarShortArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 879, "test_Spectrum_Short_DevVarShortArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Short_DevVarShortArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Short_DevVarShortArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Long_DevVarLongArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Long_DevVarLongArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 903, "test_Spectrum_Long_DevVarLongArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Long_DevVarLongArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Long_DevVarLongArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Double_DevVarDoubleArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Double_DevVarDoubleArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 927, "test_Spectrum_Double_DevVarDoubleArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Double_DevVarDoubleArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Double_DevVarDoubleArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_String_DevVarStringArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_String_DevVarStringArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 949, "test_Spectrum_String_DevVarStringArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_String_DevVarStringArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_String_DevVarStringArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Float_DevVarFloatArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Float_DevVarFloatArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 971, "test_Spectrum_Float_DevVarFloatArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Float_DevVarFloatArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Float_DevVarFloatArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Boolean_DevVarBooleanArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Boolean_DevVarBooleanArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 994, "test_Spectrum_Boolean_DevVarBooleanArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Boolean_DevVarBooleanArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Boolean_DevVarBooleanArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short_DevVarUShortArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short_DevVarUShortArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1022, "test_Spectrum_Unsigned_Short_DevVarUShortArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Short_DevVarUShortArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Short_DevVarUShortArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char_DevVarUCharArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char_DevVarUCharArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1044, "test_Spectrum_Unsigned_Char_DevVarUCharArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Char_DevVarUCharArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Char_DevVarUCharArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Long64_DevVarLong64Array__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Long64_DevVarLong64Array__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1070, "test_Spectrum_Long64_DevVarLong64Array__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Long64_DevVarLong64Array__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Long64_DevVarLong64Array__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long_DevVarULongArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long_DevVarULongArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1096, "test_Spectrum_Unsigned_Long_DevVarULongArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Long_DevVarULongArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long_DevVarULongArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1122, "test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop;

static class TestDescription_CmdTypesTestSuite_test_Spectrum_State_DevVarStateArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Spectrum_State_DevVarStateArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1148, "test_Spectrum_State_DevVarStateArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Spectrum_State_DevVarStateArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Spectrum_State_DevVarStateArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Image_Short_DevVarShortArray__loop : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Image_Short_DevVarShortArray__loop() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1176, "test_Image_Short_DevVarShortArray__loop" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Image_Short_DevVarShortArray__loop(); }
} testDescription_CmdTypesTestSuite_test_Image_Short_DevVarShortArray__loop;

static class TestDescription_CmdTypesTestSuite_test_Exception_Error_for_unknown_attribute_data_format : public CxxTest::RealTestDescription {
public:
 TestDescription_CmdTypesTestSuite_test_Exception_Error_for_unknown_attribute_data_format() : CxxTest::RealTestDescription( Tests_CmdTypesTestSuite, suiteDescription_CmdTypesTestSuite, 1202, "test_Exception_Error_for_unknown_attribute_data_format" ) {}
 void runTest() { if ( suite_CmdTypesTestSuite ) suite_CmdTypesTestSuite->test_Exception_Error_for_unknown_attribute_data_format(); }
} testDescription_CmdTypesTestSuite_test_Exception_Error_for_unknown_attribute_data_format;

#include "../../../../new_tests/cxx_group.cpp"

static GroupTestSuite *suite_GroupTestSuite = 0;

static CxxTest::List Tests_GroupTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<GroupTestSuite> suiteDescription_GroupTestSuite( "../../../../new_tests/cxx_group.cpp", 18, "GroupTestSuite", Tests_GroupTestSuite, suite_GroupTestSuite, 87, 92 );

static class TestDescription_GroupTestSuite_test_miscellaneous : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_miscellaneous() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 103, "test_miscellaneous" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_miscellaneous(); }
} testDescription_GroupTestSuite_test_miscellaneous;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_no_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_no_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 183, "test_synchronous_command_with_forwarding_and_no_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_forwarding_and_no_arguments(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_no_arguments;

static class TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_no_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_no_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 209, "test_asynchronous_command_with_forwarding_and_no_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_asynchronous_command_with_forwarding_and_no_arguments(); }
} testDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_no_arguments;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_no_forwarding_and_no_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_no_forwarding_and_no_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 225, "test_synchronous_command_with_no_forwarding_and_no_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_no_forwarding_and_no_arguments(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_no_forwarding_and_no_arguments;

static class TestDescription_GroupTestSuite_test_asynchronous_command_with_no_forwarding_and_no_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_asynchronous_command_with_no_forwarding_and_no_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 240, "test_asynchronous_command_with_no_forwarding_and_no_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_asynchronous_command_with_no_forwarding_and_no_arguments(); }
} testDescription_GroupTestSuite_test_asynchronous_command_with_no_forwarding_and_no_arguments;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_one_argument : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_one_argument() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 256, "test_synchronous_command_with_forwarding_and_one_argument" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_forwarding_and_one_argument(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_one_argument;

static class TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_one_argument : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_one_argument() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 273, "test_asynchronous_command_with_forwarding_and_one_argument" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_asynchronous_command_with_forwarding_and_one_argument(); }
} testDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_one_argument;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 291, "test_synchronous_command_with_forwarding_and_several_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_forwarding_and_several_arguments(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_arguments;

static class TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 312, "test_asynchronous_command_with_forwarding_and_several_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_asynchronous_command_with_forwarding_and_several_arguments(); }
} testDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_arguments;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_DeviceData_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_DeviceData_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 334, "test_synchronous_command_with_forwarding_and_several_DeviceData_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_forwarding_and_several_DeviceData_arguments(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_several_DeviceData_arguments;

static class TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 359, "test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments(); }
} testDescription_GroupTestSuite_test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments;

static class TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_wrong_number_of_arguments : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_wrong_number_of_arguments() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 393, "test_synchronous_command_with_forwarding_and_wrong_number_of_arguments" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_with_forwarding_and_wrong_number_of_arguments(); }
} testDescription_GroupTestSuite_test_synchronous_command_with_forwarding_and_wrong_number_of_arguments;

static class TestDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_on : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_on() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 406, "test_synchronous_command_throwing_exception_mode_on" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_throwing_exception_mode_on(); }
} testDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_on;

static class TestDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_off : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_off() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 427, "test_synchronous_command_throwing_exception_mode_off" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_synchronous_command_throwing_exception_mode_off(); }
} testDescription_GroupTestSuite_test_synchronous_command_throwing_exception_mode_off;

static class TestDescription_GroupTestSuite_test_read_attribute_synchronously : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_read_attribute_synchronously() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 444, "test_read_attribute_synchronously" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_read_attribute_synchronously(); }
} testDescription_GroupTestSuite_test_read_attribute_synchronously;

static class TestDescription_GroupTestSuite_test_read_attribute_asynchronously : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_read_attribute_asynchronously() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 470, "test_read_attribute_asynchronously" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_read_attribute_asynchronously(); }
} testDescription_GroupTestSuite_test_read_attribute_asynchronously;

static class TestDescription_GroupTestSuite_test_read_several_attributes_synchronously : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_read_several_attributes_synchronously() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 486, "test_read_several_attributes_synchronously" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_read_several_attributes_synchronously(); }
} testDescription_GroupTestSuite_test_read_several_attributes_synchronously;

static class TestDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_on : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_on() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 529, "test_read_attribute_synchronously_throwing_exception_mode_on" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_read_attribute_synchronously_throwing_exception_mode_on(); }
} testDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_on;

static class TestDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_off : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_off() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 550, "test_read_attribute_synchronously_throwing_exception_mode_off" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_read_attribute_synchronously_throwing_exception_mode_off(); }
} testDescription_GroupTestSuite_test_read_attribute_synchronously_throwing_exception_mode_off;

static class TestDescription_GroupTestSuite_test_write_attribute_synchronously_one_value : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_synchronously_one_value() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 567, "test_write_attribute_synchronously_one_value" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_synchronously_one_value(); }
} testDescription_GroupTestSuite_test_write_attribute_synchronously_one_value;

static class TestDescription_GroupTestSuite_test_write_attribute_asynchronously_one_value : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_asynchronously_one_value() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 608, "test_write_attribute_asynchronously_one_value" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_asynchronously_one_value(); }
} testDescription_GroupTestSuite_test_write_attribute_asynchronously_one_value;

static class TestDescription_GroupTestSuite_test_write_attribute_synchronously_several_values : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_synchronously_several_values() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 650, "test_write_attribute_synchronously_several_values" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_synchronously_several_values(); }
} testDescription_GroupTestSuite_test_write_attribute_synchronously_several_values;

static class TestDescription_GroupTestSuite_test_write_attribute_asynchronously_several_values : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_asynchronously_several_values() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 695, "test_write_attribute_asynchronously_several_values" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_asynchronously_several_values(); }
} testDescription_GroupTestSuite_test_write_attribute_asynchronously_several_values;

static class TestDescription_GroupTestSuite_test_write_attribute_synchronously_several_DeviceAttribute_values : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_synchronously_several_DeviceAttribute_values() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 741, "test_write_attribute_synchronously_several_DeviceAttribute_values" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_synchronously_several_DeviceAttribute_values(); }
} testDescription_GroupTestSuite_test_write_attribute_synchronously_several_DeviceAttribute_values;

static class TestDescription_GroupTestSuite_test_write_attribute_asynchronously_several_DeviceAttribute_values : public CxxTest::RealTestDescription {
public:
 TestDescription_GroupTestSuite_test_write_attribute_asynchronously_several_DeviceAttribute_values() : CxxTest::RealTestDescription( Tests_GroupTestSuite, suiteDescription_GroupTestSuite, 787, "test_write_attribute_asynchronously_several_DeviceAttribute_values" ) {}
 void runTest() { if ( suite_GroupTestSuite ) suite_GroupTestSuite->test_write_attribute_asynchronously_several_DeviceAttribute_values(); }
} testDescription_GroupTestSuite_test_write_attribute_asynchronously_several_DeviceAttribute_values;

#include "../../../../new_tests/cxx_encoded.cpp"

static EncodedTestSuite *suite_EncodedTestSuite = 0;

static CxxTest::List Tests_EncodedTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<EncodedTestSuite> suiteDescription_EncodedTestSuite( "../../../../new_tests/cxx_encoded.cpp", 18, "EncodedTestSuite", Tests_EncodedTestSuite, suite_EncodedTestSuite, 62, 67 );

static class TestDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceData : public CxxTest::RealTestDescription {
public:
 TestDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceData() : CxxTest::RealTestDescription( Tests_EncodedTestSuite, suiteDescription_EncodedTestSuite, 79, "test_Miscellaneous_inserters_and_extracters_for_DeviceData" ) {}
 void runTest() { if ( suite_EncodedTestSuite ) suite_EncodedTestSuite->test_Miscellaneous_inserters_and_extracters_for_DeviceData(); }
} testDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceData;

static class TestDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute : public CxxTest::RealTestDescription {
public:
 TestDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute() : CxxTest::RealTestDescription( Tests_EncodedTestSuite, suiteDescription_EncodedTestSuite, 160, "test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute" ) {}
 void runTest() { if ( suite_EncodedTestSuite ) suite_EncodedTestSuite->test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute(); }
} testDescription_EncodedTestSuite_test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute;

#include "../../../../new_tests/cxx_database.cpp"

static DatabaseTestSuite *suite_DatabaseTestSuite = 0;

static CxxTest::List Tests_DatabaseTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<DatabaseTestSuite> suiteDescription_DatabaseTestSuite( "../../../../new_tests/cxx_database.cpp", 18, "DatabaseTestSuite", Tests_DatabaseTestSuite, suite_DatabaseTestSuite, 65, 70 );

static class TestDescription_DatabaseTestSuite_test_get_device_info : public CxxTest::RealTestDescription {
public:
 TestDescription_DatabaseTestSuite_test_get_device_info() : CxxTest::RealTestDescription( Tests_DatabaseTestSuite, suiteDescription_DatabaseTestSuite, 82, "test_get_device_info" ) {}
 void runTest() { if ( suite_DatabaseTestSuite ) suite_DatabaseTestSuite->test_get_device_info(); }
} testDescription_DatabaseTestSuite_test_get_device_info;

#include <cxxtest/Root.cpp>
