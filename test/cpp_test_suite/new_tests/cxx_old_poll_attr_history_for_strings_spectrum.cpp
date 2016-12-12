#ifndef PollTestSuite_h
#define PollTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace Tango;
using namespace std;

#define cout cout << "\t"
#define BASIC_NB_POLL        16

#undef SUITE_NAME
#define SUITE_NAME OldPollTestSuite__loop

typedef enum {
    FIRST_EXCEPT = 0,
    SECOND_EXCEPT,
    FIRST_DATA,
    SECOND_DATA
} AttrResult;


class OldPollTestSuite__loop : public CxxTest::TestSuite {
protected:
    DeviceProxy *device;
    string device_name;

    bool verbose;
    int hist_depth;

public:
    SUITE_NAME() : hist_depth{10} {

//
// Arguments check -------------------------------------------------
//

        device_name = CxxTest::TangoPrinter::get_param("device1");

        verbose = CxxTest::TangoPrinter::is_param_set("verbose");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try {
            device = new DeviceProxy(device_name);
            device->set_timeout_millis(1000000);
            device->ping();
        }
        catch (const CORBA::Exception &e) {
            Except::print_exception(e);
            exit(-1);
        }

        try {
            device->poll_attribute("PollString_spec_attr", 500);
        }
        catch (const CORBA::Exception &e) {
            Except::print_exception(e);
            exit(-1);
        }

        cout << "Wait for 7 seconds to fill in polling buffer..." << endl;
        this_thread::sleep_for(chrono::seconds{3});
        cout << "Done." << endl;
    }

    virtual ~SUITE_NAME() {

//
// Clean up --------------------------------------------------------
//
//	cout << "Device name = " << device_name << endl;
//	cout << "Kill device name = " << kill_device_name << endl;


//	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;
        CxxTest::TangoPrinter::stop_poll_att_no_except<DeviceProxy, DevFailed, CORBA::Exception, Except>(device, "PollString_spec_attr");


        delete device;
    }

    static SUITE_NAME *createSuite() {
        return new SUITE_NAME();
    }

    static void destroySuite(SUITE_NAME *suite) {
        delete suite;
    }

//
// Tests -------------------------------------------------------
//

    void test_attribute_history_for_strings_spectrum(void) {
        auto a_hist = device->attribute_history("PollString_spec_attr", hist_depth);

        string first_string;
        AttrResult ar;

        vector<string> str;

        if ((*a_hist)[0].has_failed() == true) {
            if (::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(), "aaaa") == 0)
                ar = FIRST_EXCEPT;
            else
                ar = SECOND_EXCEPT;
        } else {
            (*a_hist)[0] >> str;
            if (str.size() == 2)
                ar = FIRST_DATA;
            else
                ar = SECOND_DATA;
        }

        if (verbose) {
            for (size_t i = 0; i < a_hist->size(); i++) {

                cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
                TimeVal &t = (*a_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                if ((*a_hist)[i].has_failed() == false) {
                    (*a_hist)[i] >> str;
                    cout << "Value = " << str[0];
                    if (str.size() == 2)
                        cout << ", Value = " << str[1];
                    cout << endl;
                } else {
                    cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
                    cout << "Error level 0 reason = " << ((*a_hist)[i].get_err_stack())[0].reason << endl;
                    cout << "Error level 0 desc = " << ((*a_hist)[i].get_err_stack())[0].desc << endl;
                }
                cout << endl;
            }
        }

        switch (ar) {
            case FIRST_EXCEPT:
                TS_ASSERT((*a_hist)[0].has_failed() == true);
                TS_ASSERT((*a_hist)[0].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[0].get_err_stack())[0].desc.in(), "bbb") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(), "aaaa") == 0);

                TS_ASSERT((*a_hist)[1].has_failed() == true);
                TS_ASSERT((*a_hist)[1].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[1].get_err_stack())[0].desc.in(), "yyy") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[1].get_err_stack())[0].reason.in(), "xxx") == 0);

                (*a_hist)[2] >> str;
                TS_ASSERT(str.size() == 2);
                TS_ASSERT(str[0] == "Hello world");
                TS_ASSERT(str[1] == "Hello universe");

                (*a_hist)[3] >> str;
                TS_ASSERT(str.size() == 1);
                TS_ASSERT(str[0] == "Hello Grenoble");
                break;

            case SECOND_EXCEPT:
                TS_ASSERT((*a_hist)[0].has_failed() == true);
                TS_ASSERT((*a_hist)[0].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[0].get_err_stack())[0].desc.in(), "yyy") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(), "xxx") == 0);

                (*a_hist)[1] >> str;
                TS_ASSERT(str.size() == 2);
                TS_ASSERT(str[0] == "Hello world");
                TS_ASSERT(str[1] == "Hello universe");

                (*a_hist)[2] >> str;
                TS_ASSERT(str.size() == 1);
                TS_ASSERT(str[0] == "Hello Grenoble");

                TS_ASSERT((*a_hist)[3].has_failed() == true);
                TS_ASSERT((*a_hist)[3].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[3].get_err_stack())[0].desc.in(), "bbb") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[3].get_err_stack())[0].reason.in(), "aaaa") == 0);
                break;

            case FIRST_DATA:
                (*a_hist)[0] >> str;
                TS_ASSERT(str.size() == 2);
                TS_ASSERT(str[0] == "Hello world");
                TS_ASSERT(str[1] == "Hello universe");

                (*a_hist)[1] >> str;
                TS_ASSERT(str.size() == 1);
                TS_ASSERT(str[0] == "Hello Grenoble");

                TS_ASSERT((*a_hist)[2].has_failed() == true);
                TS_ASSERT((*a_hist)[2].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[2].get_err_stack())[0].desc.in(), "bbb") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[2].get_err_stack())[0].reason.in(), "aaaa") == 0);

                TS_ASSERT((*a_hist)[3].has_failed() == true);
                TS_ASSERT((*a_hist)[3].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[3].get_err_stack())[0].desc.in(), "yyy") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[3].get_err_stack())[0].reason.in(), "xxx") == 0);
                break;

            case SECOND_DATA:
                (*a_hist)[0] >> str;
                TS_ASSERT(str.size() == 1);
                TS_ASSERT(str[0] == "Hello Grenoble");

                TS_ASSERT((*a_hist)[1].has_failed() == true);
                TS_ASSERT((*a_hist)[1].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[1].get_err_stack())[0].desc.in(), "bbb") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[1].get_err_stack())[0].reason.in(), "aaaa") == 0);

                TS_ASSERT((*a_hist)[2].has_failed() == true);
                TS_ASSERT((*a_hist)[2].get_err_stack().length() == 1);
                TS_ASSERT(::strcmp(((*a_hist)[2].get_err_stack())[0].desc.in(), "yyy") == 0);
                TS_ASSERT(::strcmp(((*a_hist)[2].get_err_stack())[0].reason.in(), "xxx") == 0);

                (*a_hist)[3] >> str;
                TS_ASSERT(str.size() == 2);
                TS_ASSERT(str[0] == "Hello world");
                TS_ASSERT(str[1] == "Hello universe");
                break;
        }
        delete a_hist;
    }
};

#undef cout
#endif // PollTestSuite_h
