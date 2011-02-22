#include <tango.h>
#include <DevTest.h>

#ifdef WIN32
#include <sys/timeb.h>
#include <process.h>
#else
#include <sys/time.h>
#endif

void EventCallBack::push_event(Tango::EventData* event_data)
{
	short value;
        struct timeval now_timeval;

#ifdef WIN32
	struct _timeb before_win;
	_ftime(&before_win);
	now_timeval.tv_sec = (unsigned long)before_win.time;
	now_timeval.tv_usec = (long)before_win.millitm * 1000;
#else
	gettimeofday(&now_timeval,NULL);
#endif
	cout << "date : tv_sec = " << now_timeval.tv_sec;
	cout << ", tv_usec = " << now_timeval.tv_usec << endl;
	
	int delta_s = now_timeval.tv_sec - old_sec;
	if (delta_s == 0)
		delta_msec = (now_timeval.tv_usec - old_usec) / 1000;
	else
	{
		delta_msec = (now_timeval.tv_usec + (1000000 - old_usec)) / 1000;
		if (delta_s > 1)
			delta_msec = delta_msec + ((delta_s - 1)* 1000);
	}
	old_sec = now_timeval.tv_sec;
	old_usec = now_timeval.tv_usec;
	
	cout << "delta_msec = " << delta_msec << endl;

	cb_executed++;
	try
	{
		cout << "StateEventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{

			*(event_data->attr_value) >> value;
			cout << "CallBack vector value : " << value << endl;
		}
		else
		{
			cout << "Error send to callback" << endl;
//			Tango::Except::print_error_stack(event_data->errors);
			if (strcmp(event_data->errors[0].reason.in(),"aaa") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		cout << "EventCallBack::push_event(): could not extract data !\n";
	}

}


//+----------------------------------------------------------------------------
//
// method : 		DevTest::DevTest()
// 
// description : 	constructor for testing use of TANGO
//
// in : - cl : Pointer to the DeviceClass object
//      - s : Device name 
//
//-----------------------------------------------------------------------------

#ifndef COMPAT
DevTest::DevTest(Tango::DeviceClass *cl,string &s):Tango::Device_4Impl(cl,s.c_str())
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s):Tango::Device_4Impl(cl,s)
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s,const char *d)
:Tango::Device_4Impl(cl,s,d)
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s,const char *d, Tango::DevState state, const char *status )
:Tango::Device_4Impl(cl,s,d, state, status)
{
	init_device();
}
#else
DevTest::DevTest(Tango::DeviceClass *cl,string &s):Tango::Device_3Impl(cl,s.c_str())
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s):Tango::Device_3Impl(cl,s)
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s,const char *d)
:Tango::Device_3Impl(cl,s,d)
{
	init_device();
}

DevTest::DevTest(Tango::DeviceClass *cl,const char *s,const char *d, Tango::DevState state, const char *status )
:Tango::Device_3Impl(cl,s,d, state, status)
{
	init_device();
}
#endif

void DevTest::init_device()
{
	cout << "DevTest::DevTest() create  " << device_name << endl;
	DEBUG_STREAM << "Creating " << device_name << endl;

	set_state(Tango::ON);	
	attr_long = 1246;
	attr_short_rw = 66;
	attr_long64_rw = 0x800000000LL;
	attr_ulong_rw = 0xC0000000L;
	attr_ulong64_rw = 0xC000000000000000LL;
	attr_state_rw = Tango::FAULT;
	PollLong_attr_num = 0;
	PollString_spec_attr_num = 0;
	
	Short_attr_except = false;
	event_change_attr_except = false;
	event_quality_attr_except = false;
	event_throw_out_of_sync = false;
	
	attr_sub_device_tst = false;
	
	attr_event_size = 4;
	attr_event[0] = 10;
	attr_event[1] = 20;
	attr_event[2] = 30;
	attr_event[3] = 40;
	attr_event[4] = 50;
	attr_event[5] = 60;
	attr_event[6] = 70;
	attr_event[7] = 80;
	attr_event[8] = 90;
	attr_event[9] = 100;
	
	attr_event64_size = 2;
	attr_event64[0] = 0x800000000LL;
	attr_event64[1] = 44;
	attr_event64[2] = 55;
	
	attr_qua_event[0] = 1.2;
	attr_qua_event[1] = 2.4;
	attr_event_qua = Tango::ATTR_VALID;
	remote_dev = NULL;
	eve_id = 0;

	slow_actua_write.tv_sec = 0;
	slow_actua = 0;
	
	attr_spec_long64_rw[0] = 1000;
	attr_spec_long64_rw[1] = 10000;
	attr_spec_long64_rw[2] = 100000;
	attr_spec_ulong_rw[0] = 2222;
	attr_spec_ulong_rw[1] = 22222;
	attr_spec_ulong_rw[2] = 222222;
	attr_spec_ulong64_rw[0] = 8888;
	attr_spec_ulong64_rw[1] = 88888;
	attr_spec_ulong64_rw[2] = 888888;
	attr_spec_state_rw[0] = Tango::ON;
	attr_spec_state_rw[1] = Tango::OFF;

#ifndef COMPAT	
  	enc_attr.encoded_format = CORBA::string_dup("Which format?");
/*  	enc_attr.encoded_data.length(200 * 1024 * 1024);
  	for (int i = 0;i < (200 * 1024 * 1024);i++)
  		enc_attr.encoded_data[i] = (unsigned char)(i % 256);*/
 	enc_attr.encoded_data.length(4);
  	enc_attr.encoded_data[0] = (unsigned char)97;
  	enc_attr.encoded_data[1] = (unsigned char)98;
  	enc_attr.encoded_data[2] = (unsigned char)99;
  	enc_attr.encoded_data[3] = (unsigned char)100;

/*	enc_format = new char[40];
	strcpy(enc_format,"Forth try");
	enc_data[0] = (unsigned char)40;
	enc_data[1] = (unsigned char)41;*/
#endif

}


//+----------------------------------------------------------------------------
//
// method : 		DevTest::always_executed_hook()
// 
// description : 	method always executed before any command is executed
//
//-----------------------------------------------------------------------------

void DevTest::always_executed_hook()
{

	cout2 << "In always_executed_hook method" << endl;
	
}

//+----------------------------------------------------------------------------
//
// method : 		DevTest::state_cmd()
// 
// description : 	command to read the device state
//
// out :		device state
//
//-----------------------------------------------------------------------------

Tango::ConstDevString DevTest::dev_status()
{

	cout2 << "In DayOfWeek state command" << endl;
#ifdef WIN32
	Tango::ConstDevString stat = DeviceImpl::dev_status();
#else
	Tango::ConstDevString stat = Tango::DeviceImpl::dev_status();
#endif
	return stat;
}

//+----------------------------------------------------------------------------
//
// method : 		DevTest::xxx
// 
// description : 	Methods for template style command implementation
//
//-----------------------------------------------------------------------------

bool DevTest::templ_state(const CORBA::Any &)
{
//
// command allowed only if the device is on
//

	if (device_state == Tango::ON)
		return(true);
	else
		return(false);
}

void DevTest::IOTempl()
{	
    	cout << "[DevTest::IOTempl]" << endl;
}

Tango::DevVarLongArray *DevTest::IOTemplOut()
{	
	cout << "[DevTest::IOTemplOut]" << endl;

    	Tango::DevVarLongArray *argout;
    	argout = new Tango::DevVarLongArray();
    
    	argout->length(4);
    	(*argout)[0] = 10;
    	(*argout)[1] = 20;
    	(*argout)[2] = 30;
    	(*argout)[3] = 40;
 
    	return argout;
}

void DevTest::IOTemplIn(Tango::DevLong received)
{	  
    	cout << "[DevTest::IOTemplIn] received " << received << endl;
}

void DevTest::IOPushEvent()
{	  
    	cout << "[DevTest::IOPushEvent] received " << endl;
	
	vector<string> f_names;
	vector<double> f_val;
	
	push_event("event_change_tst",f_names,f_val,attr_event,attr_event_size);
	
}

void DevTest::IOPushDevEncodedEvent()
{	  
	cout << "[DevTest::IOPushDevEncodedEvent] received " << endl;

#ifndef COMPAT	
	vector<string> f_names;
	vector<double> f_val;
	
	push_event("encoded_attr",f_names,f_val,&enc_attr);
#endif	
}


void DevTest::IOSubscribeEvent()
{	  
    	cout << "[DevTest::IOSubscribeEvent] received " << endl;
	
	vector<string> filters;

	if (remote_dev == NULL)	
		remote_dev = new Tango::DeviceProxy("dev/test/20");
	string att_name("short_attr");
	cb.cb_executed = 0;
	
	// start the polling first!
	remote_dev->poll_attribute(att_name,1000);		
	eve_id = remote_dev->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);
	
}

void DevTest::IOUnSubscribeEvent()
{	  
    	cout << "[DevTest::IOUnSubscribeEvent] received " << endl;
	
	if (eve_id != 0)
	{
		remote_dev->unsubscribe_event(eve_id);
		
		string att_name("short_attr");
		remote_dev->stop_poll_attribute(att_name);		
	}
	
}

void DevTest::IOFillPollBuffAttr()
{	  
    cout << "[DevTest::IOFillPollBuffAttr] received " << endl;
	
	Tango::AttrHistoryStack<Tango::DevString> ahs;
	ahs.length(3);
	Tango::Util *tg = Tango::Util::instance();
	Tango::DevString *main_array[3];
	string att_name("Poll_buff");

	Tango::DevString *array_1 = new Tango::DevString [4];
//     	array_1[0] = CORBA::string_dup("One_1");
//      array_1[1] = CORBA::string_dup("Two_1");
//      array_1[2] = CORBA::string_dup("Three_1");
//      array_1[3] = CORBA::string_dup("Four_1");
      	array_1[0] = Tango::string_dup("One_1");
      	array_1[1] = Tango::string_dup("Two_1");
      	array_1[2] = Tango::string_dup("Three_1");
      	array_1[3] = Tango::string_dup("Four_1"); 
	main_array[0] = array_1;

	Tango::DevString *array_2 = new Tango::DevString [4];
	array_2[0] = CORBA::string_dup("One_2");
	array_2[1] = CORBA::string_dup("Two_2");
	array_2[2] = CORBA::string_dup("Three_2");
	array_2[3] = CORBA::string_dup("Four_2");
	main_array[1] = array_2;
	
	Tango::DevString *array_3 = new Tango::DevString [4];
	array_3[0] = CORBA::string_dup("One_3");
	array_3[1] = CORBA::string_dup("Two_3");
	array_3[2] = CORBA::string_dup("Three_3");
	array_3[3] = CORBA::string_dup("Four_3");
	main_array[2] = array_3;

	ahs.clear();
	time_t when = time(NULL);
	for (int k =  0;k < 3;k++)
	{
		Tango::TimedAttrData<Tango::DevString> tad(main_array[k],2,2,Tango::ATTR_VALID,true,when);
		ahs.push(tad);
	}
	
	tg->fill_attr_polling_buffer(this,att_name,ahs);

	ahs.clear();
	att_name = "Poll_buffRW";
	when = time(NULL);

	Tango::DevString *rd_array_1 = new Tango::DevString[4];
	Tango::DevString *wr_array_1 = new Tango::DevString[2];

	rd_array_1[0] = CORBA::string_dup("One_rd_1");
	rd_array_1[1] = CORBA::string_dup("Two_rd_1");
	rd_array_1[2] = CORBA::string_dup("Three_rd_1");
	rd_array_1[3] = CORBA::string_dup("Four_rd_1");

	wr_array_1[0] = CORBA::string_dup("One_wr_1");
	wr_array_1[1] = CORBA::string_dup("Two_wr_1");

	Tango::TimedAttrData<Tango::DevString> tad_1(rd_array_1,2,2,wr_array_1,2,1,Tango::ATTR_VALID,true,when);
	ahs.push(tad_1);

	Tango::DevString *rd_array_2 = new Tango::DevString[4];
	Tango::DevString *wr_array_2 = new Tango::DevString[2];

	rd_array_2[0] = CORBA::string_dup("One_rd_2");
	rd_array_2[1] = CORBA::string_dup("Two_rd_2");
	rd_array_2[2] = CORBA::string_dup("Three_rd_2");
	rd_array_2[3] = CORBA::string_dup("Four_rd_2");

	wr_array_2[0] = CORBA::string_dup("One_wr_2");
	wr_array_2[1] = CORBA::string_dup("Two_wr_2");

	Tango::TimedAttrData<Tango::DevString> tad_2(rd_array_2,2,2,wr_array_2,2,1,Tango::ATTR_VALID,true,when);
	ahs.push(tad_2);

	Tango::DevString *rd_array_3 = new Tango::DevString[4];
	Tango::DevString *wr_array_3 = new Tango::DevString[2];

	rd_array_3[0] = CORBA::string_dup("One_rd_3");
	rd_array_3[1] = CORBA::string_dup("Two_rd_3");
	rd_array_3[2] = CORBA::string_dup("Three_rd_3");
	rd_array_3[3] = CORBA::string_dup("Four_rd_3");

	wr_array_3[0] = CORBA::string_dup("One_wr_3");
	wr_array_3[1] = CORBA::string_dup("Two_wr_3");

	Tango::TimedAttrData<Tango::DevString> tad_3(rd_array_3,2,2,wr_array_3,2,1,Tango::ATTR_VALID,true,when);
	ahs.push(tad_3);

	tg->fill_attr_polling_buffer(this,att_name,ahs);

	cout << "Attribute Polling buffer filled" << endl;
}

void DevTest::IOFillPollBuffEncodedAttr()
{	  
    cout << "[DevTest::IOFillPollBuffEncodedAttr] received " << endl;

#ifndef COMPAT	
	Tango::AttrHistoryStack<Tango::DevEncoded> ahs;
	ahs.length(3);
	string att_name("Encoded_attr");
	Tango::Util *tg = Tango::Util::instance();
	Tango::DevEncoded the_enc;
	Tango::DevEncoded the_enc1;
	Tango::DevEncoded the_enc2;

	the_enc.encoded_format = CORBA::string_dup("First value");
	the_enc.encoded_data.length(2);
	the_enc.encoded_data[0] = 22;
	the_enc.encoded_data[1] = 33;

	time_t when = time(NULL);
	Tango::TimedAttrData<Tango::DevEncoded> tad(&the_enc,when);
	ahs.push(tad);

	the_enc1.encoded_format = CORBA::string_dup("Second value");
	the_enc1.encoded_data.length(2);
	the_enc1.encoded_data[0] = 33;
	the_enc1.encoded_data[1] = 44;

	when = time(NULL);
	Tango::TimedAttrData<Tango::DevEncoded> tad1(&the_enc1,when);
	ahs.push(tad1);

	the_enc2.encoded_format = CORBA::string_dup("Third value");
	the_enc2.encoded_data.length(2);
	the_enc2.encoded_data[0] = 44;
	the_enc2.encoded_data[1] = 55;

	when = time(NULL);
	Tango::TimedAttrData<Tango::DevEncoded> tad2(&the_enc2,when);
	ahs.push(tad2);
	
	tg->fill_attr_polling_buffer(this,att_name,ahs);
	cout << "Attribute (DevEncoded data type) polling buffer filled" << endl;
#endif
}


void DevTest::IOFillPollBuffCmd()
{	  
    	cout << "[DevTest::IOFillPollBuffCmd] received " << endl;
	
	Tango::CmdHistoryStack<Tango::DevVarLongArray> chs;
	chs.length(3);
	Tango::Util *tg = Tango::Util::instance();

	Tango::DevVarLongArray dvla_array[10];
	string cmd_name("IOArray1");

	for (int j = 0;j < 10;j++)
	{
		dvla_array[j].length(3);
		dvla_array[j][0] = 10 + j;
		dvla_array[j][1] = 11 + j;
		dvla_array[j][2] = 12 + j;
	}
	
	chs.clear();
	time_t when = time(NULL);
	for (int k =  0;k < 3;k++)
	{
		Tango::TimedCmdData<Tango::DevVarLongArray> tad(&dvla_array[k],when);
		chs.push(tad);
	}
	tg->fill_cmd_polling_buffer(this,cmd_name,chs);
	cout << "Command Polling buffer filled" << endl;
	
}


Tango::DevVarDoubleArray *DevTest::IOTemplInOut(Tango::DevDouble db)
{	
	cout << "[DevTest::IOTemplInOut] received " << db << endl;

    	Tango::DevVarDoubleArray *argout;
    	argout = new Tango::DevVarDoubleArray();
    
    	argout->length(2);
    	(*argout)[0] = db;
    	(*argout)[1] = db * 2;
 
    	return argout;
}

void DevTest::IOSetWAttrLimit(const Tango::DevVarDoubleArray *in)
{
	Tango::WAttribute &w_attr = dev_attr->get_w_attr_by_name("Double_attr_w");
	Tango::DevDouble limit = (*in)[1];
	if ((*in)[0] == 0.0)
	{
		w_attr.set_min_value(limit);
	}
	else
	{
		w_attr.set_max_value(limit);
	}
}


void DevTest::push_data_ready(const Tango::DevVarLongStringArray *in)
{
#ifndef COMPAT
	cout << "Pushing Data Ready event for attribute " << in->svalue[0].in() << endl;
	push_data_ready_event(in->svalue[0].in(),in->lvalue[0]);
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		DevTest::signal_handler()
// 
// description : 	command to read the device state
//
// out :		device state
//
//-----------------------------------------------------------------------------

void DevTest::signal_handler(long signo)
{
	cout << "[Device signal handler] received signal number " << signo;
	cout << " for device " << device_name << endl;
	
	DEBUG_STREAM << "[Device signal handler] received signal number "
		     << signo
	 	     << " for device "
		     << device_name
		     << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		DevTest::write_xxx
// 
// description : 	Write attribute methods
//
//-----------------------------------------------------------------------------

void DevTest::write_Short_attr_w(Tango::WAttribute &att)
{
//	cout << "In write_Short_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevShort sh;
	att.get_write_value(sh);
//	cout << "Attribute value = " << sh << endl;
}

void DevTest::write_Short_attr_w2(Tango::WAttribute &att)
{
	cout << "In write_Short_attr_w2 for attribute " << att.get_name() << endl;
	
	Tango::DevShort sh;
	att.get_write_value(sh);
	cout << "Attribute value = " << sh << endl;
}

void DevTest::write_Long_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Long_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevLong lg;
	att.get_write_value(lg);
	cout << "Attribute value = " << lg << endl;	
}

void DevTest::write_Double_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Double_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevDouble db;
	att.get_write_value(db);
	cout << "Attribute value = " << db << endl;
}

void DevTest::write_String_attr_w(Tango::WAttribute &att)
{
//	cout << "In write_String_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevString str;
	att.get_write_value(str);
//	cout << "Attribute value = " << str << endl;
}

void DevTest::write_String_attr_w2(Tango::WAttribute &att)
{
	cout << "In write_String_attr_w2 for attribute " << att.get_name() << endl;
	
	Tango::DevString str;
	att.get_write_value(str);
	cout << "Attribute value = " << str << endl;
}


void DevTest::write_attr_asyn_write(Tango::WAttribute &att)
{
	cout << "In write_attr_asyn_write for attribute " << att.get_name() << endl;
	
	Tango::DevLong lg;
	att.get_write_value(lg);
	cout << "Attribute value = " << lg << endl;
	Tango_sleep(2);
}

void DevTest::write_attr_asyn_write_to(Tango::WAttribute &att)
{
	cout << "In write_attr_asyn_write_to for attribute " << att.get_name() << endl;
	
	Tango::DevLong lg;
	att.get_write_value(lg);
	cout << "Attribute value = " << lg << endl;
	Tango_sleep(4);
}

void DevTest::write_attr_asyn_write_except(Tango::WAttribute &att)
{
	cout << "In write_attr_asyn_write_except for attribute " << att.get_name() << endl;

	Tango::DevLong lg;
	att.get_write_value(lg);
	cout << "Attribute value = " << lg << endl;
	Tango_sleep(2);
	Tango::Except::throw_exception((const char *)"aaa",
					(const char *)"This is a test",
					(const char *)"DevTest::write_attr_hardware");	
}

void DevTest::write_String_spec_attr_w(Tango::WAttribute &att)
{
	cout << "In write_String_spec_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::ConstDevString *ptr;
	att.get_write_value(ptr);
	long nb_str = att.get_write_value_length();
	cout << ", Elt nb = " << nb_str << endl;
	for (int k = 0;k < nb_str;k++)
		cout << ptr[k] << endl;
}

void DevTest::write_Short_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Short_attr_rw for attribute " << att.get_name() << endl;
	att.get_write_value(attr_short_rw);	
}

void DevTest::write_Long64_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Long64_attr_rw for attribute " << att.get_name() << endl;
	att.get_write_value(attr_long64_rw);	
}

void DevTest::write_ULong_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_ULong_attr_rw for attribute " << att.get_name() << endl;
	att.get_write_value(attr_ulong_rw);
cout << "received value = " << attr_ulong_rw << endl;
	
	if (attr_ulong_rw > 1000)
		att.set_write_value((Tango::DevULong)1111);
}

void DevTest::write_ULong64_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_ULong64_attr_rw for attribute " << att.get_name() << endl;
	att.get_write_value(attr_ulong64_rw);
cout << "Received value = " << attr_ulong64_rw << endl;	
}

void DevTest::write_State_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_State_attr_rw for attribute " << att.get_name() << endl;
	att.get_write_value(attr_state_rw);	
}

void DevTest::write_Float_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Float_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevFloat fl;
	att.get_write_value(fl);
	cout << "Attribute value = " << fl << endl;
}

void DevTest::write_Boolean_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Boolean_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevBoolean bo;
	att.get_write_value(bo);
	cout << "Attribute value = " << bo << endl;
}

void DevTest::write_UShort_attr_w(Tango::WAttribute &att)
{
	cout << "In write_UShort_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevUShort ush;
	att.get_write_value(ush);
	cout << "Attribute value = " << ush << endl;
}

void DevTest::write_UChar_attr_w(Tango::WAttribute &att)
{
	cout << "In write_UChar_attr_w for attribute " << att.get_name() << endl;
	
	Tango::DevUChar uch;
	att.get_write_value(uch);
	cout << "Attribute value = " << uch << endl;
}

void DevTest::write_Float_spec_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Float_spec_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevFloat *fl;
	att.get_write_value(fl);
	long nb_fl = att.get_write_value_length();
	cout << ", Elt nb = " << nb_fl << endl;
	for (int k = 0;k < nb_fl;k++)
		cout << fl[k] << endl;
}

void DevTest::write_Boolean_spec_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Boolean_spec_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevBoolean *bo;
	att.get_write_value(bo);
	long nb_bo = att.get_write_value_length();
	cout << ", Elt nb = " << nb_bo << endl;
	for (int k = 0;k < nb_bo;k++)
		cout << bo[k] << endl;
}

void DevTest::write_UShort_spec_attr_w(Tango::WAttribute &att)
{
	cout << "In write_UShort_spec_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevUShort *ush;
	att.get_write_value(ush);
	long nb_ush = att.get_write_value_length();
	cout << ", Elt nb = " << nb_ush << endl;
	for (int k = 0;k < nb_ush;k++)
		cout << ush[k] << endl;
}

void DevTest::write_UChar_spec_attr_w(Tango::WAttribute &att)
{
	cout << "In write_UChar_spec_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevUChar *uch;
	att.get_write_value(uch);
	long nb_uch = att.get_write_value_length();
	cout << ", Elt nb = " << nb_uch << endl;
//	for (int k = 0;k < nb_uch;k++)
//		cout << uch[k] << endl;
}

void DevTest::write_Float_ima_attr_w(Tango::WAttribute &att)
{
	cout << "In write_Float_ima_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevFloat *fl;
	att.get_write_value(fl);
	long nb_fl = att.get_write_value_length();
	cout << ", Elt nb = " << nb_fl << endl;
//	for (int k = 0;k < nb_fl;k++)
//		cout << fl[k] << endl;
}

void DevTest::write_UShort_ima_attr_w(Tango::WAttribute &att)
{
	cout << "In write_UShort_ima_attr_w for attribute " << att.get_name() << endl;
	
	const Tango::DevUShort *ush;
	att.get_write_value(ush);
	long nb_ush = att.get_write_value_length();
	cout << ", Elt nb = " << nb_ush << endl;
//	for (int k = 0;k < nb_ush;k++)
//		cout << ush[k] << endl;
}

void DevTest::write_Float_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Float_spec_attr_rw for attribute " << att.get_name() << endl;
	
	const Tango::DevFloat *fl;
	att.get_write_value(fl);
	long nb_fl = att.get_write_value_length();
	cout << ", Elt nb = " << nb_fl << endl;
//	for (int k = 0;k < nb_fl;k++)
//		cout << fl[k] << endl;
}

void DevTest::write_Long_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Long_spec_attr_rw for attribute " << att.get_name() << endl;
	
	const Tango::DevLong *fl;
	att.get_write_value(fl);
	long nb_fl = att.get_write_value_length();
	cout << ", Elt nb = " << nb_fl << endl;
	for (int k = 0;k < nb_fl;k++)
		cout << fl[k] << endl;
}

void DevTest::write_UChar_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_UShort_spec_attr_rw for attribute " << att.get_name() << endl;
	
	const Tango::DevUChar *uch;
	att.get_write_value(uch);
	long nb_uch = att.get_write_value_length();
	cout << ", Elt nb = " << nb_uch << endl;
//	for (int k = 0;k < nb_uch;k++)
//		cout << uch[k] << endl;
}

void DevTest::write_Boolean_ima_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Boolean_ima_attr_rw for attribute " << att.get_name() << endl;
	
	const Tango::DevBoolean *bo;
	att.get_write_value(bo);
	long nb_bo = att.get_write_value_length();
	cout << ", Elt nb = " << nb_bo << endl;
//	for (int k = 0;k < nb_bo;k++)
//		cout << bo[k] << endl;
}

void DevTest::write_UShort_ima_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_UShort_ima_attr_rw for attribute " << att.get_name() << endl;
	
	const Tango::DevUShort *ush;
	att.get_write_value(ush);
	long nb_ush = att.get_write_value_length();
	cout << ", Elt nb = " << nb_ush << endl;
//	for (int k = 0;k < nb_ush;k++)
//		cout << ush[k] << endl;
}


void DevTest::write_slow_actuator(Tango::WAttribute &att)
{
	cout << "In write_slow_actuator for attribute " << att.get_name() << endl;
	
	att.get_write_value(slow_actua);
#ifdef WIN32
	struct _timeb before_win;
	_ftime(&before_win);
	slow_actua_write.tv_sec = (unsigned long)before_win.time;
	slow_actua_write.tv_usec = (long)before_win.millitm * 1000;
#else
	gettimeofday(&slow_actua_write,NULL);
#endif
}


void DevTest::write_fast_actuator(Tango::WAttribute &att)
{
	cout << "In write_fast_actuator for attribute " << att.get_name() << endl;

	att.get_write_value(fast_actua);
	
	att.set_value(&fast_actua);
	att.set_quality(Tango::ATTR_CHANGING, true);
	att.set_value(&fast_actua);
	att.set_quality(Tango::ATTR_VALID, true);		
}


void DevTest::write_Long64_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Long64_spec_attr_rw for attribute " << att.get_name() << endl;
	const Tango::DevLong64 *lg64;
	long nb_data = att.get_write_value_length();
	att.get_write_value(lg64);
	cout << "Received " << nb_data << " for attribute " << att.get_name() << endl;	
}

void DevTest::write_ULong_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_ULong_spec_attr_rw for attribute " << att.get_name() << endl;
	const Tango::DevULong *ulg;
	long nb_data = att.get_write_value_length();
	att.get_write_value(ulg);
	cout << "Received " << nb_data << " for attribute " << att.get_name() << endl;	
}

void DevTest::write_ULong64_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_ULong64_spec_attr_rw for attribute " << att.get_name() << endl;
	const Tango::DevULong64 *ulg64;
	long nb_data = att.get_write_value_length();
	att.get_write_value(ulg64);
	cout << "Received " << nb_data << " for attribute " << att.get_name() << endl;	
}

void DevTest::write_State_spec_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_State_aspec_ttr_rw for attribute " << att.get_name() << endl;
	const Tango::DevState *sta;
	long nb_data = att.get_write_value_length();
	att.get_write_value(sta);
	cout << "Received " << nb_data << " for attribute " << att.get_name() << endl;	
}

void DevTest::write_Encoded_attr_rw(Tango::WAttribute &att)
{
	cout << "In write_Encoded_attr_rw for attribute " << att.get_name() << endl;
#ifndef COMPAT
	const Tango::DevEncoded *enc;
	att.get_write_value(enc);
	cout << "\tReceived string = " << enc->encoded_format << endl;
	cout << "\tReceived data nb = " << enc->encoded_data.length() << endl;
	for (unsigned int i = 0;i < enc->encoded_data.length();i++)
		cout << "\t\tdata[" << i << "] = " << (unsigned long)enc->encoded_data[i] << endl;
#endif
}

void DevTest::write_Poll_buffRW(Tango::WAttribute &att)
{
	cout << "In write_Poll_buffRW for attribute " << att.get_name() << endl;
}

//+----------------------------------------------------------------------------
//
// method : 		DevTest::read_xxx
// 
// description : 	Read attribute methods
//
//-----------------------------------------------------------------------------

void DevTest::read_Short_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Short_attr" << endl;
      	attr_short = 12;
      	if (Short_attr_except == false)
		{
      		att.set_value(&attr_short);
		}
      	else
      	{
        	Tango::Except::throw_exception((const char *)"aaa",
       			       (const char *)"This is a test",
			       (const char *)"DevTest::read_attr");
      	}
}

void DevTest::read_Long_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long_attr" << endl;
      	att.set_value(&attr_long);
}

void DevTest::read_Double_attr(Tango::Attribute &att)
{
    	cout << "[DevTest::read_attr] attribute name Double_attr" << endl;
    	attr_double = 3.2;
    	att.set_value(&attr_double);
}

void DevTest::read_String_attr(Tango::Attribute &att)
{
  	static Tango::DevString s = NULL;
	
      	cout << "[DevTest::read_attr] attribute name String_attr" << endl;
      	if (s == NULL)
		s = CORBA::string_dup("test_string");
      
      	att.set_value(&s);
}

void DevTest::read_Short_spec_attr(Tango::Attribute &att)
{
  	static short attr_short_array[20];

     	cout << "[DevTest::read_attr] attribute name Short_spec_attr" << endl;
      	attr_short_array[0] = 10;
      	attr_short_array[1] = 20;
      	attr_short_array[2] = 30;
      	attr_short_array[3] = 40;
      
      	att.set_value(attr_short_array, 4);
}

void DevTest::read_Long_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long_spec_attr" << endl;
      	attr_long_array[0] = 0;
      	attr_long_array[1] = 1;
      	attr_long_array[2] = 2;
      	attr_long_array[3] = 3;
      	attr_long_array[4] = 4;
      	attr_long_array[5] = 5;
      	attr_long_array[6] = 6;
      	attr_long_array[7] = 7;
      	attr_long_array[8] = 8;
      	attr_long_array[9] = 9;
      
      	att.set_value(attr_long_array, 10);
}

void DevTest::read_Double_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Double_spec_attr" << endl;
      	attr_db_array[0] = 1.11;
      	attr_db_array[1] = 2.22;
      
      	att.set_value(attr_db_array, 2);
}

void DevTest::read_String_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name String_spec_attr" << endl;
      	attr_str_array[0] = (char *)"Hello world";
      	attr_str_array[1] = (char *)"Hello universe";
      
      	att.set_value(attr_str_array, 2);
}

void DevTest::read_Short_ima_attr(Tango::Attribute &att)
{
  	static short attr_short_array_1[20];

      	cout << "[DevTest::read_attr] attribute name Short_ima_attr" << endl;
      	attr_short_array_1[0] = 40;
      	attr_short_array_1[1] = 60;
      	attr_short_array_1[2] = 80;
      	attr_short_array_1[3] = 100;
      
      	att.set_value(attr_short_array_1,2,2);
}

void DevTest::read_Long_ima_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long_ima_attr" << endl;
      	attr_long_array[0] = 0;
      	attr_long_array[1] = 1;
      	attr_long_array[2] = 2;
      	attr_long_array[3] = 3;
      	attr_long_array[4] = 4;
      	attr_long_array[5] = 5; 
          
      	att.set_value(attr_long_array,3,2);
}

void DevTest::read_Double_ima_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Double_ima_attr" << endl;
      	attr_db_array[0] = 5.55;
      	attr_db_array[1] = 6.66;
      
      	att.set_value(attr_db_array,2,1);
}

void DevTest::read_String_ima_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name String_ima_attr" << endl;
      	attr_str_array[0] = (char *)"Hello milky way";
      	attr_str_array[1] = (char *)"Hello moon";
      
      	att.set_value(attr_str_array,1,2);
}

void DevTest::read_attr_no_data(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_no_data" << endl;
      	att.set_quality(Tango::ATTR_VALID);
}

void DevTest::read_attr_wrong_type(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_wrong_type" << endl;
      	attr_long = 1246;
      	att.set_value(&attr_long);
}

void DevTest::read_attr_wrong_size(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_wrong_size" << endl;
      	att.set_value(&attr_long,1000,1000);
}

void DevTest::read_attr_no_alarm(Tango::Attribute &att)
{
     	cout << "[DevTest::read_attr] attribute name attr_no_alarm" << endl;
      	attr_long = 1246;
      	att.check_alarm();
      	att.set_value(&attr_long);
}

void DevTest::read_Short_attr_with_w(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Short_attr_with_w" << endl;
      	attr_short = 44;
      	att.set_value(&attr_short);
}

void DevTest::read_Long_attr_with_w(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long_attr_with_w" << endl;
      	attr_long = 1246;
      	att.set_value(&attr_long);
}

void DevTest::read_Short_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Short_attr_rw" << endl;
      	att.set_value(&attr_short_rw);
}

void DevTest::read_Long64_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long64_attr_rw" << endl;
      	att.set_value(&attr_long64_rw);
}

void DevTest::read_ULong_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name ULong_attr_rw" << endl;
      	att.set_value(&attr_ulong_rw);
}

void DevTest::read_ULong64_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name ULong64_attr_rw" << endl;
      	att.set_value(&attr_ulong64_rw);
}

void DevTest::read_State_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name State_attr_rw" << endl;
      	att.set_value(&attr_state_rw);
}

void DevTest::read_attr_asyn(Tango::Attribute &att)
{
     	 cout << "[DevTest::read_attr] attribute attr_asyn" << endl;
      	Tango_sleep(2);
      	attr_double = 5.55;
      	att.set_value(&attr_double);
      	cout << "Leaving reading attr_asyn attribute" << endl;
}

void DevTest::read_attr_asyn_to(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute attr_asyn_to" << endl;
      	Tango_sleep(4);
      	attr_double = 5.55;
      	att.set_value(&attr_double);
      	cout << "Leaving reading attr_asyn_to attribute" << endl;
}

void DevTest::read_attr_asyn_except(Tango::Attribute &att)
{
       	cout << "[DevTest::read_attr] attribute attr_asyn_except" << endl;
       	Tango_sleep(2);
       	cout << "Leaving reading attr_asyn_except attribute" << endl;

       	Tango::Except::throw_exception((const char *)"aaa",
       			       (const char *)"This is a test",
			       (const char *)"DevTest::read_attr");
}

void DevTest::read_PollLong_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name PollLong_attr" << endl;
      	PollLong_attr_num++;
      	if ((PollLong_attr_num % 2) == 0)
      		Poll_attr_long = 5555;
      	else
      		Poll_attr_long = 6666;
      	att.set_value(&Poll_attr_long);
}

void DevTest::read_PollString_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name PollString_spec_attr" << endl;
      	PollString_spec_attr_num++;
      	if ((PollString_spec_attr_num % 4) == 0)
      	{
      		attr_str_array[0] = (char *)"Hello world";
      		attr_str_array[1] = (char *)"Hello universe";
      		att.set_value(attr_str_array, 2);
      	}
      	else if ((PollString_spec_attr_num % 4) == 1)
      	{
      		attr_str_array[0] = (char *)"Hello Grenoble";
//      		attr_str_array[1] = (char *)"Hello Paris";
      		att.set_value(attr_str_array, 1);
      	}
      	else if ((PollString_spec_attr_num % 4) == 2)
      	{
      		Tango::Except::throw_exception((const char *)"aaaa",(const char *)"bbb",(const char *)"ccc");
      	}
      	else
      	{
      		Tango::Except::throw_exception((const char *)"xxx",(const char *)"yyy",(const char *)"zzz");
      	}
      
//      	att.set_value(attr_str_array, 2);
}	

void DevTest::read_attr_dq_sh(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_dq_sh" << endl;
      	attr_dq_short = 77;
#ifndef WIN32
      	struct timeval tv;
      	struct timezone tz;
      	gettimeofday(&tv,&tz);
      	att.set_value_date_quality(&attr_dq_short,tv,Tango::ATTR_VALID);
#else
	struct _timeb tv;
	_ftime(&tv);
      	att.set_value_date_quality(&attr_dq_short,tv,Tango::ATTR_VALID);
#endif
}

void DevTest::read_attr_dq_lo(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_dq_lo" << endl;
      	attr_dq_long = 7777;
      	time_t da;
      	time(&da);
      	att.set_value_date_quality(&attr_dq_long,da,Tango::ATTR_ALARM);
}

void DevTest::read_attr_dq_db(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name attr_dq_db" << endl;
      	attr_dq_double = 8.888;
#ifndef WIN32
      	struct timeval tv;
      	struct timezone tz;
      	gettimeofday(&tv,&tz);
      	att.set_value_date_quality(&attr_dq_double,tv,Tango::ATTR_VALID);
#else
	struct _timeb tv;
	_ftime(&tv);
      	att.set_value_date_quality(&attr_dq_double,tv,Tango::ATTR_VALID);
#endif
}

void DevTest::read_attr_dq_str(Tango::Attribute &att)
{
  	static Tango::DevString s = NULL;

      	cout << "[DevTest::read_attr] attribute name attr_dq_str" << endl;
      	s = attr_dq_str;
      	strcpy(attr_dq_str,"Setting value date and quality");
      	time_t da;
      	time(&da);
      	att.set_value_date_quality(&s,da,Tango::ATTR_ALARM);
}

void DevTest::read_Short_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Short_spec_attr_rw" << endl;
      	short_spec_attr[0] = 8;
      	short_spec_attr[1] = 9;
      
      	att.set_value(short_spec_attr, 2);
}
	
void DevTest::read_String_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name String_spec_attr_rw" << endl;
      	string_spec_attr[0] = "Thank's god";
      	string_spec_attr[1] = "It's friday";
            
      	att.set_value(const_cast<char **>(string_spec_attr), 2);
}

void DevTest::read_Long_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long_spec_attr_rw" << endl;
      	long_spec_attr[0] = 88;
      	long_spec_attr[1] = 99;
      	long_spec_attr[2] = 111;
      
      	att.set_value(long_spec_attr, 3);
}
	
void DevTest::read_Short_ima_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Short_ima_attr_rw" << endl;
      	short_ima_attr[0] = 6;
      	short_ima_attr[1] = 7;
      	short_ima_attr[2] = 8;
      	short_ima_attr[3] = 9;
            
      	att.set_value(short_ima_attr, 2,2);
}

void DevTest::read_String_ima_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name String_ima_attr_rw" << endl;
      	string_ima_attr[0] = "Alors la, pour une surprise";
      	string_ima_attr[1] = "c'est une surprise";
            
      	att.set_value(const_cast<char **>(string_ima_attr), 2,1);
}	

void DevTest::read_Event_change_tst(Tango::Attribute &att)
{
	cout << "[DevTest::read_attr] attribute name event_change_tst" << endl;
      	if (event_change_attr_except == false)
      	{
			if (event_throw_out_of_sync == true)
			{
				struct timespec sleep_time;
				sleep_time.tv_sec = 0;
				sleep_time.tv_nsec = 400000000;
				nanosleep(&sleep_time,NULL);
			}
      		att.set_value(attr_event,attr_event_size);
      	}
      	else
      	{
        	Tango::Except::throw_exception((const char *)"bbb",
       			       		       (const char *)"This is a test",
			       		       (const char *)"DevTest::read_attr");
      	}
}

void DevTest::read_Event64_change_tst(Tango::Attribute &att)
{
	cout << "[DevTest::read_attr] attribute name event64_change_tst" << endl;
      	if (event_change_attr_except == false)
      	{
      		att.set_value(attr_event64,attr_event64_size);
      	}
      	else
      	{
        	Tango::Except::throw_exception((const char *)"bbb64",
       			       		       (const char *)"This is a test",
			       		       (const char *)"DevTest::read_attr");
      	}
}
void DevTest::read_Event_quality_tst(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name event_quality_tst" << endl;
      	if (event_quality_attr_except == false)
      	{
        	if (attr_event_qua != Tango::ATTR_INVALID)
      			att.set_value(attr_qua_event,2);
      		att.set_quality(attr_event_qua);
      	}
      	else
      	{
       	 	Tango::Except::throw_exception((const char *)"ccc",
       			       (const char *)"This is a test",
			       (const char *)"DevTest::read_attr");
      	}
}


void DevTest::read_Poll_buff(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Poll_buff" << endl;
}

void DevTest::read_Poll_buffRW(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Poll_buffRW" << endl;
}

void DevTest::read_Float_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Float_attr" << endl;
      	attr_float = 4.5;
      	att.set_value(&attr_float);
}

void DevTest::read_Boolean_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Boolean_attr" << endl;
      	attr_boolean = true;
	att.set_value(&attr_boolean);
}

void DevTest::read_UShort_attr(Tango::Attribute &att)
{
    	cout << "[DevTest::read_attr] attribute name UShort_attr" << endl;
    	attr_ushort = 111;
    	att.set_value(&attr_ushort);
}

void DevTest::read_UChar_attr(Tango::Attribute &att)
{
    	cout << "[DevTest::read_attr] attribute name UChar_attr" << endl;
    	attr_uchar = 88;
    	att.set_value(&attr_uchar);	
}

void DevTest::read_Float_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Float_spec_attr" << endl;
      	attr_spec_float[0] = 4.5;
       	attr_spec_float[1] = 8.5;
      	attr_spec_float[2] = 16.5;
     	att.set_value(attr_spec_float,3);
}

void DevTest::read_Boolean_spec_attr(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Boolean_spec_attr" << endl;
      	attr_spec_boolean[0] = true;
      	attr_spec_boolean[1] = true;
      	attr_spec_boolean[2] = false;
      	attr_spec_boolean[3] = true;
      	attr_spec_boolean[4] = true;
	att.set_value(attr_spec_boolean,5);
}

void DevTest::read_UShort_spec_attr(Tango::Attribute &att)
{
    	cout << "[DevTest::read_attr] attribute name UShort_spec_attr" << endl;
    	attr_spec_ushort[0] = 333;
    	attr_spec_ushort[1] = 444;
    	att.set_value(attr_spec_ushort,2);
}

void DevTest::read_UChar_spec_attr(Tango::Attribute &att)
{
    	cout << "[DevTest::read_attr] attribute name UChar_spec_attr" << endl;
    	attr_spec_uchar[0] = 28;
    	attr_spec_uchar[1] = 45;
    	attr_spec_uchar[2] = 156;
    	attr_spec_uchar[3] = 34;
    	attr_spec_uchar[4] = 200;
    	attr_spec_uchar[5] = 12;
    	att.set_value(attr_spec_uchar,6);	
}

void DevTest::read_Float_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Float_spec_attr_rw" << endl;
      	attr_spec_float[0] = 5.5;
      	attr_spec_float[1] = 11.5;
      
      	att.set_value(attr_spec_float, 2);
}

void DevTest::read_UChar_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name UChar_spec_attr_rw" << endl;
      	attr_spec_uchar[0] = 22;
      	attr_spec_uchar[1] = 44;
      	attr_spec_uchar[2] = 66;
      
      	att.set_value(attr_spec_uchar, 3);
}

void DevTest::read_Boolean_ima_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Boolean_ima_attr_rw" << endl;
      	attr_spec_boolean[0] = true;
      	attr_spec_boolean[1] = false;
      
      	att.set_value(attr_spec_boolean, 2,1);
}

void DevTest::read_UShort_ima_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name UShort_ima_attr_rw" << endl;
      	attr_spec_ushort[0] = 2;
      	attr_spec_ushort[1] = 3;
      
      	att.set_value(attr_spec_ushort, 2,1);
}

void DevTest::read_slow_actuator(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name slow_actuator" << endl;
	slow_actua++;
	att.set_value(&slow_actua);
	
	struct timeval now;
#ifdef WIN32
	struct _timeb before_win;
	_ftime(&before_win);
	now.tv_sec = (unsigned long)before_win.time;
	now.tv_usec = (long)before_win.millitm * 1000;
#else
	gettimeofday(&now,NULL);
#endif
	long delta;
	if (slow_actua_write.tv_sec != 0)
	{
		COMPUTE_TIME_DIFF(delta,slow_actua_write,now);
		cout << "Delta time = " << delta << endl;
		if (delta > 3000)
		{
			att.set_quality(Tango::ATTR_VALID);
			slow_actua_write.tv_sec = 0;
		}
		else
		{
		att.set_quality(Tango::ATTR_CHANGING);
		}
	}
}

void DevTest::read_fast_actuator(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name fast_actuator" << endl;
	att.set_value(&fast_actua);
}

void DevTest::read_Long64_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Long64_spec_attr_rw" << endl;
      	att.set_value(attr_spec_long64_rw,3);
}

void DevTest::read_ULong_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name ULong_spec_attr_rw" << endl;
      	att.set_value(attr_spec_ulong_rw,3);
}

void DevTest::read_ULong64_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name ULong64_spec_attr_rw" << endl;
      	att.set_value(attr_spec_ulong64_rw,3);
}

void DevTest::read_State_spec_attr_rw(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name State_spec_attr_rw" << endl;
      	att.set_value(attr_spec_state_rw,2);
}

void DevTest::read_Sub_device_tst(Tango::Attribute &att)
{
      	cout << "[DevTest::read_attr] attribute name Sub_device_tst" << endl;
      	
		try
		{
			Tango::DeviceProxy *remote_dev;
			remote_dev = new Tango::DeviceProxy("dev/test/11");
			attr_sub_device_tst = true;
		}
		catch (...)
		{
			attr_sub_device_tst = false;
		}
		
		att.set_value(&attr_sub_device_tst);
}

void DevTest::read_Encoded_attr_rw(Tango::Attribute &att)
{
//		static int count = 0;
      	cout << "[DevTest::read_attr] attribute name Encoded_attr_rw" << endl;
#ifndef COMPAT

/*		if ((count % 2) == 0)
		{
			enc_attr.encoded_format = CORBA::string_dup("First string");
			enc_attr.encoded_data[0] = 11;
		}
		else
		{
			enc_attr.encoded_format = CORBA::string_dup("Second string");
			enc_attr.encoded_data[0] = 22;
		}*/

		att.set_value(&enc_attr);
//		count++;

/*		enc_attr_ptr = new Tango::DevEncoded;
		enc_attr_ptr->encoded_format = CORBA::string_dup("Which format?");
		enc_attr_ptr->encoded_data.length(4); 
  		enc_attr_ptr->encoded_data[0] = (unsigned char)97;
  		enc_attr_ptr->encoded_data[1] = (unsigned char)98;
  		enc_attr_ptr->encoded_data[2] = (unsigned char)99;
  		enc_attr_ptr->encoded_data[3] = (unsigned char)100;
    	
      	att.set_value(enc_attr_ptr,1,0,true);*/

/*		char *str_ptr = new char[80];
		strcpy(str_ptr,"Hola");

		unsigned char *data_ptr = new unsigned char [4];
		data_ptr[0] = (unsigned char)79;
		data_ptr[1] = (unsigned char)80;
		data_ptr[2] = (unsigned char)81;
		data_ptr[3] = (unsigned char)82;

		att.set_value(&str_ptr,data_ptr,4,true);*/

/*		att.set_value(&enc_format,&(enc_data[0]),2);	*/	
#endif
}

void DevTest::read_Encoded_attr_image(Tango::Attribute &att)
{
	cout << "[DevTest::read_attr] attribute name Encoded_attr_image" << endl;
#ifndef COMPAT

  // Build a dummy image
  unsigned char *imageData = (unsigned char *)malloc(256*256);
  for(int i=0;i<256;i++) {
    for(int j=0;j<256;j++) {
      imageData[i+j*256] = i;
    }
  }

  // Encode a JPEG image
  jpeg.encode_jpeg_gray8(imageData,256,256,50.0);
  att.set_value(&jpeg);
  free(imageData);

#endif
}
