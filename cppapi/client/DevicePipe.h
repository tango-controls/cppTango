//===================================================================================================================
//
// DevicePipe.h - include file for TANGO device api class DevicePipe
//
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
//===================================================================================================================

#ifndef _DEVICEPIPE_H
#define _DEVICEPIPE_H

/****************************************************************************************
 * 																						*
 * 					The DevicePipe class					   						    *
 * 					--------------------											    *
 * 																						*
 ***************************************************************************************/


/**
 * Fundamental type for receiving data from device pipes
 *
 * This is the fundamental type for receiving data from device pipes.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DevicePipe
{

public :
///@privatesection

	DevicePipe(const DevicePipe &);

///@publicsection
/**@name Constructors */
//@{
/**
 * Create a DevicePipe object.
 *
 * Default constructor. The instance is empty
 *
 */
    DevicePipe();
//@}

/**
 * Print a DevicePipe instance
 *
 * Is an utility function to easily print the contents of a DevicePipe object. This function knows all types
 * which could be inserted in a DevicePipe object and print them accordingly. A special string is printed if
 * the DevicePipe object is empty
 * @code
 * DeviceProxy *dev = new DeviceProxy(“...”);
 * DevicePipe out;
 *
 * out = dev->read_pipe(“MyPipe”);
 * cout << “Pipe content: ” << out << endl;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] dd The instance to be printed
 */
	friend ostream &operator<<(ostream &str,DevicePipe &dd);

public :
///@privatesection

	string 					name;
	TimeVal 				time;
	vector<string>			elt_names;

	struct ClPipeDataElt
	{
		string		name;
		long		type;

		DevVarLongArray_var 	LongSeq;
		DevVarShortArray_var 	ShortSeq;
		DevVarDoubleArray_var 	DoubleSeq;

		ClPipeDataElt(const string &_na,long _ty):name(_na),type(_ty) {}
		ClPipeDataElt(const string &_na):name(_na) {}

		bool operator >> (short &);
		bool operator >> (DevLong &);
		bool operator >> (double &);
		bool operator >> (vector<short> &);
	};

	vector<ClPipeDataElt>	v_elt;

	ClPipeDataElt &operator[](const string &);
	ClPipeDataElt &operator[](size_t);

private:
    class DevicePipeExt
    {
    public:
        DevicePipeExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DevicePipeExt>   ext;
#else
	DevicePipeExt		        *ext;			// Class extension
#endif
};

#endif /* _DEVICEPIPE_H */
