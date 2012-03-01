//=============================================================================
//
// file :               SeqVec.h
//
// description :        Include for the utilities function to ease CORBA
//			sequences from standard C++ vector or in the
//			oposite way.
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
//
//=============================================================================

#ifndef _SECVEC_H
#define _SECVEC_H


#include <tango.h>


namespace Tango
{


//
// These functions are not defined within the tango namespace because the VC++
// is not able to understand that the operator overloading functions is defined
// within a namespace x from the operand namespace (c++ or aCC is able to
// do it)
//

//=============================================================================
//
//			Operator for the unsigned char array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarCharArray) and C++ vectors
//
//=============================================================================

//#define NOSPACEINDOC_SEQVEC

/** @defgroup Ope Operator overloading functions
 * Overloading of the << operator between C++ vector and Tango types */
//@{
/**
 *
 * Init a DevVarCharArray from a C++ vector of char(s).
 *
 * @param lval The DevVarCharArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarCharArray &lval,const vector<unsigned char> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of char from a DevVarCharArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarCharArray
 */
inline void operator<<(vector<unsigned char> &lval,const DevVarCharArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Short array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarShortArray from a C++ vector of short(s).
 *
 * @param lval The DevVarShortArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarShortArray &lval,const vector<short> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of short from a DevVarShortArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarShortArray
 */
inline void operator<<(vector<short> &lval,const DevVarShortArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLongArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarLongArray from a C++ vector of DevLong(s).
 *
 * @param lval The DevVarLongArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarLongArray &lval,const vector<DevLong> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of DevLong from a DevVarLongArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarLongArray
 */
inline void operator<<(vector<DevLong> &lval,const DevVarLongArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Long Long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLong64Array) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarLongArray from a C++ vector of DevLong64(s).
 *
 * @param lval The DevVarLong64Array to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarLong64Array &lval,const vector<DevLong64> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarLong64Array.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarLong64Array
 */
inline void operator<<(vector<DevLong64> &lval,const DevVarLong64Array &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Float array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLongArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarFloatArray from a C++ vector of float(s).
 *
 * @param lval The DevVarFloatArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarFloatArray &lval,const vector<float> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarFloatArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarFloatArray
 */

inline void operator<<(vector<float> &lval,const DevVarFloatArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Double array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarDoubleArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarDoubleArray from a C++ vector of double(s).
 *
 * @param lval The DevVarDoubleArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarDoubleArray &lval,const vector<double> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarDoubleArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarDoubleArray
 */

inline void operator<<(vector<double> &lval,const DevVarDoubleArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Boolean array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarBooleanArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarBooleanArray from a C++ vector of bool(s).
 *
 * @param lval The DevVarBooleanArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarBooleanArray &lval,const vector<bool> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarBooleanArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarBooleanArray
 */

inline void operator<<(vector<bool> &lval,const DevVarBooleanArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Unsigned short array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarUShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarUShortArray from a C++ vector of unsigned short(s).
 *
 * @param lval The DevVarUShortArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarUShortArray &lval,const vector<unsigned short> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarUShortArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarUShortArray
 */

inline void operator<<(vector<unsigned short> &lval,const DevVarUShortArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Unsigned long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarUShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarULongArray from a C++ vector of DevULong(s).
 *
 * @param lval The DevVarULongArray to be initialised
 * @param rval The C++ vector
 */

inline void operator<<(DevVarULongArray &lval,const vector<DevULong> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarULongArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarULongArray
 */
inline void operator<<(vector<DevULong> &lval,const DevVarULongArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Unsigned long long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarULong64Array) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarULong64Array from a C++ vector of DevULong64(s).
 *
 * @param lval The DevVarULong64Array to be initialised
 * @param rval The C++ vector
 */

inline void operator<<(DevVarULong64Array &lval,const vector<DevULong64> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarULong64Array.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarULong64Array
 */
inline void operator<<(vector<DevULong64> &lval,const DevVarULong64Array &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the string array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarStringArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarStringArray from a C++ vector of string(s).
 *
 * @param lval The DevVarStringArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarStringArray &lval,const vector<string> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = CORBA::string_dup(rval[i].c_str());
}

/**
 * Init a C++ vector of long from a DevVarStringArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarStringArray
 */
inline void operator<<(vector<string> &lval,const DevVarStringArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	string str;
	for (long i = 0;i < nb_elt;i++)
	{
		str = rval[i];
		lval.push_back(str);
	}
}

//=============================================================================
//
//			Operator for the DevState array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarStateArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarStateArray from a C++ vector of state(s).
 *
 * @param lval The DevVarStateArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarStateArray &lval,const vector<DevState> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarStateArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarStateArray
 */
inline void operator<<(vector<DevState> &lval,const DevVarStateArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//@}

//=============================================================================
//
//			Print operator for sequence types
//
// description :	These operators allow simple printing of sequences
//
//=============================================================================
//#ifndef TANGO_HAS_LOG4TANGO
/** @defgroup Eas Easy printing operator overloading functions
 * Overloading of the << operator between C++ ostream and some Tango types */
//@{
/**
 * Print a DevVarCharArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarCharArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarCharArray &rval);
/**
 * Print a DevVarShortArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarShortArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarShortArray &rval);
/**
 * Print a DevVarLongArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarLongArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarLongArray &rval);
/**
 * Print a DevVarLong64Array.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarLong64Array sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarLong64Array &rval);
/**
 * Print a DevVarFloatArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarFloatArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarFloatArray &rval);
/**
 * Print a DevVarDoubleArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarDoubleArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarDoubleArray &rval);
/**
 * Print a DevVarBooleanArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarBooleanArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarBooleanArray &rval);
/**
 * Print a DevVarUShortArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarUShortArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarUShortArray &rval);
/**
 * Print a DevVarULongArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarULongArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarULongArray &rval);
/**
 * Print a DevVarULong64Array.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarULong64Array sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarULong64Array &rval);
/**
 * Print a DevVarStringArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarStringArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarStringArray &rval);
/**
 * Print a DevVarStateArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarStateArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarStateArray &rval);
/**
 * Print a DevVarEncodedArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarEncodedArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarEncodedArray &rval);
//@}

//#endif // TANGO_HAS_LOG4TANGO

} // End of Tango namespace


#endif /* _SECVEC_ */
