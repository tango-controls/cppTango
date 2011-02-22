#ifndef _DEV_TEST_H
#define  _DEV_TEST_H
#include <tango.h>


class EventCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
};

#ifndef COMPAT
class DevTest : public Tango::Device_4Impl {
#else
class DevTest : public Tango::Device_3Impl {
#endif
public :
	DevTest(Tango::DeviceClass *,string &);
	DevTest(Tango::DeviceClass *,const char *);
	DevTest(Tango::DeviceClass *,const char *,const char *);
	DevTest(Tango::DeviceClass *,const char *,const char *,Tango::DevState, const char *);
	~DevTest() { };
	
	// virtual Tango_DevString& dev_InOutString(Tango_DevString DateToParse);
	virtual Tango::ConstDevString dev_status();
	virtual void always_executed_hook();
	virtual void signal_handler(long signo);
		
	virtual void init_device();

	short *get_short_array() {return array2_buffer;}
		
	bool templ_state(const CORBA::Any &);
	void IOTempl();
	void IOPushEvent();
	void IOPushDevEncodedEvent();
	void IOSubscribeEvent();
	void IOUnSubscribeEvent();
	void IOFillPollBuffAttr();
	void IOFillPollBuffEncodedAttr();
	void IOFillPollBuffCmd();
	void IOTemplIn(Tango::DevLong);
	void FileDb();
	void IOSetWAttrLimit(const Tango::DevVarDoubleArray *);
	void push_data_ready(const Tango::DevVarLongStringArray *);

	Tango::DevVarLongArray *IOTemplOut();
	Tango::DevVarDoubleArray *IOTemplInOut(Tango::DevDouble);

	void set_attr_long(Tango::DevLong l) {attr_long = l;}

//
// Attribute related methods
//

	void read_Short_attr(Tango::Attribute &att);
	void read_Long_attr(Tango::Attribute &att);
	void read_Double_attr(Tango::Attribute &att);
	void read_String_attr(Tango::Attribute &att);
	void read_Short_spec_attr(Tango::Attribute &att);
	void read_Long_spec_attr(Tango::Attribute &att);
	void read_Double_spec_attr(Tango::Attribute &att);
	void read_String_spec_attr(Tango::Attribute &att);
	void read_Short_ima_attr(Tango::Attribute &att);
	void read_Long_ima_attr(Tango::Attribute &att);
	void read_Double_ima_attr(Tango::Attribute &att);
	void read_String_ima_attr(Tango::Attribute &att);
	void read_attr_no_data(Tango::Attribute &att);
	void read_attr_wrong_type(Tango::Attribute &att);
	void read_attr_wrong_size(Tango::Attribute &att);
	void read_attr_no_alarm(Tango::Attribute &att);
	void read_Short_attr_with_w(Tango::Attribute &att);
	void read_Long_attr_with_w(Tango::Attribute &att);
	void read_Short_attr_rw(Tango::Attribute &att);
	void read_Long64_attr_rw(Tango::Attribute &att);
	void read_ULong_attr_rw(Tango::Attribute &att);
	void read_ULong64_attr_rw(Tango::Attribute &att);
	void read_State_attr_rw(Tango::Attribute &att);
	void read_attr_asyn(Tango::Attribute &att);
	void read_attr_asyn_to(Tango::Attribute &att);
	void read_attr_asyn_except(Tango::Attribute &att);
	void read_PollLong_attr(Tango::Attribute &att);
	void read_PollString_spec_attr(Tango::Attribute &att);
	void read_attr_dq_sh(Tango::Attribute &att);
	void read_attr_dq_lo(Tango::Attribute &att);
	void read_attr_dq_db(Tango::Attribute &att);
	void read_attr_dq_str(Tango::Attribute &att);
	void read_Short_spec_attr_rw(Tango::Attribute &att);	
	void read_String_spec_attr_rw(Tango::Attribute &att);	
	void read_Long_spec_attr_rw(Tango::Attribute &att);	
	void read_Short_ima_attr_rw(Tango::Attribute &att);
	void read_String_ima_attr_rw(Tango::Attribute &att);
	void read_Event_change_tst(Tango::Attribute &att);
	void read_Event64_change_tst(Tango::Attribute &att);
	void read_Event_quality_tst(Tango::Attribute &att);
	void read_Poll_buff(Tango::Attribute &att);
	void read_Poll_buffRW(Tango::Attribute &att);
	
	void read_Long64_spec_attr_rw(Tango::Attribute &att);
	void read_ULong_spec_attr_rw(Tango::Attribute &att);
	void read_ULong64_spec_attr_rw(Tango::Attribute &att);
	void read_State_spec_attr_rw(Tango::Attribute &att);
	
	void read_Float_attr(Tango::Attribute &att);
	void read_Boolean_attr(Tango::Attribute &att);
	void read_UShort_attr(Tango::Attribute &att);
	void read_UChar_attr(Tango::Attribute &att);
	void read_Float_spec_attr(Tango::Attribute &att);
	void read_Boolean_spec_attr(Tango::Attribute &att);
	void read_UShort_spec_attr(Tango::Attribute &att);
	void read_UChar_spec_attr(Tango::Attribute &att);
	void read_Float_spec_attr_rw(Tango::Attribute &att);
	void read_UChar_spec_attr_rw(Tango::Attribute &att);
	void read_Boolean_ima_attr_rw(Tango::Attribute &att);
	void read_UShort_ima_attr_rw(Tango::Attribute &att);
	void read_slow_actuator(Tango::Attribute &att);
	void read_fast_actuator(Tango::Attribute &att);
	void read_Encoded_attr_rw(Tango::Attribute &att);
	void read_Encoded_attr_image(Tango::Attribute &att);
	
	void read_Sub_device_tst(Tango::Attribute &att);
	
	void write_Short_attr_rw(Tango::WAttribute &att);
	void write_Long64_attr_rw(Tango::WAttribute &att);
	void write_ULong_attr_rw(Tango::WAttribute &att);
	void write_ULong64_attr_rw(Tango::WAttribute &att);
	void write_State_attr_rw(Tango::WAttribute &att);
	void write_Short_attr_w(Tango::WAttribute &att);
	void write_Short_attr_w2(Tango::WAttribute &att);
	void write_Long_attr_w(Tango::WAttribute &att);
	void write_Double_attr_w(Tango::WAttribute &att);
	void write_String_attr_w(Tango::WAttribute &att);
	void write_String_attr_w2(Tango::WAttribute &att);
	void write_attr_asyn_write(Tango::WAttribute &att);
	void write_attr_asyn_write_to(Tango::WAttribute &att);
	void write_attr_asyn_write_except(Tango::WAttribute &att);	
	void write_Short_spec_attr_w(Tango::WAttribute &att) {};
	void write_Long_spec_attr_w(Tango::WAttribute &att) {};
	void write_Double_spec_attr_w(Tango::WAttribute &att) {};
	void write_String_spec_attr_w(Tango::WAttribute &att);
	void write_Short_ima_attr_w(Tango::WAttribute &att) {};
	void write_String_ima_attr_w(Tango::WAttribute &att) {};		
	void write_Short_spec_attr_rw(Tango::WAttribute &att) {};
	void write_String_spec_attr_rw(Tango::WAttribute &att) {};
	void write_Long_spec_attr_rw(Tango::WAttribute &att);
	void write_Short_ima_attr_rw(Tango::WAttribute &att) {};
	void write_String_ima_attr_rw(Tango::WAttribute &att) {};
	void write_Float_attr_w(Tango::WAttribute &att);
	void write_Boolean_attr_w(Tango::WAttribute &att);
	void write_UShort_attr_w(Tango::WAttribute &att);
	void write_UChar_attr_w(Tango::WAttribute &att);
	void write_Float_spec_attr_w(Tango::WAttribute &att);
	void write_Boolean_spec_attr_w(Tango::WAttribute &att);
	void write_UShort_spec_attr_w(Tango::WAttribute &att);
	void write_UChar_spec_attr_w(Tango::WAttribute &att);
	void write_Float_ima_attr_w(Tango::WAttribute &att);
	void write_UShort_ima_attr_w(Tango::WAttribute &att);
	void write_Float_spec_attr_rw(Tango::WAttribute &att);
	void write_UChar_spec_attr_rw(Tango::WAttribute &att);
	void write_Boolean_ima_attr_rw(Tango::WAttribute &att);
	void write_UShort_ima_attr_rw(Tango::WAttribute &att);
	void write_slow_actuator(Tango::WAttribute &att);
	void write_fast_actuator(Tango::WAttribute &att);
	void write_Encoded_attr_rw(Tango::WAttribute &att);
	void write_Poll_buffRW(Tango::WAttribute &att);
	
	void write_Long64_spec_attr_rw(Tango::WAttribute &att);
	void write_ULong_spec_attr_rw(Tango::WAttribute &att);
	void write_ULong64_spec_attr_rw(Tango::WAttribute &att);
	void write_State_spec_attr_rw(Tango::WAttribute &att);
						
	friend class IOAttrThrowEx;
	friend class IOAddOneElt;
	friend class IORemoveOneElt;
	friend class IOIncValue;
	friend class IODecValue;
	friend class IOChangeQuality;
	friend class IOGetCbExecuted;
	friend class ChangeEncodedFormat;
	friend class ChangeEncodedData;
	
protected :
	Tango::DevDouble attr_double;
	Tango::DevShort attr_short;
	Tango::DevShort attr_short_rw;
	Tango::DevLong64 attr_long64_rw;
	Tango::DevULong attr_ulong_rw;
	Tango::DevULong64 attr_ulong64_rw;
	Tango::DevState attr_state_rw;
	Tango::DevLong attr_long;
	Tango::DevDouble attr_db_array[3];
	Tango::DevLong attr_long_array[10];
	Tango::DevString attr_str_array[2];
	
	Tango::DevLong64 attr_spec_long64_rw[3];
	Tango::DevULong attr_spec_ulong_rw[3];
	Tango::DevULong64 attr_spec_ulong64_rw[3];
	Tango::DevState attr_spec_state_rw[3];
	
	Tango::DevShort short_spec_attr[2];
	Tango::DevLong long_spec_attr[3];
	Tango::DevShort short_ima_attr[4];
	Tango::ConstDevString  string_spec_attr[2];
	Tango::ConstDevString  string_ima_attr[2];
	
	Tango::DevDouble attr_dq_double;
	Tango::DevShort attr_dq_short;
	Tango::DevLong attr_dq_long;
	char attr_dq_str[80];
	
	long PollLong_attr_num;
	Tango::DevLong Poll_attr_long;
	
	long PollString_spec_attr_num;
	
	short array2_buffer[10];
	
	bool Short_attr_except;
	bool event_change_attr_except;
	bool event_quality_attr_except;
	bool event_throw_out_of_sync;
	
	Tango::DevLong attr_event[10];
	long attr_event_size;
	Tango::DevLong64 attr_event64[4];
	long attr_event64_size;
	Tango::DevDouble attr_qua_event[2];
	Tango::AttrQuality attr_event_qua;
	
	EventCallBack cb;
	Tango::DeviceProxy *remote_dev;
	int eve_id;
	
	Tango::DevFloat attr_float;
	Tango::DevBoolean attr_boolean;
	Tango::DevUShort attr_ushort;
	Tango::DevUChar attr_uchar;
	
	Tango::DevFloat attr_spec_float[3];
	Tango::DevBoolean attr_spec_boolean[5];
	Tango::DevUShort attr_spec_ushort[2];
	Tango::DevUChar attr_spec_uchar[6];
	
	struct timeval		slow_actua_write;
	Tango::DevShort		slow_actua;
	Tango::DevDouble	fast_actua;
	
	Tango::DevBoolean attr_sub_device_tst;
	
#ifndef COMPAT
	Tango::EncodedAttribute jpeg;
	Tango::DevEncoded	enc_attr;
	Tango::DevEncoded	*enc_attr_ptr;
	Tango::DevString	enc_format;
	unsigned char		enc_data[10];
#endif

};

#endif



