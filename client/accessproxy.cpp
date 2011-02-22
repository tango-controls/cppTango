//+======================================================================
// $Source$
//
// Project:   Tango
//
// Description:  Cpp source code for the AccessProxy class definition .
//
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
// $Log$
// Revision 3.12  2010/02/08 14:40:19  taurel
// - Add a patch from Nicolas about Mac-OS port
//
// Revision 3.11  2009/09/22 11:04:45  taurel
// - Environment variables in file also supported for Windows
//
// Revision 3.10  2009/04/20 13:25:50  taurel
// - Fix bug in case of default constructed DeviceProxy and alias() method
// - Add some ctors from "const char *" to make programmer's life easier
//
// Revision 3.9  2009/04/08 08:30:39  taurel
// - Fix a bug in case of TangoAccessControl server started with
// SUPER_TANGO set to 1
//
// Revision 3.8  2009/03/20 11:52:06  taurel
// - Add tangorc files management (for env. variables)
//
// Revision 3.7  2009/03/13 09:32:27  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.6  2009/03/02 15:55:51  taurel
// - Ported to Windows
//
// Revision 3.5  2009/02/19 12:25:36  taurel
// - Other Changes for Solaris compilation
//
// Revision 3.4  2009/01/21 12:45:14  taurel
// - Change CopyRights for 2009
//
// Revision 3.3  2008/10/06 15:02:16  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.2  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.1  2008/05/20 12:42:28  taurel
// - Commit after merge with release 7 branch
//
// Revision 1.1.2.1  2008/02/07 15:56:58  taurel
// - First implementation of the Controlled Access done
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
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

	try
	{

//
//	If not already done, get user name.
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

		if (host.empty() == true)
		{
			char h_name[80];
			int res = gethostname(h_name,80);
			if (res == 0)
			{
#ifndef _TG_WINDOWS_
				struct hostent my_addr;
				int err;
				char buffer[__AC_BUFFER_SIZE];
#endif
				struct hostent *my_addr_ptr;

#if defined (sun)
				if (gethostbyname_r(h_name,&my_addr,buffer,sizeof(buffer),&err) != 0)
#elif defined (_TG_WINDOWS_) || defined (__MACOS__) || defined (__darwin__)
				my_addr_ptr = gethostbyname(h_name);
				if (my_addr_ptr == NULL)
#else
				if (gethostbyname_r(h_name,&my_addr,buffer,sizeof(buffer),&my_addr_ptr,&err) != 0)
#endif
				{
					cerr << "AccessProxy::check_access_control: Can't get my IP address !" << endl;
					cerr << "Access right set to ACCESS_READ" << endl;

					return ACCESS_READ;
				}

				struct in_addr **addr_list;

//
// TODO : Manage the case of host with several network address
//

#ifndef _TG_WINDOWS_
				addr_list = (struct in_addr **)my_addr.h_addr_list;
#else
				addr_list = (struct in_addr **)my_addr_ptr->h_addr_list;
#endif
				host = inet_ntoa(*addr_list[0]);
			}
			else
			{
				cerr << "AccessProxy::check_access_control: Can't get my host name !" << endl;
				cerr << "Access right set to ACCESS_READ" << endl;

				return ACCESS_READ;
			}
		}

//
//	Check for user and host rights on specified device
//

		DeviceData din,dout;
		DevVarStringArray dvsa;
		dvsa.length(3);
		dvsa[0] = CORBA::string_dup(user.c_str());
		dvsa[1] = CORBA::string_dup(host.c_str());
		dvsa[2] = CORBA::string_dup(devname.c_str());
		din << dvsa;

		dout = command_inout("GetAccess",din);

		string right;
		dout >> right;
		if (right == "write")
			return ACCESS_WRITE;
		else
			return ACCESS_READ;
	}
	catch (Tango::DevFailed &e)
	{
		if (::strcmp(e.errors[0].reason.in(),"API_DeviceNotExported") == 0)
		{
			Except::re_throw_exception(e,(const char *)"API_CannotCheckAccessControl",
										(const char *)"Cannot import Access Control device !",
										(const char *)"AccessProxy::check_access_control()");
		}
		else
			throw;
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
