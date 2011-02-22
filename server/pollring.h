//=============================================================================
//
// file :               PollRing.h
//
// description :        Include for the PollRing object. This class implements
//                      the polling ring buffer. Command result or attribute
//			values are stored in this buffer manages as a ring
//			buffer.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// $Log$
// Revision 3.8  2008/12/19 14:27:08  taurel
// - First changes for compatibility between IDL 3 and IDL 4
//
// Revision 3.7  2008/12/17 09:50:59  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.6  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.5  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.4  2008/05/20 12:44:12  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.3.2.3  2008/05/20 06:17:46  taurel
// - Last commit before merge with trunk
// (start the implementation of the new DevEncoded data type)
//
// Revision 3.3.2.2  2007/11/20 14:40:19  taurel
// - Add the new way to retrieve command history from polling buffer
// implemented in Tango V7
//
// Revision 3.3.2.1  2007/11/16 14:12:35  taurel
// - Added a new IDL interface (Device_4)
// - Added a new way to get attribute history from polling buffer (must faster)
//
// Revision 3.3  2007/04/16 14:57:42  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.2  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.0.4.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.0  2003/03/25 16:44:10  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.8  2002/12/16 12:07:33  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:23  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:11  taurel
// See Tango WEB pages for list of changes
//
//=============================================================================

#ifndef _POLLRING_H
#define _POLLRING_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The RingElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================


class RingElt
{
public:
	RingElt();

	CORBA::Any					*cmd_result;
	Tango::AttributeValueList	*attr_value;
	Tango::AttributeValueList_3	*attr_value_3;
	Tango::AttributeValueList_4	*attr_value_4;
	Tango::DevFailed			*except;
	struct timeval				when;
};

inline bool operator<(const RingElt &l,const RingElt &r)
{
	return true;
}

inline bool operator==(const RingElt &l,const RingElt &r)
{
	return true;
}

//=============================================================================
//
//			The PollRing class
//
// description :	Class to implement the ring buffer itself. This is mainly
//			a vector of RingElt managed as a circular buffer
//
//=============================================================================

class PollRing
{
public:
	PollRing();
	PollRing(long);
	~PollRing();

	void insert_data(CORBA::Any *,struct timeval &);
	void insert_data(Tango::AttributeValueList *,struct timeval &);
	void insert_data(Tango::AttributeValueList_3 *,struct timeval &);
	void insert_data(Tango::AttributeValueList_4 *,struct timeval &);
	void insert_except(Tango::DevFailed *,struct timeval &);

	void force_copy_data(Tango::AttributeValueList_4 *);
	
	void get_delta_t(vector<double> &,long nb);
	struct timeval get_last_insert_date();
	bool is_empty() {if (nb_elt == 0) return true;else return false;}
	
	bool is_last_an_error();
	bool is_last_cmd_an_error();
	bool is_last_attr_an_error();
	
	Tango::DevFailed *get_last_except();
	Tango::DevErrorList &get_last_attr_error();
	
	CORBA::Any *get_last_cmd_result();
	Tango::AttributeValue &get_last_attr_value();
	Tango::AttributeValue_3 &get_last_attr_value_3();
	Tango::AttributeValue_4 &get_last_attr_value_4();
	long get_nb_elt() {return nb_elt;}
	
	void get_cmd_history(long,Tango::DevCmdHistoryList *);
	void get_cmd_history(long,Tango::DevCmdHistory_4 *,Tango::CmdArgType &);
	
	void get_attr_history(long,Tango::DevAttrHistoryList *,long);
	void get_attr_history(long,Tango::DevAttrHistoryList_3 *,long);
	void get_attr_history(long,Tango::DevAttrHistory_4 *,long);

	void get_attr_history_43(long,Tango::DevAttrHistoryList_3 *,long);
	
private:
	void inc_indexes();
	
	vector<RingElt>		ring;
	long				insert_elt;
	long				nb_elt;
	long				max_elt;
};


#define ADD_ELT_DATA_TO_GLOBAL_SEQ(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT->length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			(*GLOB)[IND + k] = (*ELT)[k]; \
		IND = IND + elt_data_length; \
	}

#define ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT.length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			GLOB[IND + k] = ELT[k]; \
		IND = IND + elt_data_length; \
	}
	
#define ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_PTR_REF(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT.length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			(*GLOB)[IND + k] = ELT[k]; \
		IND = IND + elt_data_length; \
	}
	
#define MANAGE_DIM_ARRAY(LENGTH) \
	if (last_dim.dim_x == LENGTH) \
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = LENGTH; \
		last_dim.dim_y = 0; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}

#define ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(GLOB,ELT,IND) \
	(*GLOB)[IND] = ELT; \
	IND = IND + 1;
		
#define MANAGE_DIM_SIMPLE() \
	if (last_dim.dim_x == 1) \
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = 1; \
		last_dim.dim_y = 0; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}

#define MANAGE_DIM_ARRAY_SEQ(LENGTH_STR,LENGTH_NUM) \
	if ((last_dim.dim_x == LENGTH_STR)  && (last_dim.dim_y == LENGTH_NUM))\
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = LENGTH_STR; \
		last_dim.dim_y = LENGTH_NUM; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}
						
} // End of Tango namespace

#endif /* _POLLRING_ */
