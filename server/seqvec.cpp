//=============================================================================
//
// file :               SeqVec.cpp
//
// description :        Dource file for CORBA sequence printing functions.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
//=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <seqvec.h>

namespace Tango
{

//
// These functions are not defined within the tango namespace because the VC++
// is not able to understand that the operator overloading functions is defined
// within a namespace x from th eoerand namespace (c++ or aCC is able to
// do it)
//

//=============================================================================
//
//			The sequence print facilities functions
//
// description :	These methods allow an easy way to print all sequence
//			element using the following syntax
//				cout << seq << endl;
//
//=============================================================================


ostream &operator<<(ostream &o,const DevVarCharArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << (short)v[i] << dec;
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarLongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarLong64Array &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarFloatArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarDoubleArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarBooleanArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = ";
		if (v[i] == true)
			o << "true";
		else
			o << "false";

		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarUShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarULongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarULong64Array &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarStringArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i].in();
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarStateArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << '\n';
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarEncodedArray &v)
{
	long nb_elt = v.length();
	for (long loop = 0;loop < nb_elt;loop++)
	{
		o << "Encoding string: " << v[loop].encoded_format << endl;
		long nb_data_elt = v[loop].encoded_data.length();
		for (long i = 0;i < nb_data_elt;i++)
		{
			o << "Data element number [" << i << "] = " << (int)v[loop].encoded_data[i];
			if (i < (nb_data_elt - 1))
				o << '\n';
		}
		if (loop < (nb_elt - 1))
			o << '\n';
	}
	return o;
}
} // End of Tango namespace
