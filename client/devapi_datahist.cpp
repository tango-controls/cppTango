static const char *RcsId = "$Header$";
//
// devapi_datahist.cpp 	- C++ source code file for TANGO devapi class 
//			  DeviceDataHistory and DeviceAttributeHistory
//
// programmer(s) 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 		- June 2002
//
// $Log$
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

} // End of Tango namepsace
