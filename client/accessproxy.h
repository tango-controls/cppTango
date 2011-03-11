//
// AccessProxy.h -	include file for TANGO AccessProxy class
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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

#ifndef _ACCESSPROXY_H
#define _ACCESSPROXY_H

#include <tango.h>

using namespace std;

namespace Tango {

#define		__AC_BUFFER_SIZE		1024

class AccessProxy: public Tango::DeviceProxy
{
public:
	AccessProxy(string &);
	AccessProxy(const char *);
	~AccessProxy() {}

	AccessControlType check_access_control(string &);
	bool is_command_allowed(string &,string &);

protected:
	string							user;
	vector<string>					host_ips;
	bool							forced;
	map<string,vector<string> > 	allowed_cmd_table;

	void get_allowed_commands(string &,vector<string> &);
	
private:
	void real_ctor();
};

} // End of Tango namespace

#endif /* _ACCESSPROXY_H */
