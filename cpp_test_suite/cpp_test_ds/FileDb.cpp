#include <tango.h>
#include <DevTest.h>
#include <assert.h>

#ifdef WIN32
#include <sys/timeb.h>
#include <process.h>
#else
#include <sys/time.h>
#endif


void DevTest::FileDb()
{	  
    	cout << "[DevTest::FileDb] received" << endl;
	
//
// First DbInfo call
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::Database *db = tg->get_database();
	string db_inf = db->get_info();
	cout << db_inf << endl;
	
//
// Test class property
//

	Tango::DbClass db_class("DevTest",db);
	cout << "   DbClass object created --> OK" << endl;
	
	Tango::DbData db_dat_put,db_dat_get,db_dat_del;
	Tango::DbDatum cl_prop("ClassTest");
	db_dat_get.push_back(cl_prop);
	db_class.get_property(db_dat_get);
	
	assert (db_dat_get[0].is_empty() == true);
	cout << "   Property not defined --> OK" << endl;
	db_dat_get.clear();

	cl_prop << "Hello girl";
	db_dat_put.push_back(cl_prop);
	
	db_class.put_property(db_dat_put);
	cout << "   Put class property --> OK" << endl;
	
	Tango::DbDatum cl_prop_get("ClassTest");
	db_dat_get.push_back(cl_prop_get);
	
	db_class.get_property(db_dat_get);
	assert (db_dat_get[0].is_empty() == false);
	string pr;
	db_dat_get[0] >> pr;
	assert(pr == "Hello girl");
	cout << "   Get class property --> OK" << endl;
	db_dat_get.clear();
	
	Tango::DbDatum cl_prop_del("ClassTest");
	db_dat_del.push_back(cl_prop_del);
	
	db_class.delete_property(db_dat_del);
	
	db_dat_get.push_back(cl_prop_get);
	db_class.get_property(db_dat_get);
	assert (db_dat_get[0].is_empty() == true);
	
	cout << "   Delete class property --> OK" << endl;	

//
// Test device property
//
	
	Tango::DbData dev_dat_put,dev_dat_get,dev_dat_del;
	Tango::DbDatum dev_prop("DeviceTest");
	Tango::DbDevice *db_d = get_db_device();
	
	dev_dat_get.push_back(dev_prop);
	db_d->get_property(dev_dat_get);
	
	assert (dev_dat_get[0].is_empty() == true);
	cout << endl;
	cout << "   Device property not defined --> OK" << endl;
	dev_dat_get.clear();
	
	dev_prop << "Hello women";
	dev_dat_put.push_back(dev_prop);
	
	db_d->put_property(dev_dat_put);
	cout << "   Put device property --> OK" << endl;
	
	Tango::DbDatum dev_prop_get("DeviceTest");
	dev_dat_get.push_back(dev_prop_get);
	
	db_d->get_property(dev_dat_get);
	assert (dev_dat_get[0].is_empty() == false);
	dev_dat_get[0] >> pr;
	assert (pr == "Hello women");
	cout << "   Get device property --> OK" << endl;
	dev_dat_get.clear();
	
	Tango::DbDatum dev_prop_del("DeviceTest");
	dev_dat_del.push_back(dev_prop_del);
	
	db_d->delete_property(dev_dat_del);
	
	dev_dat_get.push_back(cl_prop_get);
	db_d->get_property(db_dat_get);
	assert (dev_dat_get[0].is_empty() == true);
	
	cout << "   Delete device property --> OK" << endl;


//
// Test class attribute property
//

	cout << endl;
	Tango::DbData cl_att_put,cl_att_get,cl_att_del;
	Tango::DbDatum cl_att_prop("ClassTest");
	cl_att_get.push_back(cl_att_prop);
	db_class.get_property(cl_att_get);
	
	assert (cl_att_get[0].is_empty() == true);
	cout << "   Class attribute property not defined --> OK" << endl;
	cl_att_get.clear();
	
	cl_att_prop << "You look nice";
	cl_att_put.push_back(cl_att_prop);
	
	db_class.put_property(cl_att_put);
	cout << "   Put class attribute property --> OK" << endl;
	
	Tango::DbDatum cl_att_prop_get("ClassTest");
	cl_att_get.push_back(cl_att_prop_get);
	
	db_class.get_property(cl_att_get);
	assert (cl_att_get[0].is_empty() == false);
	cl_att_get[0] >> pr;
	assert( pr == "You look nice");
	cout << "   Get class attribute property --> OK" << endl;
	cl_att_get.clear();
	
	Tango::DbDatum cl_att_prop_del("ClassTest");
	cl_att_del.push_back(cl_att_prop_del);
	
	db_class.delete_property(cl_att_del);
	
	cl_att_get.push_back(cl_att_prop_get);
	db_class.get_property(cl_att_get);
	assert (cl_att_get[0].is_empty() == true);
	
	cout << "   Delete class attribute property --> OK" << endl;
	
//
// Test device attribute property
//
	
	cout << endl;
	Tango::DbData dev_att_put,dev_att_get,dev_att_del;
	Tango::DbDatum dev_att_prop("DeviceTest");
	dev_att_get.push_back(dev_att_prop);
	db_d->get_property(dev_att_get);
	
	assert (dev_att_get[0].is_empty() == true);
	cout << "   Device attribute property not defined --> OK" << endl;
	dev_att_get.clear();
	
	dev_att_prop << "You look nice";
	dev_att_put.push_back(cl_att_prop);
	
	db_d->put_property(dev_att_put);
	cout << "   Put device attribute property --> OK" << endl;
	
	Tango::DbDatum dev_att_prop_get("ClassTest");
	dev_att_get.push_back(dev_att_prop_get);
	
	db_d->get_property(dev_att_get);
	assert (dev_att_get[0].is_empty() == false);
	dev_att_get[0] >> pr;
	assert( pr == "You look nice");
	cout << "   Get device attribute property --> OK" << endl;
	dev_att_get.clear();
	
	Tango::DbDatum dev_att_prop_del("ClassTest");
	dev_att_del.push_back(dev_att_prop_del);
	
	db_d->delete_property(dev_att_del);
	
	dev_att_get.push_back(dev_att_prop_get);
	db_d->get_property(dev_att_get);
	assert (dev_att_get[0].is_empty() == true);
	
	cout << "   Delete device attribute property --> OK" << endl;
}

