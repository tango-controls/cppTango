//
// Created by ingvord on 11/2/16.
//

#include "event_system.hxx"
#include "helpers.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>

Tango::polling::EventSystem::EventSystem(Tango::EventSupplier *event_supplier_nd,
                                         Tango::EventSupplier *event_supplier_zmq) :
        event_supplier_nd_(event_supplier_nd), event_supplier_zmq_(event_supplier_zmq) {}


void Tango::polling::EventSystem::push_event(Tango::WorkItem & item) {
//
// Retrieve the event supplier(s) for this attribute
//

        for (size_t i = 0, size = item.name.size(); i < size; i++) {


            if ((event_supplier_nd_ != nullptr) || (event_supplier_zmq_ != nullptr)) {
                long idl_vers = item.dev->get_dev_idl_version();

                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &reinterpret_cast<AttributeValueList_5*>(item.values)->operator[](i);
                else if (idl_vers == 4)
                    ad.attr_val_4 = &reinterpret_cast<AttributeValueList_4*>(item.values)->operator[](i);
                else if (idl_vers == 3)
                    ad.attr_val_3 = &reinterpret_cast<AttributeValueList_3*>(item.values)->operator[](i);
                else
                    ad.attr_val = &reinterpret_cast<AttributeValueList*>(item.values)->operator[](i);


                struct timeval tv = duration_to_timeval(item.start_time);
                auto name = item.name[i];
                //TODO get_if
                DevFailed* error = item.errors.count(name) == 1 ? item.errors.at(name) : nullptr;
                push_event(item.dev, move(ad), error, move(name), &tv);
            }
        }
}

void Tango::polling::EventSystem::push_error_event(Tango::WorkItem &item, Tango::DevFailed &error) {
//
// Retrieve the event supplier(s) for this attribute
//

    for (size_t i = 0, size = item.name.size(); i < size; ++i) {
        Attribute &att = item.dev->get_device_attr()->get_attr_by_name(item.name[i].c_str());

        if ((event_supplier_nd_ != NULL && att.use_notifd_event()) ||
                (event_supplier_zmq_ != NULL && att.use_zmq_event())) {
            long idl_vers = item.dev->get_dev_idl_version();

            struct EventSupplier::SuppliedEventData ad{};
            memset(&ad, 0, sizeof(ad));

            AttributeValue		dummy_att{};
            AttributeValue_3	dummy_att3{};
            AttributeValue_4 	dummy_att4{};
            AttributeValue_5	dummy_att5{};

            if (idl_vers > 4)
                ad.attr_val_5 = &dummy_att5;
            else if (idl_vers == 4)
                ad.attr_val_4 = &dummy_att4;
            else if (idl_vers == 3)
                ad.attr_val_3 = &dummy_att3;
            else
                ad.attr_val = &dummy_att;

//
// Fire event
//

            push_event(item.dev, move(ad), &error, move(item.name[i]), nullptr);

        }
    }


}

void Tango::polling::EventSystem::push_event(Tango::DeviceImpl * device, Tango::EventSupplier::SuppliedEventData && data,
                                             Tango::DevFailed * error, std::string && name, struct timeval * tv) {
    Attribute &att = device->get_device_attr()->get_attr_by_name(name.c_str());

    SendEventType send_event;
    if (event_supplier_nd_ != NULL)
        send_event = event_supplier_nd_->detect_and_push_events(device, data, error, name, tv);
    if (event_supplier_zmq_ != NULL) {
        if (event_supplier_nd_ != NULL) {
            vector<string> f_names;
            vector<double> f_data;
            vector<string> f_names_lg;
            vector<long> f_data_lg;

            if (send_event.change)
                event_supplier_zmq_->push_event_loop(device, CHANGE_EVENT, f_names, f_data, f_names_lg,
                                                     f_data_lg, data, att, error);
            if (send_event.archive)
                event_supplier_zmq_->push_event_loop(device, ARCHIVE_EVENT, f_names, f_data, f_names_lg,
                                                     f_data_lg, data, att, error);
            if (send_event.periodic)
                event_supplier_zmq_->push_event_loop(device, PERIODIC_EVENT, f_names, f_data, f_names_lg,
                                                     f_data_lg, data, att, error);
        } else
            event_supplier_zmq_->detect_and_push_events(device, data, error, name,tv);
    }
}


