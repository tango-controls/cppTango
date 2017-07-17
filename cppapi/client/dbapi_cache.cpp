//+=================================================================================================================
//
// file :			Deviceclass.cpp
//
// description :	C++ source code for the DbServerCache class
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

//------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerCache::DbServerCache()
//
// description:
//		Constructor of the DbServerCache class. This class manage a data cache used during the device server
//		process startup phase.
//
// arguments:
// 		in :
//			- db : The database object
//			- ds_name : The device server name (exec_name/inst_name)
//			- host : The host name
//
//------------------------------------------------------------------------------------------------------------------

DbServerCache::DbServerCache(Database *db,string &ds_name,string &host)
{

//
// Get all data from database
//

	try
	{
		received = db->fill_server_cache(ds_name,host);
	}
	catch (Tango::DevFailed &)
	{
		cout3 << "Got an exception while getting cache data from database !!" << endl;
		throw;
	}

	received.inout() >>= data_list;
	n_data = data_list->length();

//
// Extract the different blocks from the big list. First the stored procedure release nb
// The release number is stored as:
//		(major_number * 100) + minor_number
// Ex: 1.9 -> 109
//

	proc_release = 0;
	string proc_rel((*data_list)[0]);
	if (proc_rel.find("release") != string::npos)
	{
		string rel = proc_rel.substr(8);
		string::size_type pos = rel.find('.');
		if (pos != string::npos)
		{
			string maj_str = rel.substr(0,pos);
			string min_str = rel.substr(pos + 1);

			int maj = atoi(maj_str.c_str());
			int min = atoi(min_str.c_str());

			proc_release = (maj * 100) + min;
		}
	}

//
// First, the device server admin device parameters
//

	int start_idx = 0;
	int stop_idx = 0;

	if (proc_release != 0)
	{
		start_idx++;
		stop_idx++;
	}

	if (n_data == 2 || n_data == 3)
	{
		imp_adm.first_idx = start_idx;
		imp_adm.last_idx = start_idx + 1;

		ctrl_serv_prop.first_idx = -1;
		DServer_class_prop.first_idx = -1;
		Default_prop.first_idx = -1;
		adm_dev_prop.first_idx = -1;

		return;
	}

	stop_idx = start_idx + 7;
	imp_adm.first_idx = start_idx;
	imp_adm.last_idx = stop_idx;

//
// Event factory
//

	start_idx = stop_idx + 1;
	if (::strcmp((*data_list)[start_idx + 1],"Not Found") == 0)
	{
		stop_idx = stop_idx + 2;
	}
	else
	{
		stop_idx = stop_idx + 6;
	}
	imp_notifd_event.first_idx = start_idx;
	imp_notifd_event.last_idx = stop_idx;

//
// Server event
//

	start_idx = stop_idx + 1;
	if (::strcmp((*data_list)[start_idx + 1],"Not Found") == 0)
	{
		stop_idx = stop_idx + 2;
	}
	else
	{
		stop_idx = stop_idx + 6;
	}
	imp_adm_event.first_idx = start_idx;
	imp_adm_event.last_idx = stop_idx;

//
// DServer class prop
//

	prop_indexes(start_idx,stop_idx,DServer_class_prop,data_list);

//
// Default prop
//

	prop_indexes(start_idx,stop_idx,Default_prop,data_list);

//
// Admin device prop
//

	prop_indexes(start_idx,stop_idx,adm_dev_prop,data_list);

//
// Embedded classes number
//

	start_idx = stop_idx + 1;
	class_nb = ::atoi((*data_list)[start_idx + 1]);
	stop_idx = stop_idx  + 2;
	classes_idx = new ClassEltIdx [class_nb];

	for (int cl_loop = 0;cl_loop < class_nb;cl_loop++)
	{

//
// Embedded class prop
//

		prop_indexes(start_idx,stop_idx,classes_idx[cl_loop].class_prop,data_list);

//
// Embedded class attribute prop
//

		prop_att_indexes(start_idx,stop_idx,classes_idx[cl_loop].class_att_prop,data_list);

//
// Embedded class pipe prop
//

		if (proc_release >= 109)
			prop_pipe_indexes(start_idx,stop_idx,classes_idx[cl_loop].class_pipe_prop,data_list);
		else
			classes_idx[cl_loop].class_pipe_prop.atts_idx = NULL;

//
// Device list
//

		start_idx = stop_idx + 1;
		int nb_dev = ::atoi((*data_list)[start_idx + 1]);
		stop_idx = stop_idx + nb_dev + 2;

		classes_idx[cl_loop].dev_list.first_idx = start_idx;
		classes_idx[cl_loop].dev_list.last_idx = stop_idx;
		classes_idx[cl_loop].dev_nb = nb_dev;
		classes_idx[cl_loop].devs_idx = new DevEltIdx [nb_dev];

		for (int loop = 0;loop < nb_dev;loop++)
		{

//
// Device properties
//

			prop_indexes(start_idx,stop_idx,classes_idx[cl_loop].devs_idx[loop].dev_prop,data_list);

//
// Device attribute properties
//

			prop_att_indexes(start_idx,stop_idx,classes_idx[cl_loop].devs_idx[loop].dev_att_prop,data_list);

//
// Device pipe properties
//

			if (proc_release >= 109)
				prop_pipe_indexes(start_idx,stop_idx,classes_idx[cl_loop].devs_idx[loop].dev_pipe_prop,data_list);
			else
				classes_idx[cl_loop].devs_idx[loop].dev_pipe_prop.atts_idx = NULL;
		}
	}

//
// Control System Service(s) property
//

	prop_indexes(start_idx,stop_idx,ctrl_serv_prop,data_list);

//
// Tac device import info
//

    start_idx = stop_idx + 1;
    if (stop_idx == n_data)
    {
        stop_idx = -1;
    }
    else
    {
        stop_idx = n_data;
        imp_tac.first_idx = start_idx;
        imp_tac.last_idx = stop_idx;
    }

}

//------------------------------------------------------------------------------------------------------------------
//
// method:
//  	DbServerCache::import_adm_dev()
//
// description:
//		This method returns to the caller of the info necessary to import the DS admin device. The returned data
//		are the same than the one returned by the classical API
//
// return:
//		This method returns a pointer to a DevVarLongStringArray initilised with the adm device import data
//
//------------------------------------------------------------------------------------------------------------------

const DevVarLongStringArray *DbServerCache::import_adm_dev()
{
	int last_index;
	if (proc_release >= 109)
		last_index = 2;
	else
		last_index = 1;

	if (imp_adm.last_idx == last_index)
	{
		Tango::Except::throw_exception("aaa","bbb","ccc");
	}

	imp_adm_data.lvalue.length(2);
	imp_adm_data.svalue.length(5);

	imp_adm_data.lvalue[0] = ::atoi((*data_list)[imp_adm.first_idx + 5]);
	imp_adm_data.lvalue[1] = ::atoi((*data_list)[imp_adm.last_idx]);

	for (int loop = 0;loop < 5;loop++)
		imp_adm_data.svalue[loop] = CORBA::string_dup((*data_list)[imp_adm.first_idx + loop]);

	return &imp_adm_data;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::import_notifd_event()
//
// description :
//		This method returns to the caller of the info necessary to import the event associated to the notifd running
// 		on the DS host. The returned data are the same than the one returned by the classical API
//
// return :
//		This method returns a pointer to a DevVarLongStringArray initilised with the notifd event  import data
//
//------------------------------------------------------------------------------------------------------------------

const DevVarLongStringArray *DbServerCache::import_notifd_event()
{
	if (imp_notifd_event.last_idx == imp_notifd_event.first_idx + 1)
	{
		Tango::Except::throw_exception("aaa","bbb","ccc");
	}

	imp_notifd_event_data.lvalue.length(2);
	imp_notifd_event_data.svalue.length(5);

	imp_notifd_event_data.lvalue[0] = ::atoi((*data_list)[imp_notifd_event.first_idx + 4]);
	imp_notifd_event_data.lvalue[1] = ::atoi((*data_list)[imp_notifd_event.last_idx]);

	for (int loop = 0;loop < 4;loop++)
		imp_notifd_event_data.svalue[loop] = CORBA::string_dup((*data_list)[imp_notifd_event.first_idx + loop]);

	return &imp_notifd_event_data;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
// 		DbServerCache::import_adm_event()
//
// description :
//		This method returns to the caller of the info necessary to import the event associated to the DS server.
//		The returned data are the same than the one returned by the classical API
//
// return :
//		This method returns a pointer to a DevVarLongStringArray initilised with the DS event  import data
//
//-------------------------------------------------------------------------------------------------------------------


const DevVarLongStringArray *DbServerCache::import_adm_event()
{
	if (imp_adm_event.last_idx == imp_adm_event.first_idx + 1)
	{
		Tango::Except::throw_exception("aaa","bbb","ccc");
	}

	imp_adm_event_data.lvalue.length(2);
	imp_adm_event_data.svalue.length(5);

	imp_adm_event_data.lvalue[0] = ::atoi((*data_list)[imp_adm_event.first_idx + 4]);
	imp_adm_event_data.lvalue[1] = ::atoi((*data_list)[imp_notifd_event.last_idx]);

	for (int loop = 0;loop < 4;loop++)
		imp_adm_event_data.svalue[loop] = CORBA::string_dup((*data_list)[imp_adm_event.first_idx + loop]);

	return &imp_adm_event_data;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_class_property()
//
// description :
//		This method returns to the caller the wanted class properties. The returned data are the same than the one
//		returned by the classical API
//
// argument :
// 		in :
//			- in_param : The class name followed by the wanted property names
//
// return :
//		This method returns a pointer to a DevVarStringArray initilised with the class properties
//
//-------------------------------------------------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_class_property(DevVarStringArray *in_param)
{
	int ret_length = 2;

	if (TG_strcasecmp((*in_param)[0],"DServer") == 0)
	{
		ret_obj_prop.length(ret_length);
		ret_obj_prop[0] = CORBA::string_dup("DServer");

		get_obj_prop(in_param,DServer_class_prop);
	}
	else if (TG_strcasecmp((*in_param)[0],"Default") == 0)
	{
		ret_obj_prop.length(ret_length);
		ret_obj_prop[0] = CORBA::string_dup("Default");

		get_obj_prop(in_param,Default_prop);
	}
	else
	{
		ret_obj_prop.length(ret_length);
		ret_obj_prop[0] = CORBA::string_dup((*in_param)[0]);

		int cl_idx = find_class((*in_param)[0]);
		if (cl_idx != -1)
		{
			get_obj_prop(in_param,classes_idx[cl_idx].class_prop);
		}
		else
		{
			TangoSys_OMemStream o;
			o << "Class " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_ClassNotFoundInCache",o.str(),
										   (const char *)"DbServerCache::get_dev_property");
		}
	}

	return &ret_obj_prop;
}

//------------------------------------------------------------------------------------------------------------------
//
// method:
//  	DbServerCache::get_obj_prop()
//
// description :
//		This method searches the index(es) of the object wanted property within all the data returne by the DB server.
// 		A structure will be initialised with the indexes
//
// argument :
// 		in :
//			- in_param : The object name followed by the wanted property names
//			- dev_prop : Boolean set to true in case of device property. For device property, a undefined prop is
//						 returned with a value set to " " !
//  	out :
//			- obj : Reference to the structure which will be initialsed by this method
//
//------------------------------------------------------------------------------------------------------------------

void DbServerCache::get_obj_prop(DevVarStringArray *in_param,PropEltIdx &obj,bool dev_prop)
{
	int ret_length = 2;
	int found_prop = 0;
	char n_prop_str[256];

	int nb_wanted_prop = in_param->length() - 1;
	int obj_prop = obj.prop_nb;

	for (int loop = 0;loop < nb_wanted_prop;loop++)
	{
		int lo;
		for (lo = 0;lo < obj_prop * 2;lo = lo + 2)
		{
			if (TG_strcasecmp((*in_param)[loop + 1],(*data_list)[obj.props_idx[lo]]) == 0)
			{
				int old_ret_length = ret_length;
				int nb_elt = obj.props_idx[lo + 1];

				ret_length = ret_length + 2 + nb_elt;

				ret_obj_prop.length(ret_length);
				ret_obj_prop[old_ret_length] = CORBA::string_dup((*in_param)[loop + 1]);
				ret_obj_prop[old_ret_length + 1] = CORBA::string_dup((*data_list)[obj.props_idx[lo] + 1]);

				for (int k = 0;k < nb_elt;k++)
				{
					ret_obj_prop[old_ret_length + 2 + k] = CORBA::string_dup((*data_list)[obj.props_idx[lo] + 2 + k]);
				}
				found_prop++;
				break;
			}
		}
		if (lo >= (obj_prop * 2))
		{
			int old_length = ret_length;
			ret_length = ret_length + 2;
			if (dev_prop == true)
				ret_length++;

			ret_obj_prop.length(ret_length);
			ret_obj_prop[old_length] = CORBA::string_dup((*in_param)[loop + 1]);
			ret_obj_prop[old_length + 1] = CORBA::string_dup("0");
			if (dev_prop == true)
				ret_obj_prop[old_length + 2] = CORBA::string_dup(" ");
			found_prop++;

		}
	}
	::sprintf(n_prop_str,"%d",found_prop);
	ret_obj_prop[1] = CORBA::string_dup(n_prop_str);

//	cout4 << "DbCache --> Data returned for a get_obj_property for object " << (*in_param)[0] << endl;
//	for (unsigned int ll=0;ll< ret_obj_prop.length();ll++)
//		cout4 << "    DbCache --> Returned string = " << ret_obj_prop[ll] << endl;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_dev_property()
//
// description :
//		This method returns to the caller the wanted device properties The returned data are the same than the one
//		returned by the classical API
//
// argument :
// 		in :
//			- in_param : The device name followed by the wanted property names
//
// return :
//		This method returns a pointer to a DevVarStringArray initilised with the device properties
//
//-------------------------------------------------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_dev_property(DevVarStringArray *in_param)
{
	int ret_length = 2;

//
// There is a special case for the dserver admin device
//

	if (TG_strncasecmp((*in_param)[0],"dserver/",8) == 0)
	{
	    if (adm_dev_prop.first_idx == -1)
	    {
			TangoSys_OMemStream o;
			o << "Device " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
										   (const char *)"DbServerCache::get_dev_property");
	    }
		ret_obj_prop.length(ret_length);
		ret_obj_prop[0] = CORBA::string_dup((*in_param)[0]);

		get_obj_prop(in_param,adm_dev_prop,true);
	}
	else
	{
		int class_ind,dev_ind;
		int res = find_dev_att((*in_param)[0],class_ind,dev_ind);
		if (res == -1)
		{
			TangoSys_OMemStream o;
			o << "Device " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
										   (const char *)"DbServerCache::get_dev_property");
		}
		else
		{
			ret_obj_prop.length(ret_length);
			ret_obj_prop[0] = CORBA::string_dup((*in_param)[0]);

			get_obj_prop(in_param,classes_idx[class_ind].devs_idx[dev_ind].dev_prop,true);
		}
	}

	return &ret_obj_prop;
}


//-----------------------------------------------------------------------------
//
//  DbServerCache::get_dev_list()
//
//	This method returns to the list of devices for a specified class embedded
//  within the device server process
//	The returned data are the same than the one returned by
//  the classical API
//
// 	in :	in_param : The device server name followed by the class name
//
//	This method returns a pointer to a DevVarStringArray initilised
//  with the device list
//-----------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_dev_list(DevVarStringArray *in_param)
{
	int cl_idx = find_class((*in_param)[1]);
	if (cl_idx != -1)
	{
		ret_dev_list.length(classes_idx[cl_idx].dev_nb);
		for (int loop = 0;loop < classes_idx[cl_idx].dev_nb;loop++)
			ret_dev_list[loop] = CORBA::string_dup((*data_list)[classes_idx[cl_idx].dev_list.first_idx + 2 + loop]);
	}
	else
		ret_dev_list.length(0);

	return &ret_dev_list;
}

//-----------------------------------------------------------------------------
//
//  DbServerCache::find_class()
//
//	This method returns the index of the class with name passed as parameter
//	into the array of classes info
//
// 	in :	cl_name : The class name
//
//	This method returns the index in the classes info array of the wanted
//  class
//-----------------------------------------------------------------------------

int DbServerCache::find_class(DevString cl_name)
{
	for (int loop = 0;loop < class_nb;loop++)
	{
		if (TG_strcasecmp((*data_list)[classes_idx[loop].class_prop.first_idx],cl_name) == 0)
			return loop;
	}
	return -1;
}

//-----------------------------------------------------------------------------
//
//  DbServerCache::get_class_att_property()
//
//	This method returns to the caller the class attribute properties for the
//	class and attributes specified in the in parameters
//	The returned data are the same than the one returned by
//  the classical API
//
// 	in :	in_param : The class name followed by the wanted
//					   attribute names
//
//	This method returns a pointer to a DevVarStringArray initilised
//  with the class attribute properties
//-----------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_class_att_property(DevVarStringArray *in_param)
{
	char n_att_str[256];

	ret_obj_att_prop.length(2);
	ret_obj_att_prop[0] = CORBA::string_dup((*in_param)[0]);

	int cl_idx = find_class((*in_param)[0]);
	if (cl_idx != -1)
	{
		int found_att = 0;

//
// The class is found
//

		int wanted_att_nb = in_param->length() - 1;
		int class_att_nb = classes_idx[cl_idx].class_att_prop.att_nb;
		for (int loop = 0;loop < wanted_att_nb;loop++)
		{
			int ll;
			for (ll = 0;ll < class_att_nb;ll++)
			{
				if (TG_strcasecmp((*in_param)[loop + 1],(*data_list)[classes_idx[cl_idx].class_att_prop.atts_idx[ll]]) == 0)
				{

//
// The attribute is found, copy all its properties
//

					int att_index = classes_idx[cl_idx].class_att_prop.atts_idx[ll];
					int nb_prop = ::atoi((*data_list)[att_index + 1]);
					int nb_elt = 0;
					int nb_to_copy = 0;
					int tmp_idx = att_index + 2;
					nb_to_copy = 2;
					for (int k = 0;k < nb_prop;k++)
					{
						nb_elt = ::atoi((*data_list)[tmp_idx + 1]);
						tmp_idx = tmp_idx + nb_elt + 2;
						nb_to_copy = nb_to_copy + 2 + nb_elt;
					}

					int old_length = ret_obj_att_prop.length();
					ret_obj_att_prop.length(old_length + nb_to_copy);
					for (int j = 0;j < nb_to_copy;j++)
						ret_obj_att_prop[old_length + j] = CORBA::string_dup((*data_list)[att_index + j]);
					found_att++;
					break;
				}
			}
			if (ll == class_att_nb)
			{
				found_att++;
				int old_length = ret_obj_att_prop.length();
				ret_obj_att_prop.length(old_length + 2);
				ret_obj_att_prop[old_length] = CORBA::string_dup((*in_param)[loop + 1]);
				ret_obj_att_prop[old_length + 1] = CORBA::string_dup("0");
			}
		}
		::sprintf(n_att_str,"%d",found_att);
		ret_obj_att_prop[1] = CORBA::string_dup(n_att_str);
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Class " << (*in_param)[0] << " not found in DB cache" << ends;

		Tango::Except::throw_exception((const char *)"DB_ClassNotFoundInCache",o.str(),
										   (const char *)"DbServerCache::get_dev_property");
	}

//	cout4 << "DbCache --> Returned data for a get_class_att_property for class " << (*in_param)[0] << endl;
//	for (unsigned int ll=0;ll< ret_obj_att_prop.length();ll++)
//		cout4 << "    DbCache --> Returned object att prop = " << ret_obj_att_prop[ll] << endl;

	return &ret_obj_att_prop;
}

//-----------------------------------------------------------------------------
//
//  DbServerCache::get_dev_att_property()
//
//	This method returns to the caller the device attribute properties for the
//	device and attributes specified in the in parameters
//	The returned data are the same than the one returned by
//  the classical API
//
// 	in :	in_param : The device name followed by the wanted
//					   attribute names
//
//	This method returns a pointer to a DevVarStringArray initialised
//  with the device attribute properties
//-----------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_dev_att_property(DevVarStringArray *in_param)
{
	int found_att = 0;
	char n_att_str[256];

	ret_obj_att_prop.length(2);
	ret_obj_att_prop[0] = CORBA::string_dup((*in_param)[0]);

	int class_ind,dev_ind;

	int ret_value = find_dev_att((*in_param)[0],class_ind,dev_ind);
	if (ret_value != -1)
	{
		int wanted_att_nb = in_param->length() - 1;
		int dev_att_nb = classes_idx[class_ind].devs_idx[dev_ind].dev_att_prop.att_nb;
		for (int loop = 0;loop < wanted_att_nb;loop++)
		{
			int ll;
			for (ll = 0;ll < dev_att_nb;ll++)
			{
				if (TG_strcasecmp((*in_param)[loop + 1],(*data_list)[classes_idx[class_ind].devs_idx[dev_ind].dev_att_prop.atts_idx[ll]]) == 0)
				{
					int att_index = classes_idx[class_ind].devs_idx[dev_ind].dev_att_prop.atts_idx[ll];
					int nb_prop = ::atoi((*data_list)[att_index + 1]);
					int nb_elt = 0;
					int nb_to_copy = 0;
					int tmp_idx = att_index + 2;
					nb_to_copy = 2;
					for (int k = 0;k < nb_prop;k++)
					{
						nb_elt = ::atoi((*data_list)[tmp_idx + 1]);
						tmp_idx = tmp_idx + nb_elt + 2;
						nb_to_copy = nb_to_copy + 2 + nb_elt;
					}

					int old_length = ret_obj_att_prop.length();
					ret_obj_att_prop.length(old_length + nb_to_copy);
					for (int j = 0;j < nb_to_copy;j++)
						ret_obj_att_prop[old_length + j] = CORBA::string_dup((*data_list)[att_index + j]);
					found_att++;
					break;
				}
			}
			if (ll == dev_att_nb)
			{
				found_att++;
				int old_length = ret_obj_att_prop.length();

				ret_obj_att_prop.length(old_length + 2);
				ret_obj_att_prop[old_length] = CORBA::string_dup((*in_param)[loop + 1]);
				ret_obj_att_prop[old_length + 1] = CORBA::string_dup("0");
			}
		}
		::sprintf(n_att_str,"%d",found_att);
		ret_obj_att_prop[1] = CORBA::string_dup(n_att_str);
	}
	else
	{
		if (TG_strncasecmp("dserver/",(*in_param)[0],8) != 0)
		{
			TangoSys_OMemStream o;
			o << "Device " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
											(const char *)"DbServerCache::get_dev_att_property");
		}
		else
		{
			::sprintf(n_att_str,"%d",found_att);
			ret_obj_att_prop[1] = CORBA::string_dup(n_att_str);
		}
	}

//	cout4 << "DbCache --> Returned data for a get_dev_att_property for device " << (*in_param)[0] << endl;
//	for (unsigned int ll=0;ll< ret_obj_att_prop.length();ll++)
//		cout4 << "    DbCache --> Returned object att prop = " << ret_obj_att_prop[ll] << endl;

	return &ret_obj_att_prop;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::find_dev_att()
//
// description :
//		This method sets the class and device index for the device with name passed in the in parameter. These indexes
// 		are indexes in the local structure arrays
//
// argument :
// 		in :
//			- dev_name : The device name
// 	 	out :
//			- class_ind : The index of the data related to the device class
//			- dev_ind : The index of the data related to the device itself
//
// return :
//		This method returns 0 if everything OK or -1 if the device is not found
//
//-------------------------------------------------------------------------------------------------------------------

int DbServerCache::find_dev_att(DevString dev_name,int &class_ind,int &dev_ind)
{
	for (int loop = 0;loop < class_nb;loop++)
	{
		for (int ll = 0;ll < classes_idx[loop].dev_nb;ll++)
		{
			if (TG_strcasecmp((*data_list)[classes_idx[loop].dev_list.first_idx + 2 + ll],dev_name) == 0)
			{
				class_ind = loop;
				dev_ind = ll;
				return 0;
			}
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method
//  	DbServerCache::find_obj()
//
//	description :
//		This method sets the class and device index for the device with name passed in the in parameter.
//		These indexes are indexes in the local structure arrays
//
// argument
// 		in :
//			- obj_name : The object name
//  	out :
//			- obj_ind : The index of the data related to the object
//
// return :
//		This method returns 0 if everything OK or -1 if the device is not found
//
//--------------------------------------------------------------------------------------------------------------------

int DbServerCache::find_obj(DevString obj_name,int &obj_ind)
{

    if (ctrl_serv_prop.first_idx == -1)
		return -1;

	if (TG_strcasecmp((*data_list)[ctrl_serv_prop.first_idx],obj_name) == 0)
	{
		obj_ind = ctrl_serv_prop.first_idx;
		return 0;
	}

	return -1;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
// 		DbServerCache::~DbServerCache()
//
// description :
//		destructor of the DbServerCache class
//
//------------------------------------------------------------------------------------------------------------------

DbServerCache::~DbServerCache()
{
    delete [] DServer_class_prop.props_idx;
    delete [] Default_prop.props_idx;
    delete [] adm_dev_prop.props_idx;
    delete [] ctrl_serv_prop.props_idx;

	for (int cl_loop = 0;cl_loop < class_nb;cl_loop++)
	{
		if (classes_idx[cl_loop].class_prop.props_idx != NULL)
			delete [] classes_idx[cl_loop].class_prop.props_idx;
		if (classes_idx[cl_loop].class_att_prop.atts_idx != NULL)
			delete [] classes_idx[cl_loop].class_att_prop.atts_idx;
		if (classes_idx[cl_loop].class_pipe_prop.atts_idx != NULL)
			delete [] classes_idx[cl_loop].class_pipe_prop.atts_idx;

		for (int dev_loop = 0;dev_loop < classes_idx[cl_loop].dev_nb;dev_loop++)
		{
			if (classes_idx[cl_loop].devs_idx[dev_loop].dev_prop.props_idx != NULL)
				delete [] classes_idx[cl_loop].devs_idx[dev_loop].dev_prop.props_idx;
			if (classes_idx[cl_loop].devs_idx[dev_loop].dev_att_prop.atts_idx != NULL)
				delete [] classes_idx[cl_loop].devs_idx[dev_loop].dev_att_prop.atts_idx;
			if (classes_idx[cl_loop].devs_idx[dev_loop].dev_pipe_prop.atts_idx != NULL)
				delete [] classes_idx[cl_loop].devs_idx[dev_loop].dev_pipe_prop.atts_idx;
		}
		delete [] classes_idx[cl_loop].devs_idx;
	}
	delete [] classes_idx;
}

//-----------------------------------------------------------------------------
//
//  DbServerCache::prop_indexes()
//
//	This method computes where is the last data related to the object
//	in the big array coming from the Db server
//
// 	args (in) :	start : The index in the array of the first data related
//					    to the object
//			    list : The string array coming from Db server
//
//  args (out) : stop : The index in the array of the last data related
//						to the object
//				 obj : Structure where all object parameters must be stored
//-----------------------------------------------------------------------------

void DbServerCache::prop_indexes(int &start,int &stop,PropEltIdx &obj,const DevVarStringArray *list)
{
	start = stop + 1;
	int nb_prop = atoi((*list)[start + 1]);
	if (nb_prop == 0)
	{
		stop = start + 1;
		obj.last_idx = stop;
		obj.first_idx = start;
		obj.prop_nb = 0;
		obj.props_idx = NULL;
		return;
	}
	stop = stop + 2;

	int id = 0;
	obj.props_idx = new int[nb_prop * 2];
	for (int loop = 0;loop < nb_prop;loop++)
	{
		obj.props_idx[id++] = stop + 1;
		int nb_elt = atoi((*list)[stop + 2]);
		obj.props_idx[id++] = nb_elt;
		stop = stop + nb_elt + 2;
	}

	obj.last_idx = stop;
	obj.first_idx = start;
	obj.prop_nb = nb_prop;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::prop_att_indexes()
//
// description :
//		This method computes where is the last data related to the object (for attribute related object) in the big
//		array coming from the Db server
//
// argument :
// 		in :
//			- start : The index in the array of the first data related to the object
//			- list : The string array coming from Db server
//
//  	out :
//			- stop : The index in the array of the last data related to the object
//			- obj : Structure where all object parameters must be stored
//
//-------------------------------------------------------------------------------------------------------------------

void DbServerCache::prop_att_indexes(int &start,int &stop,AttPropEltIdx &obj,const DevVarStringArray *list)
{
	start = stop + 1;
	int nb_att = atoi((*list)[start + 1]);
	if (nb_att == 0)
	{
		stop = start + 1;
		obj.last_idx = stop;
		obj.first_idx = start;
		obj.att_nb = 0;
		obj.atts_idx = NULL;
		return;
	}

	int id = 0;
	obj.att_nb = nb_att;
	obj.atts_idx = new int[nb_att];
	stop = start + 2;

	for (int ll = 0;ll < nb_att;ll++)
	{
		obj.atts_idx[id++] = stop;
		int nb_prop = atoi((*list)[stop + 1]);
		stop = stop + 2;

		for (int loop = 0;loop < nb_prop;loop++)
		{
			int nb_elt = atoi((*list)[stop + 1]);
			stop = stop + nb_elt + 2;
		}
		if (ll == (nb_att - 1))
			stop--;
	}

	obj.last_idx = stop;
	obj.first_idx = start;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::prop_pipe_indexes()
//
// description :
//		This method computes where is the last data related to the object (for attribute related object) in the big
//		array coming from the Db server
//
// argument :
//		in :
//			- start : The index in the array of the first data related to the object
//			- list : The string array coming from Db server
//		out :
//			- stop : The index in the array of the last data related to the object
//			- obj : Structure where all object parameters must be stored
//
//------------------------------------------------------------------------------------------------------------------

void DbServerCache::prop_pipe_indexes(int &start,int &stop,AttPropEltIdx &obj,const DevVarStringArray *list)
{
	start = stop + 1;
	int nb_att = atoi((*list)[start + 1]);
	if (nb_att == 0)
	{
		stop = start + 1;
		obj.last_idx = stop;
		obj.first_idx = start;
		obj.att_nb = 0;
		obj.atts_idx = NULL;
		return;
	}

	int id = 0;
	obj.att_nb = nb_att;
	obj.atts_idx = new int[nb_att];
	stop = start + 2;

	for (int ll = 0;ll < nb_att;ll++)
	{
		obj.atts_idx[id++] = stop;
		int nb_prop = atoi((*list)[stop + 1]);
		stop = stop + 2;

		for (int loop = 0;loop < nb_prop;loop++)
		{
			int nb_elt = atoi((*list)[stop + 1]);
			stop = stop + nb_elt + 2;
		}
		if (ll == (nb_att - 1))
			stop--;
	}

	obj.last_idx = stop;
	obj.first_idx = start;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method
//  	DbServerCache::get_obj_property()
//
// description :
//		This method returns to the caller the wanted object properties. The returned data are the same than the one
//		returned by the classical API
//
// argument :
// 		in :
//			- in_param : The object name followed by the wanted property names
//
// return :
//		This method returns a pointer to a DevVarStringArray initilised with the object properties
//
//-------------------------------------------------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_obj_property(DevVarStringArray *in_param)
{
	int obj_ind;
	int res = find_obj((*in_param)[0],obj_ind);
	if (res == -1)
	{
		TangoSys_OMemStream o;
		o << "Object " << (*in_param)[0] << " not found in DB cache" << ends;

		Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
									   (const char *)"DbServerCache::get_obj_property");
	}
	else
	{
	    int ret_length = 2;

		ret_obj_prop.length(ret_length);
		ret_obj_prop[0] = CORBA::string_dup((*in_param)[0]);

		get_obj_prop(in_param,ctrl_serv_prop,true);
	}

	return &ret_obj_prop;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_device_property_list()
//
// description :
//		This method returns to the caller the device property names. The returned data are the same than the one
//		returned by the classical API
//
// argument :
// 		in :
//			- in_param : The device name followed by a wildcard
//
// return :
//	This method returns a pointer to a DevVarStringArray initilised with the device property names
//
//------------------------------------------------------------------------------------------------------------------


const DevVarStringArray *DbServerCache::get_device_property_list(DevVarStringArray *in_param)
{

//
// There is a special case for the dserver admin device
//

	if (TG_strncasecmp((*in_param)[0],"dserver/",8) == 0)
	{
		ret_prop_list.length(0);
		get_obj_prop_list(in_param,adm_dev_prop);
	}
	else
	{
		int class_ind,dev_ind;
		int res = find_dev_att((*in_param)[0],class_ind,dev_ind);
		if (res == -1)
		{
			TangoSys_OMemStream o;
			o << "Device " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
										   (const char *)"DbServerCache::get_device_property_list");
		}
		else
		{
			ret_prop_list.length(0);
			get_obj_prop_list(in_param,classes_idx[class_ind].devs_idx[dev_ind].dev_prop);
		}
	}

	return &ret_prop_list;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_obj_prop_list()
//
// description :
//		This method retrieves device property list for a device
//
// argument :
// 		in :
//			- in_param : The device name followed by a wildcard
//			- obj : The device object in the cache
//
// retrun :
//		This method returns a pointer to a DevVarStringArray initialised with the device property names
//
//------------------------------------------------------------------------------------------------------------------

void DbServerCache::get_obj_prop_list(DevVarStringArray *in_param,PropEltIdx &obj)
{

//
// First analyse the wildcard
//

	bool before = false,after = false,wildcard_used;
	string before_str;
	string after_str;
	string wildcard((*in_param)[1]);

	transform(wildcard.begin(),wildcard.end(),wildcard.begin(),::tolower);

	string::size_type pos = wildcard.find('*');
	if (pos != string::npos)
	{
		wildcard_used = true;
		if (pos == 0)
		{
			if (wildcard.size() == 1)
			{
				before = false;
				after = false;
			}
			else
			{
				before = false;
				after = true;
				after_str = wildcard.substr(pos + 1);
			}
		}
		else
		{
			before = true;
			before_str = wildcard.substr(0,pos);
			if (pos == wildcard.size() - 1)
				after = false;
			else
			{
				after = true;
				after_str = wildcard.substr(pos + 1);
			}
		}
	}
	else
		wildcard_used = false;

	int ret_length = 1;
	int obj_prop = obj.prop_nb;

	int lo;
	string::size_type pos_after,pos_before;

//
// A loop on each prop.
//

	for (lo = 0;lo < obj_prop * 2;lo = lo + 2)
	{

//
// Check according to the user wildcard, if we have to returned this property name
// Note: The property name is case independant
//

		bool store = false;
		if (wildcard_used == true)
		{
			if (before == false)
			{
				if (after == false)
					store = true;
				else
				{
					string tmp_name((*data_list)[obj.props_idx[lo]]);
					transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);

					pos_after = tmp_name.rfind(after_str);
					if ((pos_after != string::npos) && (pos_after == (tmp_name.size() - after_str.size())))
						store = true;
				}
			}
			else
			{
				string tmp_name((*data_list)[obj.props_idx[lo]]);
				transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);

				if (after == false)
				{
					pos_before = tmp_name.find(before_str);
					if ((pos_before != string::npos) && (pos_before == 0))
						store = true;
				}
				else
				{
					pos_before = tmp_name.find(before_str);
					pos_after = tmp_name.rfind(after_str);
					if ((pos_before != string::npos) && (pos_before == 0) &&
						(pos_after != string::npos) && (pos_after == (tmp_name.size() - after_str.size())))
						store = true;
				}
			}
		}
		else
		{
			string tmp_name((*data_list)[obj.props_idx[lo]]);
			transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);

			if (tmp_name == wildcard)
				store = true;
		}

//
// Store the property name if decided
//

		if (store == true)
		{
			ret_prop_list.length(ret_length);
			ret_prop_list[ret_length - 1] = CORBA::string_dup((*data_list)[obj.props_idx[lo]]);
			ret_length = ret_length + 1;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::import_tac_dev()
//
// description :
//		This method returns to the caller of the info necessary to import the TAC device.
//		The returned data are the same than the one returned by the classical API
//
// return :
//		This method returns a pointer to a DevVarLongStringArray initilised with the Tango access control import data
//
//-------------------------------------------------------------------------------------------------------------------

const DevVarLongStringArray *DbServerCache::import_tac_dev(string &tac_dev)
{

//
// Throw exception if no info in cache
//

	if (imp_tac.last_idx == -1 || imp_tac.first_idx >= (int)data_list->length())
	{
		Tango::Except::throw_exception((const char *)API_DatabaseCacheAccess,
                                       (const char *)"No TAC device in Db cache",
                                       (const char *)"DbServerCache::import_tac_dev");
	}

//
// Throw exception if the device in cache is not the requested one
//

    if (tac_dev.size() != strlen((*data_list)[imp_tac.first_idx]))
    {
		Tango::Except::throw_exception((const char *)API_DatabaseCacheAccess,
                                       (const char *)"Device not available from cache",
                                       (const char *)"DbServerCache::import_tac_dev");
    }

    string local_tac_dev(tac_dev);
    string cache_tac_dev((*data_list)[imp_tac.first_idx]);

    transform(local_tac_dev.begin(),local_tac_dev.end(),local_tac_dev.begin(),::tolower);
    transform(cache_tac_dev.begin(),cache_tac_dev.end(),cache_tac_dev.begin(),::tolower);

    if (local_tac_dev != cache_tac_dev)
    {
        Tango::Except::throw_exception((const char *)API_DatabaseCacheAccess,
                                       (const char *)"Device not available from cache",
                                       (const char *)"DbServerCache::import_tac_dev");
    }

//
// Throw exception if the device is not found in DB
//

	if (::strcmp((*data_list)[imp_tac.first_idx + 1],"Not Found") == 0)
	{
		TangoSys_OMemStream o;
		o << "Device " << tac_dev << " not defined in database" << ends;

		Tango::Except::throw_exception((const char *)"DB_DeviceNotDefined",o.str(),
									   (const char *)"DbServerCache::import_tac_dev");
	}

//
// Return cache data
//

	imp_tac_data.lvalue.length(2);
	imp_tac_data.svalue.length(5);

	imp_tac_data.lvalue[0] = ::atoi((*data_list)[imp_tac.first_idx + 5]);
	imp_tac_data.lvalue[1] = ::atoi((*data_list)[imp_tac.last_idx - 1]);

	for (int loop = 0;loop < 5;loop++)
		imp_tac_data.svalue[loop] = CORBA::string_dup((*data_list)[imp_tac.first_idx + loop]);

	return &imp_tac_data;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_class_pipe_property()
//
// description :
//		This method returns to the caller the class pipe properties for the class and pipes specified in
// 		the in parameters. The returned data are the same than the one returned by the classical API
//
// argument
// 		in :
//			- in_param : The class name followed by the wanted pipe names
//
// return :
//		This method returns a pointer to a DevVarStringArray initilised with the class pipe properties
//
//-------------------------------------------------------------------------------------------------------------------

const DevVarStringArray *DbServerCache::get_class_pipe_property(DevVarStringArray *in_param)
{

//
// Throw exception if stored procedure does not support pipe
//

	if (proc_release < 109)
	{
		string mess("Your database stored procedure is too old to support pipe. Please update to stored procedure release 1.9 or more");
		Tango::Except::throw_exception("DB_TooOldStoredProc",mess,"DbServerCache::get_class_pipe_property");
	}

	int found_pipe = 0;
	char n_pipe_str[256];

	ret_obj_pipe_prop.length(2);
	ret_obj_pipe_prop[0] = CORBA::string_dup((*in_param)[0]);

	int cl_idx = find_class((*in_param)[0]);
	if (cl_idx != -1)
	{

//
// The class is found
//

		int wanted_pipe_nb = in_param->length() - 1;
		int class_pipe_nb = classes_idx[cl_idx].class_pipe_prop.att_nb;
		for (int loop = 0;loop < wanted_pipe_nb;loop++)
		{
			int ll;
			for (ll = 0;ll < class_pipe_nb;ll++)
			{
				if (TG_strcasecmp((*in_param)[loop + 1],(*data_list)[classes_idx[cl_idx].class_pipe_prop.atts_idx[ll]]) == 0)
				{

//
// The pipe is found, copy all its properties
//

					int pipe_index = classes_idx[cl_idx].class_pipe_prop.atts_idx[ll];
					int nb_prop = ::atoi((*data_list)[pipe_index + 1]);
					int nb_elt = 0;
					int nb_to_copy = 0;
					int tmp_idx = pipe_index + 2;
					nb_to_copy = 2;
					for (int k = 0;k < nb_prop;k++)
					{
						nb_elt = ::atoi((*data_list)[tmp_idx + 1]);
						tmp_idx = tmp_idx + nb_elt + 2;
						nb_to_copy = nb_to_copy + 2 + nb_elt;
					}

					int old_length = ret_obj_pipe_prop.length();
					ret_obj_pipe_prop.length(old_length + nb_to_copy);
					for (int j = 0;j < nb_to_copy;j++)
						ret_obj_pipe_prop[old_length + j] = CORBA::string_dup((*data_list)[pipe_index + j]);
					found_pipe++;
					break;
				}
			}
			if (ll == class_pipe_nb)
			{
				found_pipe++;
				int old_length = ret_obj_pipe_prop.length();
				ret_obj_pipe_prop.length(old_length + 2);
				ret_obj_pipe_prop[old_length] = CORBA::string_dup((*in_param)[loop + 1]);
				ret_obj_pipe_prop[old_length + 1] = CORBA::string_dup("0");
			}
		}
		::sprintf(n_pipe_str,"%d",found_pipe);
		ret_obj_pipe_prop[1] = CORBA::string_dup(n_pipe_str);
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Class " << (*in_param)[0] << " not found in DB cache" << ends;

		Tango::Except::throw_exception("DB_ClassNotFoundInCache",o.str(),
										   "DbServerCache::get_class_pipe_property");
	}

//	cout4 << "DbCache --> Returned data for a get_class_pipe_property for class " << (*in_param)[0] << endl;
//	for (unsigned int ll=0;ll< ret_obj_pipe_prop.length();ll++)
//		cout4 << "    DbCache --> Returned object pipe prop = " << ret_obj_pipe_prop[ll] << endl;

	return &ret_obj_pipe_prop;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//  	DbServerCache::get_dev_pipe_property()
//
// description :
//		This method returns to the caller the device pipe properties for the device and pipes specified in
//		the in parameters. The returned data are the same than the one returned by the classical API
//
// argument
// 		in :
//			- in_param : The device name followed by the wanted pipe names
//
// return :
//		This method returns a pointer to a DevVarStringArray initialised with the device pipe properties
//
//------------------------------------------------------------------------------------------------------------------

const DevVarStringArray *DbServerCache::get_dev_pipe_property(DevVarStringArray *in_param)
{

//
// Throw exception if stored procedure does not support pipe
//

	if (proc_release < 109)
	{
		string mess("Your database stored procedure is too old to support pipe. Please update to stored procedure release 1.9 or more");
		Tango::Except::throw_exception("DB_TooOldStoredProc",mess,"DbServerCache::get_dev_pipe_property");
	}

	int found_pipe = 0;
	char n_pipe_str[256];

	ret_obj_pipe_prop.length(2);
	ret_obj_pipe_prop[0] = CORBA::string_dup((*in_param)[0]);

	int class_ind,dev_ind;

	int ret_value = find_dev_att((*in_param)[0],class_ind,dev_ind);
	if (ret_value != -1)
	{
		int wanted_pipe_nb = in_param->length() - 1;
		int dev_pipe_nb = classes_idx[class_ind].devs_idx[dev_ind].dev_pipe_prop.att_nb;
		for (int loop = 0;loop < wanted_pipe_nb;loop++)
		{
			int ll;
			for (ll = 0;ll < dev_pipe_nb;ll++)
			{
				if (TG_strcasecmp((*in_param)[loop + 1],(*data_list)[classes_idx[class_ind].devs_idx[dev_ind].dev_pipe_prop.atts_idx[ll]]) == 0)
				{
					int pipe_index = classes_idx[class_ind].devs_idx[dev_ind].dev_pipe_prop.atts_idx[ll];
					int nb_prop = ::atoi((*data_list)[pipe_index + 1]);
					int nb_elt = 0;
					int nb_to_copy = 0;
					int tmp_idx = pipe_index + 2;
					nb_to_copy = 2;
					for (int k = 0;k < nb_prop;k++)
					{
						nb_elt = ::atoi((*data_list)[tmp_idx + 1]);
						tmp_idx = tmp_idx + nb_elt + 2;
						nb_to_copy = nb_to_copy + 2 + nb_elt;
					}

					int old_length = ret_obj_pipe_prop.length();
					ret_obj_pipe_prop.length(old_length + nb_to_copy);
					for (int j = 0;j < nb_to_copy;j++)
						ret_obj_pipe_prop[old_length + j] = CORBA::string_dup((*data_list)[pipe_index + j]);
					found_pipe++;
					break;
				}
			}
			if (ll == dev_pipe_nb)
			{
				found_pipe++;
				int old_length = ret_obj_pipe_prop.length();

				ret_obj_pipe_prop.length(old_length + 2);
				ret_obj_pipe_prop[old_length] = CORBA::string_dup((*in_param)[loop + 1]);
				ret_obj_pipe_prop[old_length + 1] = CORBA::string_dup("0");
			}
		}
		::sprintf(n_pipe_str,"%d",found_pipe);
		ret_obj_pipe_prop[1] = CORBA::string_dup(n_pipe_str);
	}
	else
	{
		if (TG_strncasecmp("dserver/",(*in_param)[0],8) != 0)
		{
			TangoSys_OMemStream o;
			o << "Device " << (*in_param)[0] << " not found in DB cache" << ends;

			Tango::Except::throw_exception((const char *)"DB_DeviceNotFoundInCache",o.str(),
											(const char *)"DbServerCache::get_dev_pipe_property");
		}
		else
		{
			::sprintf(n_pipe_str,"%d",found_pipe);
			ret_obj_pipe_prop[1] = CORBA::string_dup(n_pipe_str);
		}
	}

//	cout4 << "DbCache --> Returned data for a get_dev_pipe_property for device " << (*in_param)[0] << endl;
//	for (unsigned int ll=0;ll< ret_obj_pipe_prop.length();ll++)
//		cout4 << "    DbCache --> Returned object pipe prop = " << ret_obj_pipe_prop[ll] << endl;

	return &ret_obj_pipe_prop;
}

} // End of Tango namespace
