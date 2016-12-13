//
// Created by ingvord on 12/13/16.
//
#ifndef ReconnectionTestSuite_h
#define ReconnectionTestSuite_h

#include <config.h>
#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <cstdlib>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

void start_server(const char* instance);
void kill_server();


#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME ReconnectionTestSuite

class ReconnectionTestSuite: public CxxTest::TestSuite
{
protected:
    DeviceProxy *device1, *device2;
    string device1_name, device2_name, dev1_instance_name, dev2_instance_name;

public:
    SUITE_NAME():
            dev1_instance_name("test"),//TODO pass via cli
            dev2_instance_name("test2")
    {

//
// Arguments check -------------------------------------------------
//

        device1_name = CxxTest::TangoPrinter::get_param("device1");
        device2_name = CxxTest::TangoPrinter::get_param("device2");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try
        {
            device1 = new DeviceProxy(device1_name);
            device2 = new DeviceProxy(device2_name);
        }
        catch (CORBA::Exception &e)
        {
            Except::print_exception(e);
            exit(-1);
        }

    }

    virtual ~SUITE_NAME()
    {
        if (CxxTest::TangoPrinter::is_restore_set(dev2_instance_name + " started."))
            kill_server();

        delete device1;
        delete device2;
    }

    static SUITE_NAME *createSuite()
    {
        return new SUITE_NAME();
    }

    static void destroySuite(SUITE_NAME *suite)
    {
        delete suite;
    }

//
// Tests -------------------------------------------------------
//

    void test_server_event(void)
    {
        //TODO start server 2 and set fallback point
        start_server(dev2_instance_name.c_str());
        CxxTest::TangoPrinter::restore_set(dev2_instance_name + " started.");
        //TODO

        kill_server();
        CxxTest::TangoPrinter::restore_unset(dev2_instance_name + " started.");
    }

    void test_event_reconnection(void)
    {
        //TODO
    }

    void test_stateless_event_connection(void)
    {
        //TODO
    }

};

void start_server(const char* instance){
    string command=kStartServerCmd;
    command += instance;
    system(command.c_str());

    cout << std::ifstream(kProjectBinaryDir + "/DevTest_" + instance + ".out").rdbuf();
}

void kill_server(){
    system(kKillServerCmd.c_str());
}

#undef cout
#endif // ReconnectionTestSuite_h


