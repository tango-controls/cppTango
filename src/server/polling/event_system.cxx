//
// Created by ingvord on 11/2/16.
//

#include "event_system.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>

Tango::polling::EventSystem::EventSystem(Tango::EventSupplier *event_supplier_nd,
                                         Tango::EventSupplier *event_supplier_zmq) :
        event_supplier_nd_(event_supplier_nd), event_supplier_zmq_(event_supplier_zmq) {}


void Tango::polling::EventSystem::push_event(Tango::WorkItem & item) {
//
// Retrieve the event supplier(s) for this attribute
//

        size_t nb_obj = item.name.size();
        for (size_t ctr = 0; ctr < nb_obj; ctr++) {
            Attribute &att = item.dev->get_device_attr()->get_attr_by_name(item.name[ctr].c_str());

            if ((event_supplier_nd_ != nullptr) || (event_supplier_zmq_ != nullptr)) {
                long idl_vers = item.dev->get_dev_idl_version();

                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &reinterpret_cast<AttributeValueList_5*>(item.values)->operator[](ctr);
                else if (idl_vers == 4)
                    ad.attr_val_4 = &reinterpret_cast<AttributeValueList_4*>(item.values)->operator[](ctr);
                else if (idl_vers == 3)
                    ad.attr_val_3 = &reinterpret_cast<AttributeValueList_3*>(item.values)->operator[](ctr);
                else
                    ad.attr_val = &reinterpret_cast<AttributeValueList*>(item.values)->operator[](ctr);

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                SendEventType send_event;

                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(item.dev, ad, tmp_except,
                                                                           item.name[ctr], &before_cmd);
                if (event_supplier_zmq != NULL) {
                    if (event_supplier_nd != NULL) {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(item.dev, CHANGE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(item.dev, PERIODIC_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(item.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                    } else
                        event_supplier_zmq->detect_and_push_events(item.dev, ad, tmp_except, item.name[ctr],
                                                                   &before_cmd);
                }
            }
        }
}

void Tango::polling::EventSystem::push_error_event(Tango::WorkItem &, Tango::DevFailed &) {
//
// Retrieve the event supplier(s) for this attribute
//

    size_t nb_obj = item.name.size();
    for (size_t ctr = 0; ctr < nb_obj; ctr++) {
        Attribute &att = item.dev->get_device_attr()->get_attr_by_name(item.name[ctr].c_str());

        if (att.use_notifd_event() == true && event_supplier_nd == NULL)
            event_supplier_nd = Util::instance()->get_notifd_event_supplier();
        if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
            event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

        if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL)) {
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

            SendEventType send_event;
            if (event_supplier_nd != NULL)
                send_event = event_supplier_nd->detect_and_push_events(item.dev, ad, &except, item.name[ctr],
                                                                       (struct timeval *) NULL);
            if (event_supplier_zmq != NULL) {
                if (event_supplier_nd != NULL) {
                    vector<string> f_names;
                    vector<double> f_data;
                    vector<string> f_names_lg;
                    vector<long> f_data_lg;

                    if (send_event.change == true)
                        event_supplier_zmq->push_event_loop(item.dev, CHANGE_EVENT, f_names, f_data, f_names_lg,
                                                            f_data_lg, ad, att, &except);
                    if (send_event.archive == true)
                        event_supplier_zmq->push_event_loop(item.dev, ARCHIVE_EVENT, f_names, f_data, f_names_lg,
                                                            f_data_lg, ad, att, &except);
                    if (send_event.periodic == true)
                        event_supplier_zmq->push_event_loop(item.dev, PERIODIC_EVENT, f_names, f_data, f_names_lg,
                                                            f_data_lg, ad, att, &except);
                } else
                    event_supplier_zmq->detect_and_push_events(item.dev, ad, &except, item.name[ctr],
                                                               (struct timeval *) NULL);
            }
        }
    }
}


