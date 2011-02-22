/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	AttributeProxy *attr;
	
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <attribute>" << endl;
		exit(-1);
	}

	string attribute_name = argv[1];

	try 
	{
		attr = new AttributeProxy(attribute_name);
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }

	cout << '\n' << "new AttributeProxy returned" << '\n' << endl;

	int elapsed;
	try
	{
		
// Test ping
			
		elapsed = attr->ping();

		cout << "   Ping ( " << elapsed << " us ) --> OK" << endl;
	
// Test state

		DevState sta;
		sta = attr->state();

		assert ( sta == Tango::ON);
		cout << "   State --> OK" << endl;
	
// Test status

		string str;
		str = attr->status();
	
		assert ( str == "The device is in ON state.");
		cout << "   Status --> OK" << endl;
		
// Test name

		string name;
		name = attr->name();
	
		assert ( name == "Short_attr_rw");
		cout << "   Name --> OK" << endl;

// Test get_attribute_config
		
		AttributeInfo attr_conf;
		
		attr_conf = attr->get_config();
		
		assert ( attr_conf.name == "Short_attr_rw" );
		assert ( attr_conf.data_format == SCALAR );
		assert ( attr_conf.data_type == DEV_SHORT );

		AttributeInfoEx attr_conf_ex;
		
		attr_conf_ex = attr->get_config();

		assert ( attr_conf_ex.name == "Short_attr_rw" );
		assert ( attr_conf_ex.data_format == SCALAR );
		assert ( attr_conf_ex.data_type == DEV_SHORT );
		assert ( attr_conf_ex.alarms.min_alarm == "Not specified" );
		assert ( attr_conf_ex.alarms.delta_t == "Not specified" );
		assert ( attr_conf_ex.events.ch_event.abs_change == "Not specified" );		
						
		cout << "   Get config --> OK" << endl;

// Test set_attribute_config

		AttributeInfo co = attr->get_config();

#ifdef WIN32
		int pid = _getpid();
#else
		pid_t pid = getpid();
#endif

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
		stringstream st;
#else
		strstream st;
#endif
		string s;
		st << pid;
		st >> s;
		
		co.format = s;
		
		attr->set_config(co);
		
		AttributeInfo res = attr->get_config();
		assert ( res.format == s );


		AttributeInfoEx co_ex = attr->get_config();
		co_ex.display_unit = s;
		
		attr->set_config(co_ex);
		
		AttributeInfoEx res_ex = attr->get_config();
		assert (res_ex.display_unit == s);
				
		cout << "   Set config --> OK" << endl;
		
// Test read

		DeviceAttribute da;
		da = attr->read();
		short val;
		da >> val;
		assert ( val == 66 );
		
		cout << "   Read --> OK" << endl;
		
// Test write

		val = 88;
		da << val;
		attr->write(da);
		
		DeviceAttribute da_res;
		da_res = attr->read();
		short val_res;
		da_res >> val_res;
		
		assert (val_res == 88);
		
		cout << "   Write --> OK" << endl;
		
		val = 66;
		da << val;
		attr->write(da);
		
// Test poll method

		bool pol = attr->is_polled();
		assert (pol == false);
		
		attr->poll(1000);
		
		pol = attr->is_polled();
		assert (pol == true);
		
		cout << "   poll --> OK" << endl;
		
// Test get_poll_period

		int pol_per = attr->get_poll_period();
		assert (pol_per == 1000);
		
		cout << "   get_poll_period --> OK" << endl;

// Test history
		
		Tango_sleep(3);
		vector<DeviceAttributeHistory> *hist = attr->history(2);
		
		int i;
		for (i = 0;i < 2;i++)
		{
			assert ((*hist)[i].get_name() == "Short_attr_rw");
			assert ((*hist)[i].has_failed() == false);
			val = 0;
			(*hist)[i] >> val;
			assert (val == 66);
		}
		
		cout << "   history --> OK" << endl;
		
		delete hist;
		
//  Test stop_poll

		attr->stop_poll();
		pol = attr->is_polled();
		assert (pol == false);
		
		cout << "   stop_poll --> OK" << endl;

// Test read_asynch and read_reply

		long id = attr->read_asynch();
		Tango_sleep(1);
		DeviceAttribute *da_ptr = attr->read_reply(id);
		val = 0;
		(*da_ptr) >> val;
		assert (val == 66);
		
		cout << "   read_asynch() and read_reply() --> OK" << endl;
		delete da_ptr;
		
// Test write_asynch and write_reply

		val = 99;
		da << val;
		id = attr->write_asynch(da);
		Tango_sleep(1);
		attr->write_reply(id);
		da_res = attr->read();
		val = 0;
		da_res >> val_res;
		assert (val_res == 99);
		
		val = 66;
		da << val;
		attr->write(da);
		
		cout << "   write_asynch() and write_reply() --> OK" << endl;		

// Test put property

		DbDatum dd("tst_prop");
		long lg = 1234;
		dd << lg;
		DbData in_prop;
		in_prop.push_back(dd);
		
		attr->put_property(in_prop);
				
// Test get property

		string prop_name("tst_prop");
		DbData out_prop;
		
		attr->get_property(prop_name,out_prop);
		
		long lg_res = 0;
		out_prop[0] >> lg_res;
		
		assert(lg_res == 1234);		
		cout << "   put_property() and get_property() --> OK" << endl;
		
// Test delete property

		attr->delete_property(prop_name);
		
		DbData no_prop;
		attr->get_property(prop_name,no_prop);
		assert (no_prop[0].is_empty() == true);
		
		cout << "   delete_property() --> OK" << endl;

// Test multiple get_property
		
		DbDatum dd_1("tst_prop_1");
		DbDatum dd_2("tst_prop_2");
		long lg_i = 3333;
		dd_1 << lg_i;
		lg_i = 4444;
		dd_2 << lg_i;
		DbData in_prop_m;
		in_prop_m.push_back(dd_1);
		in_prop_m.push_back(dd_2);
				
		attr->put_property(in_prop_m);		
		
		vector<string> name_list;
		name_list.push_back("tst_prop_1");
		name_list.push_back("tst_prop_10");
		name_list.push_back("tst_prop_2");
		DbData out_prop_m;
		
		attr->get_property(name_list,out_prop_m);
		
		long lg_res_2 = 0;
		long lg_res_1 = 0;
		out_prop_m[0] >> lg_res_1;
		out_prop_m[2] >> lg_res_2;
		
		assert (lg_res_1 == 3333);		
		assert (lg_res_2 == 4444);		
		assert (out_prop_m[1].is_empty() == true);
		
		DbData db_dat;
		db_dat.push_back(DbDatum("no"));
		db_dat.push_back(DbDatum("tst_prop_2"));
		db_dat.push_back(DbDatum("yes"));
		
		attr->get_property(db_dat);
		
		long lg_3;
		db_dat[1] >> lg_3;
		
		assert(db_dat[0].is_empty() == true);
		assert(db_dat[2].is_empty() == true);
		assert(lg_3 == 4444);

		cout << "   get_property() for multiple properties --> OK" << endl;

// Test deleting multiple properties

		vector<string> p_name;
		p_name.push_back("tst_prop_1");
		p_name.push_back("tst_prop_2");
		
		attr->delete_property(p_name);
		
		DbData no_prop_bis;
		attr->get_property(p_name,no_prop_bis);
		assert (no_prop_bis[0].is_empty() == true);
		assert (no_prop_bis[1].is_empty() == true);
		
		cout << "   delete_property() for multiple properties --> OK" << endl;
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	delete attr;
	
	return 0;
}
