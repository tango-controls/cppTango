//====================================================================================================================
//
//  file : 				eventsupplier.cpp
//
//	description : 		C++ classes for implementing the event server and client singleton classes - EventSupplier
//						This class is used to send events from the server
//
//  author(s) : 		E.Taurel (taurel@esrf.fr)
//
//	original : 			29 June 2004
//
//  Copyright (C) :     2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//	$Revision$
//
//
//====================================================================================================================

#include <tango.h>
#include <eventsupplier.h>

#ifdef _TG_WINDOWS_
#include <float.h>
#endif // _TG_WINDOWS_

namespace Tango
{

omni_mutex        EventSupplier::event_mutex;

omni_mutex        EventSupplier::detect_mutex;

omni_mutex        EventSupplier::push_mutex;

omni_condition    EventSupplier::push_cond(&EventSupplier::push_mutex);

string        EventSupplier::fqdn_prefix;

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::EventSupplier()
//
// description :
//		EventSupplier class ctor
//
// argument :
//		in :
//			- tg : ptr to the Util class singleton
//
//--------------------------------------------------------------------------------------------------------------------


EventSupplier::EventSupplier(Util *tg)
    : one_subscription_cmd(false)
{
    if (fqdn_prefix.empty() == true)
    {
        fqdn_prefix = "tango://";
        if (Util::_UseDb == false || Util::_FileDb == true)
        {
            fqdn_prefix = fqdn_prefix + tg->get_host_name() + ':' + tg->get_svr_port_num() + '/';
        }
        else
        {
            Database *db = tg->get_database();
            fqdn_prefix = fqdn_prefix + db->get_db_host() + ':' + db->get_db_port() + '/';
        }
        transform(fqdn_prefix.begin(), fqdn_prefix.end(), fqdn_prefix.begin(), ::tolower);
    }
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::detect_and_push_events()
//
// description :
//		Method to detect if it is necessary to push an event
//
// argument :
//		in :
//			- device_impl : The device
//			- attr_value : The attribute value
//			- except : The exception thrown during the last attribute reading. NULL if no exception
//			- attr_name : The attribute name
//          - time_bef_attr : Exact date when the attribute has been read
//
//--------------------------------------------------------------------------------------------------------------------

SendEventType EventSupplier::detect_and_push_events(DeviceImpl *device_impl, struct SuppliedEventData &attr_value,
                                                    DevFailed *except, string &attr_name, struct timeval *time_bef_attr)
{
    string event, domain_name;
    time_t now, change3_subscription, periodic3_subscription, archive3_subscription;
    time_t change4_subscription, periodic4_subscription, archive4_subscription;
    time_t change5_subscription, periodic5_subscription, archive5_subscription;
    SendEventType ret;
    cout3 << "EventSupplier::detect_and_push_events(): called for attribute " << attr_name << endl;

    Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

    now = time(NULL);

    {
        omni_mutex_lock oml(event_mutex);

        change3_subscription = now - attr.event_change3_subscription;
        periodic3_subscription = now - attr.event_periodic3_subscription;
        archive3_subscription = now - attr.event_archive3_subscription;

        change4_subscription = now - attr.event_change4_subscription;
        periodic4_subscription = now - attr.event_periodic4_subscription;
        archive4_subscription = now - attr.event_archive4_subscription;

        change5_subscription = now - attr.event_change5_subscription;
        periodic5_subscription = now - attr.event_periodic5_subscription;
        archive5_subscription = now - attr.event_archive5_subscription;
    }

    cout3 << "EventSupplier::detect_and_push_events(): last subscription for change5 " << change5_subscription
          << " periodic5 " << periodic5_subscription << " archive5 " << archive5_subscription << endl;

//
// For change event
// First check if it is necessary to send the event in case clients are not there any more
//

    ret.change = false;
    vector<int> client_libs = attr.get_client_lib(CHANGE_EVENT);    // We want a copy

    vector<int>::iterator ite;
    for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
    {
        switch (*ite)
        {
            case 5:
                if (change5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(5, string(EventName[CHANGE_EVENT]));
                }
                break;

            case 4:
                if (change4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(4, string(EventName[CHANGE_EVENT]));
                }
                break;

            default:
                if (change3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(3, string(EventName[CHANGE_EVENT]));
                }
                break;
        }

    }

    if (client_libs.empty() == false)
    {
        if (detect_and_push_change_event(device_impl, attr_value, attr, attr_name, except) == true)
        {
            ret.change = true;
        }
    }

//
// For periodic event
//

    ret.periodic = false;
    client_libs.clear();
    client_libs = attr.get_client_lib(PERIODIC_EVENT);    // We want a copy

    for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
    {
        switch (*ite)
        {
            case 5:
                if (periodic5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(5, string(EventName[PERIODIC_EVENT]));
                }
                break;

            case 4:
                if (periodic4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(4, string(EventName[PERIODIC_EVENT]));
                }
                break;

            default:
                if (periodic3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(3, string(EventName[PERIODIC_EVENT]));
                }
                break;
        }

    }

    if (client_libs.empty() == false)
    {
        if (detect_and_push_periodic_event(device_impl, attr_value, attr, attr_name, except, time_bef_attr) == true)
        {
            ret.periodic = true;
        }
    }

//
// For archive event
//

    ret.archive = false;
    client_libs.clear();
    client_libs = attr.get_client_lib(ARCHIVE_EVENT);    // We want a copy

    for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
    {
        switch (*ite)
        {
            case 5:
                if (archive5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(5, string(EventName[ARCHIVE_EVENT]));
                }
                break;

            case 4:
                if (archive4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(4, string(EventName[ARCHIVE_EVENT]));
                }
                break;

            default:
                if (archive3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
                {
                    attr.remove_client_lib(3, string(EventName[ARCHIVE_EVENT]));
                }
                break;
        }

    }

    if (client_libs.empty() == false)
    {
        if (detect_and_push_archive_event(device_impl, attr_value, attr, attr_name, except, time_bef_attr) == true)
        {
            ret.archive = true;
        }
    }

    return ret;
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::detect_and_push_change_event()
//
// description :
//		Method to detect if there it is necessary to push a change event
//
// argument :
//		in :
//			- device_impl : The device
//			- attr_value : The attribute value
//			- attr : The attribute object
//			- attr_name : The attribute name
//			- except : The exception thrown during the last attribute reading. NULL if no exception
// 			- user_push : Flag set to true if it is a user push
//
//--------------------------------------------------------------------------------------------------------------------

bool EventSupplier::detect_and_push_change_event(DeviceImpl *device_impl, struct SuppliedEventData &attr_value,
                                                 Attribute &attr, string &attr_name, DevFailed *except, TANGO_UNUSED(bool user_push))
{
    string event, domain_name;
    double delta_change_rel = 0.0;
    double delta_change_abs = 0.0;
    bool is_change = false;
    bool force_change = false;
    bool quality_change = false;
    bool ret = false;

    cout3 << "EventSupplier::detect_and_push_change_event(): called for attribute " << attr_name << endl;

    Tango::AttrQuality the_quality;

    if (attr_value.attr_val_5 != NULL)
    {
        the_quality = attr_value.attr_val_5->quality;
    }
    else if (attr_value.attr_val_4 != NULL)
    {
        the_quality = attr_value.attr_val_4->quality;
    }
    else if (attr_value.attr_val_3 != NULL)
    {
        the_quality = attr_value.attr_val_3->quality;
    }
    else
    {
        the_quality = attr_value.attr_val->quality;
    }

//
// get the mutex to synchronize the sending of events
//

    omni_mutex_lock l(event_mutex);

//
// if no attribute of this name is registered with change then insert the current value
//

    if (attr.prev_change_event.inited == false)
    {
        if (except != NULL)
        {
            attr.prev_change_event.err = true;
            attr.prev_change_event.except = *except;
        }
        else
        {
            if (attr_value.attr_val_5 != NULL)
            {
                attr.prev_change_event.value_4 = attr_value.attr_val_5->value;
            }
            else if (attr_value.attr_val_4 != NULL)
            {
                attr.prev_change_event.value_4 = attr_value.attr_val_4->value;
            }
            else if (attr_value.attr_val_3 != NULL)
            {
                attr.prev_change_event.value = attr_value.attr_val_3->value;
            }
            else
            {
                attr.prev_change_event.value = attr_value.attr_val->value;
            }

            attr.prev_change_event.quality = the_quality;
            attr.prev_change_event.err = false;
        }
        attr.prev_change_event.inited = true;
        is_change = true;
    }
    else
    {

//
// Determine delta_change in percent compared with previous event sent
//

        is_change = detect_change(attr,
                                  attr_value,
                                  false,
                                  delta_change_rel,
                                  delta_change_abs,
                                  except,
                                  force_change,
                                  device_impl);
        cout3 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change "
              << delta_change_abs << " is change = " << is_change << endl;
    }

//
// Check whether the data quality has changed. Fire event on a quality change.
//

    if ((except == NULL) && (attr.prev_change_event.quality != the_quality))
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
        {
            attr.prev_change_event.err = true;
            attr.prev_change_event.except = *except;
        }
        else
        {
            if (attr_value.attr_val_5 != NULL)
            {
                attr.prev_change_event.value_4 = attr_value.attr_val_5->value;
            }
            else if (attr_value.attr_val_4 != NULL)
            {
                attr.prev_change_event.value_4 = attr_value.attr_val_4->value;
            }
            else if (attr_value.attr_val_3 != NULL)
            {
                attr.prev_change_event.value = attr_value.attr_val_3->value;
            }
            else
            {
                attr.prev_change_event.value = attr_value.attr_val->value;
            }
            attr.prev_change_event.quality = the_quality;
            attr.prev_change_event.err = false;
        }

//
// Prepare to push the event
//

        domain_name = device_impl->get_name() + "/" + attr_name;
        filterable_names.push_back("delta_change_rel");
        filterable_data.push_back(delta_change_rel);
        filterable_names.push_back("delta_change_abs");
        filterable_data.push_back(delta_change_abs);

        filterable_names.push_back("forced_event");
        if (force_change == true)
        {
            filterable_data.push_back((double) 1.0);
        }
        else
        {
            filterable_data.push_back((double) 0.0);
        }

        filterable_names.push_back("quality");
        if (quality_change == true)
        {
            filterable_data.push_back((double) 1.0);
        }
        else
        {
            filterable_data.push_back((double) 0.0);
        }

        vector<int> &client_libs = attr.get_client_lib(CHANGE_EVENT);
        vector<int>::iterator ite;
        string ev_name = EventName[CHANGE_EVENT];
        bool inc_ctr = true;

        for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
        {
            bool need_free = false;
            bool name_changed = false;

            struct SuppliedEventData sent_value;
            ::memset(&sent_value, 0, sizeof(sent_value));

            switch (*ite)
            {
                case 5:
                {
                    convert_att_event_to_5(attr_value, sent_value, need_free, attr);
                    ev_name = EVENT_COMPAT_IDL5 + ev_name;
                    name_changed = true;
                }
                    break;

                case 4:
                {
                    convert_att_event_to_4(attr_value, sent_value, need_free, attr);
                }
                    break;

                default:
                {
                    convert_att_event_to_3(attr_value, sent_value, need_free, attr);
                }
                    break;
            }

            push_event(device_impl,
                       ev_name,
                       filterable_names,
                       filterable_data,
                       filterable_names_lg,
                       filterable_data_lg,
                       sent_value,
                       attr_name,
                       except,
                       inc_ctr);

            inc_ctr = false;
            if (need_free == true)
            {
                if (sent_value.attr_val_5 != NULL)
                {
                    delete sent_value.attr_val_5;
                }
                else if (sent_value.attr_val_4 != NULL)
                {
                    delete sent_value.attr_val_4;
                }
                else if (sent_value.attr_val_3 != NULL)
                {
                    delete sent_value.attr_val_3;
                }
                else
                {
                    delete sent_value.attr_val;
                }
            }
            if (name_changed == true)
            {
                ev_name = EventName[CHANGE_EVENT];
            }
        }
        ret = true;

    }

    cout3 << "EventSupplier::detect_and_push_change_event(): leaving for attribute " << attr_name << endl;
    return ret;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::detect_and_push_archive_event()
//
// description :
//		Method to detect if there it is necessary to push an archive event
//
// argument :
//		in :
//			- device_impl : The device
//			- attr_value : The attribute value
//			- attr : The attribute object
//			- attr_name : The attribute name
//			- except : The exception thrown during the last attribute reading. NULL if no exception
//			- time_bef_attr : Date before the attribute was read
//			- user_push : Flag set to true if it's the user who fires the event
//
//------------------------------------------------------------------------------------------------------------------

bool EventSupplier::detect_and_push_archive_event(DeviceImpl *device_impl,
                                                  SuppliedEventData &attr_value,
                                                  Attribute &attr,
                                                  string &attr_name,
                                                  DevFailed *except,
                                                  struct timeval *time_bef_attr,
                                                  TANGO_UNUSED(bool user_push))
{
    string event, domain_name;
    double delta_change_rel = 0.0;
    double delta_change_abs = 0.0;
    bool is_change = false;
    bool force_change = false;
    bool period_change = false;
    bool quality_change = false;
    bool ret = false;

    cout3 << "EventSupplier::detect_and_push_archive_event(): called for attribute " << attr_name << endl;

    double now_ms, ms_since_last_periodic;
    Tango::AttrQuality the_quality;

    if (attr_value.attr_val_5 != NULL)
    {
        the_quality = attr_value.attr_val_5->quality;
    }
    else if (attr_value.attr_val_4 != NULL)
    {
        the_quality = attr_value.attr_val_4->quality;
    }
    else if (attr_value.attr_val_3 != NULL)
    {
        the_quality = attr_value.attr_val_3->quality;
    }
    else
    {
        the_quality = attr_value.attr_val->quality;
    }

    struct timeval now;
    if (time_bef_attr == NULL)
    {
#ifdef _TG_WINDOWS_
        struct _timeb now_win;

        _ftime(&now_win);
        now.tv_sec = (unsigned long)now_win.time;
        now.tv_usec = (long)now_win.millitm * 1000;
#else
        gettimeofday(&now, NULL);
#endif
        now.tv_sec = now.tv_sec - DELTA_T;
    }

//
// get the mutex to synchronize the sending of events
//

    omni_mutex_lock l(event_mutex);

//
// Do not get time now. This method is executed after the attribute has been read.
// For some device, reading one attribute could be long and even worse could have an unstable reading time.
// If we takes time now, it will also be unstable.
// Use the time taken in the polling thread before the attribute was read. This one is much more stable
//

    if (time_bef_attr != NULL)
    {
        now_ms = (double) time_bef_attr->tv_sec * 1000. + (double) time_bef_attr->tv_usec / 1000.;
    }
    else
    {
        now_ms = (double) now.tv_sec * 1000. + (double) now.tv_usec / 1000.;
    }
    ms_since_last_periodic = now_ms - attr.archive_last_periodic;

    int arch_period;
    TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
    mon1.get_monitor();
    arch_period = attr.archive_period;
    mon1.rel_monitor();

//
// Specify the precision interval for the archive period testing 2% are used for periods < 5000 ms and
// 100ms are used for periods > 5000 ms.
// If the attribute archive period is INT_MAX, this means that the user does not want the periodic part of the
// archive event
//

    if (arch_period != INT_MAX)
    {
        if (arch_period >= 5000)
        {
            arch_period = arch_period - DELTA_PERIODIC_LONG;
        }
        else
        {
#ifdef _TG_WINDOWS_
            double tmp = (double) arch_period * DELTA_PERIODIC;
            double int_part, eve_round;
            double frac = modf(tmp, &int_part);
            if (frac >= 0.5)
            {
                eve_round = ceil(tmp);
            }
            else
            {
                eve_round = floor(tmp);
            }
#else
            double eve_round = round((double) arch_period * DELTA_PERIODIC);
#endif
            arch_period = (int) eve_round;
        }

        cout3 << "EventSupplier::detect_and_push_archive_event(): ms_since_last_periodic = " << ms_since_last_periodic
              << ", arch_period = " << arch_period << ", attr.prev_archive_event.inited = "
              << attr.prev_archive_event.inited << endl;

        if ((ms_since_last_periodic > arch_period) && (attr.prev_archive_event.inited == true))
        {
            is_change = true;
            period_change = true;
        }
    }

//
// If no attribute of this name is registered with change then insert the current value
//


    if (attr.prev_archive_event.inited == false)
    {
        if (except != NULL)
        {
            attr.prev_archive_event.err = true;
            attr.prev_archive_event.except = *except;
        }
        else
        {
            if (attr_value.attr_val_5 != NULL)
            {
                attr.prev_archive_event.value_4 = attr_value.attr_val_5->value;
            }
            else if (attr_value.attr_val_4 != NULL)
            {
                attr.prev_archive_event.value_4 = attr_value.attr_val_4->value;
            }
            else if (attr_value.attr_val_3 != NULL)
            {
                attr.prev_archive_event.value = attr_value.attr_val_3->value;
            }
            else
            {
                attr.prev_archive_event.value = attr_value.attr_val->value;
            }

            attr.prev_archive_event.quality = the_quality;
            attr.prev_archive_event.err = false;
        }
        attr.archive_last_periodic = now_ms;
        attr.archive_last_event = now_ms;
        attr.prev_archive_event.inited = true;
        is_change = true;
    }
    else
    {

//
// determine delta_change in percent compared with previous event sent
//

        if (is_change == false)
        {
            is_change = detect_change(attr,
                                      attr_value,
                                      true,
                                      delta_change_rel,
                                      delta_change_abs,
                                      except,
                                      force_change,
                                      device_impl);
        }
    }

//
// check whether the data quality has changed. Fire event on a quality change.
//

    if (except == NULL &&
        attr.prev_archive_event.quality != the_quality)
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

        domain_name = device_impl->get_name() + "/" + attr_name;

        if (except != NULL)
        {
            attr.prev_archive_event.err = true;
            attr.prev_archive_event.except = *except;
        }
        else
        {
            if (attr_value.attr_val_5 != NULL)
            {
                attr.prev_archive_event.value_4 = attr_value.attr_val_5->value;
            }
            else if (attr_value.attr_val_4 != NULL)
            {
                attr.prev_archive_event.value_4 = attr_value.attr_val_4->value;
            }
            else if (attr_value.attr_val_3 != NULL)
            {
                attr.prev_archive_event.value = attr_value.attr_val_3->value;
            }
            else
            {
                attr.prev_archive_event.value = attr_value.attr_val->value;
            }
            attr.prev_archive_event.quality = the_quality;
            attr.prev_archive_event.err = false;
        }

//
// Prepare to push the event
//

        filterable_names_lg.push_back("counter");
        if (period_change == true)
        {
            attr.archive_periodic_counter++;
            attr.archive_last_periodic = now_ms;
            filterable_data_lg.push_back(attr.archive_periodic_counter);
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
        {
            filterable_data.push_back((double) 1.0);
        }
        else
        {
            filterable_data.push_back((double) 0.0);
        }

        filterable_names.push_back("quality");
        if (quality_change == true)
        {
            filterable_data.push_back((double) 1.0);
        }
        else
        {
            filterable_data.push_back((double) 0.0);
        }

        filterable_names.push_back("delta_event");
        filterable_data.push_back(now_ms - attr.archive_last_event);
        attr.archive_last_event = now_ms;

        vector<int> &client_libs = attr.get_client_lib(ARCHIVE_EVENT);
        vector<int>::iterator ite;
        string ev_name = EventName[ARCHIVE_EVENT];
        bool inc_ctr = true;

        for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
        {
            bool need_free = false;
            bool name_changed = false;

            struct SuppliedEventData sent_value;
            ::memset(&sent_value, 0, sizeof(sent_value));

            switch (*ite)
            {
                case 5:
                {
                    convert_att_event_to_5(attr_value, sent_value, need_free, attr);
                    ev_name = EVENT_COMPAT_IDL5 + ev_name;
                    name_changed = true;
                }
                    break;

                case 4:
                {
                    convert_att_event_to_4(attr_value, sent_value, need_free, attr);
                }
                    break;

                default:
                {
                    convert_att_event_to_3(attr_value, sent_value, need_free, attr);
                }
                    break;
            }

            push_event(device_impl,
                       ev_name,
                       filterable_names,
                       filterable_data,
                       filterable_names_lg,
                       filterable_data_lg,
                       sent_value,
                       attr_name,
                       except,
                       inc_ctr);

            inc_ctr = false;
            if (need_free == true)
            {
                if (sent_value.attr_val_5 != NULL)
                {
                    delete sent_value.attr_val_5;
                }
                else if (sent_value.attr_val_4 != NULL)
                {
                    delete sent_value.attr_val_4;
                }
                else if (sent_value.attr_val_3 != NULL)
                {
                    delete sent_value.attr_val_3;
                }
                else
                {
                    delete sent_value.attr_val;
                }
            }
            if (name_changed == true)
            {
                ev_name = EventName[ARCHIVE_EVENT];
            }
        }

        ret = true;
    }

    return ret;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::detect_and_push_periodic_event()
//
// description :
//		Method to detect if there it is necessary to push a periodic event
//
// argument :
//		in :
//			- device_impl : The device
//			- attr_value : The attribute value
//			- attr : The attribute object
//			- attr_name : The attribute name
//			- except : The exception thrown during the last attribute reading. NULL if no exception
//			- time_bef_attr : Date before the attribute was read
//
//------------------------------------------------------------------------------------------------------------------

bool EventSupplier::detect_and_push_periodic_event(DeviceImpl *device_impl,
                                                   struct SuppliedEventData &attr_value,
                                                   Attribute &attr,
                                                   string &attr_name,
                                                   DevFailed *except,
                                                   struct timeval *time_bef_attr)
{
    string event, domain_name;
    double now_ms, ms_since_last_periodic;
    bool ret = false;

    struct timeval now;
    if (time_bef_attr == NULL)
    {
#ifdef _TG_WINDOWS_
        struct _timeb now_win;

        _ftime(&now_win);
        now.tv_sec = (unsigned long)now_win.time;
        now.tv_usec = (long)now_win.millitm * 1000;
#else
        gettimeofday(&now, NULL);
#endif
        now.tv_sec = now.tv_sec - DELTA_T;
    }

//
// Do not get time now. This metthod is executed after the attribute has been read.
// For some device, reading one attribute could be long and even worse could have an
// unstabe reading time. If we takes time now, it will also be unstable.
// Use the time taken inthe polling thread befor the attribute was read. This one is much
// more stable
//

    if (time_bef_attr != NULL)
    {
        now_ms = (double) time_bef_attr->tv_sec * 1000. + (double) time_bef_attr->tv_usec / 1000.;
    }
    else
    {
        now_ms = (double) now.tv_sec * 1000. + (double) now.tv_usec / 1000.;
    }

//
// get the mutex to synchronize the sending of events
//

    omni_mutex_lock l(event_mutex);

//
// get the event period
//

    int eve_period;
    TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
    mon1.get_monitor();
    eve_period = attr.event_period;
    mon1.rel_monitor();

//
// Specify the precision interval for the event period testing 2% are used for periods < 5000 ms and
// 100ms are used for periods > 5000 ms.
//

    if (eve_period >= 5000)
    {
        eve_period = eve_period - DELTA_PERIODIC_LONG;
    }
    else
    {
#ifdef _TG_WINDOWS_
        double tmp = (double) eve_period * DELTA_PERIODIC;
        double int_part, eve_round;
        double frac = modf(tmp, &int_part);
        if (frac >= 0.5)
        {
            eve_round = ceil(tmp);
        }
        else
        {
            eve_round = floor(tmp);
        }
#else
        double eve_round = round((double) eve_period * DELTA_PERIODIC);
#endif
        eve_period = (int) eve_round;
    }

//
// calculate the time
//

    ms_since_last_periodic = now_ms - attr.last_periodic;
    cout3 << "EventSupplier::detect_and_push_is_periodic_event(): delta since last periodic " << ms_since_last_periodic
          << " event_period " << eve_period << " for " << device_impl->get_name() + "/" + attr_name << endl;

    if (ms_since_last_periodic > eve_period)
    {

//
// Prepare to push the event
//

        vector<string> filterable_names;
        vector<double> filterable_data;
        vector<string> filterable_names_lg;
        vector<long> filterable_data_lg;

        attr.periodic_counter++;
        attr.last_periodic = now_ms;
        filterable_names_lg.push_back("counter");
        filterable_data_lg.push_back(attr.periodic_counter);

        vector<int> &client_libs = attr.get_client_lib(PERIODIC_EVENT);
        vector<int>::iterator ite;
        string ev_name = EventName[PERIODIC_EVENT];
        bool inc_ctr = true;

        cout3 << "EventSupplier::detect_and_push_is_periodic_event(): detected periodic event for "
              << device_impl->get_name() + "/" + attr_name << endl;

        for (ite = client_libs.begin(); ite != client_libs.end(); ++ite)
        {
            bool need_free = false;
            bool name_changed = false;

            struct SuppliedEventData sent_value;
            ::memset(&sent_value, 0, sizeof(sent_value));

            switch (*ite)
            {
                case 5:
                {
                    convert_att_event_to_5(attr_value, sent_value, need_free, attr);
                    ev_name = EVENT_COMPAT_IDL5 + ev_name;
                    name_changed = true;
                }
                    break;

                case 4:
                {
                    convert_att_event_to_4(attr_value, sent_value, need_free, attr);
                }
                    break;

                default:
                {
                    convert_att_event_to_3(attr_value, sent_value, need_free, attr);
                }
                    break;
            }

            push_event(device_impl,
                       ev_name,
                       filterable_names,
                       filterable_data,
                       filterable_names_lg,
                       filterable_data_lg,
                       sent_value,
                       attr_name,
                       except,
                       inc_ctr);

            inc_ctr = false;
            if (need_free == true)
            {
                if (sent_value.attr_val_5 != NULL)
                {
                    delete sent_value.attr_val_5;
                }
                else if (sent_value.attr_val_4 != NULL)
                {
                    delete sent_value.attr_val_4;
                }
                else if (sent_value.attr_val_3 != NULL)
                {
                    delete sent_value.attr_val_3;
                }
                else
                {
                    delete sent_value.attr_val;
                }
            }
            if (name_changed == true)
            {
                ev_name = EventName[PERIODIC_EVENT];
            }
        }
        ret = true;
    }

    return ret;
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::detect_change()
//
// description :
//		Method to detect if there is a change according to the criterions and return a boolean set to true if a change
//		is detected
//
// argument :
//		in :
//			- attr : The attribute object
//			- attr_value : The current attribute value
//			- archive :
//			- delta_change_rel :
//			- delta_change_abs :
//			- except : The exception thrown during the last attribute reading. NULL if no exception
//			- force_change : A flag set to true if the change is due to a non mathematical reason
//				       (array size change, from exception to classic...)
//			- dev : Pointer to the device
//
//-------------------------------------------------------------------------------------------------------------------

bool EventSupplier::detect_change(Attribute &attr, struct SuppliedEventData &attr_value, bool archive,
                                  double &delta_change_rel, double &delta_change_abs, DevFailed *except,
                                  bool &force_change, DeviceImpl *dev)
{
    bool is_change = false;

    cout3 << "EventSupplier::detect_change(): called for attribute " << attr.get_name() << endl;

    Tango::AttrQuality the_new_quality;
    const CORBA::Any *the_new_any = NULL;

    if (attr_value.attr_val_5 != NULL)
    {
        the_new_quality = attr_value.attr_val_5->quality;
    }
    else if (attr_value.attr_val_4 != NULL)
    {
        the_new_quality = attr_value.attr_val_4->quality;
    }
    else if (attr_value.attr_val_3 != NULL)
    {
        the_new_quality = attr_value.attr_val_3->quality;
        the_new_any = &(attr_value.attr_val_3->value);
    }
    else
    {
        the_new_quality = attr_value.attr_val->quality;
        the_new_any = &(attr_value.attr_val->value);
    }

//
// Send event, if the read_attribute failed or if it is the first time that the read_attribute succeed after a failure.
// Same thing if the attribute quality factor changes to INVALID
//

    if (archive == true)
    {

//
// force an event only when the last reading was not returning an exception or not returning the same exception
//

        if (except != NULL)
        {
            if (attr.prev_archive_event.err == true)
            {
                if (Except::compare_exception(*except, attr.prev_archive_event.except) == true)
                {
                    force_change = false;
                    return false;
                }
            }

            force_change = true;
            return true;
        }

//
// force an archive event when the last reading was still returning an exception
//

        if ((except == NULL) && (attr.prev_archive_event.err == true))
        {
            force_change = true;
            return true;
        }

//
// check wether the quality is invalid. Force an event only if the last reading was valid
//

        if (the_new_quality == Tango::ATTR_INVALID)
        {
            if (attr.prev_archive_event.quality == Tango::ATTR_INVALID)
            {
                force_change = false;
                return false;
            }

            force_change = true;
            return true;
        }

//
// force an archive event when the last reding was still marked as invalid data
//

        if ((the_new_quality != Tango::ATTR_INVALID) && (attr.prev_archive_event.quality == Tango::ATTR_INVALID))
        {
            force_change = true;
            return true;
        }
    }
    else
    {

//
// force an event only when the last reading was not returning an exception or not returning the same exception
//

        if (except != NULL)
        {
            if (attr.prev_change_event.err == true)
            {
                if (Except::compare_exception(*except, attr.prev_change_event.except) == true)
                {
                    force_change = false;
                    return false;
                }
            }

            force_change = true;
            return true;
        }

//
// force an change event when the last reding was still returning an exception
//

        if ((except == NULL) && (attr.prev_change_event.err == true))
        {
            force_change = true;
            return true;
        }

//
// check wether the quality is invalid. Force an event only if the last reading was valid
//

        if (the_new_quality == Tango::ATTR_INVALID)
        {
            if (attr.prev_change_event.quality == Tango::ATTR_INVALID)
            {
                force_change = false;
                return false;
            }

            force_change = true;
            return true;
        }

//
// force an change event when the last reding was still marked as invalid data
//

        if ((the_new_quality != Tango::ATTR_INVALID) && (attr.prev_change_event.quality == Tango::ATTR_INVALID))
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
    bool inited;
    CORBA::TypeCode_var ty;
    delta_change_rel = delta_change_abs = 0;

    bool enable_check = false;

    TangoMonitor &mon1 = dev->get_att_conf_monitor();
    mon1.get_monitor();
    if (! archive)
    {
        rel_change[0] = attr.rel_change[0];
        rel_change[1] = attr.rel_change[1];
        abs_change[0] = attr.abs_change[0];
        abs_change[1] = attr.abs_change[1];
        inited = attr.prev_change_event.inited;
        if ((attr.prev_change_event.quality != Tango::ATTR_INVALID) && (the_new_quality != Tango::ATTR_INVALID))
        {
            enable_check = true;
        }
    }
    else
    {
        rel_change[0] = attr.archive_rel_change[0];
        rel_change[1] = attr.archive_rel_change[1];
        abs_change[0] = attr.archive_abs_change[0];
        abs_change[1] = attr.archive_abs_change[1];
        inited = attr.prev_archive_event.inited;
        if ((attr.prev_archive_event.quality != Tango::ATTR_INVALID) && (the_new_quality != Tango::ATTR_INVALID))
        {
            enable_check = true;
        }
    }
    mon1.rel_monitor();

    if (inited)
    {
        if (enable_check == true)
        {
            unsigned int curr_seq_nb, prev_seq_nb;
            unsigned int i;

            if (the_new_any != NULL)
            {
                ty = the_new_any->type();
            }

//
// First, analyse the DevEncoded data type
//

            if (((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_ENCODED)) ||
                ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_ENCODED)))
            {
                unsigned int curr_seq_str_nb, prev_seq_str_nb;
                const char *curr_encoded_format, *prev_encoded_format;
                const Tango::DevVarUCharArray *curr_data_ptr, *prev_data_ptr;

                const Tango::DevVarEncodedArray *un_seq;
                if (attr_value.attr_val_5 != NULL)
                {
                    un_seq = &(attr_value.attr_val_5->value.encoded_att_value());
                }
                else
                {
                    un_seq = &(attr_value.attr_val_4->value.encoded_att_value());
                }

                curr_seq_str_nb = strlen((*un_seq)[0].encoded_format.in());
                curr_seq_nb = (*un_seq)[0].encoded_data.length();
                curr_encoded_format = (*un_seq)[0].encoded_format.in();
                curr_data_ptr = &((*un_seq)[0].encoded_data);

                if (archive == true)
                {
                    DevVarEncodedArray &union_seq = attr.prev_archive_event.value_4.encoded_att_value();
                    prev_seq_nb = union_seq[0].encoded_data.length();
                    prev_seq_str_nb = strlen(union_seq[0].encoded_format.in());
                    prev_encoded_format = union_seq[0].encoded_format.in();
                    prev_data_ptr = &union_seq[0].encoded_data;
                }
                else
                {
                    DevVarEncodedArray &union_seq = attr.prev_change_event.value_4.encoded_att_value();
                    prev_seq_nb = union_seq[0].encoded_data.length();
                    prev_seq_str_nb = strlen(union_seq[0].encoded_format.in());
                    prev_encoded_format = union_seq[0].encoded_format.in();
                    prev_data_ptr = &union_seq[0].encoded_data;
                }
                if ((curr_seq_nb != prev_seq_nb) || (curr_seq_str_nb != prev_seq_str_nb))
                {
                    force_change = true;
                    return true;
                }

                if (strcmp(curr_encoded_format, prev_encoded_format) != 0)
                {
                    delta_change_rel = delta_change_abs = 100.;
                    is_change = true;
                    return (is_change);
                }

                if ((rel_change[0] != INT_MAX) || (rel_change[1] != INT_MAX) || (abs_change[0] != INT_MAX)
                    || (abs_change[1] != INT_MAX))
                {
                    for (i = 0; i < curr_seq_nb; i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_data_ptr)[i] != 0)
                            {
                                delta_change_rel =
                                    ((*curr_data_ptr)[i] - (*prev_data_ptr)[i]) * 100 / (*prev_data_ptr)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_data_ptr)[i] == (*prev_data_ptr)[i])
                                {
                                    delta_change_rel = 0;
                                }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (*curr_data_ptr)[i] - (*prev_data_ptr)[i];
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                }
            }

//
// Now, the DevState data type
//

            else
            {
                DevState curr_sta, prev_sta;
                bool dev_state_type = false;
                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == DEVICE_STATE))
                {
                    dev_state_type = true;
                    curr_sta = attr_value.attr_val_5->value.dev_state_att();
                    if (archive == true)
                    {
                        prev_sta = attr.prev_archive_event.value_4.dev_state_att();
                    }
                    else
                    {
                        prev_sta = attr.prev_change_event.value_4.dev_state_att();
                    }
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == DEVICE_STATE))
                {
                    dev_state_type = true;
                    curr_sta = attr_value.attr_val_4->value.dev_state_att();
                    if (archive == true)
                    {
                        prev_sta = attr.prev_archive_event.value_4.dev_state_att();
                    }
                    else
                    {
                        prev_sta = attr.prev_change_event.value_4.dev_state_att();
                    }
                }
                else if ((the_new_any != NULL) && (ty->kind() == CORBA::tk_enum))
                {
                    dev_state_type = true;
                    *the_new_any >>= curr_sta;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_sta;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_sta;
                    }

                }

                if (dev_state_type == true)
                {
                    if (curr_sta != prev_sta)
                    {
                        delta_change_rel = delta_change_abs = 100.;
                        is_change = true;
                    }
                    return is_change;
                }

                CORBA::TypeCode_var ty_alias;
                CORBA::TypeCode_var ty_seq;

                if (the_new_any != NULL)
                {
                    ty_alias = ty->content_type();
                    ty_seq = ty_alias->content_type();
                }

//
// Now, the long data type
//

                bool long_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_LONG))
                {
                    GET_SEQ(long_type, curr_seq_lo, long_att_value, prev_seq_lo, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_LONG))
                {
                    GET_SEQ(long_type, curr_seq_lo, long_att_value, prev_seq_lo, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_long))
                {
                    long_type = true;
                    *the_new_any >>= curr_seq_lo;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_lo;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_lo;
                    }
                }

                if (long_type == true)
                {
                    curr_seq_nb = curr_seq_lo->length();
                    prev_seq_nb = prev_seq_lo->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_lo->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_lo)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_lo)[i] - (*prev_seq_lo)[i]) * 100 / (*prev_seq_lo)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_lo)[i] == (*prev_seq_lo)[i])
                                { delta_change_rel = 0; }
                            }

                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (*curr_seq_lo)[i] - (*prev_seq_lo)[i];
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the long 64 bits data type
//

                bool long_long_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_LONG64))
                {
                    GET_SEQ(long_long_type, curr_seq_64, long64_att_value, prev_seq_64, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_LONG64))
                {
                    GET_SEQ(long_long_type, curr_seq_64, long64_att_value, prev_seq_64, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_longlong))
                {
                    long_long_type = true;
                    *the_new_any >>= curr_seq_64;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_64;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_64;
                    }
                }

                if (long_long_type == true)
                {
                    curr_seq_nb = curr_seq_64->length();
                    prev_seq_nb = prev_seq_64->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_64->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_64)[i] != 0)
                            {
                                delta_change_rel =
                                    (double) (((*curr_seq_64)[i] - (*prev_seq_64)[i]) * 100 / (*prev_seq_64)[i]);
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_64)[i] == (*prev_seq_64)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (double) ((*curr_seq_64)[i] - (*prev_seq_64)[i]);
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the short data type
//

                bool short_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_SHORT))
                {
                    GET_SEQ(short_type, curr_seq_sh, short_att_value, prev_seq_sh, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_SHORT))
                {
                    GET_SEQ(short_type, curr_seq_sh, short_att_value, prev_seq_sh, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_short))
                {
                    short_type = true;
                    *the_new_any >>= curr_seq_sh;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_sh;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_sh;
                    }
                }

                if (short_type == true)
                {
                    curr_seq_nb = curr_seq_sh->length();
                    prev_seq_nb = prev_seq_sh->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }

                    if (attr.data_type == DEV_ENUM)
                    {
                        for (i = 0; i < curr_seq_sh->length(); i++)
                        {
                            if ((*curr_seq_sh)[i] != (*prev_seq_sh)[i])
                            {
                                delta_change_rel = delta_change_abs = 100.;
                                is_change = true;
                            }
                            return is_change;
                        }
                    }
                    else
                    {
                        for (i = 0; i < curr_seq_sh->length(); i++)
                        {
                            if (rel_change[0] != INT_MAX)
                            {
                                if ((*prev_seq_sh)[i] != 0)
                                {
                                    delta_change_rel =
                                        ((*curr_seq_sh)[i] - (*prev_seq_sh)[i]) * 100 / (*prev_seq_sh)[i];
                                }
                                else
                                {
                                    delta_change_rel = 100;
                                    if ((*curr_seq_sh)[i] == (*prev_seq_sh)[i])
                                    { delta_change_rel = 0; }
                                }
                                if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                                {
                                    is_change = true;
                                    return (is_change);
                                }
                            }
                            if (abs_change[0] != INT_MAX)
                            {
                                delta_change_abs = (*curr_seq_sh)[i] - (*prev_seq_sh)[i];
                                if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                                {
                                    is_change = true;
                                    return (is_change);
                                }
                            }
                        }
                        return false;
                    }
                }

//
// Now, the double data type
//

                bool double_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_DOUBLE))
                {
                    GET_SEQ(double_type, curr_seq_db, double_att_value, prev_seq_db, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_DOUBLE))
                {
                    GET_SEQ(double_type, curr_seq_db, double_att_value, prev_seq_db, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_double))
                {
                    double_type = true;
                    *the_new_any >>= curr_seq_db;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_db;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_db;
                    }
                }

                if (double_type == true)
                {
                    curr_seq_nb = curr_seq_db->length();
                    prev_seq_nb = prev_seq_db->length();

                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_db->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if (Tango_isnan((*prev_seq_db)[i]) != 0 && Tango_isnan((*curr_seq_db)[i]) == 0)
                            {
                                is_change = true;
                                return (is_change);
                            }

                            if ((*prev_seq_db)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_db)[i] - (*prev_seq_db)[i]) * 100 / (*prev_seq_db)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_db)[i] == (*prev_seq_db)[i])
                                { delta_change_rel = 0; }
                            }

                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            if (Tango_isnan((*prev_seq_db)[i]) != 0 && Tango_isnan((*curr_seq_db)[i]) == 0)
                            {
                                is_change = true;
                                return (is_change);
                            }

                            delta_change_abs = (*curr_seq_db)[i] - (*prev_seq_db)[i];

                            // Correct for rounding errors !
                            double max_change = delta_change_abs + (abs_change[1] * 1e-10);
                            double min_change = delta_change_abs + (abs_change[0] * 1e-10);

                            //if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            if (min_change <= abs_change[0] || max_change >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the string data type
//

                bool string_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_STRING))
                {
                    GET_SEQ(string_type, curr_seq_str, string_att_value, prev_seq_str, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_STRING))
                {
                    GET_SEQ(string_type, curr_seq_str, string_att_value, prev_seq_str, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_string))
                {
                    string_type = true;
                    *the_new_any >>= curr_seq_str;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_str;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_str;
                    }
                }

                if (string_type == true)
                {
                    curr_seq_nb = curr_seq_str->length();
                    prev_seq_nb = prev_seq_str->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_str->length(); i++)
                    {
                        if (strcmp((*curr_seq_str)[i], (*prev_seq_str)[i]) != 0)
                        {
                            delta_change_rel = delta_change_abs = 100.;
                            is_change = true;
                            return (is_change);
                        }
                    }
                    return false;
                }

//
// Now, the float data type
//

                bool float_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_FLOAT))
                {
                    GET_SEQ(float_type, curr_seq_fl, float_att_value, prev_seq_fl, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_FLOAT))
                {
                    GET_SEQ(float_type, curr_seq_fl, float_att_value, prev_seq_fl, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_float))
                {
                    float_type = true;
                    *the_new_any >>= curr_seq_fl;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_fl;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_fl;
                    }
                }

                if (float_type == true)
                {
                    curr_seq_nb = curr_seq_fl->length();
                    prev_seq_nb = prev_seq_fl->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }

                    for (i = 0; i < curr_seq_fl->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if (Tango_isnan((*prev_seq_fl)[i]) != 0 && Tango_isnan((*curr_seq_fl)[i]) == 0)
                            {
                                is_change = true;
                                return (is_change);
                            }

                            if ((*prev_seq_fl)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_fl)[i] - (*prev_seq_fl)[i]) * 100 / (*prev_seq_fl)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_fl)[i] == (*prev_seq_fl)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            if (Tango_isnan((*prev_seq_fl)[i]) != 0 && Tango_isnan((*curr_seq_fl)[i]) == 0)
                            {
                                is_change = true;
                                return (is_change);
                            }

                            delta_change_abs = (*curr_seq_fl)[i] - (*prev_seq_fl)[i];

                            // Correct for rounding errors !
                            double max_change = delta_change_abs + (abs_change[1] * 1e-10);
                            double min_change = delta_change_abs + (abs_change[0] * 1e-10);

                            //if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            if (min_change <= abs_change[0] || max_change >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the unsigned short data type
//

                bool unsigned_short_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_USHORT))
                {
                    GET_SEQ(unsigned_short_type, curr_seq_ush, ushort_att_value, prev_seq_ush, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_USHORT))
                {
                    GET_SEQ(unsigned_short_type, curr_seq_ush, ushort_att_value, prev_seq_ush, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ushort))
                {
                    unsigned_short_type = true;
                    *the_new_any >>= curr_seq_ush;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_ush;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_ush;
                    }
                }

                if (unsigned_short_type == true)
                {
                    curr_seq_nb = curr_seq_ush->length();
                    prev_seq_nb = prev_seq_ush->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_ush->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_ush)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_ush)[i] - (*prev_seq_ush)[i]) * 100 / (*prev_seq_ush)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_ush)[i] == (*prev_seq_ush)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (*curr_seq_ush)[i] - (*prev_seq_ush)[i];
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the boolean data type
//

                bool boolean_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_BOOL))
                {
                    GET_SEQ(boolean_type, curr_seq_bo, bool_att_value, prev_seq_bo, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_BOOL))
                {
                    GET_SEQ(boolean_type, curr_seq_bo, bool_att_value, prev_seq_bo, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_boolean))
                {
                    boolean_type = true;
                    *the_new_any >>= curr_seq_bo;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_bo;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_bo;
                    }
                }

                if (boolean_type == true)
                {
                    curr_seq_nb = curr_seq_bo->length();
                    prev_seq_nb = prev_seq_bo->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_bo->length(); i++)
                    {
                        if ((*curr_seq_bo)[i] != (*prev_seq_bo)[i])
                        {
                            delta_change_rel = delta_change_abs = 100.;
                            is_change = true;
                            return (is_change);
                        }
                    }
                    return false;
                }

//
// Now, the char data type
//

                bool char_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_UCHAR))
                {
                    GET_SEQ(char_type, curr_seq_uch, uchar_att_value, prev_seq_uch, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_UCHAR))
                {
                    GET_SEQ(char_type, curr_seq_uch, uchar_att_value, prev_seq_uch, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_octet))
                {
                    char_type = true;
                    *the_new_any >>= curr_seq_uch;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_uch;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_uch;
                    }
                }

                if (char_type == true)
                {
                    curr_seq_nb = curr_seq_uch->length();
                    prev_seq_nb = prev_seq_uch->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_uch->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_uch)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_uch)[i] - (*prev_seq_uch)[i]) * 100 / (*prev_seq_uch)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_uch)[i] == (*prev_seq_uch)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (*curr_seq_uch)[i] - (*prev_seq_uch)[i];
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the unsigned long data type
//

                bool unsigned_long_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_ULONG))
                {
                    GET_SEQ(unsigned_long_type, curr_seq_ulo, ulong_att_value, prev_seq_ulo, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_ULONG))
                {
                    GET_SEQ(unsigned_long_type, curr_seq_ulo, ulong_att_value, prev_seq_ulo, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ulong))
                {
                    unsigned_long_type = true;
                    *the_new_any >>= curr_seq_ulo;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_ulo;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_ulo;
                    }
                }

                if (unsigned_long_type == true)
                {
                    curr_seq_nb = curr_seq_ulo->length();
                    prev_seq_nb = prev_seq_ulo->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_ulo->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_ulo)[i] != 0)
                            {
                                delta_change_rel = ((*curr_seq_ulo)[i] - (*prev_seq_ulo)[i]) * 100 / (*prev_seq_ulo)[i];
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_ulo)[i] == (*prev_seq_ulo)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (*curr_seq_ulo)[i] - (*prev_seq_ulo)[i];
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the unsigned 64 bits data type
//

                bool unsigned_64_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_ULONG64))
                {
                    GET_SEQ(unsigned_64_type, curr_seq_u64, ulong64_att_value, prev_seq_u64, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_ULONG64))
                {
                    GET_SEQ(unsigned_64_type, curr_seq_u64, ulong64_att_value, prev_seq_u64, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ulonglong))
                {
                    unsigned_64_type = true;
                    *the_new_any >>= curr_seq_u64;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_u64;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_u64;
                    }

                }

                if (unsigned_64_type == true)
                {
                    curr_seq_nb = curr_seq_u64->length();
                    prev_seq_nb = prev_seq_u64->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_u64->length(); i++)
                    {
                        if (rel_change[0] != INT_MAX)
                        {
                            if ((*prev_seq_u64)[i] != 0)
                            {
                                delta_change_rel =
                                    (double) (((*curr_seq_u64)[i] - (*prev_seq_u64)[i]) * 100 / (*prev_seq_u64)[i]);
                            }
                            else
                            {
                                delta_change_rel = 100;
                                if ((*curr_seq_u64)[i] == (*prev_seq_u64)[i])
                                { delta_change_rel = 0; }
                            }
                            if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                        if (abs_change[0] != INT_MAX)
                        {
                            delta_change_abs = (double) ((*curr_seq_u64)[i] - (*prev_seq_u64)[i]);
                            if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
                            {
                                is_change = true;
                                return (is_change);
                            }
                        }
                    }
                    return false;
                }

//
// Now, the state data type
//

                bool state_type = false;

                if ((attr_value.attr_val_5 != NULL) && (attr_value.attr_val_5->value._d() == ATT_STATE))
                {
                    GET_SEQ(state_type, curr_seq_state, state_att_value, prev_seq_state, attr_value.attr_val_5);
                }
                else if ((attr_value.attr_val_4 != NULL) && (attr_value.attr_val_4->value._d() == ATT_STATE))
                {
                    GET_SEQ(state_type, curr_seq_state, state_att_value, prev_seq_state, attr_value.attr_val_4);
                }
                else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_enum))
                {
                    state_type = true;
                    *the_new_any >>= curr_seq_state;
                    if (archive == true)
                    {
                        attr.prev_archive_event.value >>= prev_seq_state;
                    }
                    else
                    {
                        attr.prev_change_event.value >>= prev_seq_state;
                    }
                }

                if (state_type == true)
                {
                    curr_seq_nb = curr_seq_state->length();
                    prev_seq_nb = prev_seq_state->length();
                    if (curr_seq_nb != prev_seq_nb)
                    {
                        force_change = true;
                        return true;
                    }
                    for (i = 0; i < curr_seq_state->length(); i++)
                    {
                        if ((*curr_seq_state)[i] != (*prev_seq_state)[i])
                        {
                            delta_change_rel = delta_change_abs = 100.;
                            is_change = true;
                            return (is_change);
                        }
                    }
                }

            }
        }
    }

    cout3 << "EventSupplier::detect_change(): leaving for attribute " << attr.get_name() << endl;
    return (is_change);
}


//+--------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::push_att_data_ready_event()
//
// description :
//		Push a data ready event
//
// argument :
//		in :
//			- device_impl : Pointer to device
//			- attr_name : Attribute name
//			- data_type : Attribute data type
//			- ctr : Counter sent in event
//
//-------------------------------------------------------------------------------------------------------------

void
EventSupplier::push_att_data_ready_event(DeviceImpl *device_impl, const string &attr_name, long data_type, DevLong ctr)
{
    cout3 << "EventSupplier::push_att_data_ready_event(): called for attribute " << attr_name << endl;

    vector<string> filterable_names;
    vector<double> filterable_data;
    vector<string> filterable_names_lg;
    vector<long> filterable_data_lg;

    string ev_type(DATA_READY_TYPE_EVENT);

    AttDataReady dat_ready;
    dat_ready.name = attr_name.c_str();
    dat_ready.data_type = (int) data_type;
    dat_ready.ctr = ctr;

    SuppliedEventData ad;
    ::memset(&ad, 0, sizeof(ad));
    ad.attr_dat_ready = &dat_ready;

    push_event(device_impl,
               ev_type,
               filterable_names,
               filterable_data,
               filterable_names_lg,
               filterable_data_lg,
               ad,
               const_cast<string &>(attr_name),
               NULL, true);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::push_att_conf_event()
//
// description :
//		Method to push attribute configration event
//
// argument :
//		in :
//			- device_impl : The device
//			- attr_conf : The attribute configuration
//			- except : The exception thrown during the last attribute reading. NULL if no exception
//          - attr_name : The attribute name
//
//------------------------------------------------------------------------------------------------------------------

void EventSupplier::push_att_conf_events(DeviceImpl *device_impl,
                                         SuppliedEventData &attr_conf,
                                         DevFailed *except,
                                         string &attr_name)
{
    string event, domain_name;
    time_t now, att_conf_subscription, attr_sub;

    cout3 << "EventSupplier::push_att_conf_events(): called for attribute " << attr_name << endl;

    Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

//
// Called for AttributeConfig_3 or AttributeConfig_5 ?
//

    bool conf5 = false;
    int vers = 4;

    if (attr_conf.attr_conf_5 != NULL)
    {
        conf5 = true;
        vers = 5;
    }

//
// Return if there is no client or if the last client subscription is more than 10 mins ago
//

    {
        omni_mutex_lock oml(event_mutex);

        if (conf5 == true)
        {
            attr_sub = attr.event_attr_conf5_subscription;
        }
        else
        {
            attr_sub = attr.event_attr_conf_subscription;
        }
    }

    if (attr_sub == 0)
    {
        return;
    }

    now = time(NULL);
    att_conf_subscription = now - attr_sub;

    cout3 << "EventSupplier::push_att_conf_events(): delta since last subscription " << att_conf_subscription << endl;

    if (att_conf_subscription > EVENT_RESUBSCRIBE_PERIOD)
    {
        attr.remove_client_lib(vers, string(EventName[ATTR_CONF_EVENT]));
        return;
    }

//
// Push event
//

    vector<string> filterable_names;
    vector<double> filterable_data;
    vector<string> filterable_names_lg;
    vector<long> filterable_data_lg;

    string ev_type = CONF_TYPE_EVENT;
    if (conf5 == true)
    {
        ev_type = EVENT_COMPAT_IDL5 + ev_type;
    }

    push_event(device_impl,
               ev_type,
               filterable_names,
               filterable_data,
               filterable_names_lg,
               filterable_data_lg,
               attr_conf,
               attr_name,
               except,
               true);
}

//+--------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::push_dev_intr_change_event()
//
// description :
//		Push a device interface change event
//
// argument :
//		in :
//			- device_impl : Pointer to device
//			- dev_started : Device started flag
//			- cmds_list: Device commands list
//			- atts_list: Device attribute list
//
//-------------------------------------------------------------------------------------------------------------

void EventSupplier::push_dev_intr_change_event(DeviceImpl *device_impl,
                                               bool dev_start,
                                               DevCmdInfoList_2 *cmds_list,
                                               AttributeConfigList_5 *atts_list)
{
    cout3 << "EventSupplier::push_dev_intr_change_event(): called for device " << device_impl->get_name() << endl;

    vector<string> filterable_names;
    vector<double> filterable_data;
    vector<string> filterable_names_lg;
    vector<long> filterable_data_lg;

    string ev_type(EventName[INTERFACE_CHANGE_EVENT]);
    time_t now, dev_intr_subscription;

//
// If no client, do not send event
//

    now = time(NULL);
    dev_intr_subscription = now - device_impl->get_event_intr_change_subscription();

    cout3 << "EventSupplier::push_dev_intr_event(): delta since last subscription " << dev_intr_subscription << endl;

    if (dev_intr_subscription > EVENT_RESUBSCRIBE_PERIOD)
    {
        delete cmds_list;
        delete atts_list;

        return;
    }

    DevIntrChange dev_intr;

    dev_intr.dev_started = dev_start;
    dev_intr.cmds = *cmds_list;
    dev_intr.atts = *atts_list;

    SuppliedEventData ad;
    ::memset(&ad, 0, sizeof(ad));
    ad.dev_intr_change = &dev_intr;

    string att_name("dummy");
    push_event(device_impl,
               ev_type,
               filterable_names,
               filterable_data,
               filterable_names_lg,
               filterable_data_lg,
               ad,
               att_name,
               NULL,
               true);

//
// Free memory allocated for the two pointers we receive
//

    delete cmds_list;
    delete atts_list;
}

//+--------------------------------------------------------------------------------------------------------------
//
// method :
//		EventSupplier::any_dev_intr_client
//
// description :
//		Check if there is at least a device interface change client
//
// argument :
//		in :
//			- device_impl : Pointer to device
//
//	return :
//		True if there is at least one client listening for this event
//
//-------------------------------------------------------------------------------------------------------------

bool EventSupplier::any_dev_intr_client(DeviceImpl *device_impl)
{
    bool ret = false;

    time_t now = time(NULL);
    time_t dev_intr_subscription = now - device_impl->get_event_intr_change_subscription();

    if (dev_intr_subscription < EVENT_RESUBSCRIBE_PERIOD)
    {
        ret = true;
    }

    return ret;
}

void EventSupplier::convert_att_event_to_5(struct EventSupplier::SuppliedEventData &attr_value,
                                           struct EventSupplier::SuppliedEventData &sent_value,
                                           bool &need_free, Attribute &attr)
{
    if (attr_value.attr_val_3 != Tango_nullptr)
    {
        AttributeValue_5 *tmp_attr_val_5 = new AttributeValue_5();
        attr.AttributeValue_3_2_AttributeValue_5(attr_value.attr_val_3, tmp_attr_val_5);
        sent_value.attr_val_5 = tmp_attr_val_5;
        need_free = true;
    }
    else if (attr_value.attr_val_4 != Tango_nullptr)
    {
        AttributeValue_5 *tmp_attr_val_5 = new AttributeValue_5();
        attr.AttributeValue_4_2_AttributeValue_5(attr_value.attr_val_4, tmp_attr_val_5);
        sent_value.attr_val_5 = tmp_attr_val_5;
        need_free = true;
    }
    else
    {
        sent_value.attr_val_5 = attr_value.attr_val_5;
    }
}

void EventSupplier::convert_att_event_to_4(struct EventSupplier::SuppliedEventData &attr_value,
                                           struct EventSupplier::SuppliedEventData &sent_value,
                                           bool &need_free, Attribute &attr)
{
    if (attr_value.attr_val_3 != Tango_nullptr)
    {
        AttributeValue_4 *tmp_attr_val_4 = new AttributeValue_4();
        attr.AttributeValue_3_2_AttributeValue_4(attr_value.attr_val_3, tmp_attr_val_4);
        sent_value.attr_val_4 = tmp_attr_val_4;
        need_free = true;
    }
    else if (attr_value.attr_val_5 != Tango_nullptr)
    {
        AttributeValue_4 *tmp_attr_val_4 = new AttributeValue_4();
        attr.AttributeValue_5_2_AttributeValue_4(attr_value.attr_val_5, tmp_attr_val_4);
        sent_value.attr_val_4 = tmp_attr_val_4;
        need_free = true;
    }
    else
    {
        sent_value.attr_val_4 = attr_value.attr_val_4;
    }
}

void EventSupplier::convert_att_event_to_3(struct EventSupplier::SuppliedEventData &attr_value,
                                           struct EventSupplier::SuppliedEventData &sent_value,
                                           bool &need_free, Attribute &attr)
{
    if (attr_value.attr_val_4 != Tango_nullptr)
    {
        AttributeValue_3 *tmp_attr_val_3 = new AttributeValue_3();
        attr.AttributeValue_4_2_AttributeValue_3(attr_value.attr_val_4, tmp_attr_val_3);
        sent_value.attr_val_3 = tmp_attr_val_3;
        need_free = true;
    }
    else if (attr_value.attr_val_5 != Tango_nullptr)
    {
        AttributeValue_3 *tmp_attr_val_3 = new AttributeValue_3();
        attr.AttributeValue_5_2_AttributeValue_3(attr_value.attr_val_5, tmp_attr_val_3);
        sent_value.attr_val_3 = tmp_attr_val_3;
        need_free = true;
    }
    else
    {
        sent_value.attr_val_3 = attr_value.attr_val_3;
    }
}

} /* End of Tango namespace */
