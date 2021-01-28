#ifndef PollTestSuite_h
#define PollTestSuite_h

#include <thread>
#include <chrono>
#include "cxx_common.h"

#define BASIC_NB_POLL        16u
#define TEST_CLASS "devTest"

#undef SUITE_NAME
#define SUITE_NAME OldPollTestSuite__loop

typedef enum {
    FIRST_EXCEPT = 0,
    SECOND_EXCEPT,
    FIRST_DATA,
    SECOND_DATA
} AttrResult;

typedef enum {
    EXCEPT = 0,
    FIRST_STR,
    SECOND_STR
} CmdResult;

void split_string(string &, char, vector <string> &);

void stop_poll_att_no_except(DeviceProxy *, const char *);

void stop_poll_cmd_no_except(DeviceProxy *, const char *);

void del_device_no_error(Database &, string&);

class OldPollTestSuite__loop : public CxxTest::TestSuite {
protected:
    DeviceProxy *device;
    string device_name, device2_name, alias_name, serv_name, admin_dev_name, inst_name;
    string new_dev, new_dev1_th2, new_dev2_th2, new_dev1_th3;
    vector <string> ref_polling_pool_conf;

    bool verbose;
    int hist_depth;

public:
    SUITE_NAME() : admin_dev_name{"dserver/"},
                   inst_name{"debian8"},//TODO pass from cmd
                   new_dev{"test/debian8/77"},
                   new_dev1_th2{"test/debian8/800"},
                   new_dev2_th2{"test/debian8/801"},
                   new_dev1_th3{"test/debian8/9000"},
                   ref_polling_pool_conf{1, "test/debian8/10,test/debian8/11"},
                   hist_depth{10} {

//
// Arguments check -------------------------------------------------
//

        device_name = CxxTest::TangoPrinter::get_param("device1");
        device2_name = CxxTest::TangoPrinter::get_param("device2");
        alias_name = CxxTest::TangoPrinter::get_param("devicealias");
        serv_name = CxxTest::TangoPrinter::get_param("fulldsname");

        admin_dev_name += serv_name;

        verbose = CxxTest::TangoPrinter::is_param_set("verbose");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try {
            device = new DeviceProxy(device_name);
            device->ping();
        }
        catch (const CORBA::Exception &e) {
            Except::print_exception(e);
            exit(-1);
        }

        try {
            device->poll_command("IOPollStr1", 500);
            device->poll_command("IOPollArray2", 500);
            device->poll_command("IOExcept", 2000);
            device->poll_command("OEncoded", 500);
            device->poll_command("State", 2000);
            device->poll_command("Status", 2000);

            device->poll_attribute("PollLong_attr", 1000);
            device->poll_attribute("PollString_spec_attr", 500);
            device->poll_attribute("attr_wrong_type", 500);

            device->poll_attribute("Long64_attr_rw", 500);
            device->poll_attribute("ULong_spec_attr_rw", 500);
            device->poll_attribute("ULong64_attr_rw", 500);
            device->poll_attribute("State_spec_attr_rw", 500);
            device->poll_attribute("Encoded_attr", 500);
        }
        catch (const CORBA::Exception &e) {
            Except::print_exception(e);
            exit(-1);
        }

        cout << "Wait for 7 seconds to fill in polling buffer..." << endl;
        this_thread::sleep_for(chrono::seconds{7});
        cout << "Done." << endl;
    }

    virtual ~SUITE_NAME() {

//
// Clean up --------------------------------------------------------
//
//	cout << "Device name = " << device_name << endl;
//	cout << "Kill device name = " << kill_device_name << endl;


        if (CxxTest::TangoPrinter::is_restore_set("dev1_source_cache"))
            device->set_source(Tango::CACHE_DEV);

        if (CxxTest::TangoPrinter::is_restore_set("dev2_poll_PollLong_attr_1000"))
            stop_poll_att_no_except(new DeviceProxy(device2_name), "PollLong_attr");

//	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;
        stop_poll_cmd_no_except(device, "IOPollStr1");
        stop_poll_cmd_no_except(device, "IOArray1");
        stop_poll_cmd_no_except(device, "IOPollArray2");
        stop_poll_cmd_no_except(device, "IOExcept");
        stop_poll_cmd_no_except(device, "OEncoded");
        stop_poll_cmd_no_except(device, "State");
        stop_poll_cmd_no_except(device, "Status");

        stop_poll_att_no_except(device, "PollLong_attr");
        stop_poll_att_no_except(device, "PollString_spec_attr");
        stop_poll_att_no_except(device, "attr_wrong_type");

        stop_poll_att_no_except(device, "Long64_attr_rw");
        stop_poll_att_no_except(device, "ULong_spec_attr_rw");
        stop_poll_att_no_except(device, "ULong64_attr_rw");
        stop_poll_att_no_except(device, "State_spec_attr_rw");
        stop_poll_att_no_except(device, "Encoded_attr");

        stop_poll_att_no_except(device, "event_change_tst");
        stop_poll_att_no_except(device, "event64_change_tst");
        stop_poll_att_no_except(device, "short_attr");
        stop_poll_att_no_except(device, "slow_actuator");
        stop_poll_att_no_except(device, "fast_actuator");

        if (CxxTest::TangoPrinter::is_restore_set("reset_device_server"))
            reset_device_server();

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

    void test_read_command_history_string(void) {
        auto d_hist = device->command_history("IOPollStr1", hist_depth);

        TSM_ASSERT_LESS_THAN("Not enough data in the polling buffer, restart later", 4u, d_hist->size());

        CmdResult cr;

        if ((*d_hist)[0].has_failed() == true) {
            cr = EXCEPT;
        } else {
            string str;
            (*d_hist)[0] >> str;
            if (str[0] == 'E')
                cr = FIRST_STR;
            else
                cr = SECOND_STR;
        }

        for (size_t i = 0; i < d_hist->size(); i++) {
            string str;
            if ((*d_hist)[i].has_failed() == false)
                (*d_hist)[i] >> str;

            if (verbose) {
                cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
                if ((*d_hist)[i].has_failed() == false) {
                    (*d_hist)[i] >> str;
                    cout << "Value = " << str << endl;
                }
                TimeVal &t = (*d_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
                cout << endl;
            }
        }

        DevErrorList del;
        string simple_str;

        switch (cr) {
            case EXCEPT:
                TS_ASSERT((*d_hist)[0].has_failed() == true);
                TS_ASSERT((*d_hist)[0].get_err_stack().length() == 1);
                del = (*d_hist)[0].get_err_stack();
                TS_ASSERT(::strcmp(del[0].desc.in(), "www") == 0);

                (*d_hist)[1] >> simple_str;
                TS_ASSERT((*d_hist)[1].has_failed() == false);
                TS_ASSERT((*d_hist)[1].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Even value from IOPollStr1");

                (*d_hist)[2] >> simple_str;
                TS_ASSERT((*d_hist)[2].has_failed() == false);
                TS_ASSERT((*d_hist)[2].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Odd value from IOPollStr1");
                break;

            case FIRST_STR:
                (*d_hist)[0] >> simple_str;
                TS_ASSERT((*d_hist)[0].has_failed() == false);
                TS_ASSERT((*d_hist)[0].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Even value from IOPollStr1");

                (*d_hist)[1] >> simple_str;
                TS_ASSERT((*d_hist)[1].has_failed() == false);
                TS_ASSERT((*d_hist)[1].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Odd value from IOPollStr1");

                TS_ASSERT((*d_hist)[2].has_failed() == true);
                TS_ASSERT((*d_hist)[2].get_err_stack().length() == 1);
                del = (*d_hist)[2].get_err_stack();
                TS_ASSERT(::strcmp(del[0].desc.in(), "www") == 0);
                break;

            case SECOND_STR:
                (*d_hist)[0] >> simple_str;
                TS_ASSERT((*d_hist)[0].has_failed() == false);
                TS_ASSERT((*d_hist)[0].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Odd value from IOPollStr1");

                TS_ASSERT((*d_hist)[1].has_failed() == true);
                TS_ASSERT((*d_hist)[1].get_err_stack().length() == 1);
                del = (*d_hist)[1].get_err_stack();
                TS_ASSERT(::strcmp(del[0].desc.in(), "www") == 0);

                (*d_hist)[2] >> simple_str;
                TS_ASSERT((*d_hist)[2].has_failed() == false);
                TS_ASSERT((*d_hist)[2].get_err_stack().length() == 0);
                TS_ASSERT(simple_str == "Even value from IOPollStr1");
                break;
        }

        delete d_hist;
    }

    void test_command_history_array(void) {
        auto d_hist = device->command_history("IOPollArray2", hist_depth);

        TSM_ASSERT_LESS_THAN("Not enough data in the polling buffer, restart later", 4u, d_hist->size());

        short first_val_first_rec;
        for (size_t i = 0; i < d_hist->size(); i++) {
            vector<short> vect;
            (*d_hist)[i] >> vect;

            if (verbose) {
                cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
                cout << "Value 0 = " << vect[0] << ", Value 1 = " << vect[1] << endl;
                TimeVal &t = (*d_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
                cout << endl;
            }

            if (i == 0)
                first_val_first_rec = vect[0];

            TS_ASSERT((*d_hist)[i].has_failed() == false);
            TS_ASSERT((*d_hist)[i].get_err_stack().length() == 0);
            if (i != 0) {
                if (first_val_first_rec == 100) {
                    if ((i % 2) == 0) {
                        TS_ASSERT(vect[0] == 100);
                        TS_ASSERT(vect[1] == 200);
                    } else {
                        TS_ASSERT(vect[0] == 300);
                        TS_ASSERT(vect[1] == 400);
                    }
                } else {
                    if ((i % 2) == 0) {
                        TS_ASSERT(vect[0] == 300);
                        TS_ASSERT(vect[1] == 400);
                    } else {
                        TS_ASSERT(vect[0] == 100);
                        TS_ASSERT(vect[1] == 200);
                    }
                }
            }
        }
        delete d_hist;
    }

    void test_command_history_with_exception(void) {
        auto d_hist = device->command_history("IOExcept", hist_depth);

        for (size_t i = 0; i < d_hist->size(); i++) {
            if (verbose) {
                cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
                TimeVal &t = (*d_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
                cout << endl;
            }

            TS_ASSERT((*d_hist)[i].has_failed() == true);
            TS_ASSERT((*d_hist)[i].get_err_stack().length() == 1);
            TS_ASSERT(!strcmp((*d_hist)[i].get_err_stack()[0].reason, API_ThrowException));
        }
        delete d_hist;
    }


    void test_command_history_for_state(void) {
        auto d_hist = device->command_history("State", hist_depth);

        for (size_t i = 0; i < d_hist->size(); i++) {
            if (verbose) {
                cout << (*d_hist)[i] << endl;
            }

            TS_ASSERT((*d_hist)[i].has_failed() == false);
            TS_ASSERT((*d_hist)[i].get_err_stack().length() == 0);
            Tango::DevState ds;
            (*d_hist)[i] >> ds;
            TS_ASSERT(ds == Tango::ON);
        }
        delete d_hist;
    }

    void test_command_history_for_status(void) {
        auto d_hist = device->command_history("Status", hist_depth);

        for (size_t i = 0; i < d_hist->size(); i++) {
            if (verbose) {
                cout << (*d_hist)[i] << endl;
            }

            TS_ASSERT((*d_hist)[i].has_failed() == false);
            TS_ASSERT((*d_hist)[i].get_err_stack().length() == 0);
            string str;
            (*d_hist)[i] >> str;
            TS_ASSERT(::strcmp(str.c_str(), "The device is in ON state.") == 0);
        }
        delete d_hist;
    }

    void test_command_history_DevEncoded(void) {
        auto d_hist = device->command_history("OEncoded", hist_depth);

        TSM_ASSERT_LESS_THAN("Not enough data in the polling buffer, restart later", 4u, d_hist->size());

        unsigned char first_val_enc;
        for (size_t i = 0; i < d_hist->size(); i++) {
            DevEncoded the_enc;
            (*d_hist)[i] >> the_enc;

            if (verbose) {
                cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
                cout << "Encoded_format = " << the_enc.encoded_format << endl;
                for (unsigned int ii = 0; ii < the_enc.encoded_data.length(); ++ii)
                    cout << "Encoded_data = " << (int) the_enc.encoded_data[ii] << endl;
                TimeVal &t = (*d_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
                cout << endl;
            }

            TS_ASSERT((*d_hist)[i].has_failed() == false);
            TS_ASSERT((*d_hist)[i].get_err_stack().length() == 0);

            if (i == 0)
                first_val_enc = the_enc.encoded_data[0];

            if ((i % 2) == 0) {
                if (first_val_enc == 11) {
                    TS_ASSERT(!strcmp(the_enc.encoded_format, "Odd - OEncoded format"));
                    TS_ASSERT(the_enc.encoded_data.length() == 2);
                    TS_ASSERT(the_enc.encoded_data[0] == 11);
                    TS_ASSERT(the_enc.encoded_data[1] == 21);
                } else {
                    TS_ASSERT(!strcmp(the_enc.encoded_format, "Even - OEncoded format"));
                    TS_ASSERT(the_enc.encoded_data[0] == 10);
                    TS_ASSERT(the_enc.encoded_data[1] == 20);
                    TS_ASSERT(the_enc.encoded_data[2] == 30);
                    TS_ASSERT(the_enc.encoded_data[3] == 40);
                }
            } else {
                if (first_val_enc == 11) {
                    TS_ASSERT(!strcmp(the_enc.encoded_format, "Even - OEncoded format"));
                    TS_ASSERT(the_enc.encoded_data[0] == 10);
                    TS_ASSERT(the_enc.encoded_data[1] == 20);
                    TS_ASSERT(the_enc.encoded_data[2] == 30);
                    TS_ASSERT(the_enc.encoded_data[3] == 40);
                } else {
                    TS_ASSERT(!strcmp(the_enc.encoded_format, "Odd - OEncoded format"));
                    TS_ASSERT(the_enc.encoded_data.length() == 2);
                    TS_ASSERT(the_enc.encoded_data[0] == 11);
                    TS_ASSERT(the_enc.encoded_data[1] == 21);
                }
            }
        }
        delete d_hist;
    }

    void test_attribute_history_for_long(void) {
        auto a_hist = device->attribute_history("PollLong_attr", hist_depth);

        DevLong first_val;
        for (size_t i = 0; i < a_hist->size(); i++) {
            DevLong lo;
            (*a_hist)[i] >> lo;

            if (i == 0)
                first_val = lo;

            if (verbose) {
                cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
                cout << "Value = " << lo << endl;
                TimeVal &t = (*a_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
                cout << endl;
            }

            TS_ASSERT((*a_hist)[i].has_failed() == false);
            TS_ASSERT((*a_hist)[i].get_err_stack().length() == 0);

            TS_ASSERT((*a_hist)[i].get_dim_x() == 1);
            TS_ASSERT((*a_hist)[i].get_dim_y() == 0);

            if (first_val == 5555) {
                if ((i % 2) == 0) {
                    TS_ASSERT(lo == 5555);
                } else {
                    TS_ASSERT(lo == 6666);
                }
            } else {
                if ((i % 2) == 0) {
                    TS_ASSERT(lo == 6666);
                } else {
                    TS_ASSERT(lo == 5555);
                }
            }
        }
        delete a_hist;
    }

    void test_attribute_history_for_strings_spectrum(void) {
        auto a_hist = device->attribute_history("PollString_spec_attr", hist_depth);

        string first_string;
        AttrResult ar;

        vector <string> str;

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

        for (size_t i = 0; i < a_hist->size(); i++) {
            if (verbose) {
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

    void test_attribute_history_for_dev_encoded(void) {
        auto enc_hist = device->attribute_history("Encoded_attr", hist_depth);

        for (size_t i = 0; i < enc_hist->size(); i++) {

            if (verbose) {
                cout << "Value = " << (*enc_hist)[i] << endl;
                cout << endl;
            }

            TS_ASSERT((*enc_hist)[i].has_failed() == false);
            TS_ASSERT((*enc_hist)[i].get_err_stack().length() == 0);

            TS_ASSERT((*enc_hist)[i].get_dim_x() == 1);
            TS_ASSERT((*enc_hist)[i].get_dim_y() == 0);

            DevEncoded enc;
            (*enc_hist)[i] >> enc;

            TS_ASSERT(::strcmp(enc.encoded_format, "Which format?") == 0);
            TS_ASSERT(enc.encoded_data[0] == 97);
            TS_ASSERT(enc.encoded_data[1] == 98);
            TS_ASSERT(enc.encoded_data[2] == 99);
            TS_ASSERT(enc.encoded_data[3] == 100);
        }
        delete enc_hist;
    }

    void test_attribute_history_with_exception(void) {
        auto a_hist = device->attribute_history("attr_wrong_type", hist_depth);

        for (size_t i = 0; i < a_hist->size(); i++) {
            if (verbose) {
                cout << "Command failed = " << (*a_hist)[i].has_failed() << endl;
                TimeVal &t = (*a_hist)[i].get_date();
                cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
                cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
                cout << "Error level 0 reason = " << ((*a_hist)[i].get_err_stack())[0].reason << endl;
                cout << "Error level 0 desc = " << ((*a_hist)[i].get_err_stack())[0].desc << endl;
                cout << endl;
            }

            TS_ASSERT((*a_hist)[i].has_failed() == true);
            TS_ASSERT((*a_hist)[i].get_err_stack().length() == 1);
            TS_ASSERT(!strcmp((*a_hist)[i].get_err_stack()[0].reason, API_AttrOptProp));
//			AttributeDimension dim;
//			dim = (*a_hist)[i].get_r_dimension();

            TS_ASSERT((*a_hist)[i].get_dim_x() == 0);
            TS_ASSERT((*a_hist)[i].get_dim_y() == 0);

//			dim = (*a_hist)[i].get_w_dimension();

//			TS_ASSERT ( dim.dim_x == 0);
//			TS_ASSERT ( dim.dim_y == 0);

        }
        delete a_hist;
    }

    void test_getting_a_long_64_attribute_from_polling_buffer(void) {
        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE));
        CxxTest::TangoPrinter::restore_set("dev1_source_cache");

        DeviceAttribute da;
        int data_type;

        TS_ASSERT_THROWS_NOTHING(da = device->read_attribute("Long64_attr_rw"));
        DevLong64 lo;
        da >> lo;
        data_type = da.get_type();
        if (verbose)
            cout << "64 bits attribute data = " << hex << lo << dec << endl;
        TS_ASSERT(lo == 0x800000000LL);
        TS_ASSERT(data_type == Tango::DEV_LONG64);
    }

    void test_getting_an_unsigned_long_64_bits_attribute_from_polling_buffer(void) {
        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE));
        CxxTest::TangoPrinter::restore_set("dev1_source_cache");

        DeviceAttribute da_ulo;
        TS_ASSERT_THROWS_NOTHING(da_ulo = device->read_attribute("ULong64_attr_rw"));
        DevULong64 ulo;
        da_ulo >> ulo;
        int data_type_ulo = da_ulo.get_type();
        if (verbose)
            cout << "unsigned 64 bits attribute data = " << hex << ulo << dec << endl;
        TS_ASSERT(ulo == 0xC000000000000000LL);
        TS_ASSERT(data_type_ulo == Tango::DEV_ULONG64);
    }

    void test_getting_a_unsigned_long_spectrum_attribute_from_polling_buffer(void) {
        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE));
        CxxTest::TangoPrinter::restore_set("dev1_source_cache");

        DeviceAttribute da;
        TS_ASSERT_THROWS_NOTHING(da = device->read_attribute("ULong_spec_attr_rw"));
        vector <DevULong> v_lo;
        auto ret = (da >> v_lo);

        TS_ASSERT(ret == true);
        TS_ASSERT(v_lo[0] == 2222);
        TS_ASSERT(v_lo[1] == 22222);
        TS_ASSERT(v_lo[2] == 222222);
    }

    void test_getting_a_state_spectrum_attribute_from_polling_buffer(void) {
        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE));
        CxxTest::TangoPrinter::restore_set("dev1_source_cache");

        DeviceAttribute da;
        TS_ASSERT_THROWS_NOTHING(da = device->read_attribute("State_spec_attr_rw"));
        vector <DevState> v_sta;
        auto ret = (da >> v_sta);

        TS_ASSERT(ret == true);
        TS_ASSERT(v_sta[0] == Tango::ON);
        TS_ASSERT(v_sta[1] == Tango::OFF);
    }

    void test_getting_a_dev_encoded_attribute_from_polling_buffer(void) {
        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE));
        CxxTest::TangoPrinter::restore_set("dev1_source_cache");

        DeviceAttribute da;
        TS_ASSERT_THROWS_NOTHING(da = device->read_attribute("Encoded_attr"));
        DevEncoded enc_lo;
        da >> enc_lo;
        auto data_type = da.get_type();
        TS_ASSERT(::strcmp(enc_lo.encoded_format.in(), "Which format?") == 0);
        TS_ASSERT(data_type == Tango::DEV_ENCODED);
        TS_ASSERT(enc_lo.encoded_data.length() == 4);
        TS_ASSERT(enc_lo.encoded_data[0] == 97);
        TS_ASSERT(enc_lo.encoded_data[1] == 98);
        TS_ASSERT(enc_lo.encoded_data[2] == 99);
        TS_ASSERT(enc_lo.encoded_data[3] == 100);
    }

    void test_polling_status_from_device_name(void) {
        if (CxxTest::TangoPrinter::is_restore_set("dev1_source_cache"))
            TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE_DEV));

        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device->polling_status());

        unsigned long nb_polled = BASIC_NB_POLL;
        for (unsigned int i = 0; i < poll_str->size(); i++) {
            if ((*poll_str)[i].find("String_attr") != string::npos) {
                nb_polled++;
            }
            if ((*poll_str)[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        TS_ASSERT_EQUALS(poll_str->size(), nb_polled);

        delete poll_str;
    }

    void test_polling_status_from_device_name_2(void) {
        auto device2 = new DeviceProxy(device2_name);
        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device2->polling_status());

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        TS_ASSERT(poll_str->size() == 0);
        delete poll_str;
        delete device2;
    }

    void test_polling_status_from_alias_name(void) {

        string adm_name = device->adm_name();
        DeviceProxy *admin_dev = new DeviceProxy(adm_name);


        DeviceData d_send, d_received;
        d_send << alias_name;
        TS_ASSERT_THROWS_NOTHING(d_received = admin_dev->command_inout("DevPollStatus", d_send));
        vector <string> v_str;
        d_received >> v_str;

        auto nb_polled = BASIC_NB_POLL;
        for (unsigned int i = 0; i < v_str.size(); i++) {
            if (v_str[i].find("String_attr") != string::npos) {
                nb_polled++;
                continue;
            }
            if (v_str[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        if (verbose) {
            cout << v_str.size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < v_str.size(); i++)
                cout << "Polling status = " << v_str[i] << endl;
            cout << endl;
        }

        TS_ASSERT_EQUALS(v_str.size(), nb_polled);
    }

    void test_get_command_poll_period(void) {

        string cmd("IOExcept");
        int per = 0;
        TS_ASSERT_THROWS_NOTHING(per = device->get_command_poll_period(cmd));

        if (verbose) {
            cout << "Cmd " << cmd << " polling period = " << per << endl;
        }

        TS_ASSERT(per == 2000);

        string attr("PollLong_attr");
        TS_ASSERT_THROWS_NOTHING(per = device->get_attribute_poll_period(attr));

        if (verbose)
            cout << "Attr " << attr << " polling period = " << per << endl;

        TS_ASSERT(per == 1000);
    }

    void test_poll_command(void) {

        string cmd{"IOArray1"};
        bool poll;
        TS_ASSERT_THROWS_NOTHING(poll = device->is_command_polled(cmd));

        TS_ASSERT(poll == false);

        TS_ASSERT_THROWS_NOTHING(device->poll_command(cmd, 3000));
        TS_ASSERT_THROWS_NOTHING(poll = device->is_command_polled(cmd));

        TS_ASSERT(poll == true);

        int per;
        TS_ASSERT_THROWS_NOTHING(per = device->get_command_poll_period(cmd));

        TS_ASSERT(per == 3000);

#ifdef WIN32
        Sleep(3000);
#else
        this_thread::sleep_for(chrono::seconds{3});
#endif

        TS_ASSERT_THROWS_NOTHING(device->poll_command(cmd, 5000));
        TS_ASSERT_THROWS_NOTHING(per = device->get_command_poll_period(cmd));

        TS_ASSERT(per == 5000);

#ifdef WIN32
        Sleep(3000);
#else
        this_thread::sleep_for(chrono::seconds{3});
#endif
        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device->polling_status());

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        std::size_t nb_polled = BASIC_NB_POLL + 1;
        for (unsigned int i = 0; i < poll_str->size(); i++) {
            if ((*poll_str)[i].find("String_attr") != string::npos) {
                nb_polled++;
            }
            if ((*poll_str)[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        TS_ASSERT_EQUALS(poll_str->size(), nb_polled);

        delete poll_str;
    }

    void test_min_polling_period(void) {
//
//  WARNING, this test works only if device property min_poll_period set to 200
//  and cmd_min_poll_period set to IOExcept,500
//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcatch-value"
#endif

#undef _TS_LAST_CATCH
#define _TS_LAST_CATCH(b) _TS_CATCH_TYPE( (...), b )

        TS_ASSERT_THROWS(device->poll_command("IOExcept", 300), Tango::DevFailed);

        TS_ASSERT_THROWS(device->poll_command("IOExcept", 100), Tango::DevFailed);

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

        TS_ASSERT_THROWS_NOTHING(device->poll_command("IOExcept", 500));

#ifdef WIN32
        Sleep(3000);
#else
        this_thread::sleep_for(chrono::seconds{3});
#endif

        TS_ASSERT_THROWS_NOTHING(device->poll_command("IOExcept", 2000));

        DbData db;
        vector <string> prop_vs;
        prop_vs.push_back("IOExcept");
        prop_vs.push_back("500");
        db.push_back(DbDatum("cmd_min_poll_period"));
        db[0] << prop_vs;
        TS_ASSERT_THROWS_NOTHING(device->put_property(db));
    }

    void test_stop_poll_command(void) {
        string cmd{"IOArray1"};
        TS_ASSERT_THROWS_NOTHING(device->stop_poll_command(cmd));

        bool poll;
        TS_ASSERT_THROWS_NOTHING(poll = device->is_command_polled(cmd));

        TS_ASSERT(poll == false);

        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device->polling_status());

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        std::size_t nb_polled = BASIC_NB_POLL;
        for (unsigned int i = 0; i < poll_str->size(); i++) {
            if ((*poll_str)[i].find("String_attr") != string::npos) {
                nb_polled++;
            }
            if ((*poll_str)[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        TS_ASSERT_EQUALS(poll_str->size(), nb_polled);

        delete poll_str;
    }

    void test_poll_attribute(void) {

        string attr{"Double_attr"};
        bool poll;
        TS_ASSERT_THROWS_NOTHING(poll = device->is_attribute_polled(attr));

        TS_ASSERT(poll == false);

        TS_ASSERT_THROWS_NOTHING(device->poll_attribute(attr, 3000));
        TS_ASSERT_THROWS_NOTHING(poll = device->is_attribute_polled(attr));

        TS_ASSERT(poll == true);

        int per;
        TS_ASSERT_THROWS_NOTHING(per = device->get_attribute_poll_period(attr));

        TS_ASSERT(per == 3000);

#ifdef WIN32
        Sleep(3000);
#else
        this_thread::sleep_for(chrono::seconds{3});
#endif

        TS_ASSERT_THROWS_NOTHING(device->poll_attribute(attr, 5000));
        TS_ASSERT_THROWS_NOTHING(per = device->get_attribute_poll_period(attr));

        TS_ASSERT(per == 5000);

#ifdef WIN32
        Sleep(3000);
#else
        this_thread::sleep_for(chrono::seconds{3});
#endif

        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device->polling_status());

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        std::size_t nb_polled = BASIC_NB_POLL + 1;
        for (unsigned int i = 0; i < poll_str->size(); i++) {
            if ((*poll_str)[i].find("String_attr") != string::npos) {
                nb_polled++;
            }
            if ((*poll_str)[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        TS_ASSERT_EQUALS(poll_str->size(), nb_polled);

        delete poll_str;
    }

    void test_stop_poll_attribute(void) {
        string attr{"Double_attr"};
        TS_ASSERT_THROWS_NOTHING(device->stop_poll_attribute(attr));

        bool poll;
        TS_ASSERT_THROWS_NOTHING(poll = device->is_attribute_polled(attr));

        TS_ASSERT(poll == false);

        vector <string> *poll_str;
        TS_ASSERT_THROWS_NOTHING(poll_str = device->polling_status());

        if (verbose) {
            cout << poll_str->size() << " object(s) polled for device" << endl;
            cout << endl;
            for (unsigned int i = 0; i < poll_str->size(); i++)
                cout << "Polling status = " << (*poll_str)[i] << endl;
            cout << endl;
        }

        std::size_t nb_polled = BASIC_NB_POLL;
        for (unsigned int i = 0; i < poll_str->size(); i++) {
            if ((*poll_str)[i].find("String_attr") != string::npos) {
                nb_polled++;
            }
            if ((*poll_str)[i].find("IOStartPoll") != string::npos) {
                nb_polled++;
            }
        }

        TS_ASSERT_EQUALS(poll_str->size(), nb_polled);

        delete poll_str;
    }

    void test_poll_device_2(void) {
        auto dev2 = new DeviceProxy(device2_name);
        TS_ASSERT_THROWS_NOTHING(dev2->poll_attribute("PollLong_attr", 1000));
        CxxTest::TangoPrinter::restore_set("dev2_poll_PollLong_attr_1000");

        this_thread::sleep_for(chrono::seconds{2});

        vector <string> polled_devs;
        split_string(ref_polling_pool_conf[0], ',', polled_devs);
        int nb_polled_devs = polled_devs.size();

// Add a device into device server and restart it
// Also add property to poll one of the device attribute


        DbDevInfo my_device_info{
                new_dev.c_str(),
                TEST_CLASS,
                serv_name.c_str()
        };

        Database db{};
        TS_ASSERT_THROWS_NOTHING(db.add_device(my_device_info));

        DbDatum poll_prop("polled_attr");
        vector <string> poll_param;
        poll_param.push_back("PollLong_attr");
        poll_param.push_back("1000");
        poll_prop << poll_param;
        DbData db_poll;
        db_poll.push_back(poll_prop);
        TS_ASSERT_THROWS_NOTHING(db.put_device_property(new_dev.c_str(), db_poll));
        CxxTest::TangoPrinter::restore_set("reset_device_server");

        auto admin_dev = new DeviceProxy(admin_dev_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev->command_inout("RestartServer"));

        this_thread::sleep_for(chrono::seconds{5});

// Read polling threads pool conf once more

        DeviceData da;

        DeviceProxy dev{device_name};
        TS_ASSERT_THROWS_NOTHING(da = dev.command_inout("PollingPoolTst"));
        vector <string> new_polling_pool_conf;
        da >> new_polling_pool_conf;

        TS_ASSERT(ref_polling_pool_conf.size() == new_polling_pool_conf.size());
        split_string(new_polling_pool_conf[0], ',', polled_devs);
        int new_nb_polled_devs = polled_devs.size();

        TS_ASSERT(new_nb_polled_devs == nb_polled_devs + 1);

        auto iter = find(polled_devs.begin(), polled_devs.end(), new_dev);
        TS_ASSERT(iter != polled_devs.end());
    }

    void test_change_polling_thread_number_and_add_2_more_devices(void) {
        DbDevInfo my_device_info{
                new_dev1_th2.c_str(),
                TEST_CLASS,
                serv_name.c_str()
        };

        Database db{};
        TS_ASSERT_THROWS_NOTHING(db.add_device(my_device_info));

        my_device_info.name = new_dev2_th2.c_str();
        TS_ASSERT_THROWS_NOTHING(db.add_device(my_device_info));

        DbDatum pool_size("polling_threads_pool_size");
        DbData db_data;

        pool_size << 2L;
        db_data.push_back(pool_size);
        TS_ASSERT_THROWS_NOTHING(db.put_device_property(admin_dev_name.c_str(), db_data));


        DbDatum poll_prop("polled_attr");
        vector <string> poll_param;
        poll_param.push_back("PollLong_attr");
        poll_param.push_back("1000");
        poll_prop << poll_param;
        DbData db_poll;
        db_poll.push_back(poll_prop);

        TS_ASSERT_THROWS_NOTHING(db.put_device_property(new_dev1_th2.c_str(), db_poll));
        TS_ASSERT_THROWS_NOTHING(db.put_device_property(new_dev2_th2.c_str(), db_poll));
        CxxTest::TangoPrinter::restore_set("reset_device_server");

        auto admin_dev = new DeviceProxy(admin_dev_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev->command_inout("RestartServer"));

        this_thread::sleep_for(chrono::seconds{5});

// Check new pool conf

        DeviceData dz;

        DeviceProxy dev(device_name);
        TS_ASSERT_THROWS_NOTHING(dz = dev.command_inout("PollingPoolTst"));

        vector <string> new_polling_pool_conf{};
        dz >> new_polling_pool_conf;

        TS_ASSERT(new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 1);

        vector <string> polled_devs{};
        split_string(new_polling_pool_conf[1], ',', polled_devs);
        auto new_nb_polled_devs = polled_devs.size();

        TS_ASSERT(new_nb_polled_devs == 2);

        auto iter = find(polled_devs.begin(), polled_devs.end(), new_dev1_th2);
        TS_ASSERT(iter != polled_devs.end());

        iter = find(polled_devs.begin(), polled_devs.end(), new_dev2_th2);
        TS_ASSERT(iter != polled_devs.end());
    }

    void test_change_polling_thread_number_to_3_and_add_1_more_device(void) {
        DbDevInfo my_device_info{
                new_dev1_th3.c_str(),
                TEST_CLASS,
                serv_name.c_str()
        };

        Database db{};
        TS_ASSERT_THROWS_NOTHING(db.add_device(my_device_info));

        DbDatum pool_size3("polling_threads_pool_size");
        DbData db_data3;

        pool_size3 << 3L;
        db_data3.push_back(pool_size3);
        TS_ASSERT_THROWS_NOTHING(db.put_device_property(admin_dev_name.c_str(), db_data3));

        DbDatum poll_prop("polled_attr");
        vector <string> poll_param;
        poll_param.push_back("PollLong_attr");
        poll_param.push_back("1000");
        poll_prop << poll_param;
        DbData db_poll;
        db_poll.push_back(poll_prop);

        TS_ASSERT_THROWS_NOTHING(db.put_device_property(new_dev1_th3.c_str(), db_poll));
        CxxTest::TangoPrinter::restore_set("reset_device_server");

        auto admin_dev = new DeviceProxy(admin_dev_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev->command_inout("RestartServer"));

        this_thread::sleep_for(chrono::seconds{5});

// Check new pool conf

        DeviceData dx;

        DeviceProxy dev{device_name};
        TS_ASSERT_THROWS_NOTHING(dx = dev.command_inout("PollingPoolTst"));

        vector <string> new_polling_pool_conf{};
        dx >> new_polling_pool_conf;

        TS_ASSERT(new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 2);

        vector <string> polled_devs;
        split_string(new_polling_pool_conf[2], ',', polled_devs);

        auto new_nb_polled_devs = polled_devs.size();
        TS_ASSERT(new_nb_polled_devs == 1);

        auto iter = find(polled_devs.begin(), polled_devs.end(), new_dev1_th3);
        TS_ASSERT(iter != polled_devs.end());
    }

    void test_delete_1_device_to_check_automatic_polling_pool_reconfiguration(void) {
        Database db{};
        TS_ASSERT_THROWS_NOTHING(db.delete_device(new_dev1_th3));

        auto admin_dev = new DeviceProxy(admin_dev_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev->command_inout("RestartServer"));

        this_thread::sleep_for(chrono::seconds{5});

// Check pool conf

        DeviceData dv;

        DeviceProxy dev{device_name};
        TS_ASSERT_THROWS_NOTHING(dv = dev.command_inout("PollingPoolTst"));

        vector <string> new_polling_pool_conf;
        dv >> new_polling_pool_conf;

        TS_ASSERT(new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 1);

        vector <string> polled_devs;
        split_string(new_polling_pool_conf[1], ',', polled_devs);
        auto new_nb_polled_devs = polled_devs.size();

        TS_ASSERT(new_nb_polled_devs == 2);

        auto iter = find(polled_devs.begin(), polled_devs.end(), new_dev1_th2);
        TS_ASSERT(iter != polled_devs.end());

        iter = find(polled_devs.begin(), polled_devs.end(), new_dev2_th2);
        TS_ASSERT(iter != polled_devs.end());
    }

    void reset_device_server() {
        Database db{};

        del_device_no_error(db, new_dev);
        del_device_no_error(db, new_dev1_th2);
        del_device_no_error(db, new_dev1_th3);
        del_device_no_error(db, new_dev2_th2);

        DbDatum pool_conf("polling_threads_pool_conf");
        DbData db_data;

        pool_conf << ref_polling_pool_conf;
        db_data.push_back(pool_conf);
        db.put_device_property(admin_dev_name.c_str(), db_data);

        DbDatum del_prop("polling_threads_pool_size");
        db_data.clear();
        db_data.push_back(del_prop);
        db.delete_device_property(admin_dev_name.c_str(), db_data);

        auto admin_dev = new DeviceProxy(admin_dev_name);
        admin_dev->command_inout("RestartServer");

        this_thread::sleep_for(chrono::seconds{5});
    }
};

void split_string(string &the_str, char delim, vector <string> &splitted_str) {
    string::size_type pos, start;
    splitted_str.clear();

    start = 0;
    while ((pos = the_str.find(delim, start)) != string::npos) {
        splitted_str.push_back(the_str.substr(start, pos - start));
        start = pos + 1;
    }

    splitted_str.push_back(the_str.substr(start));
}


void stop_poll_att_no_except(DeviceProxy *dev, const char *att_name) {
    try {
        dev->stop_poll_attribute(att_name);
    }
    catch (Tango::DevFailed &) {}
    catch (CORBA::Exception &e) {
        Except::print_exception(e);
        exit(-1);
    }
}

void stop_poll_cmd_no_except(DeviceProxy *dev, const char *cmd_name) {
    try {
        dev->stop_poll_command(cmd_name);
    }
    catch (Tango::DevFailed &) {}
    catch (CORBA::Exception &e) {
        Except::print_exception(e);
        exit(-1);
    }
}

void del_device_no_error(Database &db, string& d_name) {
    try {
        db.delete_device(d_name.c_str());
    }
    catch (DevFailed &) {}
}


#undef cout
#endif // PollTestSuite_h
