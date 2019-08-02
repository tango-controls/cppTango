//===================================================================================================================
//
// file :               device_3.tpp
//
// description :        C++ source code for the Device_3Impl class template methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012,2013,2014,2015
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
//
//==================================================================================================================

#ifndef _DEVICE_3_TPP
#define _DEVICE_3_TPP

namespace Tango
{

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_3Impl::set_attribute_config_3_local
//
// description :
//		Set attribute configuration for both AttributeConfig_3 and AttributeConfig_5
//
// args :
//		in :
// 			- new_conf : The new attribute configuration
//			- dummy_arg : Dummy and unnused arg. Just to help template coding
//			- fwd_cb : Set to true if called from fwd att call back
//			- caller_idl : IDL release used by caller
//
//----------------------------------------------------------------------------------------------------------------

template <typename T,typename V>
void Device_3Impl::set_attribute_config_3_local(const T &new_conf,TANGO_UNUSED(const V &dummy_arg),
												bool fwd_cb,int caller_idl)
{
	cout4 << "Entering Device_3Impl::set_attribute_config_3_local" << std::endl;

//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)API_AttrNotFound,
				        (const char *)"The device does not have any attribute",
				        (const char *)"Device_3Impl::set_attribute_config_3_local");
	}

//
// Get some event related data
//

	EventSupplier *event_supplier_nd = NULL;
	EventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Tango::Util::instance();

//
// Update attribute config first locally then in database
//

	long nb_attr = new_conf.length();
	long i;

    EventSupplier::SuppliedEventData ad;
    ::memset(&ad,0,sizeof(ad));

	try
	{
		for (i = 0;i < nb_attr;i++)
		{
			Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
			bool old_alarm = attr.is_alarmed().any();

//
// Special case for forwarded attributes
//

			if (attr.is_fwd_att() == true)
			{
				FwdAttribute &fwd_attr = static_cast<FwdAttribute &>(attr);
				if (fwd_cb == true)
					fwd_attr.set_att_config(new_conf[i]);
				else
				{
					fwd_attr.upd_att_config_base(new_conf[i].label.in());
					fwd_attr.upd_att_config(new_conf[i]);
				}
			}
			else
			{
				attr.set_upd_properties(new_conf[i],device_name);
			}

//
// In case the attribute quality factor was set to ALARM, reset it to VALID
//

			if ((attr.get_quality() == Tango::ATTR_ALARM) &&
				(old_alarm == true) &&
				(attr.is_alarmed().any() == false))
				attr.set_quality(Tango::ATTR_VALID);

//
// Send the event
//

			if (attr.use_notifd_event() == true)
				event_supplier_nd = tg->get_notifd_event_supplier();
			else
				event_supplier_nd = NULL;

			if (attr.use_zmq_event() == true)
				event_supplier_zmq = tg->get_zmq_event_supplier();
			else
				event_supplier_zmq = NULL;

			if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
			{
				std::string tmp_name(new_conf[i].name);

//
// The event data has to be the new attribute conf which could be different than the one we received (in case some
// of the parameters are reset to lib/user/class default value)
//

				V mod_conf;
				attr.get_prop(mod_conf);

				const V *tmp_ptr = &mod_conf;

				Tango::AttributeConfig_3 conf3;
				Tango::AttributeConfig_5 conf5;
				AttributeConfig_3 *tmp_conf_ptr;
				AttributeConfig_5 *tmp_conf_ptr5;

				if (get_dev_idl_version() > 4)
				{
					std::vector<int> cl_lib = attr.get_client_lib(ATTR_CONF_EVENT);

					if (caller_idl <= 4)
					{

//
// Even if device is IDL 5, the change has been done from one old client (IDL4) thus with AttributeConfig_3.
// If a new client is listening to event, don't forget to send it.
//

						for (size_t i = 0;i < cl_lib.size();i++)
						{
							if (cl_lib[i] == 5)
							{
								attr.AttributeConfig_3_2_AttributeConfig_5(mod_conf,conf5);
								attr.add_config_5_specific(conf5);
								tmp_conf_ptr5 = &conf5;

								::memcpy(&(ad.attr_conf_5),&(tmp_conf_ptr5),sizeof(V *));
							}
							else
							{
								::memcpy(&(ad.attr_conf_3),&(tmp_ptr),sizeof(V *));
							}

							if (event_supplier_nd != NULL)
								event_supplier_nd->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
							if (event_supplier_zmq != NULL)
								event_supplier_zmq->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);

							if (cl_lib[i] == 5)
								ad.attr_conf_5 = NULL;
							else
								ad.attr_conf_3 = NULL;
						}
					}
					else
					{
						for (size_t i = 0;i < cl_lib.size();i++)
						{
							if (cl_lib[i] < 5)
							{
								attr.AttributeConfig_5_2_AttributeConfig_3(mod_conf,conf3);
								tmp_conf_ptr = &conf3;

								::memcpy(&(ad.attr_conf_3),&(tmp_conf_ptr),sizeof(V *));
							}
							else
							{
								::memcpy(&(ad.attr_conf_5),&(tmp_ptr),sizeof(V *));
							}

							if (event_supplier_nd != NULL)
								event_supplier_nd->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
							if (event_supplier_zmq != NULL)
								event_supplier_zmq->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);

							if (cl_lib[i] == 5)
								ad.attr_conf_5 = NULL;
							else
								ad.attr_conf_3 = NULL;
						}
					}
				}
				else
				{
					::memcpy(&(ad.attr_conf_3),&(tmp_ptr),sizeof(V *));

					if (event_supplier_nd != NULL)
						event_supplier_nd->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
					if (event_supplier_zmq != NULL)
						event_supplier_zmq->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
				}
			}
		}

	}
	catch (Tango::DevFailed &e)
	{

//
// Re build the list of "alarmable" attribute
//

		dev_attr->get_alarm_list().clear();
		for (long j = 0;j < nb_dev_attr;j++)
		{
			Attribute &att = dev_attr->get_attr_by_ind(j);
			if (att.is_alarmed().any() == true)
			{
				if (att.get_writable() != Tango::WRITE)
					dev_attr->get_alarm_list().push_back(j);
			}
		}

//
// Change the exception reason flag
//

		TangoSys_OMemStream o;

		o << e.errors[0].reason;
		if (i != 0)
			o << "\nAll previous attribute(s) have been successfully updated";
		if (i != (nb_attr - 1))
			o << "\nAll remaining attribute(s) have not been updated";
		o << std::ends;

		std::string s = o.str();
		e.errors[0].reason = Tango::string_dup(s.c_str());
		throw;
	}

//
// Re build the list of "alarmable" attribute
//

	dev_attr->get_alarm_list().clear();
	for (i = 0;i < nb_dev_attr;i++)
	{
		Tango::Attribute &attr = dev_attr->get_attr_by_ind(i);
		Tango::AttrWriteType w_type = attr.get_writable();
		if (attr.is_alarmed().any() == true)
		{
			if (w_type != Tango::WRITE)
				dev_attr->get_alarm_list().push_back(i);
		}
	}

//
// Return to caller
//

	cout4 << "Leaving Device_3Impl::set_attribute_config_3_local" << std::endl;
}

template <typename T>
inline void Device_3Impl::error_from_devfailed(T &back,DevFailed &e,const char *na)
{
	back.err_list = e.errors;
	back.quality = ATTR_INVALID;
	back.name = Tango::string_dup(na);
	clear_att_dim(back);
}

template <typename T>
inline void Device_3Impl::error_from_errorlist(T &back,DevErrorList &e,const char *na)
{
	back.err_list = e;
	back.quality = ATTR_INVALID;
	back.name = Tango::string_dup(na);
	clear_att_dim(back);
}

template <typename T>
inline void Device_3Impl::one_error(T &back,const char *reas,const char *ori,std::string &mess,Attribute &att)
{
	back.err_list.length(1);

	back.err_list[0].severity = Tango::ERR;
	back.err_list[0].reason = Tango::string_dup(reas);
	back.err_list[0].origin = Tango::string_dup(ori);
	back.err_list[0].desc = Tango::string_dup(mess.c_str());

	back.quality = Tango::ATTR_INVALID;
	back.name = Tango::string_dup(att.get_name().c_str());
	clear_att_dim(back);
}

template <typename T>
inline void Device_3Impl::one_error(T &back,const char *reas,const char *ori,std::string &mess,const char *na)
{
	back.err_list.length(1);

	back.err_list[0].severity = Tango::ERR;
	back.err_list[0].reason = Tango::string_dup(reas);
	back.err_list[0].origin = Tango::string_dup(ori);
	back.err_list[0].desc = Tango::string_dup(mess.c_str());

	back.quality = Tango::ATTR_INVALID;
	back.name = Tango::string_dup(na);
	clear_att_dim(back);
}

template <typename T,typename V>
inline void Device_3Impl::init_polled_out_data(T &back,V &att_val)
{
	back.quality = att_val.quality;
	back.time = att_val.time;
	back.r_dim = att_val.r_dim;
	back.w_dim = att_val.w_dim;
	back.name = Tango::string_dup(att_val.name);
}

template <typename T>
inline void Device_3Impl::init_out_data(T &back,Attribute &att,AttrWriteType &w_type)
{
	back.time = att.get_when();
	back.quality = att.get_quality();
	back.name = Tango::string_dup(att.get_name().c_str());
	back.r_dim.dim_x = att.get_x();
	back.r_dim.dim_y = att.get_y();
	if ((w_type == Tango::READ_WRITE) ||
		(w_type == Tango::READ_WITH_WRITE))
	{
		WAttribute &assoc_att = dev_attr->get_w_attr_by_ind(att.get_assoc_ind());
		back.w_dim.dim_x = assoc_att.get_w_dim_x();
		back.w_dim.dim_y = assoc_att.get_w_dim_y();
	}
	else
	{
		if ( w_type == Tango::WRITE)
		{
			// for write only attributes read and set value are the same!
			back.w_dim.dim_x = att.get_x();
			back.w_dim.dim_y = att.get_y();
		}
		else
		{
			// Tango::Read : read only attributes
			back.w_dim.dim_x = 0;
			back.w_dim.dim_y = 0;
		}
	}
}

template <typename T>
inline void Device_3Impl::init_out_data_quality(T &back,Attribute &att,AttrQuality qual)
{
	back.time = att.get_when();
	back.quality = qual;
	back.name = Tango::string_dup(att.get_name().c_str());
	back.r_dim.dim_x = att.get_x();
	back.r_dim.dim_y = att.get_y();
	back.r_dim.dim_x = 0;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

template <typename T>
inline void Device_3Impl::base_state2attr(T &back)
{

#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.name = Tango::string_dup("State");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

template <typename T>
inline void Device_3Impl::base_status2attr(T &back)
{

#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.name = Tango::string_dup("Status");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

} // End of Tango namespace

#endif /* DEVICE_3_TPP */
