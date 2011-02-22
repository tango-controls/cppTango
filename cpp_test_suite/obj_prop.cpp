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

		string str("my_obj");
			
//
// Try to put object properties
//

		DbData da;
		DbDatum speed("speed"), acceleration("acceleration");
		speed << (long)22;
		acceleration << (long)123;
		da.push_back(speed);
		da.push_back(acceleration);
		
		dbase->put_property(str,da);
		
		cout << "   Put object property --> OK" << endl;
		
//
// Get prop. value
//

		DbData db;
		db.push_back(DbDatum("speed"));
		db.push_back(DbDatum("acceleration"));
		
		dbase->get_property(str,db);
		
		long lg1,lg2;
		db[0] >> lg1;
		db[1] >> lg2;
		
		assert ( lg1 == 22 );
		assert ( lg2 == 123 );
		
		cout << "   Get object property --> OK" << endl;
		
//
// Update property
//

		DbData dc;
		DbDatum speeda("speed");
		speeda << (long)44;
		dc.push_back(speeda);
		
		dbase->put_property(str,dc);
		
		DbData dd;
		dd.push_back(DbDatum("speed"));
		
		dbase->get_property(str,dd);
		
		long lg10;
		dd[0] >> lg10;
		
		assert ( lg10 == 44 );
		
		cout << "   Update object property --> OK" << endl;
		
//
// Delete prop
//

		DbData de;
		de.push_back(DbDatum("speed"));
		de.push_back(DbDatum("acceleration"));
		
		dbase->delete_property(str,de);

		DbData df;
		df.push_back(DbDatum("speed"));
		df.push_back(DbDatum("acceleration"));
		
		dbase->get_property(str,df);

		assert( df[0].is_empty() == true );
		assert( df[1].is_empty() == true );
				
		cout << "   Delete object property --> OK" << endl;
		
//
// Try to put object properties
//

		DbData da_v;
		DbDatum speed_v("vect");
		vector<double> vf;
		vf.push_back(1.2345);
		vf.push_back(5.0);
		vf.push_back(4.5678);
		vf.push_back(2.0);
		speed_v << vf;
		da_v.push_back(speed_v);
		
		dbase->put_property(str,da_v);
		
		cout << "   Put object property (for vectors) --> OK" << endl;

//
// Get prop. value
//

		DbData db_v1;
		db_v1.push_back(DbDatum("vect"));
		
		dbase->get_property(str,db_v1);
		
		vector<double> vf1;
		db_v1[0] >> vf1;

		assert ( vf1[0] == 1.2345 );
		assert ( vf1[1] == 5 );
		assert ( vf1[2] == 4.5678 );
		assert ( vf1[3] == 2);
		
		cout << "   Get object property (for vectors) --> OK" << endl;

//
// Delete prop
//

		DbData de_v;
		de_v.push_back(DbDatum("vect"));
		
		dbase->delete_property(str,de_v);

		DbData df_v;
		df_v.push_back(DbDatum("vect"));
		
		dbase->get_property(str,df_v);

		assert( df_v[0].is_empty() == true );
				
		cout << "   Delete object property (for vectors) --> OK" << endl;
										
	}
	
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	delete dbase;
	return 0;
}
