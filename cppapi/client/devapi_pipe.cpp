static const char *RcsId = "$Id$";

//===================================================================================================================
//
// devapi_pipe.cpp 	- C++ source code file for TANGO devapi class DevicePipe
//
// programmer(s) 	- E. Taurel
//
// original 		- March 2001
//
// Copyright (C) :      2014
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
// version 		- $Revision$
//
//====================================================================================================================

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
// 		DevicePipe::DevicePipe()
//
// description:
//		Constructor for DevicePipe class
//
//------------------------------------------------------------------------------------------------------------------

DevicePipe::DevicePipe():ext(Tango_nullptr)
{

}

//-----------------------------------------------------------------------------------------------------------------
//
// method
//		DeviceData::DeviceData()
//
// description:
//		Copy ctor for DevicePipe class
//
//-----------------------------------------------------------------------------------------------------------------

DevicePipe::DevicePipe(const DevicePipe &source):ext(Tango_nullptr)
{

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DevicePipeExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DevicePipeExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

DevicePipe::ClPipeDataElt &DevicePipe::operator[](const string &elt_name)
{
	vector<ClPipeDataElt>::iterator ite;
	bool ret = false;

	for (ite = v_elt.begin();ite != v_elt.end();++ite)
	{
		if (ite->name == elt_name)
		{
			ret = true;
			break;
		}

	}

	if (ret == false)
	{
		stringstream ss;
		ss << "Data element " << elt_name << " not found in data blob for pipe " << name << endl;
		Except::throw_exception(API_PipeDataEltNotFound,ss.str(),"DevicePipe::operator[]");
	}

	return *ite;
}

DevicePipe::ClPipeDataElt &DevicePipe::operator[](size_t ind)
{
	if (ind > v_elt.size())
	{
		stringstream ss;
		ss << "Given index (" << ind << ") above number of data element (" << v_elt.size() << ") in data blob for pipe " << name << endl;
		Except::throw_exception(API_PipeDataEltNotFound,ss.str(),"DevicePipe::operator[]");
	}

	return v_elt[ind];
}

bool DevicePipe::ClPipeDataElt::operator>>(short &datum)
{
//	bool ret = check_for_data();
	bool ret = true;
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			datum = ShortSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
//		ret = check_wrong_type_exception();
	}

	return ret;
}

bool DevicePipe::ClPipeDataElt::operator>>(DevLong &datum)
{
//	bool ret = check_for_data();
	bool ret = true;
	if ( ret == false)
		return false;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			datum = LongSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
//		ret = check_wrong_type_exception();
	}

	return ret;
}

bool DevicePipe::ClPipeDataElt::operator>>(double &datum)
{
//	bool ret = check_for_data();
	bool ret = true;
	if ( ret == false)
		return false;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			datum = DoubleSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
//		ret = check_wrong_type_exception();
	}

	return ret;
}

bool DevicePipe::ClPipeDataElt::operator>>(vector<short> &d_out)
{
	bool ret = true;
	return ret;
}


void DevicePipe::ClPipeDataElt::operator<<(short &datum)
{
	ShortSeq = new DevVarShortArray(1);
	ShortSeq->length(1);
	ShortSeq[0] = datum;

	type = DEV_SHORT;
}

void DevicePipe::ClPipeDataElt::operator << (DevLong &datum)
{
	LongSeq = new DevVarLongArray(1);
	LongSeq->length(1);
	LongSeq[0] = datum;

	type = DEV_LONG;
}

void DevicePipe::ClPipeDataElt::operator << (double &datum)
{
	DoubleSeq = new DevVarDoubleArray(1);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;

	type = DEV_DOUBLE;
}
void DevicePipe::ClPipeDataElt::operator << (vector<short> &)
{

}

//+------------------------------------------------------------------------------------------------------------------
//
// method
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the DevicePipe class
//
//-------------------------------------------------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DevicePipe &dd)
{
//
// pipe date
//
	if (dd.time.tv_sec != 0)
	{
		time_t tmp_val = dd.time.tv_sec;
		char tmp_date[128];
#ifdef _TG_WINDOWS_
		ctime_s(tmp_date,128,&tmp_val);
#else
		ctime_r(&tmp_val,tmp_date);
#endif
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << dd.time.tv_sec << "," << dd.time.tv_usec << " sec) : ";
	}

//
// print pipe name
//

	o_str << dd.name << endl;

//
// Data element name/value
//

	for (size_t ctr = 0;ctr < dd.v_elt.size();ctr++)
	{
		o_str << "Data element name = " << dd.v_elt[ctr].name << endl;
		o_str << "Data element value:" << endl;

		if (dd.v_elt[ctr].LongSeq.operator->() != NULL)
			o_str << *(dd.v_elt[ctr].LongSeq.operator->());
		else if (dd.v_elt[ctr].ShortSeq.operator->() != NULL)
			o_str << *(dd.v_elt[ctr].ShortSeq.operator->());
		else
			o_str << *(dd.v_elt[ctr].DoubleSeq.operator->());

		if (ctr < dd.v_elt.size() - 1)
			o_str << endl;
	}

	return o_str;
}


} // End of Tango namepsace
