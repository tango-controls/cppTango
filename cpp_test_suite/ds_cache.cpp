/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	BASE	68
#define	coutv	if (verbose == true) cout
bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{

	if (argc == 2)
	{
		if (strcmp(argv[1],"-v") == 0)
			verbose = true;
	}

	try
	{

//
// Compute cache size
//

		Database *db = new Database();
		string obj("CtrlSystem");
		string wild("*");

		DbDatum d = db->get_object_property_list(obj,wild);
		vector<string> vs_obj;
		d >> vs_obj;
		int nb_prop = vs_obj.size();

		DbData dd;
		int loop;

		for (loop = 0;loop < nb_prop;loop++)
		{
			dd.push_back(DbDatum(vs_obj[loop]));
		}
		db->get_property(obj,dd);

		int prop_elt = 0;
		for (loop = 0;loop < nb_prop;loop++)
		{
			prop_elt = prop_elt + dd[loop].size();
		}
		int cache_size = BASE + 2 + (2 * nb_prop) + prop_elt + 8;
		
//
// Try to create a DS cache
//

		string ds_name("DsCache/test");
		string h_name("pcantares.esrf.fr");
		DbServerCache *dsc = new DbServerCache(db,ds_name,h_name);	
		
		cout << "   Filling db cache --> OK" << endl;

//
// Check cache size
//

		int ca_size = dsc->get_data_nb();
		coutv << "Cache size = " << ca_size << endl;
		assert(ca_size == cache_size);

		cout << "   Db cache size --> OK" << endl;

//
// Check indexes in cache
//

		const DbServerCache::EltIdx &imp_data = dsc->get_imp_dat();
		coutv << "Adm dev import data between " << imp_data.first_idx << " and " << imp_data.last_idx << endl;
		assert(imp_data.first_idx == 0);
		assert(imp_data.last_idx == 7);
		
		const DbServerCache::EltIdx &notifd_eve = dsc->get_imp_notifd_event();
		coutv << "Notifd import event data between " << notifd_eve.first_idx << " and " << notifd_eve.last_idx << endl;
		assert(notifd_eve.first_idx == 8);
		assert(notifd_eve.last_idx == 13);

		const DbServerCache::EltIdx &ds_eve = dsc->get_imp_adm_event();
		coutv << "DS import event data between " << ds_eve.first_idx << " and " << ds_eve.last_idx << endl;
		assert(ds_eve.first_idx == 14);
		assert(ds_eve.last_idx == 15);

		const DbServerCache::PropEltIdx &DServer_prop = dsc->get_DServer_class_prop();
		coutv << "DServer class prop data between " << DServer_prop.first_idx << " and " << DServer_prop.last_idx << endl;
		assert(DServer_prop.first_idx == 16);
		assert(DServer_prop.last_idx == 28);

		const DbServerCache::PropEltIdx &Default_prop = dsc->get_Default_prop();
		coutv << "Default prop data between " << Default_prop.first_idx << " and " << Default_prop.last_idx << endl;
		assert(Default_prop.first_idx == 29);
		assert(Default_prop.last_idx == 30);

		const DbServerCache::PropEltIdx &adm_prop = dsc->get_adm_dev_prop();
		coutv << "Admin dev prop data between " << adm_prop.first_idx << " and " << adm_prop.last_idx << endl;
		assert(adm_prop.first_idx == 31);
		assert(adm_prop.last_idx == 32);

		int class_nb = dsc->get_class_nb();
		coutv << "class nb = " << class_nb << endl;
		assert(class_nb == 2);

		const DbServerCache::ClassEltIdx *class_elt = dsc->get_classes_elt();
		coutv << "Class prop data between " << class_elt->class_prop.first_idx << " and " << class_elt->class_prop.last_idx << endl;
		assert(class_elt->class_prop.first_idx == 35);
		assert(class_elt->class_prop.last_idx == 36);

		coutv << "Class attribute prop data between " << class_elt->class_att_prop.first_idx << " and " << class_elt->class_att_prop.last_idx << endl;
		assert(class_elt->class_att_prop.first_idx == 37);
		assert(class_elt->class_att_prop.last_idx == 38);

		assert(class_elt->dev_nb == 2);

		const DbServerCache::PropEltIdx &ctrl_prop = dsc->get_ctrl_serv_prop();
		coutv << "Control system object prop data between " << ctrl_prop.first_idx << " and " << ctrl_prop.last_idx << endl;
		assert(ctrl_prop.first_idx == BASE);
		assert(ctrl_prop.last_idx == (BASE + 2 + (2 * nb_prop) + prop_elt) - 1);

		cout << "   Indexes in db cache --> OK" << endl;

//
// Get some data from cache
//

		string cl_name("CacheTest1");

		Tango::DbDatum db_data = db->get_device_name(ds_name,cl_name,dsc);

		vector<string> vs;
		db_data >> vs;
		assert(vs.size() == 2);

		cout << "   Getting device name list from cache --> OK" << endl;

//
// Get some device property from cache
//

		string dev_name("test/cache1/2");
		Tango::DbData db_prop;
		db_prop.push_back(DbDatum("tst_property"));

		db->get_device_property(dev_name,db_prop,dsc);

		db_prop[0] >> vs;

		assert(vs.size() == 1);
		assert(vs[0] == "25");

		cout << "   Getting device property from cache --> OK" << endl;

//
// Get class prop
//

		cl_name = "DServer";
		db_prop.clear();
		db_prop.push_back(DbDatum("AllowedAccessCmd"));

		db->get_class_property(cl_name,db_prop,dsc);

		db_prop[0] >> vs;

		coutv << "Prop nb = " << vs.size() << ", first value = " << vs[0] << endl;
		assert(vs.size() == 9);
		assert(vs[0] == "QueryClass");

		cout << "   Getting class property --> OK" << endl;


		delete db;
		delete dsc;										
	}
	
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	return 0;
}
