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

//-----------------------------------------------------------------------------
//
// DevicePipe::DevicePipe() - copy constructor to create DevicePipe
//
//-----------------------------------------------------------------------------

DevicePipe::DevicePipe(const DevicePipe & source):ext(Tango_nullptr)
{
	name = source.name;
	time = source.time;
	the_root_blob = source.the_root_blob;

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

//-----------------------------------------------------------------------------
//
// DevicePipe::operator= - assignment operator for DevicePipe class
//
//-----------------------------------------------------------------------------

DevicePipe &DevicePipe::operator=(const DevicePipe &rhs)
{
	if (this != &rhs)
    {
		name = rhs.name;
    	time = rhs.time;
		the_root_blob = rhs.the_root_blob;

#ifdef HAS_UNIQUE_PTR
		if (rhs.ext.get() != NULL)
		{
			ext.reset(new DevicePipeExt);
			*(ext.get()) = *(rhs.ext.get());
		}
#else
		if (rhs.ext != NULL)
		{
			ext = new DevicePipeExt();
			*ext = *(rhs.ext);
		}
		else
			ext = NULL;
#endif
    }
	return *this;
}

//-----------------------------------------------------------------------------
//
// DevicePipe::DevicePipe() - Move copy constructor to create DevicePipe
//
//-----------------------------------------------------------------------------

DevicePipe::DevicePipe(DevicePipe && source):ext(Tango_nullptr)
{
	name = move(source.name);
	time = source.time;
	the_root_blob = move(source.the_root_blob);

    if (source.ext.get() != NULL)
    {
        ext = move(source.ext);
    }
}

//-----------------------------------------------------------------------------
//
// DevicePipe::operator= - Move assignment operator for DevicePipe class
//
//-----------------------------------------------------------------------------

DevicePipe &DevicePipe::operator=(DevicePipe &&rhs)
{
	name = move(rhs.name);
	time = rhs.time;
	the_root_blob = move(rhs.the_root_blob);

	if (rhs.ext.get() != NULL)
	{
		ext = move(rhs.ext);
	}
	else
		ext.reset();

	return *this;
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

//------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DevicePipeBlob::DevicePipeBlob()
//
// description:
//		Constructor for DevicePipeBlob class
//
//------------------------------------------------------------------------------------------------------------------

DevicePipeBlob::DevicePipeBlob():failed(false),insert_elt_array(Tango_nullptr),insert_ctr(0),
extract_elt_array(Tango_nullptr),extract_ctr(0),extract_delete(false),ext(Tango_nullptr)
{
	exceptions_flags.set();
	ext_state.reset();
}


DevicePipeBlob::DevicePipeBlob(const string &blob_name):name(blob_name),failed(false),
insert_elt_array(Tango_nullptr),insert_ctr(0),extract_elt_array(Tango_nullptr),extract_ctr(0),
extract_delete(false),ext(Tango_nullptr)
{
	exceptions_flags.set();
	ext_state.reset();
}

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

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::DevicePipeBlob() - copy constructor to create DevicePipeBlob
//
//-----------------------------------------------------------------------------

DevicePipeBlob::DevicePipeBlob(const DevicePipeBlob & source):ext(Tango_nullptr)
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	ext_state = source.ext_state;
	failed = source.failed;

	if (source.insert_elt_array != Tango_nullptr)
		(*insert_elt_array) = (*source.insert_elt_array);
	else
		insert_elt_array = Tango_nullptr;
	insert_ctr = source.insert_ctr;

	if (source.extract_elt_array != Tango_nullptr)
	{
		DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
		*tmp = (*source.extract_elt_array);
		set_extract_data(tmp);
		extract_delete = true;
	}
	else
	{
		extract_delete = source.extract_delete;
	}
	extract_ctr = source.extract_ctr;

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DevicePipeBlobExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DevicePipeBlobExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::operator= - assignment operator for DevicePipeBlob class
//
//-----------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator=(const DevicePipeBlob &rhs)
{
	if (this != &rhs)
    {
		name = rhs.name;
		exceptions_flags = rhs.exceptions_flags;
		ext_state = rhs.ext_state;
		failed = rhs.failed;

		if (rhs.insert_elt_array != Tango_nullptr)
			(*insert_elt_array) = (*rhs.insert_elt_array);
		else
			insert_elt_array = Tango_nullptr;
		insert_ctr = rhs.insert_ctr;

		if (rhs.extract_elt_array != Tango_nullptr)
		{
			DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
			*tmp = (*rhs.extract_elt_array);
			set_extract_data(tmp);
			extract_delete = true;
		}
		else
		{
			extract_delete = rhs.extract_delete;
		}
		extract_ctr = rhs.extract_ctr;

#ifdef HAS_UNIQUE_PTR
        if (rhs.ext.get() != NULL)
        {
            ext.reset(new DevicePipeBlobExt);
            *(ext.get()) = *(rhs.ext.get());
        }
        else
            ext.reset();
#else
        delete ext;
        if (rval.ext != NULL)
        {
            ext = new DevicePipeBlobExt();
            *ext = *(rval.ext);
        }
        else
            ext = NULL;
#endif
    }
	return *this;
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::DevicePipeBlob() - Move copy constructor to create DevicePipeBlob
//
//-----------------------------------------------------------------------------

DevicePipeBlob::DevicePipeBlob(DevicePipeBlob && source):ext(Tango_nullptr)
{
	name = move(source.name);
	exceptions_flags = source.exceptions_flags;
	ext_state = source.ext_state;
	failed = source.failed;

	if (source.insert_elt_array != Tango_nullptr)
		(*insert_elt_array) = (*source.insert_elt_array);
	else
		insert_elt_array = Tango_nullptr;
	insert_ctr = source.insert_ctr;

	if (source.extract_elt_array != Tango_nullptr)
	{
		DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
		*tmp = (*source.extract_elt_array);
		set_extract_data(tmp);
		extract_delete = true;
	}
	else
	{
		extract_delete = source.extract_delete;
	}
	extract_ctr = source.extract_ctr;

    if (source.ext.get() != NULL)
    {
        ext = move(source.ext);
    }
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::operator= - Move assignment operator for DevicePipeBlob class
//
//-----------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator=(DevicePipeBlob &&rhs)
{
	name = move(rhs.name);
	exceptions_flags = rhs.exceptions_flags;
	ext_state = rhs.ext_state;
	failed = rhs.failed;

	if (rhs.insert_elt_array != Tango_nullptr)
		insert_elt_array = rhs.insert_elt_array;
	rhs.insert_elt_array = Tango_nullptr;
	insert_ctr = rhs.insert_ctr;

	if (rhs.extract_elt_array != Tango_nullptr)
		extract_elt_array = rhs.extract_elt_array;
	rhs.extract_elt_array = Tango_nullptr;
	extract_delete = rhs.extract_delete;
	extract_ctr = rhs.extract_ctr;


    if (rhs.ext.get() != NULL)
    {
        ext = move(rhs.ext);
    }
    else
		ext.reset();

	return *this;
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
		ret = DEV_PIPE_BLOB;
	}
	else
	{
		switch((*extract_elt_array)[_ind].value._d())
		{
			case ATT_BOOL:
			{
				const DevVarBooleanArray &dvba = (*extract_elt_array)[_ind].value.bool_att_value();
				if (dvba.length() > 1)
					ret = DEVVAR_BOOLEANARRAY;
				else
					ret = DEV_BOOLEAN;
			}
			break;

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

			case ATT_LONG64:
			{
				const DevVarLong64Array &dvla = (*extract_elt_array)[_ind].value.long64_att_value();
				if (dvla.length() > 1)
					ret = DEVVAR_LONG64ARRAY;
				else
					ret = DEV_LONG64;
			}
			break;

			case ATT_FLOAT:
			{
				const DevVarFloatArray &dvfa = (*extract_elt_array)[_ind].value.float_att_value();
				if (dvfa.length() > 1)
					ret = DEVVAR_FLOATARRAY;
				else
					ret = DEV_FLOAT;
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

			case ATT_UCHAR:
			{
				const DevVarUCharArray &dvuch = (*extract_elt_array)[_ind].value.uchar_att_value();
				if (dvuch.length() > 1)
					ret = DEVVAR_CHARARRAY;
				else
					ret = DEV_UCHAR;
			}
			break;

			case ATT_USHORT:
			{
				const DevVarUShortArray &dvuch = (*extract_elt_array)[_ind].value.ushort_att_value();
				if (dvuch.length() > 1)
					ret = DEVVAR_USHORTARRAY;
				else
					ret = DEV_USHORT;
			}
			break;

			case ATT_ULONG:
			{
				const DevVarULongArray &dvulo = (*extract_elt_array)[_ind].value.ulong_att_value();
				if (dvulo.length() > 1)
					ret = DEVVAR_ULONGARRAY;
				else
					ret = DEV_ULONG;
			}
			break;

			case ATT_ULONG64:
			{
				const DevVarULong64Array &dvulo64 = (*extract_elt_array)[_ind].value.ulong64_att_value();
				if (dvulo64.length() > 1)
					ret = DEVVAR_ULONG64ARRAY;
				else
					ret = DEV_ULONG;
			}
			break;

			case ATT_STRING:
			{
				const DevVarStringArray &dvsa = (*extract_elt_array)[_ind].value.string_att_value();
				if (dvsa.length() > 1)
					ret = DEVVAR_STRINGARRAY;
				else
					ret = DEV_STRING;
			}
			break;

			case ATT_STATE:
			{
				ret = DEV_STATE;
			}
			break;

			case ATT_ENCODED:
			{
				ret = DEV_ENCODED;
			}
			break;

			default:
			Except::throw_exception(API_PipeWrongArg,
									"Unsupported data type in data element! (ATT_NO_DATA, DEVICE_STATE)",
									"DevicePipeBlob::get_data_elt_type()");
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
	insert_elt_array = new DevVarPipeDataEltArray();
	insert_elt_array->length(elt_names.size());

	for (size_t loop = 0;loop < elt_names.size();loop++)
	{
		(*insert_elt_array)[loop].name = CORBA::string_dup(elt_names[loop].c_str());
		(*insert_elt_array)[loop].value.union_no_data(true);
	}
}

void DevicePipeBlob::set_data_elt_nb(size_t _nb)
{
	insert_elt_array = new DevVarPipeDataEltArray();
	insert_elt_array->length(_nb);

	for (size_t loop = 0;loop < _nb;loop++)
	{
		(*insert_elt_array)[loop].value.union_no_data(true);
	}
}

//******************************************************************************************************************

DevicePipeBlob & DevicePipeBlob::operator<<(short &datum)
{
	INSERT_BASIC_TYPE(DevVarShortArray,short_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevLong &datum)
{
	INSERT_BASIC_TYPE(DevVarLongArray,long_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(double &datum)
{
	INSERT_BASIC_TYPE(DevVarDoubleArray,double_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevicePipeBlob &datum)
{
	if (insert_ctr > insert_elt_array->length() - 1)
		throw_too_many("operator<<",false);

	DevVarPipeDataEltArray *tmp_ptr = datum.get_insert_data();
	if (tmp_ptr != Tango_nullptr)
	{
		(*insert_elt_array)[insert_ctr].inner_blob.replace(tmp_ptr->length(),tmp_ptr->length(),tmp_ptr->get_buffer(),true);
		(*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(datum.get_name().c_str());
		insert_ctr++;
	}

	return *this;
}

//******************************************************************************************************************

DevicePipeBlob &DevicePipeBlob::operator >> (short &datum)
{
	EXTRACT_BASIC_TYPE(ATT_SHORT,short_att_value,"DevShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevLong &datum)
{
	EXTRACT_BASIC_TYPE(ATT_LONG,long_att_value,"DevLong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (double &datum)
{
	EXTRACT_BASIC_TYPE(ATT_DOUBLE,double_att_value,"DevDouble")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<short> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_SHORT,short_att_value,DevVarShortArray,"DevShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevicePipeBlob &datum)
{
	failed = false;
	ext_state.reset();

	if (extract_ctr > extract_elt_array->length() - 1)
		ext_state.set(notenoughde_flag);
	else
	{
		if ((*extract_elt_array)[extract_ctr].inner_blob.length() == 0)
		{
			if ((*extract_elt_array)[extract_ctr].value._d() == ATT_NO_DATA)
				ext_state.set(isempty_flag);
			else
				ext_state.set(wrongtype_flag);
		}
		else
		{
			datum.set_extract_data(&((*extract_elt_array)[extract_ctr].inner_blob));
			string tmp((*extract_elt_array)[extract_ctr].inner_blob_name.in());
			datum.set_name(tmp);
			datum.reset_extract_ctr();
			extract_ctr++;
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true)
		throw_is_empty("operator>>");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator>>",true);

	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true)
		throw_type_except("DevicePipeBlob","operator>>");

	return *this;
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
	ApiDataExcept::throw_exception(API_IncompatibleArgumentType,ss.str(),m_name.c_str());
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
//			- _extract : Flag set to true if this method is called during extraction. Otherwise, false
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_too_many(const string &_meth,bool _extract)
{
	stringstream ss;
	ss << "Not enough data element in blob (";
	if (_extract == true)
		ss << get_data_elt_nb();
	else
		ss << insert_elt_array->length();
	ss << " data elt in created/received blob)";
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	ApiDataExcept::throw_exception(API_PipeWrongArg,ss.str(),m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_is_empty
//
// description :
//		Throw exception when user try to insert/extract data from one empty data element
//
// argument:
//		in :
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_is_empty(const string &_meth)
{
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	ApiDataExcept::throw_exception(API_PipeWrongArg,"The data element is empmty",m_name.c_str());
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
//			- insert_extract: Flag set to true is insert data should be printed, else extract data are printed
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::print(ostream &o_str,int indent,bool insert_extract)
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

	const DevVarPipeDataEltArray *dvpdea;
	if (insert_extract == false)
		dvpdea = get_extract_data();
	else
		dvpdea = get_insert_data();

	if (dvpdea != Tango_nullptr)
	{
		for (size_t ctr = 0;ctr < dvpdea->length();ctr++)
		{
			for (int loop = 0;loop < indent;loop++)
				o_str << "\t";
			o_str << "Data element name = " << (*dvpdea)[ctr].name.in() << endl;
			for (int loop = 0;loop < indent;loop++)
				o_str<< "\t";
			o_str << "Data element value:" << endl;

//
// Print indent level
//

			for (int loop = 0;loop < indent;loop++)
				o_str << "\t";

//
// Print data element value
//

			switch ((*dvpdea)[ctr].value._d())
			{
				case ATT_BOOL:
				{
					const DevVarBooleanArray &dvba = (*dvpdea)[ctr].value.bool_att_value();
					o_str << dvba;
				}
				break;

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

				case ATT_LONG64:
				{
					const DevVarLong64Array &dvlo64 = (*dvpdea)[ctr].value.long64_att_value();
					o_str << dvlo64;
				}
				break;

				case ATT_FLOAT:
				{
					const DevVarFloatArray &dvfa = (*dvpdea)[ctr].value.float_att_value();
					o_str << dvfa;
				}
				break;

				case ATT_DOUBLE:
				{
					const DevVarDoubleArray &dvda = (*dvpdea)[ctr].value.double_att_value();
					o_str << dvda;
				}
				break;

				case ATT_UCHAR:
				{
					const DevVarCharArray &dvda = (*dvpdea)[ctr].value.uchar_att_value();
					o_str << dvda;
				}
				break;

				case ATT_USHORT:
				{
					const DevVarUShortArray &dvda = (*dvpdea)[ctr].value.ushort_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ULONG:
				{
					const DevVarULongArray &dvda = (*dvpdea)[ctr].value.ulong_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ULONG64:
				{
					const DevVarULong64Array &dvda = (*dvpdea)[ctr].value.ulong64_att_value();
					o_str << dvda;
				}
				break;

				case ATT_STRING:
				{
					const DevVarStringArray &dvda = (*dvpdea)[ctr].value.string_att_value();
					o_str << dvda;
				}
				break;

				case ATT_STATE:
				{
					const DevVarStateArray &dvda = (*dvpdea)[ctr].value.state_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ENCODED:
				{
					const DevVarEncodedArray &dvda = (*dvpdea)[ctr].value.encoded_att_value();
					o_str << dvda;
				}
				break;

				case ATT_NO_DATA:
				case DEVICE_STATE:
				o_str << "Unsupported data type in data element (ATT_NO_DATA, DEVICE_STATE)!" << endl;
				break;
			}

//
// Print inner blob if any
//

			if ((*dvpdea)[ctr].inner_blob.length() != 0)
			{
				DevicePipeBlob dpb((*dvpdea)[ctr].name.in());
				dpb.print(o_str,indent + 1,true);
				dpb.print(o_str,indent + 1,false);
			}

			if (ctr < dvpdea->length() - 1)
				o_str << endl;
		}
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

	DevicePipeBlob &dpb = dd.get_root_blob();
	if (dpb.get_insert_data() == Tango_nullptr && dpb.get_extract_data() == Tango_nullptr)
		o_str << "DevicePipe is empty";
	else
	{
		dpb.print(o_str,0,true);
		dpb.print(o_str,0,false);
	}

	return o_str;
}

/*DevicePipeBlob &operator>>(DevicePipeBlob &_dpb,short &_datum)
{
	_dpb.operator>>(_datum);
	return _dpb;
}
DevicePipeBlob &operator>>(DevicePipeBlob &_dpb,double &_datum)
{
	_dpb.operator>>(_datum);
	return _dpb;
}*/

} // End of Tango namepsace
