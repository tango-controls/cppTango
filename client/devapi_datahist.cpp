static const char *RcsId = "$Id$\n$Name$";

//
// devapi_datahist.cpp 	- C++ source code file for TANGO devapi class 
//			  DeviceDataHistory and DeviceAttributeHistory
//
// programmer(s) 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 		- June 2002
//
// $Log$
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
                                                     
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DeviceDataHistory::DeviceDataHistory() - constructors to create DeviceDataHistory 
//
//-----------------------------------------------------------------------------


DeviceDataHistory::DeviceDataHistory(int n, int *ref,DevCmdHistoryList *ptr)
{
	ref_ctr_ptr = ref;
	seq_ptr = ptr;

	(*ref_ctr_ptr)++;
		
	any = &((*ptr)[n].value);
	fail = (*ptr)[n].cmd_failed;
	time = (*ptr)[n].time;
	err = &((*ptr)[n].errors);
}



DeviceDataHistory::DeviceDataHistory(const DeviceDataHistory & source) 
{
	exceptions_flags = source.exceptions_flags;
	any = const_cast<DeviceDataHistory &>(source).any._retn();
	
	fail = source.fail;
	time = source.time;
	err = const_cast<DeviceDataHistory &>(source).err._retn();
	
	seq_ptr = source.seq_ptr;
	ref_ctr_ptr = source.ref_ctr_ptr;
	(*ref_ctr_ptr)++;	
}

//-----------------------------------------------------------------------------
//
// DeviceDataHistory::~DeviceDataHistory() - Destructor 
//
//-----------------------------------------------------------------------------

DeviceDataHistory::~DeviceDataHistory()
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


//-----------------------------------------------------------------------------
//
// DeviceDataHistory::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceDataHistory & DeviceDataHistory::operator=(const DeviceDataHistory &rval)
{
	exceptions_flags = rval.exceptions_flags;
	any = const_cast<DeviceDataHistory &>(rval).any._retn();
	
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

	char *tmp_date = asctime(localtime((time_t *)&dh.time.tv_sec));
	tmp_date[strlen(tmp_date) - 1] = '\0';
	o_str << tmp_date;
	o_str << " (" << dh.time.tv_sec << "," << dh.time.tv_usec << " sec) : ";
	
//
// Print data or error stack
//

	if (dh.fail == true)
	{
		int nb_err = dh.err.in().length();
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


DeviceAttributeHistory::DeviceAttributeHistory(int n,DevAttrHistoryList_var &seq)
{		
	fail = seq[n].attr_failed;
	err = seq[n].errors;
	
	time = seq[n].value.time;
	quality = seq[n].value.quality;
	dim_x = seq[n].value.dim_x;
	dim_y = seq[n].value.dim_y;
	name = seq[n].value.name;
	
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
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
		}
	}	

}



DeviceAttributeHistory::DeviceAttributeHistory(const DeviceAttributeHistory & source) 
{	
	fail = source.fail;
	err = source.err;
	
	exceptions_flags = source.exceptions_flags;
	time = source.time;
	quality = source.quality;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	name = source.name;
	
	DeviceAttributeHistory &nc_source = const_cast<DeviceAttributeHistory &>(source);
	if (nc_source.LongSeq.operator->() != NULL)	
		LongSeq = nc_source.LongSeq._retn();
	if (nc_source.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_source.ShortSeq._retn();	
	if (nc_source.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_source.DoubleSeq._retn();
	if (nc_source.StringSeq.operator->() != NULL)	
		StringSeq = nc_source.StringSeq._retn();	
}

//-----------------------------------------------------------------------------
//
// DeviceAttributeHistory::~DeviceAttributeHistory() - Destructor 
//
//-----------------------------------------------------------------------------

DeviceAttributeHistory::~DeviceAttributeHistory()
{
}


//-----------------------------------------------------------------------------
//
// DeviceAttributeHistory::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttributeHistory & DeviceAttributeHistory::operator=(const DeviceAttributeHistory &rval)
{	
	fail = rval.fail;
	err = rval.err;

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
		char *tmp_date = asctime(localtime((time_t *)&dah.time.tv_sec));
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << dah.time.tv_sec << "," << dah.time.tv_usec << " sec) : ";
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
	}
	
//
// Print data (if valid) or error stack
//

	if (dah.fail == true)
	{
		int nb_err = dah.err.length();
		for (unsigned long i = 0;i < nb_err;i++)
		{
			o_str << "Tango error stack" << endl;
			o_str << "Severity = ";
			switch (dah.err[i].severity)
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
			o_str << "Error reason = " << dah.err[i].reason.in() << endl;
			o_str << "Desc : " << dah.err[i].desc.in() << endl;
			o_str << "Origin : " << dah.err[i].origin.in();
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
				else
					o_str << *(dah.StringSeq.operator->());
			}	
		}
	}
	
	return o_str;
}

} // End of Tango namepsace
