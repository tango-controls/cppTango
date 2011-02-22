static const char *RcsId = "$Id$\n$Name$";

//
// devapi_datahist.cpp 	- C++ source code file for TANGO devapi class 
//			  DeviceDataHistory and DeviceAttributeHistory
//
// programmer(s) 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 		- June 2002
//
// Copyright (C) :      2002,2003,2004,2005,2006,2007,2008,2009,2010,2011
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
//
// $Log$
// Revision 3.19  2010/12/08 10:10:54  taurel
// - Commit after a merge with the bugfixes branch
//
// Revision 3.18.2.1  2010/11/26 07:56:12  taurel
// - Fix date in date cmoputation in the printing stream inserter operators
// for class DeviceAttribute and associated (history)
//
// Revision 3.18  2010/09/09 13:44:06  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.17  2009/04/30 12:25:00  taurel
// - Fix a bug in the printing utility (Thank's to Jens Kruger)
//
// Revision 3.16  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.15  2008/10/06 15:02:16  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.14  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.13  2008/09/23 14:38:28  taurel
// - Commit after the end of DevEncoded data type implementation
// - The new test suite is also now running fine
//
// Revision 3.12  2008/05/20 12:42:30  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.11  2008/03/11 14:36:44  taurel
// - Apply patches from Frederic Picca about compilation with gcc 4.2
// Revision 3.10.2.2  2007/11/20 14:39:12  taurel
// - Add the new way to retrieve command history from polling buffer
// implemented in Tango V7
//
// Revision 3.10.2.1  2007/11/16 14:10:56  taurel
// - Added a new IDL interface (Device_4)
// - Added a new way to get attribute history from polling buffer (must faster)
//
// Revision 3.10  2007/03/06 08:20:45  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.9  2005/05/10 13:56:46  taurel
// - Error in the Windows change !!
//
// Revision 3.8  2005/05/10 13:50:55  taurel
// - Minor changes for Windows compiler
//
// Revision 3.7  2005/04/15 11:35:05  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.6  2005/01/13 08:36:37  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.5.2.2  2004/10/27 05:58:11  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.5.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.5  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.4  2003/08/21 07:22:02  taurel
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
// Revision 3.3  2003/07/03 07:37:56  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.2.2.3  2004/03/02 07:40:23  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.2.2.2  2003/12/11 11:47:40  taurel
// Added CHANGING attribute quality factor
//
// Revision 3.2.2.1  2003/09/18 14:07:41  taurel
// Fixes some bugs:
//  - Bug fix in DeviceProxy copy constructor and assignement operator
//  - Change the way how DeviceProxy::write_attribute() is coded
//  - Added DeviceAttribute ctors from "const char *"
//  - Split "str().c_str()" in two lines of code. It was the reason of some
//    problems using Windows VC6
//
// Revision 3.2  2003/05/28 14:42:56  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.1  2003/04/03 15:21:56  taurel
// Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// and DeviceAttributeHistory instance
//
// Revision 3.0  2003/03/25 16:30:34  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 2.4  2003/01/09 12:00:33  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.3  2002/12/16 11:58:37  taurel
// - Change the underlying ORB fom ORBacus to omniORB
// - New method get_device_list() in Util class
// - Util::get_class_list() takes DServer device into account
// - Util::get_device_by_name() takes DSErver device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
//
// Revision 2.2  2002/10/14 09:32:43  taurel
// Fix bugs in devapi_base.cpp file :
// - In read_attribute and read_attributes method of the DeviceProxy class
//   Do not create sequence the same way if the call is local or remote.
// - Add reconnection in the Connection::set_timeout_millis method
// - Add flags to the Connection::set_timeout_millis method
// - Fix bug in the DeviceProxy constructor when device is not marked as exported
//   in the database. The constructor was not stateless in this case.
//
// Revision 2.1  2002/08/12 12:43:24  taurel
// Fix bug in DeviceProxy::write_attributes method when writing several
// attributes in one call. (File devapi_base.cpp)
//
// Revision 2.0  2002/06/28 13:43:08  taurel
// Lot of changes since last releases :
// 	- Database object managed as a singleton per control system
// 	- Support all tango device naming syntax (using TANGO_HOST env.
//  	  variable, without env variable and non database device)
// 	- No more copy during read_attribute and command_inout
// 	- Added some missing methods
// 	- Build an exception class hierarchy
// 	- Added correct management of device time-out
// 	- Support all Tango device interface release 2 features
// 	  (data/attribute comming from polling buffer, polling related methods,
// 	   command/attribute result history)
//
//
// version 		- $Version$
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <iomanip>
                                                     
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DeviceDataHistory::DeviceDataHistory() - constructors to create DeviceDataHistory 
//
//-----------------------------------------------------------------------------

DeviceDataHistory::DeviceDataHistory():DeviceData(),ext_hist(NULL)
{
	fail = false;
	err = new DevErrorList();
	seq_ptr = NULL;
	ref_ctr_ptr = NULL;
}

DeviceDataHistory::DeviceDataHistory(int n, int *ref,DevCmdHistoryList *ptr):ext_hist(NULL)
{
	ref_ctr_ptr = ref;
	seq_ptr = ptr;

	(*ref_ctr_ptr)++;
		
	any = &((*ptr)[n].value);
	fail = (*ptr)[n].cmd_failed;
	time = (*ptr)[n].time;
	err = &((*ptr)[n].errors);
}

DeviceDataHistory::DeviceDataHistory(const DeviceDataHistory & source):DeviceData(source) 
{
	fail = source.fail;
	time = source.time;
	err = const_cast<DeviceDataHistory &>(source).err._retn();
	
	seq_ptr = source.seq_ptr;
	ref_ctr_ptr = source.ref_ctr_ptr;
	if (ref_ctr_ptr != NULL)
		(*ref_ctr_ptr)++;
	
	if (source.ext_hist == NULL)
		ext_hist = NULL;
	else
	{
		ext_hist = new DeviceDataHistoryExt();
		*ext_hist = *(source.ext_hist);
	}	
}

//-----------------------------------------------------------------------------
//
// DeviceDataHistory::~DeviceDataHistory() - Destructor 
//
//-----------------------------------------------------------------------------

DeviceDataHistory::~DeviceDataHistory()
{
	if (seq_ptr != NULL)
	{
		any._retn();
		err._retn();
		
		(*ref_ctr_ptr)--;
		if (*ref_ctr_ptr == 0)
		{
			delete seq_ptr;
			delete ref_ctr_ptr;
		}
	}
	
	if (ext_hist != NULL)
		delete ext_hist;
}


//-----------------------------------------------------------------------------
//
// DeviceDataHistory::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceDataHistory & DeviceDataHistory::operator=(const DeviceDataHistory &rval)
{

//
// Assignement of DeviceData class members first
//

	exceptions_flags = rval.exceptions_flags;
	any = const_cast<DeviceDataHistory &>(rval).any._retn();
	if (ext != NULL)
		delete ext;
	if (rval.ext != NULL)
	{
		ext = new DeviceDataExt();
		*ext = *(rval.ext);
	}
	else
		ext = NULL;
		

//
// Then, assignement of DeviceDataHistory members
//
			
	fail = rval.fail;
	time = rval.time;
	err = const_cast<DeviceDataHistory &>(rval).err._retn();

	(*ref_ctr_ptr)--;
	if (*ref_ctr_ptr == 0)
	{
		delete seq_ptr;
		delete ref_ctr_ptr;
	}
		
	seq_ptr = rval.seq_ptr;
	ref_ctr_ptr = rval.ref_ctr_ptr;
	(*ref_ctr_ptr)++;
	
	if (ext_hist != NULL)
		delete ext_hist;
	if (rval.ext_hist != NULL)
	{
		ext_hist = new DeviceDataHistoryExt();
		*ext_hist = *(rval.ext_hist);
	}
	else
		ext_hist = NULL;
	
	return *this;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			DeviceDataHistory class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceDataHistory &dh)
{

//
// First, print date
//

#ifdef _TG_WINDOWS_
	time_t tmp_val = dh.time.tv_sec;
	struct tm *tmp_time = localtime(&tmp_val);
	char *tmp_date = asctime(tmp_time);
#else
	char *tmp_date = asctime(localtime((time_t *)&dh.time.tv_sec));
#endif
	tmp_date[strlen(tmp_date) - 1] = '\0';
	o_str << tmp_date;
	o_str << " (" << dh.time.tv_sec << "," << setw(6) << setfill('0') << dh.time.tv_usec << " sec) : ";
	
//
// Print data or error stack
//

	if (dh.fail == true)
	{
		unsigned int nb_err = dh.err.in().length();
		for (unsigned long i = 0;i < nb_err;i++)
		{
			o_str << "Tango error stack" << endl;
			o_str << "Severity = ";
			switch ((dh.err.in())[i].severity)
			{
			case Tango::WARN :
				o_str << "WARNING ";
				break;
						
			case Tango::ERR :
				o_str << "ERROR ";
				break;
					
			case Tango::PANIC :
				o_str << "PANIC ";
				break;
						
			default :
				o_str << "Unknown severity code";
				break;
			}
			o_str << endl;
			o_str << "Error reason = " << (dh.err.in())[i].reason.in() << endl;
			o_str << "Desc : " << (dh.err.in())[i].desc.in() << endl;
			o_str << "Origin : " << (dh.err.in())[i].origin.in();
			if (i != nb_err - 1)
				o_str << endl;
		}
	}
	else
	{
		o_str << static_cast<DeviceData &>(dh);
	}
	
	return o_str;
}

//-----------------------------------------------------------------------------
//
// DeviceAttributeHistory::DeviceAttributeHistory() - constructors to create DeviceAttributeHistory 
//
//-----------------------------------------------------------------------------

DeviceAttributeHistory::DeviceAttributeHistory():DeviceAttribute(),ext_hist(NULL)
{
	fail = false;
	ext->err_list = new DevErrorList();
}

DeviceAttributeHistory::DeviceAttributeHistory(int n,DevAttrHistoryList_var &seq):ext_hist(NULL)
{		
	fail = seq[n].attr_failed;
	
	ext->err_list = new DevErrorList(seq[n].errors);	
	time = seq[n].value.time;
	quality = seq[n].value.quality;
	dim_x = seq[n].value.dim_x;
	dim_y = seq[n].value.dim_y;
	name = seq[n].value.name;
	
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarLong64Array *tmp_seq_lolo;
	CORBA::LongLong *tmp_lolo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	const DevVarFloatArray *tmp_seq_fl;
	CORBA::Float *tmp_fl;
	const DevVarBooleanArray *tmp_seq_boo;
	CORBA::Boolean *tmp_boo;
	const DevVarUShortArray *tmp_seq_ush;
	CORBA::UShort *tmp_ush;
	const DevVarCharArray *tmp_seq_uch;
	CORBA::Octet *tmp_uch;
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_ulolo;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;
	
	CORBA::ULong max,len;

	if ((fail == false) && (quality != Tango::ATTR_INVALID))
	{
		CORBA::TypeCode_var ty = seq[n].value.value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
		switch (ty_seq->kind())
		{
		case tk_long:
			seq[n].value.value >>= tmp_seq_lo;
			max = tmp_seq_lo->maximum();
			len = tmp_seq_lo->length();
			tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
			LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
			break;
		
		case tk_longlong:
			seq[n].value.value >>= tmp_seq_lolo;
			max = tmp_seq_lolo->maximum();
			len = tmp_seq_lolo->length();
			tmp_lolo = (const_cast<DevVarLong64Array *>(tmp_seq_lolo))->get_buffer((CORBA::Boolean)true);
			ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
			break;
			
		case tk_short:
			seq[n].value.value >>= tmp_seq_sh;
			max = tmp_seq_sh->maximum();
			len = tmp_seq_sh->length();
			tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
			ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
			break;
		
		case tk_double:
			seq[n].value.value >>= tmp_seq_db;
			max = tmp_seq_db->maximum();
			len = tmp_seq_db->length();
			tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
			DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
			break;
		
		case tk_string:
			seq[n].value.value >>= tmp_seq_str;
			max = tmp_seq_str->maximum();
			len = tmp_seq_str->length();
			tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
			StringSeq = new DevVarStringArray(max,len,tmp_str,true);
			break;
			
		case tk_float:		
			seq[n].value.value >>= tmp_seq_fl;
			max = tmp_seq_fl->maximum();
			len = tmp_seq_fl->length();
			tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean)true);
			FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
			break;
		
		case tk_boolean:
			seq[n].value.value >>= tmp_seq_boo;
			max = tmp_seq_boo->maximum();
			len = tmp_seq_boo->length();
			tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean)true);
			BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
			break;
		
		case tk_ushort:
			seq[n].value.value >>= tmp_seq_ush;
			max = tmp_seq_ush->maximum();
			len = tmp_seq_ush->length();
			tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean)true);
			UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
			break;
		
		case tk_octet:
			seq[n].value.value >>= tmp_seq_uch;
			max = tmp_seq_uch->maximum();
			len = tmp_seq_uch->length();
			tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean)true);
			UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
			break;
			
		case tk_ulong:
			seq[n].value.value >>= tmp_seq_ulo;
			max = tmp_seq_ulo->maximum();
			len = tmp_seq_ulo->length();
			tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
			ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
			break;
			
		case tk_ulonglong:
			seq[n].value.value >>= tmp_seq_ulolo;
			max = tmp_seq_ulolo->maximum();
			len = tmp_seq_ulolo->length();
			tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_ulolo))->get_buffer((CORBA::Boolean)true);
			ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
			break;
			
		case tk_enum:
			seq[n].value.value >>= tmp_seq_state;
			max = tmp_seq_state->maximum();
			len = tmp_seq_state->length();
			tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
			ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
		default:
			break;
		}
	}	

}


DeviceAttributeHistory::DeviceAttributeHistory(int n,DevAttrHistoryList_3_var &seq):ext_hist(NULL)
{		
	fail = seq[n].attr_failed;
	
	ext->err_list = new DevErrorList(seq[n].value.err_list);	
	time = seq[n].value.time;
	quality = seq[n].value.quality;
	dim_x = seq[n].value.r_dim.dim_x;
	dim_y = seq[n].value.r_dim.dim_y;
	ext->w_dim_x = seq[n].value.w_dim.dim_x;
	ext->w_dim_y = seq[n].value.w_dim.dim_y;
	name = seq[n].value.name;

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarLong64Array *tmp_seq_lolo;
	CORBA::LongLong *tmp_lolo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	const DevVarFloatArray *tmp_seq_fl;
	CORBA::Float *tmp_fl;
	const DevVarBooleanArray *tmp_seq_boo;
	CORBA::Boolean *tmp_boo;
	const DevVarUShortArray *tmp_seq_ush;
	CORBA::UShort *tmp_ush;
	const DevVarCharArray *tmp_seq_uch;
	CORBA::Octet *tmp_uch;
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_ulolo;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;
		
	CORBA::ULong max,len;

	if ((fail == false) && (quality != Tango::ATTR_INVALID))
	{
		CORBA::TypeCode_var ty = seq[n].value.value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
		switch (ty_seq->kind())
		{
		case tk_long:
			seq[n].value.value >>= tmp_seq_lo;
			max = tmp_seq_lo->maximum();
			len = tmp_seq_lo->length();
			tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
			LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
			break;
		
		case tk_longlong:
			seq[n].value.value >>= tmp_seq_lolo;
			max = tmp_seq_lolo->maximum();
			len = tmp_seq_lolo->length();
			tmp_lolo = (const_cast<DevVarLong64Array *>(tmp_seq_lolo))->get_buffer((CORBA::Boolean)true);
			ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
			break;
			
		case tk_short:
			seq[n].value.value >>= tmp_seq_sh;
			max = tmp_seq_sh->maximum();
			len = tmp_seq_sh->length();
			tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
			ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
			break;
		
		case tk_double:
			seq[n].value.value >>= tmp_seq_db;
			max = tmp_seq_db->maximum();
			len = tmp_seq_db->length();
			tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
			DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
			break;
		
		case tk_string:
			seq[n].value.value >>= tmp_seq_str;
			max = tmp_seq_str->maximum();
			len = tmp_seq_str->length();
			tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
			StringSeq = new DevVarStringArray(max,len,tmp_str,true);
			break;
			
		case tk_float:		
			seq[n].value.value >>= tmp_seq_fl;
			max = tmp_seq_fl->maximum();
			len = tmp_seq_fl->length();
			tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean)true);
			FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
			break;
		
		case tk_boolean:
			seq[n].value.value >>= tmp_seq_boo;
			max = tmp_seq_boo->maximum();
			len = tmp_seq_boo->length();
			tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean)true);
			BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
			break;
		
		case tk_ushort:
			seq[n].value.value >>= tmp_seq_ush;
			max = tmp_seq_ush->maximum();
			len = tmp_seq_ush->length();
			tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean)true);
			UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
			break;
		
		case tk_octet:
			seq[n].value.value >>= tmp_seq_uch;
			max = tmp_seq_uch->maximum();
			len = tmp_seq_uch->length();
			tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean)true);
			UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
			break;
			
		case tk_ulong:
			seq[n].value.value >>= tmp_seq_ulo;
			max = tmp_seq_ulo->maximum();
			len = tmp_seq_ulo->length();
			tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
			ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
			break;
			
		case tk_ulonglong:
			seq[n].value.value >>= tmp_seq_ulolo;
			max = tmp_seq_ulolo->maximum();
			len = tmp_seq_ulolo->length();
			tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_ulolo))->get_buffer((CORBA::Boolean)true);
			ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
			break;
			
		case tk_enum:
			seq[n].value.value >>= tmp_seq_state;
			max = tmp_seq_state->maximum();
			len = tmp_seq_state->length();
			tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
			ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
		default:
			break;
		}
	}	

}



DeviceAttributeHistory::DeviceAttributeHistory(const DeviceAttributeHistory & source):DeviceAttribute(source) 
{	
	fail = source.fail;
	
	if (source.ext_hist == NULL)
		ext_hist = NULL;
	else
	{
		ext_hist = new DeviceAttributeHistoryExt();
		*ext_hist = *(source.ext_hist);
	}		
}

//-----------------------------------------------------------------------------
//
// DeviceAttributeHistory::~DeviceAttributeHistory() - Destructor 
//
//-----------------------------------------------------------------------------

DeviceAttributeHistory::~DeviceAttributeHistory()
{
	if (ext_hist != NULL)
		delete ext_hist;
}


//-----------------------------------------------------------------------------
//
// DeviceAttributeHistory::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttributeHistory & DeviceAttributeHistory::operator=(const DeviceAttributeHistory &rval)
{	

//
// First, assignement of DeviceAttribute class members
//
		
	exceptions_flags = rval.exceptions_flags;
	time = rval.time;
	quality = rval.quality;
	dim_x = rval.dim_x;
	dim_y = rval.dim_y;
	name = rval.name;
		
	DeviceAttributeHistory &nc_source = const_cast<DeviceAttributeHistory &>(rval);
	if (nc_source.LongSeq.operator->() != NULL)	
		LongSeq = nc_source.LongSeq._retn();
	if (nc_source.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_source.ShortSeq._retn();	
	if (nc_source.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_source.DoubleSeq._retn();
	if (nc_source.StringSeq.operator->() != NULL)	
		StringSeq = nc_source.StringSeq._retn();
	if (nc_source.FloatSeq.operator->() != NULL)	
		FloatSeq = nc_source.FloatSeq._retn();
	if (nc_source.BooleanSeq.operator->() != NULL)	
		BooleanSeq = nc_source.BooleanSeq._retn();	
	if (nc_source.UShortSeq.operator->() != NULL)	
		UShortSeq = nc_source.UShortSeq._retn();
	if (nc_source.UCharSeq.operator->() != NULL)	
		UCharSeq = nc_source.UCharSeq._retn();
		
	if (ext != NULL)
		delete ext;
	if (rval.ext != NULL)
	{
		ext = new DeviceAttributeExt();
		*ext = *(rval.ext);
	}
	else
		ext = NULL;

//
// Then, assignement of DeviceAttributeHistory members
//
		
	fail = rval.fail;
	if (ext_hist != NULL)
		delete ext_hist;
	if (rval.ext_hist != NULL)
	{
		ext_hist = new DeviceAttributeHistoryExt();
		*ext_hist = *(rval.ext_hist);
	}
	else
		ext_hist = NULL;
							
	return *this;
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			DeviceAttributeHistory class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceAttributeHistory &dah)
{
//
// Print date
//

	if (dah.time.tv_sec != 0)
	{
#ifdef _TG_WINDOWS_
		time_t tmp_val = dah.time.tv_sec;
		struct tm *tmp_time = localtime(&tmp_val);
		char *tmp_date = asctime(tmp_time);
#else
		char *tmp_date = asctime(localtime((time_t *)&dah.time.tv_sec));
#endif
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << dah.time.tv_sec << "," << setw(6) << setfill('0') << dah.time.tv_usec << " sec) : ";
	}

//
// print attribute name
//

	o_str << dah.name;
	
//
// print dim_x and dim_y
//

	o_str << " (dim_x = " << dah.dim_x << ", dim_y = " << dah.dim_y << ", ";

//
// print write dim_x and dim_y
//

	o_str << "w_dim_x = " << dah.ext->w_dim_x << ", w_dim_y = " << dah.ext->w_dim_y << ", ";
		
//
// Print quality
//

	o_str << "Data quality factor = ";
	switch (dah.quality)
	{
	case Tango::ATTR_VALID:
		o_str << "VALID)" << endl;
		break;
		
	case Tango::ATTR_INVALID:
		o_str << "INVALID)";
		break;
		
	case Tango::ATTR_ALARM:
		o_str << "ALARM)" << endl;
		break;
		
	case Tango::ATTR_CHANGING:
		o_str << "CHANGING)" << endl;
		break;
		
	case Tango::ATTR_WARNING:
		o_str << "WARNING) " << endl;
		break;
	}
	
//
// Print data (if valid) or error stack
//

	if (dah.fail == true)
	{
		unsigned int nb_err = dah.ext->err_list.in().length();
		for (unsigned long i = 0;i < nb_err;i++)
		{
			o_str << "Tango error stack" << endl;
			o_str << "Severity = ";
			switch (dah.ext->err_list[i].severity)
			{
			case Tango::WARN :
				o_str << "WARNING ";
				break;
						
			case Tango::ERR :
				o_str << "ERROR ";
				break;
					
			case Tango::PANIC :
				o_str << "PANIC ";
				break;
						
			default :
				o_str << "Unknown severity code";
				break;
			}
			o_str << endl;
			o_str << "Error reason = " << dah.ext->err_list[i].reason.in() << endl;
			o_str << "Desc : " << dah.ext->err_list[i].desc.in() << endl;
			o_str << "Origin : " << dah.ext->err_list[i].origin.in();
			if (i != nb_err - 1)
				o_str << endl;
		}
	}
	else
	{
		if (dah.quality != Tango::ATTR_INVALID)
		{
			if (dah.is_empty() == true)
				o_str << "No data in DeviceData object";
			else
			{
				if (dah.LongSeq.operator->() != NULL)
					o_str << *(dah.LongSeq.operator->());	
				else if (dah.ShortSeq.operator->() != NULL)
					o_str << *(dah.ShortSeq.operator->());	
				else if (dah.DoubleSeq.operator->() != NULL)
					o_str << *(dah.DoubleSeq.operator->());
				else if (dah.FloatSeq.operator->() != NULL)
					o_str << *(dah.FloatSeq.operator->());
				else if (dah.BooleanSeq.operator->() != NULL)
					o_str << *(dah.BooleanSeq.operator->());	
				else if (dah.UShortSeq.operator->() != NULL)
					o_str << *(dah.UShortSeq.operator->());
				else if (dah.UCharSeq.operator->() != NULL)
					o_str << *(dah.UCharSeq.operator->());
				else if (dah.ext->Long64Seq.operator->() != NULL)
					o_str << *(dah.ext->Long64Seq.operator->());
				else if (dah.ext->ULongSeq.operator->() != NULL)
					o_str << *(dah.ext->ULongSeq.operator->());
				else if (dah.ext->ULong64Seq.operator->() != NULL)
					o_str << *(dah.ext->ULong64Seq.operator->());
				else if (dah.ext->StateSeq.operator->() != NULL)
					o_str << *(dah.ext->StateSeq.operator->());
				else if (dah.ext->EncodedSeq.operator->() != NULL)
					o_str << *(dah.ext->EncodedSeq.operator->());
				else
					o_str << *(dah.StringSeq.operator->());
			}	
		}
	}
	
	return o_str;
}

} // End of Tango namepsace
