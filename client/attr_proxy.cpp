static const char *RcsId = "$Id$\n$Name$";
//
// attr_proxy.cpp 	- C++ source code file for TANGO attribute proxy api 
//
// programmer(s)	- Andy Gotz (goetz@esrf.fr)
//
// original 		- July 2003
//
// version 		- $Version$
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */

#include <time.h>
#include <signal.h>

#include <algorithm>
                                                    
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - constructor for attribute proxy object 
//
//-----------------------------------------------------------------------------

AttributeProxy::AttributeProxy (string &name):dev_proxy(NULL),ext(NULL)
{
	real_constructor(name);
}

AttributeProxy::AttributeProxy (const char *na):dev_proxy(NULL),ext(NULL)
{
	string name(na);
	real_constructor(name);
}

void AttributeProxy::real_constructor (string &name)
{

//
// parse device name
//

	parse_name(name);
	string corba_name;

//
// Create the associated DeviceProxy object
//
		
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			dev_proxy = new DeviceProxy(device_name);
			if (ui->in_server() == true)
				db_attr = new DbAttribute(attr_name,device_name,Tango::Util::instance()->get_database());
			else
				db_attr = new DbAttribute(attr_name,device_name);
			int ind = ui->get_db_ind();
			db_host = (ui->get_db_vect())[ind]->get_db_host();
			db_port = (ui->get_db_vect())[ind]->get_db_port();
			db_port_num = (ui->get_db_vect())[ind]->get_db_port_num();
		}
		else
		{
			dev_proxy = new DeviceProxy(device_name);
			db_attr = new DbAttribute(attr_name,device_name,db_host,db_port);
		}
	}
	else
	{
		db_attr = NULL;
		
		string::size_type stop;
		stop = name.rfind(DEVICE_SEP);
		string nodb_dev_name  = name.substr(0,stop);
		nodb_dev_name = nodb_dev_name + "#dbase=no";

		dev_proxy = new DeviceProxy(nodb_dev_name);
	}

//
// Check that the device support this attribute
//

	try
	{
		dev_proxy->get_attribute_config(attr_name);
	}
	catch (Tango::ConnectionFailed &) {}
	catch (Tango::CommunicationFailed &) {}
	catch (Tango::DevFailed &dfe)
	{
		if (strcmp(dfe.errors[0].reason.in(),"API_AttrNotFound") == 0)
		{
			TangoSys_OMemStream desc;
			desc << "Attribute" << attr_name << " is not supported by device " << device_name << ends;
			
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedAttribute",
						desc.str(),
						(const char*)"AttributeProxy::real_constructor()");
		}
	}
	
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - copy constructor
//
//-----------------------------------------------------------------------------

AttributeProxy::AttributeProxy(const AttributeProxy &prev)
{

//
// First Connection call members
//
	dbase_used = prev.dbase_used;
	from_env_var = prev.from_env_var;
	host = prev.host;
	port = prev.port;
	port_num = prev.port_num;
	db_host = prev.db_host;
	db_port = prev.db_port;
	db_port_num = prev.db_port_num;
	
//
// Now AttributeProxy members
//
	
	device_name = prev.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
			{
				db_attr = new DbAttribute(attr_name,device_name,Tango::Util::instance()->get_database());
				dev_proxy = new DeviceProxy(device_name);
			}
			else
			{
				db_attr = new DbAttribute(attr_name,device_name);
				dev_proxy = new DeviceProxy(device_name);
			}
		}
		else
		{
			db_attr = new DbAttribute(attr_name,device_name,db_host,db_port);
			dev_proxy = new DeviceProxy(device_name);
		}
	}
	
	if (prev.ext != NULL)
	{
		ext = new AttributeProxyExt();
		*ext = *(prev.ext);
	}
	else
		ext = NULL;
		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - assignment operator 
//
//-----------------------------------------------------------------------------

AttributeProxy &AttributeProxy::operator=(const AttributeProxy &rval)
{

//
// First Connection call members
//
	if (dbase_used == true)
		delete db_attr;
	dbase_used = rval.dbase_used;
	from_env_var = rval.from_env_var;
	host = rval.host;
	port = rval.port;
	port_num = rval.port_num;
	db_host = rval.db_host;
	db_port = rval.db_port;
	db_port_num = rval.db_port_num;
	
//
// Now AttributeProxy members
//
	
	attr_name = rval.attr_name;
	device_name = rval.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
			{
				db_attr = new DbAttribute(attr_name,device_name,Tango::Util::instance()->get_database());
				dev_proxy = new DeviceProxy(device_name);
			}
			else
			{
				db_attr = new DbAttribute(attr_name,device_name);
				dev_proxy = new DeviceProxy(device_name);
			}
		}
		else
		{
			db_attr = new DbAttribute(attr_name,device_name,db_host,db_port);
			dev_proxy = new DeviceProxy(device_name);
		}
	}

	if (rval.ext != NULL)
	{
		ext = new AttributeProxyExt();
		*ext = *(rval.ext);
	}
	else
		ext = NULL;
			
	return *this;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::parse_name() - Parse attribute name according to Tango device
//			       name syntax
//
// in :	- full_name : The attribute name
//
//-----------------------------------------------------------------------------

void AttributeProxy::parse_name(string &full_name)
{
	string name_wo_prot;
	string name_wo_db_mod;

//
// Attribute name in lower case letters
//

	string cased_name = full_name;	
	transform(full_name.begin(),full_name.end(),full_name.begin(),::tolower);
		
//
// Try to find protocol specification in attribute name and analyse it
//

	string::size_type pos = full_name.find(PROT_SEP);
	if (pos == string::npos)
	{
		if (full_name.size() > 2)
		{
			if ((full_name[0] == '/') && (full_name[1] == '/'))
				name_wo_prot = full_name.substr(2);
			else
				name_wo_prot = full_name;
		}
		else
			name_wo_prot = full_name;
	}
	else
	{
		string protocol = full_name.substr(0,pos);

		if (protocol == TANGO_PROTOCOL)
		{
			name_wo_prot = full_name.substr(pos + 3);
		}		
		else if (protocol == TACO_PROTOCOL)
		{		
			TangoSys_OMemStream desc;
			desc << "Taco protocol is not supported" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");			exit(0);
		}
		else
		{		
			TangoSys_OMemStream desc;
			desc << protocol;
			desc << " protocol is an unsupported protocol" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");
		}			
	}

//
// Try to find database database modifier and analyse it
//

	pos = name_wo_prot.find(MODIFIER);
	if (pos != string::npos)
	{
		string mod = name_wo_prot.substr(pos + 1);
		
		if (mod == DBASE_YES)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos)); 
			dbase_used = true;
		}
		else if (mod == DBASE_NO)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos));
			dbase_used = false;
		}
		else
		{
			cerr << mod << " is a non supported database modifier!" << endl;

			TangoSys_OMemStream desc;
			desc << mod;
			desc << " modifier is an unsupported db modifier" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedDBaseModifier",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");
		}
	}
	else
	{
		name_wo_db_mod = name_wo_prot;
		dbase_used = true;
	}

	if (dbase_used == false)
	{
	
//
// Extract host name and port number
//

		pos = name_wo_db_mod.find(HOST_SEP);
		if (pos == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"AttributeProxy::parse_name()");
		}
		
		host = name_wo_db_mod.substr(0,pos);
		string::size_type tmp = name_wo_db_mod.find(PORT_SEP);
		if (tmp == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"AttributeProxy::parse_name()");
		}
		port = name_wo_db_mod.substr(pos + 1,tmp - pos - 1);
		TangoSys_MemStream s;
		s << port << ends;
		s >> port_num;
		device_name = name_wo_db_mod.substr(tmp + 1);
		
		db_host = db_port = NOT_USED;
		db_port_num = 0;
				 
	}
	else
	{
	
//
// Search if host and port are specified
//

		pos = name_wo_db_mod.find(PORT_SEP);
		if (pos == string::npos)
		{
			device_name = name_wo_db_mod;
			from_env_var = true;
			port_num = 0;
			host = FROM_IOR;
			port = FROM_IOR;
		}
		else
		{
			string bef_sep = name_wo_db_mod.substr(0,pos);
			string::size_type tmp = bef_sep.find(HOST_SEP);
			if (tmp == string::npos)
			{
				device_name = name_wo_db_mod;
				from_env_var = true;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
			else
			{
				db_host = bef_sep.substr(0,tmp);
				db_port = bef_sep.substr(tmp + 1);
				TangoSys_MemStream s;
				s << db_port << ends;
				s >> db_port_num;
				device_name = name_wo_db_mod.substr(pos + 1);
				from_env_var = false;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
		}

	}

//
// decompose device_name into device and attribute
//

	int n_sep = 0;
	string device_name_tmp(device_name);
	string::size_type device_name_end_pos=0;
	do 
	{
		pos = device_name_tmp.find(DEVICE_SEP);
		if (pos != string::npos) 
		{
			if (pos == 0)
			{
				cerr << "Wrong attribute name syntax (domain/family/member/attr_name) in " << cased_name << endl;

				ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
				(const char*)"Attribute name must have four fields separated by /'s or no /'s at all if it is an alias (e.g. my/device/name/an_attr or myalias)",
				(const char*)"AttributeProxy::parse_name()");
			}
			n_sep++;
			device_name_tmp = device_name_tmp.substr(pos+1);
			if (device_name_tmp.size() == 0)
			{
				cerr << "Wrong attribute name syntax (domain/family/member/attr_name) in " << cased_name << endl;

				ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
				(const char*)"Attribute name must have four fields separated by /'s or no /'s at all if it is an alias (e.g. my/device/name/an_attr or myalias)",
				(const char*)"AttributeProxy::parse_name()");			
			}
			device_name_end_pos += pos+1;
		}
	} 
	while (pos != string::npos);

	if ((n_sep > 1) && (n_sep != 3))
	{
		cerr << "Wrong attribute name syntax (domain/family/member/attr_name) in " << cased_name << endl;

		ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
			(const char*)"Attribute name must have four fields separated by /'s or no /'s at all if it is an alias (e.g. my/device/name/an_attr or myalias)",
			(const char*)"AttributeProxy::parse_name()");
	}

//
// if this is an alias (no slashes in name) then get the device and attribute
// name from the database
//
	
	if (n_sep == 0)
	{
		if (dbase_used == false)
		{
			cerr << "Wrong attribute name syntax (domain/family/member/attr_name) in " << cased_name << endl;
			cerr << "Rem: Alias are forbidden when not using database" << endl;
			
			ApiWrongNameExcept::throw_exception((const char *)"API_WrongAttributeNameSyntax",
						(const char *)"Attribute alias is not supported when not using database",
						(const char *)"AttributeProxy::parse_name()");
		}

//
// Check alias name syntax
//

		pos = device_name.find(HOST_SEP);
		if (pos != string::npos)
		{
			cerr << "Wrong alias name syntax in " << cased_name << " (: is not allowed in alias name)" << endl;

			ApiWrongNameExcept::throw_exception((const char *)"API_WrongAttributeNameSyntax",
			(const char *)"Wrong alias name (: not allowed in alias name)",
			(const char *)"AttributeProxy::parse_name()");
		}

		pos = device_name.find(RES_SEP);
		if (pos != string::npos)
		{
			cerr << "Wrong alias name syntax in " << cased_name << " (-> is not allowed in alias name)" << endl;

			ApiWrongNameExcept::throw_exception((const char *)"API_WrongAttributeNameSyntax",
			(const char *)"Wrong alias name (-> not allowed in alias name)",
			(const char *)"DeviceProxy::parse_name()");
		}
					
//
// Get full attribute name from database but connect to database first if it is not done already
//

		ApiUtil *ui = ApiUtil::instance();
		string db_attr_name;
		if (from_env_var == true)
		{	
			if (ui->in_server() == true)
			{
				try
				{
					Tango::Util::instance()->get_database()->get_attribute_alias(device_name,db_attr_name);
				}
				catch (DevFailed &dfe)
				{
					if (strcmp(dfe.errors[0].reason,"DB_SQLError") == 0)
					{
						TangoSys_OMemStream desc;
						desc << "Can't connect to attribute with alias " << device_name << ends;
						ApiConnExcept::re_throw_exception(dfe,
								(const char *)"API_AliasNotDefined",
					 			desc.str(),
								(const char *)"AttributeProxy::parse_name");
					}
					else
						throw;
				}
			}
			else
			{
				int ind = ui->get_db_ind();
				try
				{
					(ui->get_db_vect())[ind]->get_attribute_alias(device_name,db_attr_name);
				}
				catch (DevFailed &dfe)
				{
					if (strcmp(dfe.errors[0].reason,"DB_SQLError") == 0)
					{
						TangoSys_OMemStream desc;
						desc << "Can't connect to attribute with alias " << device_name << ends;
						ApiConnExcept::re_throw_exception(dfe,
								(const char *)"API_AliasNotDefined",
					 			desc.str(),
								(const char *)"AttributeProxy::parse_name");
					}
					else
						throw;
				}
			}
		}
		else
		{
			int ind = ui->get_db_ind(db_host,db_port_num);
			try
			{
				(ui->get_db_vect())[ind]->get_attribute_alias(device_name,db_attr_name);	
			}
			catch (DevFailed &dfe)
			{
				if (strcmp(dfe.errors[0].reason,"DB_SQLError") == 0)
				{
					TangoSys_OMemStream desc;
					desc << "Can't connect to attribute with alias " << device_name << ends;
					ApiConnExcept::re_throw_exception(dfe,
							(const char *)"API_AliasNotDefined",
					 		desc.str(),
							(const char *)"AttributeProxy::parse_name");
				}
				else
					throw;
			}
		}		

//
// A fast syntax check on the full attribute name returned from the database
//

		string attr_name_tmp = db_attr_name;
		do 
		{
			pos = attr_name_tmp.find(DEVICE_SEP);
			if (pos != string::npos) 
			{
				n_sep++;
				attr_name_tmp = attr_name_tmp.substr(pos+1);
				device_name_end_pos += pos+1;
			}
		} 
		while (pos != string::npos);

		if (n_sep != 3)
		{

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
				(const char*)"Attribute name must have four fields separated by /'s (check the alias entry in the database) ",
				(const char*)"AttributeProxy::parse_name()");
		}
		attr_name = db_attr_name.substr(device_name_end_pos);
		device_name = db_attr_name.substr(0,device_name_end_pos - 1);
	}

	
//
// attribute name has four fields, separate them into device and attribute names
// but keep attr_name as a case sentitive name
//

	else
	{
		device_name = device_name.substr(0,device_name_end_pos - 1);

		if (n_sep == 1)
		{
			if (db_host == NOT_USED)
			{
//
// We are in the following case "device alias/attribute name"
// but the no dbase option was used. This is an error.
// We can't have alias without db
//
				ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
					(const char*)"Can't use device or attribute alias without database",
					(const char*)"AttributeProxy::parse_name()");

			}
			
			if (from_env_var == false)
			{
				pos = name_wo_db_mod.rfind(DEVICE_SEP);
				device_name = name_wo_db_mod.substr(0,pos);
			}
		}
		
		
		pos = cased_name.rfind(DEVICE_SEP);
		string::size_type pos_mod = cased_name.rfind(MODIFIER);
		if (pos_mod != string::npos)
			attr_name = cased_name.substr(pos + 1,pos_mod - (pos + 1));
		else
			attr_name = cased_name.substr(pos + 1);
	}
		
		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::~AttributeProxy() - destructor to destroy proxy to TANGO device
//
//-----------------------------------------------------------------------------

AttributeProxy::~AttributeProxy()
{
	if (dbase_used == true)
		delete db_attr;
	if (dev_proxy != NULL)
		delete dev_proxy;
	if (ext != NULL)
		delete ext;		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::ping() - ping TANGO device and return time elapsed in microseconds
//
//-----------------------------------------------------------------------------

int AttributeProxy::ping()
{

	return(dev_proxy->ping());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::state() - return TANGO state of device
//
//-----------------------------------------------------------------------------

DevState AttributeProxy::state()
{

	return(dev_proxy->state());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::status() - return TANGO status of device
//
//-----------------------------------------------------------------------------

string AttributeProxy::status()
{
	return(dev_proxy->status());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(string &property_name, DbData &user_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		DbData db_data;
		db_data.resize(1);
		db_data[0] = DbDatum(attr_name);

		db_attr->get_property(db_data);
		
		long nb_prop = db_data.size();
		int i;
		for (i = 1;i < nb_prop;i++)
		{
			if (db_data[i].name == property_name)
			{
				user_data.resize(0);
				user_data.push_back(db_data[i]);
				break;
			}
		}
		
		if (i == nb_prop)
		{
			user_data.resize(0);
			DbDatum no_data;
			no_data.name = property_name;
			no_data.value_string.resize(0);
			user_data.push_back(no_data);
		}
		
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(vector<string> &property_names, DbData &user_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		long nb_prop = property_names.size();
		DbData db_data;
		db_data.resize(1);
		db_data[0] = DbDatum(attr_name);

		db_attr->get_property(db_data);	
	
		int i,j;
		user_data.resize(0);		
		long nb_recev_prop = db_data.size();
		for (i = 0;i < nb_prop;i++)
		{
			for (j = 1;j < nb_recev_prop;j++)
			{
				if (db_data[j].name == property_names[i])
				{
					user_data.push_back(db_data[j]);
					break;
				}
			}
		
			if (j == nb_recev_prop)
			{
				DbDatum no_data;
				no_data.name = property_names[i];
				no_data.value_string.resize(0);
				user_data.push_back(no_data);
			}
		}	
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(DbData &user_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		long nb_prop = user_data.size();
		DbData db_data;
		db_data.resize(1);
		db_data[0] = DbDatum(attr_name);

		db_attr->get_property(db_data);	

		int i,j;
		long nb_recev_prop = db_data.size();
		for (i = 0;i < nb_prop;i++)
		{
			for (j = 1;j < nb_recev_prop;j++)
			{
				if (db_data[j].name == user_data[i].name)
				{
					user_data[i] = db_data[j];
					break;
				}
			}
		
			if (j == nb_recev_prop)
			{
				user_data[i].value_string.resize(0);
			}
		}			
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::put_property() - put a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::put_property(DbData &user_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::put_property");
	}
	else
	{
		DbData db_data;
		DbDatum att_name(attr_name);
		long nb_prop = user_data.size();
		att_name << (short)nb_prop;
		db_data.push_back(att_name);
		for (int i = 0;i < nb_prop;i++)
			db_data.push_back(user_data[i]);
		
		db_attr->put_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(string &property_name) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		DbData db_data;

		DbDatum att(attr_name);
		db_data.push_back(att);
		db_data.push_back(DbDatum(property_name));

		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(vector<string> &property_names) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		DbData db_data;

		DbDatum att(attr_name);
		db_data.push_back(att);
		for (unsigned int i=0; i<property_names.size(); i++)
		{
			db_data.push_back(DbDatum(property_names[i]));
		}
		
		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(DbData &user_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for attribute belonging to device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		DbData db_data;

		DbDatum att(attr_name);
		db_data.push_back(att);
		for (unsigned int i=0; i<user_data.size(); i++)
		{
			db_data[i] = user_data[i];
		}
		
		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_config() - return attribute config
//
//-----------------------------------------------------------------------------
 
AttributeInfoEx AttributeProxy::get_config() 
{
	return (dev_proxy->get_attribute_config(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::set_attribute_config() - set config for attribute
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::set_config(AttributeInfo &dev_attr_info) 
{
	AttributeInfoList attr_info_list;

	attr_info_list.push_back(dev_attr_info);
	try 
	{
		dev_proxy->set_attribute_config(attr_info_list);

	}
        catch (CORBA::SystemException &ce)
        {
		TangoSys_OMemStream desc;
		desc << "Failed to execute set_attribute_config on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"AttributeProxy::set_attribute_config()");
	}
}

void AttributeProxy::set_config(AttributeInfoEx &dev_attr_info) 
{
	AttributeInfoListEx attr_info_list;

	attr_info_list.push_back(dev_attr_info);
	try 
	{
		dev_proxy->set_attribute_config(attr_info_list);

	}
        catch (CORBA::SystemException &ce)
        {
		TangoSys_OMemStream desc;
		desc << "Failed to execute set_attribute_config on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"AttributeProxy::set_attribute_config()");
	}
}
//-----------------------------------------------------------------------------
//
// AttributeProxy::read() - read attribute 
//
//-----------------------------------------------------------------------------
 

DeviceAttribute AttributeProxy::read() 
{
	return(dev_proxy->read_attribute(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::write() - write attribute
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::write(DeviceAttribute& attr_value) 
{
	attr_value.set_name(attr_name);
	dev_proxy->write_attribute(attr_value);
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::history() - get attribute history
//				      (only for polled attribute)
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttributeHistory> *AttributeProxy::history(int depth) 
{

	return(dev_proxy->attribute_history(attr_name, depth));

}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_poll_period() - Return attribute polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int AttributeProxy::get_poll_period() 
{
	return(dev_proxy->get_attribute_poll_period(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::poll() - If object is already polled, just update its
//			    polling period. If object is not polled, add
//			    it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::poll(int period) 
{
	dev_proxy->poll_attribute(attr_name, period);
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::is_polled() - return true if the attribute is polled
//
//-----------------------------------------------------------------------------

bool AttributeProxy::is_polled()
{
	return(dev_proxy->is_attribute_polled(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::stop_poll() - Stop polling attribute
//
//-----------------------------------------------------------------------------

void AttributeProxy::stop_poll()
{
	dev_proxy->stop_poll_attribute(attr_name);
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::subscribe_event - Subscribe to an event
//
//-----------------------------------------------------------------------------

int AttributeProxy::subscribe_event (EventType event, CallBack *callback, const vector<string> &filters)
{
  	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		ApiUtil::instance()->create_event_consumer();
	}

	ApiUtil::instance()->get_event_consumer()->connect(dev_proxy);

	
	return ApiUtil::instance()->get_event_consumer()->subscribe_event(dev_proxy, attr_name, event, callback, filters);
}


} // End of Tango namespace
