/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	Database *dbase = new Database();

	try
	{

		string dev_name("test/prop/list");
			
//
// Put some device properties
//

		DbData da;
		DbDatum speed("speed"), acceleration("acceleration");
		DbDatum base_speed("base_speed"), deceleration("deceleration");
		speed << (long)22;
		acceleration << (long)123;
		base_speed << (long)55;
		deceleration << (long)456;
		da.push_back(speed);
		da.push_back(acceleration);
		da.push_back(base_speed);
		da.push_back(deceleration);
		
		dbase->put_device_property(dev_name,da);
		
//
// Get prop. list
//

		DbDatum db;
		string wild("*");

		db = dbase->get_device_property_list(dev_name,wild);
		vector<string> prop_list;
		db >> prop_list;
//		cout << "Prop number = " << prop_list.size() << endl;
//		for (unsigned long l = 0;l < prop_list.size();l++)
//			cout << "Prop_name = " << prop_list[l] << endl;
		assert (prop_list.size() == 4);
		assert (strcmp(prop_list[0].c_str(),"acceleration") == 0);
		assert (strcmp(prop_list[1].c_str(),"base_speed") == 0);
		assert (strcmp(prop_list[2].c_str(),"deceleration") == 0);
		assert (strcmp(prop_list[3].c_str(),"speed") == 0);

		wild = "a*";
		db = dbase->get_device_property_list(dev_name,wild);
		db >> prop_list;
//		cout << "Prop number = " << prop_list.size() << endl;
//		for (unsigned long l = 0;l < prop_list.size();l++)
//			cout << "Prop_name = " << prop_list[l] << endl;
		assert (prop_list.size() == 1);
		assert (strcmp(prop_list[0].c_str(),"acceleration") == 0);

		cout << "   Get device property list --> OK" << endl;
		
//
// Delete property in DB
//

		dbase->delete_device_property(dev_name,da);
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	delete dbase;
	return 0;
}
