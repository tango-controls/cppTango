////////////////////////////////////////////////////////////////////////////////
///
///  file       eventsupplier.h
///
/// 	        C++ include file for implementing the TANGO event server and 
///		client singleton classes - EventSupplier and EventConsumer.
///             These classes are used to send events from the server
///             to the notification service and to receive events from
///             the notification service.
///
/// 		author(s) : A.Gotz (goetz@esrf.fr)
///
/// 		original : 7 April 2003
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010
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
///
/// 		$Revision$
///
/// 		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _EVENT_SUPPLIER_API_H
#define _EVENT_SUPPLIER_API_H

#include <except.h>

#if defined (_TG_WINDOWS_) && defined (_USRDLL) && !defined(_TANGO_LIB)
#define USE_stub_in_nt_dll
#endif

#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>

#if defined (_TG_WINDOWS_) && defined (_USRDLL) && !defined(_TANGO_LIB)
#undef USE_stub_in_nt_dll
#endif


#include <omnithread.h>
#ifndef _TG_WINDOWS_
#include <sys/time.h>
#endif
#include <math.h>


namespace Tango
{

typedef struct _NotifService
{
	CosNotifyChannelAdmin::SupplierAdmin_var 				SupAdm;
	CosNotifyChannelAdmin::ProxyID 							pID;
	CosNotifyChannelAdmin::ProxyConsumer_var 				ProCon; 
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var 	StrProPush; 
	CosNotifyChannelAdmin::EventChannelFactory_var 			EveChaFac;
	CosNotifyChannelAdmin::EventChannel_var 				EveCha;
	string													ec_ior;
} NotifService;


class EventSupplier : public POA_CosNotifyComm::StructuredPushSupplier
{
public :

	TANGO_IMP_EXP static EventSupplier *create(CORBA::ORB_var,string,Database*,string &,Util *);
	void connect();
	void disconnect_structured_push_supplier();
	void disconnect_from_notifd();
	void subscription_change(const CosNotification::EventTypeSeq& added,const CosNotification::EventTypeSeq& deled);

	void push_heartbeat_event();
	string &get_event_channel_ior() {return event_channel_ior;}
	void set_svr_port_num(string &);

protected :

	EventSupplier(CORBA::ORB_var,
		CosNotifyChannelAdmin::SupplierAdmin_var,
		CosNotifyChannelAdmin::ProxyID, 
		CosNotifyChannelAdmin::ProxyConsumer_var, 
		CosNotifyChannelAdmin::StructuredProxyPushConsumer_var, 
		CosNotifyChannelAdmin::EventChannelFactory_var,
		CosNotifyChannelAdmin::EventChannel_var,
		string &);
		
private :
	static EventSupplier 									*_instance;
	CosNotifyChannelAdmin::EventChannel_var 				eventChannel;
	CosNotifyChannelAdmin::SupplierAdmin_var 				supplierAdmin;
	CosNotifyChannelAdmin::ProxyID 							proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var 				proxyConsumer;
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var 	structuredProxyPushConsumer;
	CosNotifyChannelAdmin::EventChannelFactory_var 			eventChannelFactory;
	CORBA::ORB_var 											orb_;
	
	inline int timeval_diff(TimeVal before, TimeVal after)
	{
		return ((after.tv_sec-before.tv_sec)*1000000 + after.tv_usec - before.tv_usec);
	}
	int 		heartbeat_period;
	int 		subscription_timeout;
	string 		event_channel_ior;
	string 		fqdn_prefix;

	void get_attribute_value(AttributeValue attr_value, LastAttrValue &curr_attr_value);
	void reconnect_notifd();
	TANGO_IMP_EXP static void connect_to_notifd(NotifService &,CORBA::ORB_var &,string &,Database *,string &,Util *);
			
	// Added a mutex to synchronize the access to 
	//	detect_and_push_change_event_3	and
	// detect_and_push_archive_event_3 which are used
	// from different threads
	omni_mutex		event_mutex;
	
	// Added a mutex to synchronize the access to 
	//	push_event_3 which is used
	// from different threads
	omni_mutex		push_mutex;
	
	// Added a mutex to synchronize the access to 
	//	detect_event_3 which is used
	// from different threads
	omni_mutex		detect_mutex;
	
public :
	void push_att_data_ready_event(DeviceImpl *,const string &,long,DevLong);
	void detect_and_push_events_3(DeviceImpl *,long,AttributeValue_3 *,AttributeValue_4 *,DevFailed *,string &);


/////////////////////////////////////////////////////////////////////////////
//
//			WARNING
//			-------
//
////////////////////////////////////////////////////////////////////////////

// Unfortunately, with Suse 9.3 with gcc 3.3.4 and binutils 2.15.94,
// it was not possible to generate the shared libs of Tango
// (linker error messages).
// To solve this problem, it was necessary to duplicate these methods
// instead of simply using a template method. This generates a code
// duplicate but...... (all methods finishing with xxxx_3)
//
// Test with gcc 3.4.5 with binutils 2.16.1 did not solve the problems
// We need to check with gcc 4.x.x if we could remove this code duplicate
// The other methods are in eventsupplier.cpp
//
//
//--------------------------------------------------------------------------

				    
	template <typename T>
	void detect_and_push_events(DeviceImpl *device_impl,
				    long idl_vers,
				    T &attr_value,
				    DevFailed *except,
				    string &attr_name)
	{
		string event, domain_name;
		time_t now, change_subscription, periodic_subscription, archive_subscription;

		cout3 << "EventSupplier::detect_and_push_events(): called for attribute " << attr_name << endl;

		Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

		now = time(NULL);
		change_subscription = now - attr.ext->event_change_subscription;
		periodic_subscription = now - attr.ext->event_periodic_subscription;
		archive_subscription = now - attr.ext->event_archive_subscription;

		cout3 << "EventSupplier::detect_and_push_events(): last subscription for change " << change_subscription << " periodic " << periodic_subscription << " archive " << archive_subscription << endl;
		if (change_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_change_event(device_impl,attr_value,attr,attr_name,except);
		}
		if (periodic_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_periodic_event(device_impl,attr_value,attr,attr_name,except);
		}
		if (archive_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_archive_event(device_impl,attr_value,attr,attr_name,except);
		}
	}
	
	
	template <typename T>
	void push_att_conf_events(DeviceImpl *device_impl,
				    T &attr_conf,
				    DevFailed *except,
				    string &attr_name)
	{
		string event, domain_name;
		time_t now, att_conf_subscription;

		cout3 << "EventSupplier::push_att_conf_events(): called for attribute " << attr_name << endl;

		Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

		now = time(NULL);
		att_conf_subscription = now - attr.ext->event_attr_conf_subscription;

		cout3 << "EventSupplier::push_att_conf_events(): last subscription " << att_conf_subscription << endl;
		
		vector<string> filterable_names;
		vector<double> filterable_data;
		vector<string> filterable_names_lg;
		vector<long> filterable_data_lg;
		
		string ev_type(CONF_TYPE_EVENT);

		push_event(device_impl,
			   ev_type,
			   filterable_names,
			   filterable_data,
			   filterable_names_lg,
			   filterable_data_lg,
		       attr_conf,
			   attr_name,
			   except);
	}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_change()
// 
// description : 	Method to detect if there is a change according to the
//			criterions and return a boolean set to true if a change
//			is detected
//
// argument : in :	attr : The attribute object
//			curr_attr_value : The current attribute value
//			archive : 
//			delta_change_rel :
//			delta_change_abs :
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//			force_change : A flag set to true if the change
//				       is due to a non mathematical reason
//				       (array size change, from exception to
//					classic...)
//			dev : Pointer to the device
//
//-----------------------------------------------------------------------------


	bool detect_change_3(Attribute &attr,
				  AttributeValue_3 *curr_attr_value,
				  AttributeValue_4 *curr_attr_value_4,
				  bool archive,
				  double &delta_change_rel,
				  double &delta_change_abs,
				  DevFailed *except,
				  bool &force_change,
				  DeviceImpl *dev);
				  
				  
	template <typename T>
	bool detect_change(Attribute &attr,
				  T &curr_attr_value,
				  bool archive,
				  double &delta_change_rel,
				  double &delta_change_abs,
				  DevFailed *except,
				  bool &force_change,
				  DeviceImpl *dev)
	{
		bool is_change = false;
		
		cout3 << "EventSupplier::detect_change(): called for attribute " << attr.get_name() << endl;
		
		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(detect_mutex);
		
//
// Send event, if the read_attribute failed or if it is the first time
// that the read_attribute succeed after a failure.
// Same thing if the attribute quality factor changes to INVALID
//
	
		if (except != NULL)
		{
			force_change = true;
			return true;
		}

		if (curr_attr_value.quality == Tango::ATTR_INVALID)
		{
			force_change = true;
			return true;
		}

		if (archive == true)
		{
			if ((except == NULL) && (attr.ext->prev_archive_event.err == true))
			{
				force_change = true;
				return true;
			}

			if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_archive_event.quality == Tango::ATTR_INVALID))
			{
				force_change = true;
				return true;
			}
		}
		else
		{		
			if ((except == NULL) && (attr.ext->prev_change_event.err == true))
			{
				force_change = true;
				return true;
			}

			if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_change_event.quality == Tango::ATTR_INVALID))
			{
				force_change = true;
				return true;
			}
		}

		const DevVarLong64Array *curr_seq_64, *prev_seq_64;	
		const DevVarLongArray *curr_seq_lo, *prev_seq_lo;
		const DevVarShortArray *curr_seq_sh, *prev_seq_sh;
		const DevVarDoubleArray *curr_seq_db, *prev_seq_db;
		const DevVarStringArray *curr_seq_str, *prev_seq_str;
		const DevVarFloatArray *curr_seq_fl, *prev_seq_fl;
		const DevVarBooleanArray *curr_seq_bo, *prev_seq_bo;
		const DevVarUShortArray *curr_seq_ush, *prev_seq_ush;
		const DevVarCharArray *curr_seq_uch, *prev_seq_uch;
		const DevVarULongArray *curr_seq_ulo, *prev_seq_ulo;
		const DevVarULong64Array *curr_seq_u64, *prev_seq_u64;
		const DevVarStateArray *curr_seq_state, *prev_seq_state;		
		
		double rel_change[2], abs_change[2];
		unsigned int i;
		unsigned int curr_seq_nb,prev_seq_nb;
		bool inited;

		CORBA::TypeCode_var ty = curr_attr_value.value.type();
		delta_change_rel = delta_change_abs = 0;

		bool enable_check = false;
		
		TangoMonitor &mon1 = dev->get_att_conf_monitor();
		mon1.get_monitor();
		if (!archive)
		{
			rel_change[0] = attr.ext->rel_change[0];
			rel_change[1] = attr.ext->rel_change[1];
			abs_change[0] = attr.ext->abs_change[0];
			abs_change[1] = attr.ext->abs_change[1];
			inited = attr.ext->prev_change_event.inited;
			if ((attr.ext->prev_change_event.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
				    enable_check = true;
		}
		else
		{
			rel_change[0] = attr.ext->archive_rel_change[0];
			rel_change[1] = attr.ext->archive_rel_change[1];
			abs_change[0] = attr.ext->archive_abs_change[0];
			abs_change[1] = attr.ext->archive_abs_change[1];
			inited = attr.ext->prev_archive_event.inited;
			if ((attr.ext->prev_archive_event.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
				    enable_check = true;		
		}
		mon1.rel_monitor();

		if (inited)
		{	
			if (enable_check == true)
			{
				CORBA::TypeCode_var ty_alias = ty->content_type();
				CORBA::TypeCode_var ty_seq = ty_alias->content_type();					
				switch (ty_seq->kind())
				{
				case CORBA::tk_long:		
					curr_attr_value.value >>= curr_seq_lo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_lo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_lo;
					curr_seq_nb = curr_seq_lo->length();
					prev_seq_nb = prev_seq_lo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_lo->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_lo)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_lo)[i] - (*prev_seq_lo)[i])*100/(*prev_seq_lo)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_lo)[i] == (*prev_seq_lo)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_lo)[i] - (*prev_seq_lo)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_longlong:
					curr_attr_value.value >>= curr_seq_64;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_64;
					else
						attr.ext->prev_change_event.value >>= prev_seq_64;
					curr_seq_nb = curr_seq_64->length();
					prev_seq_nb = prev_seq_64->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_64->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_64)[i] != 0)
							{
								delta_change_rel = (double)(((*curr_seq_64)[i] - (*prev_seq_64)[i])*100/(*prev_seq_64)[i]);
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_64)[i] == (*prev_seq_64)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (double)((*curr_seq_64)[i] - (*prev_seq_64)[i]);
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_short:
					curr_attr_value.value >>= curr_seq_sh;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_sh;
					else
						attr.ext->prev_change_event.value >>= prev_seq_sh;
					curr_seq_nb = curr_seq_sh->length();
					prev_seq_nb = prev_seq_sh->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_sh->length(); i++)
					{	
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_sh)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_sh)[i] - (*prev_seq_sh)[i])*100/(*prev_seq_sh)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_sh)[i] == (*prev_seq_sh)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_sh)[i] - (*prev_seq_sh)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_double:
					curr_attr_value.value >>= curr_seq_db;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_db;
					else
						attr.ext->prev_change_event.value >>= prev_seq_db;
					curr_seq_nb = curr_seq_db->length();
					prev_seq_nb = prev_seq_db->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_db->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_db)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_db)[i] - (*prev_seq_db)[i])*100/(*prev_seq_db)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_db)[i] == (*prev_seq_db)[i]) delta_change_rel = 0;
							}
							
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_db)[i] - (*prev_seq_db)[i];
							
							// Correct for rounding errors !
							double max_change = delta_change_abs + (abs_change[1] * 1e-10);
							double min_change = delta_change_abs + (abs_change[0] * 1e-10);

							//if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							if (min_change <= abs_change[0] || max_change >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_string:
					curr_attr_value.value >>= curr_seq_str;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_str;
					else
						attr.ext->prev_change_event.value >>= prev_seq_str;
					curr_seq_nb = curr_seq_str->length();
					prev_seq_nb = prev_seq_str->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_str->length(); i++)
					{
						if (strcmp((*curr_seq_str)[i],(*prev_seq_str)[i]) != 0)
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;

				case CORBA::tk_float:
					curr_attr_value.value >>= curr_seq_fl;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_fl;
					else
						attr.ext->prev_change_event.value >>= prev_seq_fl;
					curr_seq_nb = curr_seq_fl->length();
					prev_seq_nb = prev_seq_fl->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_fl->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_fl)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_fl)[i] - (*prev_seq_fl)[i])*100/(*prev_seq_fl)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_fl)[i] == (*prev_seq_fl)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_fl)[i] - (*prev_seq_fl)[i];
							
							// Correct for rounding errors !
							double max_change = delta_change_abs + (abs_change[1] * 1e-10);
							double min_change = delta_change_abs + (abs_change[0] * 1e-10);
															
							//if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							if (min_change <= abs_change[0] || max_change >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_ushort:
					curr_attr_value.value >>= curr_seq_ush;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_ush;
					else
						attr.ext->prev_change_event.value >>= prev_seq_ush;
					curr_seq_nb = curr_seq_ush->length();
					prev_seq_nb = prev_seq_ush->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_ush->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_ush)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_ush)[i] - (*prev_seq_ush)[i])*100/(*prev_seq_ush)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_ush)[i] == (*prev_seq_ush)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_ush)[i] - (*prev_seq_ush)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_boolean:
					curr_attr_value.value >>= curr_seq_bo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_bo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_bo;
					curr_seq_nb = curr_seq_bo->length();
					prev_seq_nb = prev_seq_bo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_bo->length(); i++)
					{
						if ((*curr_seq_bo)[i] != (*prev_seq_bo)[i])
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;

				case CORBA::tk_octet:
					curr_attr_value.value >>= curr_seq_uch;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_uch;
					else
						attr.ext->prev_change_event.value >>= prev_seq_uch;
					curr_seq_nb = curr_seq_uch->length();
					prev_seq_nb = prev_seq_uch->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_uch->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_uch)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_uch)[i] - (*prev_seq_uch)[i])*100/(*prev_seq_uch)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_uch)[i] == (*prev_seq_uch)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_uch)[i] - (*prev_seq_uch)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_ulong:		
					curr_attr_value.value >>= curr_seq_ulo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_ulo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_ulo;
					curr_seq_nb = curr_seq_ulo->length();
					prev_seq_nb = prev_seq_ulo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_ulo->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_ulo)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_ulo)[i] - (*prev_seq_ulo)[i])*100/(*prev_seq_ulo)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_ulo)[i] == (*prev_seq_ulo)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_ulo)[i] - (*prev_seq_ulo)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_ulonglong:
					curr_attr_value.value >>= curr_seq_u64;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_u64;
					else
						attr.ext->prev_change_event.value >>= prev_seq_u64;
					curr_seq_nb = curr_seq_u64->length();
					prev_seq_nb = prev_seq_u64->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_u64->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_u64)[i] != 0)
							{
								delta_change_rel = (double)(((*curr_seq_u64)[i] - (*prev_seq_u64)[i])*100/(*prev_seq_u64)[i]);
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_u64)[i] == (*prev_seq_u64)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (double)((*curr_seq_u64)[i] - (*prev_seq_u64)[i]);
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_enum:
					curr_attr_value.value >>= curr_seq_state;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_state;
					else
						attr.ext->prev_change_event.value >>= prev_seq_state;
					curr_seq_nb = curr_seq_state->length();
					prev_seq_nb = prev_seq_state->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_state->length(); i++)
					{
						if ((*curr_seq_state)[i] != (*prev_seq_state)[i])
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;
					
				default:
					break;
				}
			}
		}	

		cout3 << "EventSupplier::detect_change(): leaving for attribute " << attr.get_name() << endl;
		return(is_change);
	}


	void detect_and_push_change_event_3(DeviceImpl *device_impl,
						 AttributeValue_3 *attr_value,
						 AttributeValue_4 *attr_value_4,
						 Attribute &attr,
						 string &attr_name,
						 DevFailed *except);
	
	template <typename T>
	void detect_and_push_change_event(DeviceImpl *device_impl,
						 T &attr_value,
						 Attribute &attr,
						 string &attr_name,
						 DevFailed *except)
	{
		string event, domain_name;
		double delta_change_rel = 0.0;
		double delta_change_abs = 0.0;
		bool is_change      = false;
		bool force_change   = false;
		bool quality_change = false;

		cout3 << "EventSupplier::detect_and_push_change_event(): called for attribute " << attr_name << endl;

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);

//
// if no attribute of this name is registered with change then
// insert the current value
//

		if (!attr.ext->prev_change_event.inited)
		{
			if (except != NULL)
				attr.ext->prev_change_event.err = true;
			else
			{
				attr.ext->prev_change_event.value = attr_value.value;
				attr.ext->prev_change_event.quality = attr_value.quality;
				attr.ext->prev_change_event.err = false;
			}
			attr.ext->prev_change_event.inited = true;
		}
		else
		{
	
//
// determine delta_change in percent compared with previous event sent
// 
			is_change = detect_change(attr,attr_value,false,delta_change_rel,delta_change_abs,except,force_change,device_impl);
			cout3 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change " << delta_change_abs << " is change = " << is_change << endl;
		}
		
		// check whether the data quality has changed.
		// Fire event on a quality change.
		
		if ( attr.ext->prev_change_event.quality != attr_value.quality )
			{
			is_change = true;
			quality_change = true;
			}
		
	
		if (is_change)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
				attr.ext->prev_change_event.err = true;
			else
			{
				attr.ext->prev_change_event.value = attr_value.value;
				attr.ext->prev_change_event.quality = attr_value.quality;
				attr.ext->prev_change_event.err = false;
			}

			domain_name = device_impl->get_name() + "/" + attr_name;
			filterable_names.push_back("delta_change_rel");
			filterable_data.push_back(delta_change_rel);
			filterable_names.push_back("delta_change_abs");
			filterable_data.push_back(delta_change_abs);
			
			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);
			
			filterable_names.push_back("quality");
			if (quality_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);				
			
			push_event(device_impl,
				   "change",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	cout3 << "EventSupplier::detect_and_push_change_event(): leaving for attribute " << attr_name << endl;		
	}

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_archive_event()
// 
// description : 	Method to detect if there it is necessary
//			to push an archive event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr_value_4 : The attribute value for devic eimplementing IDL 4
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

	void detect_and_push_archive_event_3(DeviceImpl *device_impl,
						  AttributeValue_3 *attr_value,
						  AttributeValue_4 *attr_value_4, 
						  Attribute &attr,
						  string &attr_name,
						  DevFailed *except);
						  
	template <typename T>
	void detect_and_push_archive_event(DeviceImpl *device_impl,
						  T &attr_value, 
						  Attribute &attr,
						  string &attr_name,
						  DevFailed *except)
	{
		string event, domain_name;
		double delta_change_rel = 0.0;
		double delta_change_abs = 0.0;
		bool is_change = false;
		bool force_change = false;
		bool period_change = false;
		bool value_change = false;

		cout3 << "EventSupplier::detect_and_push_archive_event(): called for attribute " << attr_name << endl;

		double now_ms, ms_since_last_periodic;
#ifdef _TG_WINDOWS_
        	struct _timeb           now_win;
#endif
        	struct timeval          now_timeval;

#ifdef _TG_WINDOWS_
		_ftime(&now_win);
		now_timeval.tv_sec = (unsigned long)now_win.time;
		now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now_timeval,NULL);
#endif

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);
		
		now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;
		ms_since_last_periodic = now_ms - attr.ext->archive_last_periodic;

		int arch_period;
		TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
		mon1.get_monitor();
		arch_period = attr.ext->archive_period;
		mon1.rel_monitor();

		// Specify the precision interval for the archive period testing
		// 2% are used for periods < 5000 ms and
		// 100ms are used for periods > 5000 ms.
	
		if ( arch_period >= 5000 )
			{
			 arch_period = arch_period - DELTA_PERIODIC_LONG;
			}
		else
			{
#ifdef _TG_WINDOWS_
			double tmp = (double)arch_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
					eve_round = ceil(tmp);
			else
					eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD))
			double eve_round = rint((double)arch_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)arch_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)arch_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			arch_period = (int)eve_round;
			}
		
		if ((ms_since_last_periodic > arch_period) && (attr.ext->prev_archive_event.inited == true))
		{
			is_change = true;
			period_change = true;
		}
		
//
// if no attribute of this name is registered with change then
// insert the current value
//


		if (!attr.ext->prev_archive_event.inited)
		{
			if (except != NULL)
				attr.ext->prev_archive_event.err = true;
			else
			{
				attr.ext->prev_archive_event.value = attr_value.value;
				attr.ext->prev_archive_event.quality = attr_value.quality;
				attr.ext->prev_archive_event.err = false;
			}
			attr.ext->archive_last_periodic = now_ms;
			attr.ext->prev_archive_event.inited = true;
		}
		else
		{
	
//
// determine delta_change in percent compared with previous event sent
//

			if (is_change == false)
			{
				is_change = detect_change(attr,attr_value,true,
							  delta_change_rel,
							  delta_change_abs,except,
							  force_change,device_impl);
				if (is_change == true)
					value_change = true;
			}
		}
	
		if (is_change)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			domain_name = device_impl->get_name() + "/" + attr_name;
//			if (value_change == true)
//			{
			if (except != NULL)
				attr.ext->prev_archive_event.err = true;
			else
			{
				attr.ext->prev_archive_event.value = attr_value.value;
				attr.ext->prev_archive_event.quality = attr_value.quality;
				attr.ext->prev_archive_event.err = false;
			}
//			}

			filterable_names_lg.push_back("counter");
			if (period_change == true)
			{
				attr.ext->archive_periodic_counter++;
				attr.ext->archive_last_periodic = now_ms;
				filterable_data_lg.push_back(attr.ext->archive_periodic_counter);
			}
			else
			{
				filterable_data_lg.push_back(-1);
			}

			filterable_names.push_back("delta_change_rel");
			filterable_data.push_back(delta_change_rel);
			filterable_names.push_back("delta_change_abs");
			filterable_data.push_back(delta_change_abs);
			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);		

			push_event(device_impl,
				   "archive",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_quality_change_event()
// 
// description : 	Method to detect if there it is necessary
//			to push a quality change event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

	template <typename T>
	void detect_and_push_quality_change_event(DeviceImpl *device_impl,
						  T &attr_value,
						  Attribute &attr,
						  string &attr_name,
						  DevFailed *except)
	{
		cout3 << "EventSupplier::detect_and_push_quality_change_event(): called for attribute " << attr_name << endl;
		bool is_change = false;
	
//
// if no attribute of this name is registered with change then
// insert the current value
//

		if (!attr.ext->prev_quality_event.inited)
		{
			if (except != NULL)
				attr.ext->prev_quality_event.err = true;
			else
			{
				attr.ext->prev_quality_event.quality = attr_value.quality;
				attr.ext->prev_quality_event.err = false;
			}
			attr.ext->prev_quality_event.inited = true;
		}
		else
		{
			if (except != NULL)
				is_change = true;
			else if ((except == NULL) && (attr.ext->prev_quality_event.err == true))
				is_change = true;
			else if (attr_value.quality != attr.ext->prev_quality_event.quality)
				is_change = true;
			else
				is_change = false;	
		}
	
//
// Send the event if necessary
//
	
		if (is_change)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
				attr.ext->prev_quality_event.err = true;
			else
			{
				attr.ext->prev_quality_event.quality = attr_value.quality;
				attr.ext->prev_quality_event.err = false;
			}

			push_event(device_impl,
				   "quality",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_periodic_event()
// 
// description : 	Method to detect if there it is necessary
//			to push a periodic event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr_value_4 : The attribute value for device implementing IDL 4
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

	void detect_and_push_periodic_event_3(DeviceImpl *device_impl,
					    AttributeValue_3 *attr_value,
					 	AttributeValue_4 *attr_value_4,
					    Attribute &attr,
					    string &attr_name,
					    DevFailed *except);
					    

	template <typename T>
	void detect_and_push_periodic_event(DeviceImpl *device_impl,
					    T &attr_value,
					    Attribute &attr,
					    string &attr_name,
					    DevFailed *except)
	{
		string event, domain_name;
		double now_ms, ms_since_last_periodic;
#ifdef _TG_WINDOWS_
        	struct _timeb           now_win;
#endif
        	struct timeval          now_timeval;

#ifdef _TG_WINDOWS_
		_ftime(&now_win);
		now_timeval.tv_sec = (unsigned long)now_win.time;
		now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now_timeval,NULL);
#endif

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);

		now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;

		int eve_period;		
		TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
		mon1.get_monitor();
		eve_period = attr.ext->event_period;
		mon1.rel_monitor();

		// Specify the precision interval for the event period testing
		// 2% are used for periods < 5000 ms and
		// 100ms are used for periods > 5000 ms.
	
		if ( eve_period >= 5000 )
			{
			 eve_period = eve_period - DELTA_PERIODIC_LONG;
			}
		else
			{
#ifdef _TG_WINDOWS_
			double tmp = (double)eve_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
					eve_round = ceil(tmp);
			else
					eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD))
			double eve_round = rint((double)eve_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)eve_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)eve_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			eve_period = (int)eve_round;
			}

		ms_since_last_periodic = now_ms - attr.ext->last_periodic;
		cout3 << "EventSupplier::detect_and_push_is_periodic_event(): delta since last periodic " << ms_since_last_periodic << " event_period " << eve_period << " for " << device_impl->get_name()+"/"+attr_name << endl;

		if (ms_since_last_periodic > eve_period)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			attr.ext->periodic_counter++;
			attr.ext->last_periodic = now_ms;
			filterable_names_lg.push_back("counter");
			filterable_data_lg.push_back(attr.ext->periodic_counter);

			cout3 << "EventSupplier::detect_and_push_is_periodic_event(): detected periodic event for " << device_impl->get_name()+"/"+attr_name << endl;
			push_event(device_impl,
				   "periodic",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}

	}

	
//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::push_event()
// 
// description : 	Method to send the event to the event channel
//
// argument : in :	device_impl : The device
//			event_type : The event type (change, periodic....)
//			filterable_names : 
//			filterable_data :
//			attr_value : The attribute value
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

	void push_event_3(DeviceImpl *device_impl,
				string event_type,
				vector<string> &filterable_names,
		       	vector<double> &filterable_data,
		       	vector<string> &filterable_names_lg,
		       	vector<long> &filterable_data_lg,
		       	AttributeValue_3 *attr_value,
				AttributeValue_4 *attr_value_4,
		       	string &attr_name,
		       	DevFailed *except);
			

	template <typename T>
	void push_event(DeviceImpl *device_impl,
				string event_type,
				vector<string> &filterable_names,
		       	vector<double> &filterable_data,
		       	vector<string> &filterable_names_lg,
		       	vector<long> &filterable_data_lg,
		       	T &attr_value,
		       	string &attr_name,
		       	DevFailed *except)
	{
		CosNotification::StructuredEvent struct_event;
		string domain_name;

		cout3 << "EventSupplier::push_event(): called for attribute " << attr_name << endl;
		
		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(push_mutex);
		
		string loc_attr_name = attr_name;	
		transform(loc_attr_name.begin(),loc_attr_name.end(),loc_attr_name.begin(),::tolower);
		domain_name = device_impl->get_name_lower() + "/" + loc_attr_name;

		struct_event.header.fixed_header.event_type.domain_name = CORBA::string_dup(domain_name.c_str());
  		struct_event.header.fixed_header.event_type.type_name = CORBA::string_dup(fqdn_prefix.c_str());

		struct_event.header.variable_header.length( 0 );

		unsigned long nb_filter = filterable_names.size();
		unsigned long nb_filter_lg = filterable_names_lg.size();

		struct_event.filterable_data.length(nb_filter + nb_filter_lg);

		if (nb_filter != 0)
		{	
			if (nb_filter == filterable_data.size())
			{
				for (unsigned long i = 0; i < nb_filter; i++)
				{
  					struct_event.filterable_data[i].name = CORBA::string_dup(filterable_names[i].c_str());
					struct_event.filterable_data[i].value <<= (CORBA::Double) filterable_data[i];
				}
			}
		}

		if (nb_filter_lg != 0)
		{	
			if (nb_filter_lg == filterable_data_lg.size())
			{
				for (unsigned long i = 0; i < nb_filter_lg; i++)
				{
  					struct_event.filterable_data[i + nb_filter].name = CORBA::string_dup(filterable_names_lg[i].c_str());
		  			struct_event.filterable_data[i + nb_filter].value <<= (CORBA::Long) filterable_data_lg[i];
				}
			}
		}

		if (except == NULL)	
			struct_event.remainder_of_body <<= attr_value;
		else
			struct_event.remainder_of_body <<= except->errors;
  		struct_event.header.fixed_header.event_name = CORBA::string_dup(event_type.c_str());

		cout3 << "EventSupplier::push_event(): push event " << event_type << " for " << device_impl->get_name() + "/" + attr_name << endl;

//
// Push the event
//

		bool fail = false;
		try
		{
			structuredProxyPushConsumer -> push_structured_event(struct_event);
		}
		catch(const CosEventComm::Disconnected&)
		{
			cout3 << "EventSupplier::push_event() event channel disconnected !\n";
			fail = true;
		}
    	catch(const CORBA::TRANSIENT &)
    	{
    		cout3 << "EventSupplier::push_event() caught a CORBA::TRANSIENT ! " << endl;
			fail = true;
    	}
    	catch(const CORBA::COMM_FAILURE &)
    	{
    		cout3 << "EventSupplier::push_event() caught a CORBA::COMM_FAILURE ! " << endl;
			fail = true;
		}
		catch(const CORBA::SystemException &)
		{
    		cout3 << "EventSupplier::push_event() caught a CORBA::SystemException ! " << endl;
			fail = true;
		}
	
//
// If it was not possible to communicate with notifd,
// try a reconnection
//

		if (fail == true)
		{
			try
			{
				reconnect_notifd();
			}
			catch (...) {}
		}

	}

};

} // End of namespace


#endif // _EVENT_SUPPLIER_API_H

