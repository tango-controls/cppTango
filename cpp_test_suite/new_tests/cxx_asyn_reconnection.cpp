#ifndef AsynReconnectionTestSuite_h
#define AsynReconnectionTestSuite_h

#include "cxx_common.h"

#define    coutv    if (verbose == true) cout

#undef SUITE_NAME
#define SUITE_NAME AsynReconnectionTestSuite

class AsynReconnectionTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;
	string device1_name;
	string device1_instance_name;
	bool verbose;

public:
	SUITE_NAME() :
	device1_instance_name{"test"} //TODO pass via cl
	{

//
// Arguments check -------------------------------------------------
//

		// locally defined (test suite scope) mandatory parameters
		//localparam = CxxTest::TangoPrinter::get_param_loc("localparam","description of what localparam is");

		// predefined mandatory parameters
		device1_name = CxxTest::TangoPrinter::get_param("device1");

		// predefined optional parameters
		//CxxTest::TangoPrinter::get_param_opt("loop"); // loop parameter is then managed by the CXX framework itself
		// or
		verbose = CxxTest::TangoPrinter::is_param_opt_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
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

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before Server_Killed is restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("Server_Killed"))
		{
			try
			{
                CxxTest::TangoPrinter::start_server(device1_instance_name);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// delete dynamically allocated objects
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

// Test TestNormalWriteAttributeAsync

	void test_TestNormalWriteAttributeAsynch()
	{
		try
		{
			// Write one attribute
			long id;
			DeviceAttribute send;

			send.set_name("attr_asyn_write");
			DevLong lg = 222;
			TS_ASSERT_THROWS_NOTHING(send << lg);

			TS_ASSERT_THROWS_NOTHING(id = device1->write_attribute_asynch(send));

			bool finish = false;
			long nb_not_arrived = 0;
			while (finish == false)
			{
				try
				{
					device1->write_attribute_reply(id);
					finish = true;
				}
				catch (AsynReplyNotArrived&)
				{
					finish = false;
					coutv << "Attribute not yet written" << endl;
					nb_not_arrived++;
				}
				if (finish == false)
					Tango_sleep(1);
			}

			assert(nb_not_arrived >= 1);

			cout << "   Asynchronous write_attribute in polling mode --> OK" << endl;
		}
		catch(const Tango::DevFailed &e)
		{
			Tango::Except::print_exception(e);
			exit(-1);
		}
		catch (CORBA::Exception &ex)
		{
			Except::print_exception(ex);
			exit(-1);
		}
	}


// Test TestWriteAttributeAsynchAfterReconnection

    void test_TestWriteAttributeAsynchAfterReconnection()
    {
        CxxTest::TangoPrinter::kill_server();
        CxxTest::TangoPrinter::restore_set("Server_Killed");
        Tango_sleep(1);
        CxxTest::TangoPrinter::start_server(device1_instance_name);
        Tango_sleep(1);
        CxxTest::TangoPrinter::restore_unset("Server_Killed");
        try
        {
			// Write one attribute
            long id;
            DeviceAttribute send;

            send.set_name("attr_asyn_write");
            DevLong lg = 444;
            TS_ASSERT_THROWS_NOTHING(send << lg);

            TS_ASSERT_THROWS_NOTHING(id = device1->write_attribute_asynch(send));

            bool finish = false;
            long nb_not_arrived = 0;
            while (finish == false)
            {
                try
                {
                    device1->write_attribute_reply(id);
                    finish = true;
                }
                catch (AsynReplyNotArrived&)
                {
                    finish = false;
                    coutv << "Attribute not yet written" << endl;
                    nb_not_arrived++;
                }
                if (finish == false)
                    Tango_sleep(1);
            }

            assert(nb_not_arrived >= 1);

            // Read the attribute
			long read_id;
			DeviceAttribute *received;

			read_id = device1->read_attribute_asynch("attr_asyn_write");

			finish = false;
			nb_not_arrived = 0;
			while (finish == false)
			{
				try
				{
					received = device1->read_attribute_reply(read_id);
					Tango::DevLong val;
					*received >> val;
					coutv << "attr_asyn_write attribute value = " << val << endl;
					assert( val == 444 );
					finish = true;
				}
				catch (AsynReplyNotArrived&)
				{
					finish = false;
					coutv << "Attribute not yet read" << endl;
					nb_not_arrived++;
				}
				if (finish == false)
					Tango_sleep(1);
			}
			delete received;

			assert ( nb_not_arrived >= 1);

			coutv << "   Asynchronous read_attribute in polling mode --> OK" << endl;

            cout << "   Asynchronous write_attribute in polling mode after reconnection--> OK" << endl;
        }
        catch(const Tango::DevFailed &e)
        {
            Tango::Except::print_exception(e);
            exit(-1);
        }
        catch (CORBA::Exception &ex)
        {
            Except::print_exception(ex);
            exit(-1);
        }
    }
};
#undef cout
#endif // AsynReconnectionTestSuite_h
