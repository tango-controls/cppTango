static const char *RcsId = "$Id$\n$Name$";

//+==================================================================================================================
//
// file :               W_pipe.cpp
//
// description :        C++ source code for the WPipe class. The WPipe class is a root class for derived
//						Pipe classes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WPipe::WPipe
//
// description :
//		Constructor for base class WPipe
//
//-----------------------------------------------------------------------------------------------------------------

WPipe::WPipe(const string &_name,const string &_desc,const string &_label,Tango::DispLevel _level)
:Pipe(_name,_desc,_label,_level,PIPE_READ_WRITE),w_ext(new WPipeExt)
{
}


string &WPipe::get_data_elt_name(size_t _ind)
{
	if (_ind > v_elt.size())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << v_elt.size() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"WPipe::get_data_elt_name()");
	}
	return v_elt[_ind].name;
}

int WPipe::get_data_elt_type(size_t _ind)
{
	if (_ind > v_elt.size())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << v_elt.size() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"WPipe::get_data_elt_data_type()");
	}
	return v_elt[_ind].type;
}

WPipe::SvrPipeDataElt &WPipe::get_data_elt(size_t _ind)
{
	if (_ind > v_elt.size())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << v_elt.size() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"WPipe::get_data_elt()");
	}
	return v_elt[_ind];
}

WPipe::SvrPipeDataElt &WPipe::get_data_elt(const string &elt_name)
{
	string elt_name_lower(elt_name);
	transform(elt_name_lower.begin(),elt_name_lower.end(),elt_name_lower.begin(),::tolower);

	vector<SvrPipeDataElt>::iterator ite;
	for (ite = v_elt.begin();ite != v_elt.end();++ite)
	{
		string tmp(ite->name);
		transform(tmp.begin(),tmp.end(),tmp.begin(),::tolower);

		if (tmp == elt_name_lower)
			break;
	}

	if (ite == v_elt.end())
	{
		stringstream ss;

		ss << "Data element " << elt_name << " not found in data blob";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"WPipe::get_data_elt()");
	}
	return *ite;
}

bool WPipe::SvrPipeDataElt::operator >> (short &datum)
{
	bool ret = true;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			datum = ShortSeq[0];
		else
			ret = false;
	}

	if (ret == false)
		upper->throw_type_except("DevShort","operator>>");

	return ret;
}
bool WPipe::SvrPipeDataElt::operator >> (DevLong &datum)
{
	bool ret = true;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			datum = LongSeq[0];
		else
			ret = false;
	}

	if (ret == false)
		upper->throw_type_except("DevLong","operator>>");

	return ret;
}
bool WPipe::SvrPipeDataElt::operator >> (double &datum)
{
	bool ret = true;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			datum = DoubleSeq[0];
		else
			ret = false;
	}

	if (ret == false)
		upper->throw_type_except("DevDouble","operator>>");

	return ret;
}

void WPipe::get_write_value(size_t _ind,short &datum)
{
	SvrPipeDataElt &de = get_data_elt(_ind);
	if (de.operator>>(datum) == false)
		throw_type_except("DevShort","get_write_value()");
}

void WPipe::get_write_value(const string &elt_name,short &datum)
{
	SvrPipeDataElt &de = get_data_elt(elt_name);
	if (de.operator>>(datum) == false)
		throw_type_except("DevShort","get_write_value()");
}

void WPipe::get_write_value(size_t _ind,double &datum)
{
	SvrPipeDataElt &de = get_data_elt(_ind);
	if (de.operator>>(datum) == false)
		throw_type_except("DevDouble","get_write_value()");
}

void WPipe::get_write_value(const string &elt_name,double &datum)
{
	SvrPipeDataElt &de = get_data_elt(elt_name);
	if (de.operator>>(datum) == false)
		throw_type_except("DevDouble","get_write_value()");
}


void WPipe::get_write_value(size_t _ind,DevLong &datum)
{
	SvrPipeDataElt &de = get_data_elt(_ind);
	if (de.operator>>(datum) == false)
		throw_type_except("DevLong","get_write_value()");
}

void WPipe::get_write_value(const string &elt_name,DevLong &datum)
{
	SvrPipeDataElt &de = get_data_elt(elt_name);
	if (de.operator>>(datum) == false)
		throw_type_except("DevLong","get_write_value()");
}

void WPipe::throw_type_except(const string &_ty,const string &meth)
{
	stringstream ss;
	ss << "Can't get data element into a " << _ty << " data type";
	string m_name("WPipe");
	m_name = m_name + meth;
	Except::throw_exception(API_PipeWrongArg,ss.str(),m_name);
}

WPipe &operator>>(WPipe &_pi,double &datum)
{
	_pi.get_data_elt(0) >> datum;
	return _pi;
}

WPipe &operator>>(WPipe &_pi,short &datum)
{
	_pi.get_data_elt(1) >> datum;
	return _pi;
}

WPipe &operator>>(WPipe &_pi,DevLong &datum)
{
	_pi.get_data_elt(2) >> datum;
	return _pi;
}

} // End of Tango namespace
