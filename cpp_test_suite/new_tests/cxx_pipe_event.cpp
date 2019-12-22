//
// Created by ingvord on 22.08.2019.
//
#ifndef PipeEventTestSuite__loop_h
#define PipeEventTestSuite__loop_h

#include "compare_test.h"
#include "cxx_common.h"
#include <mutex>
#include <condition_variable>


#define coutv	if (outer->verbose == true) cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME PipeEventTestSuite__loop



class PipeEventTestSuite__loop: public CxxTest::TestSuite
{
protected:
    std::mutex m;
    std::condition_variable cv;
    DeviceProxy *device1;
    int cb_executed = 0;
    int cb_err = 0;

    bool verbose;




public:
    SUITE_NAME()
    {
//
// Arguments check -------------------------------------------------
//

        auto device1_name = CxxTest::TangoPrinter::get_param("device1");

        verbose = CxxTest::TangoPrinter::is_param_set("verbose");

        CxxTest::TangoPrinter::get_param_opt("loop");
        CxxTest::TangoPrinter::get_param_opt("suiteloop");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try
        {
            device1 = new DeviceProxy(device1_name);
            device1->ping();
        }
        catch (CORBA::Exception &e)
        {
            Except::print_exception(e);
            exit(-1);
        }
    }

    virtual ~SUITE_NAME()
    {
        delete device1;
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

// Test Tango device server signal features

    void test_pipe_event__loop(void)
    {
        cb_executed = 0;
        cb_err = 0;

        class EventCallBack : public Tango::CallBack
        {
        public:
            EventCallBack(PipeEventTestSuite__loop* outer):
            root_blob_name(""),
            nb_data(0),
            outer(outer)
            {}
            std::string root_blob_name;
            size_t	nb_data;
            PipeEventTestSuite__loop* outer;
            void push_event(Tango::PipeEventData* event_data){
                {
                    std::lock_guard<std::mutex> lk(outer->m);
                    outer->cb_executed++;


                    try
                    {
                        coutv << "EventCallBack::push_event(): called pipe " << event_data->pipe_name << " event "
                              << event_data->event << "\n";
                        if (!event_data->err)
                        {
                            coutv << "Received pipe event for pipe " << event_data->pipe_name << std::endl;
//			coutv << *(event_data->pipe_value) << std::endl;
                            root_blob_name = event_data->pipe_value->get_root_blob_name();

                            if (root_blob_name == "PipeEventCase4")
                            {
                                std::vector<Tango::DevLong> v_dl;
                                (*(event_data->pipe_value))["Martes"] >> v_dl;
                                nb_data = v_dl.size();
                            }
                        }
                        else
                        {
                            coutv << "Error sent to callback" << std::endl;
//			Tango::Except::print_error_stack(event_data->errors);
                        }
                }
                catch (...)
                {
                    coutv << "EventCallBack::push_event(): could not extract data !\n";
                }
            }

            outer->cv.notify_all();
        }
    };

        EventCallBack cb(this);

//
// subscribe to a pipe event
//

        int eve_id1 = device1->subscribe_event("RWPipe",Tango::PIPE_EVENT,&cb);

//
// The callback should have been executed once
//
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }

        TS_ASSERT_EQUALS (1, cb_executed);

        cout << "   subscribe_event --> OK" << std::endl;

//
// Ask device to push a pipe event
//

        Tango::DevShort ev_type = 0;
        Tango::DeviceData dd;
        dd << ev_type;

        device1->command_inout("PushPipeEvent",dd);

        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }

//
// The callback should have been executed
//

        TS_ASSERT_EQUALS (2, cb_executed);
        TS_ASSERT_EQUALS ("PipeEventCase0", cb.root_blob_name);

//
// Ask device to push a pipe event with another data
//

        ev_type = 1;
        dd << ev_type;

        device1->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }


        TS_ASSERT_EQUALS (3, cb_executed);
        TS_ASSERT_EQUALS ("PipeEventCase1", cb.root_blob_name);

        cout << "   received event --> OK" << std::endl;

//
// Ask device to push a pipe event when date is specified
//

        ev_type = 2;
        dd << ev_type;

        device1->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }

        TS_ASSERT_EQUALS (4, cb_executed) ;
        TS_ASSERT_EQUALS ("PipeEventCase2", cb.root_blob_name);

        cout << "   received event (with specified date) --> OK" << std::endl;

//
// Ask device to push a pipe event with error
//

        ev_type = 3;
        dd << ev_type;

        device1->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//

        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }
        TS_ASSERT_EQUALS (5, cb_executed);

        cout << "   received event (with error) --> OK" << std::endl;

//
// Ask device to push a pipe event with enough data to trigger a no copy event sending
//

        ev_type = 4;
        dd << ev_type;

        device1->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }

        TS_ASSERT_EQUALS (6, cb_executed);
        TS_ASSERT_EQUALS ("PipeEventCase4", cb.root_blob_name);
        TS_ASSERT_EQUALS (3000, cb.nb_data);

        cout << "   received event (no copy sending) --> OK" << std::endl;


//
// unsubscribe to the event
//

        device1->unsubscribe_event(eve_id1);

        cout << "   unsubscribe_event --> OK" << std::endl;

//
// subscribe to a another pipe
//

        DeviceData d_in;
        d_in << (short)9;
        device1->command_inout("SetPipeOutput",d_in);

        eve_id1 = device1->subscribe_event("RPipe",Tango::PIPE_EVENT,&cb);

        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }

        TS_ASSERT_EQUALS (8, cb_executed);

        DevicePipe pipe_data = device1->read_pipe("rPipe");

        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait_for(lk, std::chrono::milliseconds(1000));
        }
        TS_ASSERT_EQUALS (9, cb_executed);

        device1->unsubscribe_event(eve_id1);

        cout << "   read_pipe which trigger a push_pipe_event --> OK" << std::endl;
    }
};
#undef cout
#endif // PipeEventTestSuite__loop_h

