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

DevicePipe::DevicePipe(const string &pipe_name):name(pipe_name),ext(Tango_nullptr)
{
}

DevicePipe::DevicePipe(const string &pipe_name,const string &root_blob_name):name(pipe_name),ext(Tango_nullptr)
{
	the_root_blob.set_name(root_blob_name);
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

//----------------------------------------------------------------------------------------------------------------
//
// DevicePipe::~DevicePipe() - destructor to destroy DevicePipe
//
//----------------------------------------------------------------------------------------------------------------

DevicePipe::~DevicePipe()
{
#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

DevicePipe &DevicePipe::operator[](const string &_na)
{
	the_root_blob.operator[](_na);
	return *this;
}

//*************************************************************************************************************
//
//						DevicePipeBlob class
//
//*************************************************************************************************************

//----------------------------------------------------------------------------------------------------------------
//
// DevicePipeBlob::~DevicePipeBlob() - destructor to destroy DevicePipeBlob instance
//
//----------------------------------------------------------------------------------------------------------------

DevicePipeBlob::~DevicePipeBlob()
{
	if (extract_delete == true)
		delete extract_elt_array;

#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_name
//
// description :
//		Get all data element name
//
// return:
//		vector with the data elt name
//
//-------------------------------------------------------------------------------------------------------------------

vector<string> DevicePipeBlob::get_data_elt_name()
{
	vector<string> v_str;
	size_t nb_elt = extract_elt_array->length();

	for (size_t loop = 0;loop < nb_elt;loop++)
	{
		v_str.push_back(string((*extract_elt_array)[loop].name.in()));
	}

	return v_str;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_name
//
// description :
//		Get data element name for a single data element
//
// argument :
//		in:
//			- _ind : Data element index in the blob
//
// return:
//		The data elt name
//
//-------------------------------------------------------------------------------------------------------------------

string DevicePipeBlob::get_data_elt_name(size_t _ind)
{
	if (_ind > extract_elt_array->length())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << get_data_elt_nb() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_data_elt_name()");
	}
	string tmp((*extract_elt_array)[_ind].name.in());
	return tmp;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_type
//
// description :
//		Get data element name for a single data element
//
// argument :
//		in:
//			- _ind : Data element index in the blob
//
// return:
//		The data elt name
//
//-------------------------------------------------------------------------------------------------------------------

int DevicePipeBlob::get_data_elt_type(size_t _ind)
{
	int ret = 0;

	if (_ind > extract_elt_array->length())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << get_data_elt_nb() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_data_elt_type()");
	}

//
// Do we have a Blob at this index?
//

	if ((*extract_elt_array)[_ind].inner_blob.length() != 0)
	{
		ret = 1000;
	}
	else
	{
		switch((*extract_elt_array)[_ind].value._d())
		{
			case ATT_SHORT:
			{
				const DevVarShortArray &dvsa = (*extract_elt_array)[_ind].value.short_att_value();
				if (dvsa.length() > 1)
					ret = DEVVAR_SHORTARRAY;
				else
					ret = DEV_SHORT;
			}
			break;

			case ATT_LONG:
			{
				const DevVarLongArray &dvla = (*extract_elt_array)[_ind].value.long_att_value();
				if (dvla.length() > 1)
					ret = DEVVAR_LONGARRAY;
				else
					ret = DEV_LONG;
			}
			break;

			case ATT_DOUBLE:
			{
				const DevVarDoubleArray &dvda = (*extract_elt_array)[_ind].value.double_att_value();
				if (dvda.length() > 1)
					ret = DEVVAR_DOUBLEARRAY;
				else
					ret = DEV_DOUBLE;
			}
			break;
		}
	}

	return ret;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_extract_ind_from_name
//
// description :
//		Get the index of a data element in the extract blob from its name. Throw exception if not found
//
// argument :
//		in:
//			- _na : Data element name
//
// return:
//		The data elt index.
//
//-------------------------------------------------------------------------------------------------------------------

size_t DevicePipeBlob::get_extract_ind_from_name(const string &_na)
{
	string lower_name(_na);
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);

	bool found = false;
	size_t nb = get_data_elt_nb();
	size_t loop;

	for (loop = 0;loop < nb;loop++)
	{
		string tmp((*extract_elt_array)[loop].name.in());
		transform(tmp.begin(),tmp.end(),tmp.begin(),::tolower);

		if (tmp == lower_name)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		stringstream ss;

		ss << "Can't get data element with name " << _na;
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_extract_ind_from_name()");
	}

	return loop;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::operator[]
//
// description :
//		Return the object itself with the extract counter set to the ind of the data element with name given as
//		parameter. This allows user to write code like
//			pipe_blob["MyDataElt"] >> sh;
//
// argument :
//		in:
//			- _na : Data element name
//
// return:
//		The DevicePipeBlob object itself
//
//-------------------------------------------------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator[](const string &_na)
{
	int ind = get_extract_ind_from_name(_na);
	extract_ctr = ind;

	return *this;
}

void DevicePipeBlob::set_data_elt_names(vector<string> &elt_names)
{
	if (insert_elt_array != Tango_nullptr)
		delete insert_elt_array;

	insert_elt_array = new DevVarPipeDataEltArray();
	insert_elt_array->length(elt_names.size());

	for (size_t loop = 0;loop < elt_names.size();loop++)
	{
		(*insert_elt_array)[loop].name = CORBA::string_dup(elt_names[loop].c_str());
		(*insert_elt_array)[loop].value.union_no_data(true);
	}
}

void DevicePipeBlob::set_data_elt_name(size_t _ind)
{
}

//******************************************************************************************************************

void DevicePipeBlob::operator<<(short &datum)
{
	if (insert_ctr > insert_elt_array->length() - 1)
		throw_too_many("operator<<");

	DevVarShortArray dvsa;
	dvsa.length(1);
	dvsa[0] = datum;

	(*insert_elt_array)[insert_ctr].value.short_att_value(dvsa);
	insert_ctr++;
}

void DevicePipeBlob::operator<<(DevLong &datum)
{
	if (insert_ctr > insert_elt_array->length() - 1)
		throw_too_many("operator<<");

	DevVarLongArray dvla;
	dvla.length(1);
	dvla[0] = datum;

	(*insert_elt_array)[insert_ctr].value.long_att_value(dvla);
	insert_ctr++;
}

void DevicePipeBlob::operator<<(double &datum)
{
	if (insert_ctr > insert_elt_array->length() - 1)
		throw_too_many("operator<<");

	DevVarDoubleArray dvda;
	dvda.length(1);
	dvda[0] = datum;

	(*insert_elt_array)[insert_ctr].value.double_att_value(dvda);
	insert_ctr++;
}

void DevicePipeBlob::operator<<(DevicePipeBlob &datum)
{
	if (insert_ctr > insert_elt_array->length() - 1)
		throw_too_many("operator<<");

	DevVarPipeDataEltArray *tmp_ptr = datum.get_insert_data();
	if (tmp_ptr != Tango_nullptr)
	{
		(*insert_elt_array)[insert_ctr].inner_blob.replace(tmp_ptr->length(),tmp_ptr->length(),tmp_ptr->get_buffer(),true);
		insert_ctr++;
	}
}

//******************************************************************************************************************

bool DevicePipeBlob::operator >> (short &datum)
{
	bool ret = true;
	bool too_many_elt = false;

	if (extract_ctr > extract_elt_array->length() - 1)
		too_many_elt = true;
	else
	{
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value);
		if (uni_ptr->_d() != ATT_SHORT)
			ret = false;
		else
		{
			datum = (uni_ptr->short_att_value())[0];
			extract_ctr++;
		}
	}

	if (too_many_elt == true)
		throw_too_many("operator>>");

	if (ret == false)
		throw_type_except("DevShort","operator>>");

	return ret;
}

bool DevicePipeBlob::operator >> (vector<short> &datum)
{
	bool ret = true;
	bool too_many_elt = false;

	if (extract_ctr > extract_elt_array->length() - 1)
		too_many_elt = true;
	else
	{
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value);
		if (uni_ptr->_d() != ATT_SHORT)
			ret = false;
		else
		{
			const DevVarShortArray &dvsa = uni_ptr->short_att_value();
			datum << dvsa;
			extract_ctr++;
		}
	}

	if (too_many_elt == true)
		throw_too_many("operator>>");

	if (ret == false)
		throw_type_except("DevShort","operator>>");

	return ret;
}

bool DevicePipeBlob::operator >> (DevLong &datum)
{
	bool ret = true;
	bool too_many_elt = false;

	if (extract_ctr > extract_elt_array->length() - 1)
		too_many_elt = true;
	else
	{
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value);
		if (uni_ptr->_d() != ATT_LONG)
			ret = false;
		else
		{
			datum = (uni_ptr->long_att_value())[0];
			extract_ctr++;
		}
	}

	if (too_many_elt == true)
		throw_too_many("operator>>");

	if (ret == false)
		throw_type_except("DevLong","operator>>");

	return ret;
}

bool DevicePipeBlob::operator >> (double &datum)
{
	bool ret = true;
	bool too_many_elt = false;

	if (extract_ctr > extract_elt_array->length() - 1)
		too_many_elt = true;
	else
	{
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value);
		if (uni_ptr->_d() != ATT_DOUBLE)
			ret = false;
		else
		{
			datum = (uni_ptr->double_att_value())[0];
			extract_ctr++;
		}
	}

	if (too_many_elt == true)
		throw_too_many("operator>>");

	if (ret == false)
		throw_type_except("DevDouble","operator>>");

	return ret;
}

bool DevicePipeBlob::operator >> (DevicePipeBlob &datum)
{
	bool ret = true;
	bool too_many_elt = false;

	if (extract_ctr > extract_elt_array->length() - 1)
		too_many_elt = true;
	else
	{
		if ((*extract_elt_array)[extract_ctr].inner_blob.length() == 0)
			ret = false;
		else
		{
			datum.set_extract_data(&((*extract_elt_array)[extract_ctr].inner_blob));
			string tmp((*extract_elt_array)[extract_ctr].name.in());
			datum.set_name(tmp);
			datum.reset_extract_ctr();
			extract_ctr++;
		}
	}

	if (too_many_elt == true)
		throw_too_many("operator>>");

	if (ret == false)
		throw_type_except("DevDouble","operator>>");

	return ret;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_type_except
//
// description :
//		Throw exception when wrong data type is used
//
// argument:
//		in :
//			- _ty: data type name
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_type_except(const string &_ty,const string &_meth)
{
	stringstream ss;

	ss << "Can't get data element " << extract_ctr << " (numbering starting from 0) into a " << _ty << " data type";
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	Except::throw_exception(API_PipeWrongArg,ss.str(),m_name);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_too_many
//
// description :
//		Throw exception when user try to insert/extract too many data from the blob
//
// argument:
//		in :
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_too_many(const string &_meth)
{
	stringstream ss;
	ss << "Not enough data element in blob (" << get_data_elt_nb() << " data elt in created/received blob)";
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	Except::throw_exception(API_PipeWrongArg,ss.str(),m_name);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method
// 		DevicePipeBlob::print
//
// description :
//		Print blob content. Due to recusrsive type, it was not possible to write this feature as a classical
//		<< operator.
//
// argument:
//		in :
//			- o_str : The stream used for printing
//			- indent : Indentation level for printing in case of blob inside blob
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::print(ostream &o_str,int indent)
{

//
// Print blob name (if any)
//

	if (name.size() != 0)
	{
		for (int loop = 0;loop < indent;loop++)
			o_str << "\t";
		o_str << "Blob name = " << name << endl;
	}


//
// Data element name/value
//

	const DevVarPipeDataEltArray *dvpdea = get_extract_data();
	for (size_t ctr = 0;ctr < dvpdea->length();ctr++)
	{
		for (int loop = 0;loop < indent;loop++)
			o_str << "\t";
		o_str << "Data element name = " << (*dvpdea)[ctr].name.in() << endl;
		for (int loop = 0;loop < indent;loop++)
			o_str<< "\t";
		o_str << "Data element value:" << endl;

		for (int loop = 0;loop < indent;loop++)
			o_str << "\t";

		switch ((*dvpdea)[ctr].value._d())
		{
			case ATT_SHORT:
			{
				const DevVarShortArray &dvsa = (*dvpdea)[ctr].value.short_att_value();
				o_str << dvsa;
			}
			break;

			case ATT_LONG:
			{
				const DevVarLongArray &dvla = (*dvpdea)[ctr].value.long_att_value();
				o_str << dvla;
			}
			break;

			case ATT_DOUBLE:
			{
				const DevVarDoubleArray &dvda = (*dvpdea)[ctr].value.double_att_value();
				o_str << dvda;
			}
			break;
		}

		if ((*dvpdea)[ctr].inner_blob.length() != 0)
		{
			DevicePipeBlob dpb((*dvpdea)[ctr].name.in());
			dpb.print(o_str,indent + 1);
		}

		if (ctr < dvpdea->length() - 1)
			o_str << endl;
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the DevicePipe class
//
//-------------------------------------------------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DevicePipe &dd)
{

//
// Pipe date
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
// Print pipe name
//

	o_str << dd.name << endl;

//
// Print blob
//

	DevicePipeBlob &dpb = dd.the_root_blob;
	dpb.print(o_str,0);

	return o_str;
}

} // End of Tango namepsace
