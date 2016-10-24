//
// Created by ingvord on 10/24/16.
//

#include <tango.h>
#include <tango/client/eventconsumer.h>

omni_mutex Tango::EventConsumer::ev_consumer_inst_mutex;

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::add_new_callback()
//
// description :
//		Add a new callback to an already existing event entry in the callback map
//
// argument :
//		in :
//			- iter : Iterator in the callback map
//			- callback : Pointer to the Callback object
//			- ev_queue : Pointer to the event queue
//			- event_id : The event identifier
//
//--------------------------------------------------------------------------------------------------------------------

int Tango::EventConsumer::add_new_callback(EvCbIte &iter, CallBack *callback, EventQueue *ev_queue, int event_id) {
    EventSubscribeStruct ess;
    int ret_event_id = event_id;

    if (ret_event_id <= 0) {
        subscribe_event_id++;
        ret_event_id = subscribe_event_id;
    }

    ess.id = ret_event_id;
    ess.callback = callback;
    ess.ev_queue = ev_queue;

    iter->second.callback_list.push_back(ess);

    return ret_event_id;
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_fire_sync_event()
//
// description :
//		Get event data and fire a synchronous event
//
// argument :
//		in :
//			- device : The device pointer
//			- callback : The callback pointer
//			- ev_queue : The event queue
//			- event : The event type
//			- event_name : The event name
//			- obj_name : The attribute/pipe name
//			- cb :
//			- callback_key :
//
//-------------------------------------------------------------------------------------------------------------------

void Tango::EventConsumer::get_fire_sync_event(DeviceProxy *device, CallBack *callback, EventQueue *ev_queue,
                                               EventType event,
                                               string &event_name, const string &obj_name, EventCallBackStruct &cb,
                                               string &callback_key) {

//
// A small 10 mS sleep here! This is required in case there is a push_event in the read_attribute (or pipe)
// method on the device side. This sleep gives time to ZMQ to send its subscription message
//

#ifndef _TG_WINDOWS_
    struct timespec to_wait, inter;
    to_wait.tv_sec = 0;
    to_wait.tv_nsec = 10000000;

    nanosleep(&to_wait, &inter);
#else
    Sleep(25);
#endif

    if ((event == CHANGE_EVENT) ||
        (event == QUALITY_EVENT) ||
        (event == ARCHIVE_EVENT) ||
        (event == USER_EVENT) ||
        (event == PERIODIC_EVENT)) {
        DevErrorList err;
        err.length(0);

        string domain_name;
        string::size_type pos;

        if ((pos = device_name.find(MODIFIER_DBASE_NO)) != string::npos) {
            domain_name = device_name;
            string tmp = '/' + obj_name_lower;
            domain_name.insert(pos, tmp);
        } else
            domain_name = device_name + '/' + obj_name_lower;

        AttributeValue_5 *av_5 = Tango_nullptr;
        DeviceAttribute *da = Tango_nullptr;
        FwdEventData *event_data;

        try {
            if (cb.fwd_att == true) {
                device->read_attribute(obj_name.c_str(), av_5);
                if (av_5->err_list.length() != 0) {
                    err = av_5->err_list;
                    err.length(err.length() - 1);
                }
            } else {
                da = new DeviceAttribute();
                *da = device->read_attribute(obj_name.c_str());
                if (da->has_failed() == true) {
                    err = da->get_err_stack();
                    err.length(err.length() - 1);
                }
            }
        }
        catch (DevFailed &e) {
            err = e.errors;
        }

        string local_event_name = event_name;
        pos = local_event_name.find(EVENT_COMPAT);
        if (pos != string::npos)
            local_event_name.erase(0, EVENT_COMPAT_IDL5_SIZE);

        if (cb.fwd_att == true) {
            da = new DeviceAttribute();
            event_data = new FwdEventData(device, domain_name, local_event_name, da, err);
            event_data->set_av_5(av_5);
        } else {
            event_data = new FwdEventData(device, domain_name, local_event_name, da, err);
        }

        AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

        if (callback != NULL) {
            try {
                callback->push_event(event_data);
            }
            catch (...) {
                cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
            }

            delete event_data;
            if (cb.fwd_att == true)
                delete[] av_5;
        }

//
// No calback method, the event has to be inserted into the event queue
//

        else {
            ev_queue->insert_event(event_data);
        }
    } else if (event == ATTR_CONF_EVENT) {
        DevErrorList err;
        err.length(0);
        string domain_name = device_name + "/" + obj_name_lower;
        AttributeInfoEx *aie = NULL;

        string local_event_name = event_name;
        string::size_type pos = local_event_name.find(EVENT_COMPAT);
        if (pos != string::npos)
            local_event_name.erase(0, EVENT_COMPAT_IDL5_SIZE);

        try {
            aie = new AttributeInfoEx();
            *aie = device->get_attribute_config(const_cast<string &>(obj_name));
        }
        catch (DevFailed &e) {
            err = e.errors;
        }

        FwdAttrConfEventData *event_data = new FwdAttrConfEventData(device,
                                                                    domain_name,
                                                                    local_event_name,
                                                                    aie,
                                                                    err);
        AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

        if (callback != NULL) {
            try {
                callback->push_event(event_data);
            }
            catch (...) {
                cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
            }
            delete event_data;
        }

//
// No calback method, the event has to be inserted into the event queue
//

        else {
            ev_queue->insert_event(event_data);
        }
    } else if (event == INTERFACE_CHANGE_EVENT) {
        DevErrorList err;
        err.length(0);
        CommandInfoList *c_list = Tango_nullptr;
        AttributeInfoListEx *a_list = Tango_nullptr;
        string ev_name(EventName[INTERFACE_CHANGE_EVENT]);

        try {
            c_list = device->command_list_query();
            a_list = device->attribute_list_query_ex();
        }
        catch (DevFailed &e) {
            delete c_list;
            c_list = Tango_nullptr;
            delete a_list;
            a_list = Tango_nullptr;

            err = e.errors;
        }

        DevIntrChangeEventData *event_data = new DevIntrChangeEventData(device,
                                                                        ev_name, device_name,
                                                                        c_list, a_list, true,
                                                                        err);

        AutoTangoMonitor _mon(cb.callback_monitor);

//
// if a callback method was specified, call it!
//

        if (callback != NULL) {
            try {
                callback->push_event(event_data);
            }
            catch (...) {
                cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
            }

            delete event_data;
            delete c_list;
            delete a_list;
        }

//
// No calback method, the event has to be instered into the event queue
//

        else {
            ev_queue->insert_event(event_data);
        }
    } else if (event == PIPE_EVENT) {
        DevErrorList err;
        err.length(0);

        string domain_name;
        string::size_type pos;

        if ((pos = device_name.find(MODIFIER_DBASE_NO)) != string::npos) {
            domain_name = device_name;
            string tmp = '/' + obj_name_lower;
            domain_name.insert(pos, tmp);
        } else
            domain_name = device_name + '/' + obj_name_lower;

        DevicePipe *da = Tango_nullptr;
        PipeEventData *event_data;

        try {
            da = new DevicePipe();
            *da = device->read_pipe(obj_name);
        }
        catch (DevFailed &e) {
            err = e.errors;
        }

        event_data = new PipeEventData(device, domain_name, event_name, da, err);

        AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

        if (callback != NULL) {
            try {
                callback->push_event(event_data);
            }
            catch (...) {
                cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
            }

            delete event_data;
        }

//
// No calback method, the event has to be inserted into the event queue
//

        else {
            ev_queue->insert_event(event_data);
        }
    }
}
