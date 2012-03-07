//+======================================================================
// $Source$
//
// Project:   Tango
//
// Description:  Cpp source code for the AccessProxy class definition .
//
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-======================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <accessproxy.h>

#include <sys/types.h>

#ifndef _TG_WINDOWS_
#include <pwd.h>
#endif

namespace Tango
{

/**
 *	This class is extends TangoApi.DeviceProxy
 *	to manage Tango access device.
 *	 - Check if control access is requested.
 *	 - Check who is the user and the host.
 *	 - Check access for this user, this host and the specified device.
 *
 * @author  verdier
 * @Revision
 */


//===============================================================
/**
*	Constructor for Access device proxy
*
*	@param devname	access device name
*/
//===============================================================

AccessProxy::AccessProxy(string &devname) : DeviceProxy(devname,false),
forced(false)
{
	real_ctor();
}


AccessProxy::AccessProxy(const char *devname) : DeviceProxy(devname,false),
forced(false)
{
	real_ctor();
}

void AccessProxy::real_ctor()
{
//
//	Check if forced mode
//
	string super_tango;

	int ret = get_env_var("SUPER_TANGO",super_tango);
	if (ret == 0)
	{
		transform(super_tango.begin(),super_tango.end(),super_tango.begin(),::tolower);
		if (super_tango == "true")
			forced = true;
	}

//
//	Build device proxy and check if present.
//

	if (forced == false)
		ping();


	set_access_control(ACCESS_WRITE);
}

//===============================================================
/**
 *	Check access for specified device
 *
 *	@param devname	device name to check access
 */
//===============================================================

AccessControlType AccessProxy::check_access_control(string &devname)
{
	if (forced)
		return ACCESS_WRITE;

	bool multi_ip = true;
	bool two_tries = false;

	while (two_tries == false)
	{
		try
		{

//
// If not already done, get user name.
// I am using the effective UID in order to allow applications using the seteuid(0) system call
// to change the effective user id and therefore to take someone else rights
//

			if (user.empty() == true)
			{
#ifndef _TG_WINDOWS_
				uid_t user_id = geteuid();

				struct passwd pw;
				struct passwd *pw_ptr;
				char buffer[__AC_BUFFER_SIZE];

				if (getpwuid_r(user_id,&pw,buffer,sizeof(buffer),&pw_ptr) != 0)
				{
					cerr << "AccessProxy::check_access_control: Can't get the user UID !" << endl;
					cerr << "Access right set to ACCESS_READ" << endl;

					return ACCESS_READ;
				}

				if (pw_ptr == NULL)
				{
					cerr << "AccessProxy::check_access_control: Can't get the user UID !" << endl;
					cerr << "Access right set to ACCESS_READ" << endl;

					return ACCESS_READ;
				}

				user = pw.pw_name;
				transform(user.begin(),user.end(),user.begin(),::tolower);
#else
				BOOL ret;
				TCHAR buffer[128];
				DWORD nb = 128;

				ret = GetUserName(buffer,&nb);
				if (ret == 0)
				{
					cerr << "AccessProxy::check_access_control: Can't get the user name !" << endl;
					cerr << "Access right set to ACCESS_READ" << endl;

					return ACCESS_READ;
				}
				user = buffer;
				transform(user.begin(),user.end(),user.begin(),::tolower);
#endif
			}

//
//	If not already done, get host address
//

            if (host_ips.empty() == true)
            {
                ApiUtil *au = ApiUtil::instance();
                vector<string> adrs;
                string at_least_one;

                try
                {
                    au->get_ip_from_if(adrs);

//
// Filter out local address and IP v6
//

                    for (unsigned int nb_adrs = 0;nb_adrs < adrs.size();nb_adrs++)
                    {
                        string &tmp_host = adrs[nb_adrs];
                        if (nb_adrs == 0)
                            at_least_one = tmp_host;

                        if (tmp_host.find("127.") == 0) {}
                        else if (tmp_host.find(":") != string::npos) {}
                        else
                            host_ips.push_back(tmp_host);
                    }

                    if (host_ips.empty() == true)
                    {
                        host_ips.push_back(at_least_one);
                    }
                }
                catch (DevFailed)
                {
                    cerr << "AccessProxy::check_access_control: Can't get my IP address !" << endl;
                    cerr << "Access right set to ACCESS_READ" << endl;

                    return ACCESS_READ;
                }
            }

//
//	Check for user and host rights on specified device
//

			DeviceData din,dout;
			DevVarStringArray dvsa;

			if (multi_ip == true)
			{
				dvsa.length(2 + host_ips.size());
				dvsa[0] = CORBA::string_dup(user.c_str());
				dvsa[1] = CORBA::string_dup(devname.c_str());
				for (unsigned int i = 0;i < host_ips.size();i++)
					dvsa[2 + i] = CORBA::string_dup(host_ips[i].c_str());
				din << dvsa;

				dout = command_inout("GetAccessForMultiIp",din);
			}
			else
			{
				two_tries = true;

				dvsa.length(3);
				dvsa[0] = CORBA::string_dup(user.c_str());
				dvsa[1] = CORBA::string_dup(host_ips[0].c_str());
				dvsa[2] = CORBA::string_dup(devname.c_str());

				din << dvsa;

				dout = command_inout("GetAccess",din);
			}
			string right;
			dout >> right;
			if (right == "write")
				return ACCESS_WRITE;
			else
				return ACCESS_READ;
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
			{
				multi_ip = false;
			}
			else if (::strcmp(e.errors[0].reason.in(),"API_DeviceNotExported") == 0)
			{
				Except::re_throw_exception(e,(const char *)"API_CannotCheckAccessControl",
											(const char *)"Cannot import Access Control device !",
											(const char *)"AccessProxy::check_access_control()");
			}
			else
				throw;
		}
	}

	return ACCESS_READ;
}



//===============================================================
/**
 *	Check for specified device, the specified command is allowed.
 *
 *	@param	classname Specified device class name.
 *	@param	cmd Specified command name.
 */
//===============================================================

bool AccessProxy::is_command_allowed(string &classname,string &cmd)
{
	bool ret = false;

//
// Try to find allowed commands for device class in map
//

	map<string,vector<string> >::iterator pos;
	pos = allowed_cmd_table.find(classname);

//
//	If allowed commands for this class not already in map, get them
//

	vector<string> allowed;
	if (pos == allowed_cmd_table.end())
		get_allowed_commands(classname,allowed);
	else
		allowed = pos->second;

//
//	If no cmd allowed returns false
//

	if (allowed.empty() == true)
		ret = false;

//
//	Else, check is the one passed as argument is allowed
//

	else
	{
		for (unsigned int i = 0;i < allowed.size();++i)
		{
			if (TG_strcasecmp(allowed[i].c_str(),cmd.c_str()) == 0)
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}


//===============================================================
/**
 *	query access device to know allowed commands for the class
 */
//===============================================================

void AccessProxy::get_allowed_commands(string &classname,vector<string> &allowed)
{
	try
	{
		DeviceData	din,dout;
		din << classname;

		dout = command_inout("GetAllowedCommands", din);

		dout >> allowed;
	}
	catch (Tango::DevFailed &)
	{
		allowed.clear();
	}
	allowed_cmd_table.insert(map<string,vector<string> >::value_type(classname, allowed));
}

} // End of Tango namespace
