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
#include "../../../../new_tests/cxx_zmcast01_simple.cpp"

static McastSimpleTestSuite *suite_McastSimpleTestSuite = 0;

static CxxTest::List Tests_McastSimpleTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<McastSimpleTestSuite> suiteDescription_McastSimpleTestSuite( "../../../../new_tests/cxx_zmcast01_simple.cpp", 26, "McastSimpleTestSuite", Tests_McastSimpleTestSuite, suite_McastSimpleTestSuite, 106, 111 );

static class TestDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_locally : public CxxTest::RealTestDescription {
public:
 TestDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_locally() : CxxTest::RealTestDescription( Tests_McastSimpleTestSuite, suiteDescription_McastSimpleTestSuite, 122, "test_Subscribe_multicast_event_locally" ) {}
 void runTest() { if ( suite_McastSimpleTestSuite ) suite_McastSimpleTestSuite->test_Subscribe_multicast_event_locally(); }
} testDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_locally;

static class TestDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_remotely : public CxxTest::RealTestDescription {
public:
 TestDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_remotely() : CxxTest::RealTestDescription( Tests_McastSimpleTestSuite, suiteDescription_McastSimpleTestSuite, 141, "test_Subscribe_multicast_event_remotely" ) {}
 void runTest() { if ( suite_McastSimpleTestSuite ) suite_McastSimpleTestSuite->test_Subscribe_multicast_event_remotely(); }
} testDescription_McastSimpleTestSuite_test_Subscribe_multicast_event_remotely;

static class TestDescription_McastSimpleTestSuite_test_first_point_received_locally_and_remotely : public CxxTest::RealTestDescription {
public:
 TestDescription_McastSimpleTestSuite_test_first_point_received_locally_and_remotely() : CxxTest::RealTestDescription( Tests_McastSimpleTestSuite, suiteDescription_McastSimpleTestSuite, 162, "test_first_point_received_locally_and_remotely" ) {}
 void runTest() { if ( suite_McastSimpleTestSuite ) suite_McastSimpleTestSuite->test_first_point_received_locally_and_remotely(); }
} testDescription_McastSimpleTestSuite_test_first_point_received_locally_and_remotely;

static class TestDescription_McastSimpleTestSuite_test_Callback_executed_after_a_change_localy_and_remotely : public CxxTest::RealTestDescription {
public:
 TestDescription_McastSimpleTestSuite_test_Callback_executed_after_a_change_localy_and_remotely() : CxxTest::RealTestDescription( Tests_McastSimpleTestSuite, suiteDescription_McastSimpleTestSuite, 173, "test_Callback_executed_after_a_change_localy_and_remotely" ) {}
 void runTest() { if ( suite_McastSimpleTestSuite ) suite_McastSimpleTestSuite->test_Callback_executed_after_a_change_localy_and_remotely(); }
} testDescription_McastSimpleTestSuite_test_Callback_executed_after_a_change_localy_and_remotely;

static class TestDescription_McastSimpleTestSuite_test_unsubscribe_event_localy_and_remotely : public CxxTest::RealTestDescription {
public:
 TestDescription_McastSimpleTestSuite_test_unsubscribe_event_localy_and_remotely() : CxxTest::RealTestDescription( Tests_McastSimpleTestSuite, suiteDescription_McastSimpleTestSuite, 208, "test_unsubscribe_event_localy_and_remotely" ) {}
 void runTest() { if ( suite_McastSimpleTestSuite ) suite_McastSimpleTestSuite->test_unsubscribe_event_localy_and_remotely(); }
} testDescription_McastSimpleTestSuite_test_unsubscribe_event_localy_and_remotely;

#include "../../../../new_tests/cxx_zmcast02_local_remote.cpp"

static McastLocalRemoteTestSuite *suite_McastLocalRemoteTestSuite = 0;

static CxxTest::List Tests_McastLocalRemoteTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<McastLocalRemoteTestSuite> suiteDescription_McastLocalRemoteTestSuite( "../../../../new_tests/cxx_zmcast02_local_remote.cpp", 26, "McastLocalRemoteTestSuite", Tests_McastLocalRemoteTestSuite, suite_McastLocalRemoteTestSuite, 163, 168 );

static class TestDescription_McastLocalRemoteTestSuite_test_Subscribe_multicast_events : public CxxTest::RealTestDescription {
public:
 TestDescription_McastLocalRemoteTestSuite_test_Subscribe_multicast_events() : CxxTest::RealTestDescription( Tests_McastLocalRemoteTestSuite, suiteDescription_McastLocalRemoteTestSuite, 179, "test_Subscribe_multicast_events" ) {}
 void runTest() { if ( suite_McastLocalRemoteTestSuite ) suite_McastLocalRemoteTestSuite->test_Subscribe_multicast_events(); }
} testDescription_McastLocalRemoteTestSuite_test_Subscribe_multicast_events;

static class TestDescription_McastLocalRemoteTestSuite_test_first_point_received : public CxxTest::RealTestDescription {
public:
 TestDescription_McastLocalRemoteTestSuite_test_first_point_received() : CxxTest::RealTestDescription( Tests_McastLocalRemoteTestSuite, suiteDescription_McastLocalRemoteTestSuite, 211, "test_first_point_received" ) {}
 void runTest() { if ( suite_McastLocalRemoteTestSuite ) suite_McastLocalRemoteTestSuite->test_first_point_received(); }
} testDescription_McastLocalRemoteTestSuite_test_first_point_received;

static class TestDescription_McastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events : public CxxTest::RealTestDescription {
public:
 TestDescription_McastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events() : CxxTest::RealTestDescription( Tests_McastLocalRemoteTestSuite, suiteDescription_McastLocalRemoteTestSuite, 217, "test_Callback_executed_after_a_change_for_both_events" ) {}
 void runTest() { if ( suite_McastLocalRemoteTestSuite ) suite_McastLocalRemoteTestSuite->test_Callback_executed_after_a_change_for_both_events(); }
} testDescription_McastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events;

static class TestDescription_McastLocalRemoteTestSuite_test_unsubscribe_event : public CxxTest::RealTestDescription {
public:
 TestDescription_McastLocalRemoteTestSuite_test_unsubscribe_event() : CxxTest::RealTestDescription( Tests_McastLocalRemoteTestSuite, suiteDescription_McastLocalRemoteTestSuite, 246, "test_unsubscribe_event" ) {}
 void runTest() { if ( suite_McastLocalRemoteTestSuite ) suite_McastLocalRemoteTestSuite->test_unsubscribe_event(); }
} testDescription_McastLocalRemoteTestSuite_test_unsubscribe_event;

#include "../../../../new_tests/cxx_zmcast03_svr_local_remote.cpp"

static SvrMcastLocalRemoteTestSuite *suite_SvrMcastLocalRemoteTestSuite = 0;

static CxxTest::List Tests_SvrMcastLocalRemoteTestSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<SvrMcastLocalRemoteTestSuite> suiteDescription_SvrMcastLocalRemoteTestSuite( "../../../../new_tests/cxx_zmcast03_svr_local_remote.cpp", 28, "SvrMcastLocalRemoteTestSuite", Tests_SvrMcastLocalRemoteTestSuite, suite_SvrMcastLocalRemoteTestSuite, 154, 159 );

static class TestDescription_SvrMcastLocalRemoteTestSuite_test_Subscribe_multicast_events : public CxxTest::RealTestDescription {
public:
 TestDescription_SvrMcastLocalRemoteTestSuite_test_Subscribe_multicast_events() : CxxTest::RealTestDescription( Tests_SvrMcastLocalRemoteTestSuite, suiteDescription_SvrMcastLocalRemoteTestSuite, 170, "test_Subscribe_multicast_events" ) {}
 void runTest() { if ( suite_SvrMcastLocalRemoteTestSuite ) suite_SvrMcastLocalRemoteTestSuite->test_Subscribe_multicast_events(); }
} testDescription_SvrMcastLocalRemoteTestSuite_test_Subscribe_multicast_events;

static class TestDescription_SvrMcastLocalRemoteTestSuite_test_first_point_received : public CxxTest::RealTestDescription {
public:
 TestDescription_SvrMcastLocalRemoteTestSuite_test_first_point_received() : CxxTest::RealTestDescription( Tests_SvrMcastLocalRemoteTestSuite, suiteDescription_SvrMcastLocalRemoteTestSuite, 205, "test_first_point_received" ) {}
 void runTest() { if ( suite_SvrMcastLocalRemoteTestSuite ) suite_SvrMcastLocalRemoteTestSuite->test_first_point_received(); }
} testDescription_SvrMcastLocalRemoteTestSuite_test_first_point_received;

static class TestDescription_SvrMcastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events : public CxxTest::RealTestDescription {
public:
 TestDescription_SvrMcastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events() : CxxTest::RealTestDescription( Tests_SvrMcastLocalRemoteTestSuite, suiteDescription_SvrMcastLocalRemoteTestSuite, 219, "test_Callback_executed_after_a_change_for_both_events" ) {}
 void runTest() { if ( suite_SvrMcastLocalRemoteTestSuite ) suite_SvrMcastLocalRemoteTestSuite->test_Callback_executed_after_a_change_for_both_events(); }
} testDescription_SvrMcastLocalRemoteTestSuite_test_Callback_executed_after_a_change_for_both_events;

static class TestDescription_SvrMcastLocalRemoteTestSuite_test_unsubscribe_event : public CxxTest::RealTestDescription {
public:
 TestDescription_SvrMcastLocalRemoteTestSuite_test_unsubscribe_event() : CxxTest::RealTestDescription( Tests_SvrMcastLocalRemoteTestSuite, suiteDescription_SvrMcastLocalRemoteTestSuite, 255, "test_unsubscribe_event" ) {}
 void runTest() { if ( suite_SvrMcastLocalRemoteTestSuite ) suite_SvrMcastLocalRemoteTestSuite->test_unsubscribe_event(); }
} testDescription_SvrMcastLocalRemoteTestSuite_test_unsubscribe_event;

#include <cxxtest/Root.cpp>
