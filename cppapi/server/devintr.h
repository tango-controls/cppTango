//===================================================================================================================
//
// file :		devintr.h
//
// description :	Include file for the DevIntr class
//					This class is used for the device interface change event generation
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2014,2015
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
//===================================================================================================================

#ifndef _DEVINTR_H
#define _DEVINTR_H

#include <tango.h>

namespace Tango
{

class DevIntr
{
public:
	DevIntr() {}

	void get_interface(DeviceImpl *);
	bool has_changed(DeviceImpl *);

protected:

private:
	struct CmdIntr
	{
		string			name;
		CmdArgType		in_type;
		CmdArgType		out_type;

		bool operator<(const struct CmdIntr &rhs) const {return name < rhs.name;}
		bool operator==(const struct CmdIntr &) const;
	};

	struct AttrIntr
	{
		string			name;
		AttrWriteType	writable;
		long			data_type;
		AttrDataFormat	data_format;
		long			max_x;
		long			max_y;
		bool			mem;
		bool			mem_init;
		string			writable_attr_name;
		vector<string>	enum_labels;

		bool operator<(const struct AttrIntr &rhs) const {return name < rhs.name;}
		bool operator==(const struct AttrIntr &) const;
	};

	vector<CmdIntr>		cmds;
	vector<AttrIntr>	atts;

	void build_cmd_interfaces(DeviceImpl *,vector<CmdIntr> &);
	void build_att_interfaces(DeviceImpl *,vector<AttrIntr> &);
};

} // End of Tango namespace

#endif /* _DEVINTR_H */
