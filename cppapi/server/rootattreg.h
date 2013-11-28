//===================================================================================================================
//
// file :		rootattreg.h
//
// description :	Include file for the RootAttRegistry and RootAttConfCallBack classes hierarchy.
//					These classes are used for the forwarded attribute feature
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2013
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

#ifndef _ROOTATTREG_H
#define _ROOTATTREG_H

#include <tango.h>

namespace Tango
{

struct NameFwdAttr
{
	string			local_name;			// Local device name
	string			local_att_name;		// Local attribute name
	string			local_label;		// Local attribute label
	FwdAttr			*fwd_attr;
};

class RootAttRegistry
{
public:
	RootAttRegistry() {}

	void add_root_att(string &,string &,string &,string &,FwdAttr *,DeviceImpl *);
	void remove_root_att(string &,string &);
	DeviceProxy *get_root_att_dp(string &);
	string get_local_att_name(string &_s) {return cbp.get_local_att_name(_s);}
	void update_label(string &_d,string &_a,string &_l) {string s(_d+'/'+_a);cbp.update_label(s,_l);}
	void update_device_impl(string &_n,DeviceImpl *_d) {cbp.update_device_impl(_n,_d);}

	void clear_attrdesc(string &,string &);

protected:

private:
	class RootAttConfCallBack: public Tango::CallBack
	{
	public:
		RootAttConfCallBack():Tango::CallBack() {ci.cpp_clnt(0);}

		virtual void push_event(Tango::AttrConfEventData *);

		void add_att(string &,string &,string &,FwdAttr *,DeviceImpl *);
		void remove_att(string &);
		void clear_attrdesc(string &);
        bool is_root_att_in_map(string &);
        int count_root_dev(string &);
		string get_local_att_name(string &);
		void update_label(string &,string &);
		void update_device_impl(string &,DeviceImpl *);

	private:
		ClntIdent 							ci;
		omni_mutex							the_lock;
		map<string,struct NameFwdAttr>		map_attrdesc;		// Key is root attribute device_name/att_name
		map<string,DeviceImpl *>			local_dis;			// Key is local device name
	};

	map<string,DeviceProxy *>		dps;				// Key is root attribute device name
	map<string,int>					map_event_id;		// key id root attribute device_name/att_name
	RootAttConfCallBack				cbp;
};

} // End of Tango namespace

#endif /* _ROOTATTREG_H */
