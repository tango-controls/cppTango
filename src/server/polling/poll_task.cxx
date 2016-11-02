//
// Created by ingvord on 11/1/16.
//

#include "poll_task.hxx"
#include "helpers.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>

Tango::polling::PollTask::PollTask(Tango::WorkItem &work, PollThread &engine) : work_(work), engine_(engine) {}

std::future<void> Tango::polling::PollTask::operator()() {
    auto future = std::async(std::launch::async, &Tango::polling::PollTask::execute, this);
    return future;
}

void Tango::polling::PollTask::execute() {
    if (work_.type == Tango::POLL_CMD)
        poll_cmd();
    else
        poll_attr();
}

void Tango::polling::PollTask::poll_cmd() {
    cout5 << "----------> Time = "
          << chrono::system_clock::now().time_since_epoch().count()
          << " Dev name = " << work_.dev->get_name()
          << ", Cmd name = " << work_.name[0] << endl;

    CORBA::Any *argout = nullptr;
    Tango::DevFailed *save_except = nullptr;
    bool cmd_failed{false};


    auto start = chrono::high_resolution_clock::now();

    //
    // Execute the command
    //
    try {
        argout = work_.dev->command_inout(work_.name[0].c_str(), CORBA::Any{});
    }
    catch (const Tango::DevFailed &e) {
        cmd_failed = true;
        save_except = new Tango::DevFailed(e);
    }

    auto stop = chrono::high_resolution_clock::now();

    work_.needed_time = stop - start;

//
// Insert result in polling buffer and simply forget this command if it is not possible to insert the result in
// polling buffer
//

    try {
        struct timeval start_as_tv = to_timeval(start);
        struct timeval delta_tv = to_timeval(work_.needed_time);
        work_.dev->get_poll_monitor().get_monitor();
        auto ite = work_.dev->get_polled_obj_by_type_name(work_.type, work_.name[0]);
        if (cmd_failed)
            (*ite)->insert_except(save_except, start_as_tv, delta_tv);
        else
            (*ite)->insert_data(argout, start_as_tv, delta_tv);
        work_.dev->get_poll_monitor().rel_monitor();
    }
    catch (Tango::DevFailed &) {
        //TODO if not thrown who will release? CORBA?
        if (cmd_failed)
            delete save_except;
        else
            delete argout;

        work_.dev->get_poll_monitor().rel_monitor();
    }
}


template<typename T>
void Tango::polling::PollTask::steal_data(T &attr_value, T &new_attr_value) {
    switch (attr_value.value._d()) {
        case ATT_BOOL: {
            DevVarBooleanArray &union_seq = attr_value.value.bool_att_value();
            DevVarBooleanArray tmp_seq(1);
            new_attr_value.value.bool_att_value(tmp_seq);
            DevVarBooleanArray &new_union_seq = new_attr_value.value.bool_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_SHORT: {
            DevVarShortArray &union_seq = attr_value.value.short_att_value();
            DevVarShortArray tmp_seq(1);
            new_attr_value.value.short_att_value(tmp_seq);
            DevVarShortArray &new_union_seq = new_attr_value.value.short_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_LONG: {
            DevVarLongArray &union_seq = attr_value.value.long_att_value();
            DevVarLongArray tmp_seq(1);
            new_attr_value.value.long_att_value(tmp_seq);
            DevVarLongArray &new_union_seq = new_attr_value.value.long_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_LONG64: {
            DevVarLong64Array &union_seq = attr_value.value.long64_att_value();
            DevVarLong64Array tmp_seq(1);
            new_attr_value.value.long64_att_value(tmp_seq);
            DevVarLong64Array &new_union_seq = new_attr_value.value.long64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_FLOAT: {
            DevVarFloatArray &union_seq = attr_value.value.float_att_value();
            DevVarFloatArray tmp_seq(1);
            new_attr_value.value.float_att_value(tmp_seq);
            DevVarFloatArray &new_union_seq = new_attr_value.value.float_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_DOUBLE: {
            DevVarDoubleArray &union_seq = attr_value.value.double_att_value();
            DevVarDoubleArray tmp_seq(1);
            new_attr_value.value.double_att_value(tmp_seq);
            DevVarDoubleArray &new_union_seq = new_attr_value.value.double_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_UCHAR: {
            DevVarUCharArray &union_seq = attr_value.value.uchar_att_value();
            DevVarUCharArray tmp_seq(1);
            new_attr_value.value.uchar_att_value(tmp_seq);
            DevVarUCharArray &new_union_seq = new_attr_value.value.uchar_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_USHORT: {
            DevVarUShortArray &union_seq = attr_value.value.ushort_att_value();
            DevVarUShortArray tmp_seq(1);
            new_attr_value.value.ushort_att_value(tmp_seq);
            DevVarUShortArray &new_union_seq = new_attr_value.value.ushort_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_ULONG: {
            DevVarULongArray &union_seq = attr_value.value.ulong_att_value();
            DevVarULongArray tmp_seq(1);
            new_attr_value.value.ulong_att_value(tmp_seq);
            DevVarULongArray &new_union_seq = new_attr_value.value.ulong_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_ULONG64: {
            DevVarULong64Array &union_seq = attr_value.value.ulong64_att_value();
            DevVarULong64Array tmp_seq(1);
            new_attr_value.value.ulong64_att_value(tmp_seq);
            DevVarULong64Array &new_union_seq = new_attr_value.value.ulong64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_STRING: {
            DevVarStringArray &union_seq = attr_value.value.string_att_value();
            DevVarStringArray tmp_seq(1);
            new_attr_value.value.string_att_value(tmp_seq);
            DevVarStringArray &new_union_seq = new_attr_value.value.string_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case ATT_STATE: {
            DevVarStateArray &union_seq = attr_value.value.state_att_value();
            DevVarStateArray tmp_seq(1);
            new_attr_value.value.state_att_value(tmp_seq);
            DevVarStateArray &new_union_seq = new_attr_value.value.state_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;

        case DEVICE_STATE: {
            DevState union_state = attr_value.value.dev_state_att();
            new_attr_value.value.dev_state_att(union_state);
        }
            break;

        case ATT_NO_DATA: {
            new_attr_value.value.union_no_data(true);
        }
            break;

        case ATT_ENCODED: {
            DevVarEncodedArray &union_seq = attr_value.value.encoded_att_value();
            DevVarEncodedArray tmp_seq(1);
            new_attr_value.value.encoded_att_value(tmp_seq);
            DevVarEncodedArray &new_union_seq = new_attr_value.value.encoded_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len, len, union_seq.get_buffer(false), false);
        }
            break;
    }
}


template<typename T>
void Tango::polling::PollTask::copy_remaining(T &old_attr_value, T &new_attr_value) {
    new_attr_value.quality = old_attr_value.quality;
    new_attr_value.data_format = old_attr_value.data_format;
    new_attr_value.time = old_attr_value.time;
    new_attr_value.r_dim = old_attr_value.r_dim;
    new_attr_value.w_dim = old_attr_value.w_dim;
    new_attr_value.err_list = old_attr_value.err_list;

    char *tmp_ptr = old_attr_value.name._retn();
    new_attr_value.name = tmp_ptr;

    omni_mutex *tmp_mut = old_attr_value.get_attr_mutex();
    if (tmp_mut != NULL) {
        new_attr_value.set_attr_mutex(tmp_mut);
        old_attr_value.set_attr_mutex(NULL);
    }

}

template <typename AttributesList>
void put_exceptions_into_map(map<size_t, Tango::DevFailed*> exceptions_map, AttributesList& attrs, size_t size){
    for(size_t i = 0; i < size; ++i){
        if (attrs[i].err_list.length() != 0) {
            exceptions_map.emplace(i, new Tango::DevFailed(attrs[i].err_list));
        }
    }
}

void Tango::polling::PollTask::poll_attr() {
    string att_list = vector_to_string(work_.name);
    cout5 << "----------> Time = "
          << chrono::system_clock::now().time_since_epoch().count()
          << " Dev name = " << work_.dev->get_name()
          << ", Attr name = " << att_list << endl;

    Tango::AttributeValueList *argout = nullptr;
    Tango::AttributeValueList_3 *argout_3 = nullptr;
    Tango::AttributeValueList_4 *argout_4 = nullptr;
    Tango::AttributeValueList_5 *argout_5 = nullptr;
    Tango::DevFailed *attr_reading_exception = nullptr;


    vector<PollObj *>::iterator ite;
    map<size_t, Tango::DevFailed *> map_except;

    long idl_vers = work_.dev->get_dev_idl_version();
    auto start = chrono::high_resolution_clock::now();

//
// Read the attributes
//
    try {
        DevVarStringArray attr_names{work_.name.size()};

        for (size_t i = 0, size = work_.name.size(); i < size; i++)
            attr_names[i] = work_.name[i].c_str();

        if (idl_vers >= 5)
            argout_5 = (static_cast<Device_5Impl *>(work_.dev))->read_attributes_5(attr_names, Tango::DEV,
                                                                                   Util::kDummyClientIdentity);
        else if (idl_vers == 4)
            argout_4 = (static_cast<Device_4Impl *>(work_.dev))->read_attributes_4(attr_names, Tango::DEV,
                                                                                   Util::kDummyClientIdentity);
        else if (idl_vers == 3)
            argout_3 = (static_cast<Device_3Impl *>(work_.dev))->read_attributes_3(attr_names, Tango::DEV);
        else
            argout = work_.dev->read_attributes(attr_names);
    }
    catch (Tango::DevFailed &e) {
        attr_reading_exception = new Tango::DevFailed(e);
    }

    auto stop = chrono::high_resolution_clock::now();

//
// Starting with IDl release 3, an attribute in error is not an exception any more. Re-create one.
// Don't forget that it is still possible to receive classical exception (in case of Monitor timeout for instance)
//


    if (idl_vers >= 3) {
        if (idl_vers >= 5) {
            put_exceptions_into_map(map_except,*argout_5,work_.name.size());
            //TODO delete argout_5?
        } else if (idl_vers == 4) {
            put_exceptions_into_map(map_except,*argout_4,work_.name.size());
            delete argout_4;
        }
    } else {
        if ((*argout_3)[0].err_list.length() != 0) {
            map_except.emplace(0, new Tango::DevFailed((*argout_3)[0].err_list));
            delete argout_3;
        }
    }



//
// Events - for each event call the detect_and_push() method this method will fire events if there are clients
// registered and if there is an event (on_change, on_alarm or periodic)
// We also have to retrieve which kind of clients made the subscription (zmq or notifd) and send the event accordingly
//

    if (attr_reading_exception != nullptr) {
        engine_.get_event_system().push_error_event(work_, *attr_reading_exception);
    } else {
        engine_.get_event_system().push_event(work_);
    }

    EventSupplier *event_supplier_nd = NULL;
    EventSupplier *event_supplier_zmq = NULL;

    for (size_t ctr = 0; ctr < nb_obj; ctr++) {
        Attribute &att = work_.dev->get_device_attr()->get_attr_by_name(work_.name[ctr].c_str());

        if (att.use_notifd_event() == true && event_supplier_nd == NULL)
            event_supplier_nd = Util::instance()->get_notifd_event_supplier();
        if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
            event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

        if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL)) {
            if (attr_failed == true) {
                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));


                AttributeValue dummy_att{};
                AttributeValue_3 dummy_att3{};
                AttributeValue_4 dummy_att4{};
                AttributeValue_5 dummy_att5{};

                if (idl_vers > 4)
                    ad.attr_val_5 = &dummy_att5;//TODO may refer to deleted object
                else if (idl_vers == 4)
                    ad.attr_val_4 = &dummy_att4;
                else if (idl_vers == 3)
                    ad.attr_val_3 = &dummy_att3;
                else
                    ad.attr_val = &dummy_att;

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                SendEventType send_event;
                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(work_.dev, ad, attr_reading_exception,
                                                                           work_.name[ctr], &before_cmd);
                if (event_supplier_zmq != NULL) {
                    if (event_supplier_nd != NULL) {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(work_.dev, CHANGE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, attr_reading_exception);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(work_.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, attr_reading_exception);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(work_.dev, PERIODIC_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, attr_reading_exception);
                    } else
                        event_supplier_zmq->detect_and_push_events(work_.dev, ad, attr_reading_exception, work_.name[ctr],
                                                                   &before_cmd);
                }
            } else {
                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &((*argout_5)[ctr]);
                else if (idl_vers == 4)
                    ad.attr_val_4 = &((*argout_4)[ctr]);
                else if (idl_vers == 3)
                    ad.attr_val_3 = &((*argout_3)[ctr]);
                else
                    ad.attr_val = &((*argout)[ctr]);

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                SendEventType send_event;

                map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                Tango::DevFailed *tmp_except;
                if (ite2 == map_except.end())
                    tmp_except = attr_reading_exception;
                else
                    tmp_except = ite2->second;

                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(work_.dev, ad, tmp_except,
                                                                           work_.name[ctr], &before_cmd);
                if (event_supplier_zmq != NULL) {
                    if (event_supplier_nd != NULL) {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(work_.dev, CHANGE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(work_.dev, PERIODIC_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(work_.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, tmp_except);
                    } else
                        event_supplier_zmq->detect_and_push_events(work_.dev, ad, tmp_except, work_.name[ctr],
                                                                   &before_cmd);
                }
            }
        }
    }


//
// Insert result in polling buffer and simply forget this attribute if it is not possible to insert the result in
// polling buffer
// Different case according to the number of attributes which has been read by this poll.
// The PollObj object are supposed to store data coming from a read_attributes() executed for 1 attribute.
// Sonce Tango 9.0.8, it is not always the case. This is why we differantiate cases when we read several att
// in once call. Sin the last case, re-create memory layout as the one coming from read_attributes called for
// single attribute.
// This new feature is available only for devices with IDL 4 or more
//

    try {
        work_.dev->get_poll_monitor().get_monitor();
        for (size_t ctr = 0; ctr < nb_obj; ctr++) {
            ite = work_.dev->get_polled_obj_by_type_name(work_.type, work_.name[ctr]);
            if (attr_failed == false) {
                if (nb_obj == 1) {
                    if (idl_vers >= 5)
                        (*ite)->insert_data(argout_5, before_cmd, work_.needed_time);
                    else if (idl_vers == 4)
                        (*ite)->insert_data(argout_4, before_cmd, work_.needed_time);
                    else if (idl_vers == 3)
                        (*ite)->insert_data(argout_3, before_cmd, work_.needed_time);
                    else
                        (*ite)->insert_data(argout, before_cmd, work_.needed_time);
                } else {
                    if (idl_vers >= 5) {
                        map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                        if (ite2 == map_except.end()) {
                            Tango::AttributeValueList_5 *new_argout_5 = new Tango::AttributeValueList_5(1);
                            new_argout_5->length(1);
                            (*new_argout_5)[0].value.union_no_data(true);
                            steal_data((*argout_5)[ctr], (*new_argout_5)[0]);
                            copy_remaining((*argout_5)[ctr], (*new_argout_5)[0]);
                            (*new_argout_5)[0].data_type = (*argout_5)[ctr].data_type;
                            (*ite)->insert_data(new_argout_5, before_cmd, work_.needed_time);
                        } else
                            (*ite)->insert_except(ite2->second, before_cmd, work_.needed_time);
                    } else {
                        map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                        if (ite2 == map_except.end()) {
                            Tango::AttributeValueList_4 *new_argout_4 = new Tango::AttributeValueList_4(1);
                            new_argout_4->length(1);
                            (*new_argout_4)[0].value.union_no_data(true);
                            steal_data((*argout_4)[ctr], (*new_argout_4)[0]);
                            copy_remaining((*argout_4)[ctr], (*new_argout_4)[0]);
                            (*ite)->insert_data(new_argout_4, before_cmd, work_.needed_time);
                        } else
                            (*ite)->insert_except(ite2->second, before_cmd, work_.needed_time);
                    }
                }
            } else {
                if (nb_obj == 1)
                    (*ite)->insert_except(attr_reading_exception, before_cmd, work_.needed_time);
                else {
                    if (ctr != nb_obj - 1) {
                        Tango::DevFailed *dup_except = new Tango::DevFailed(attr_reading_exception->errors);
                        (*ite)->insert_except(dup_except, before_cmd, work_.needed_time);
                    } else
                        (*ite)->insert_except(attr_reading_exception, before_cmd, work_.needed_time);
                }
            }
        }

        if (nb_obj != 1 && attr_failed == false) {
            if (idl_vers >= 5)
                delete argout_5;
            else
                delete argout_4;
        }

        work_.dev->get_poll_monitor().rel_monitor();
    }
    catch (Tango::DevFailed &) {
        if (attr_failed == false) {
            if (idl_vers >= 5)
                delete argout_5;
            else if (idl_vers == 4)
                delete argout_4;
            else if (idl_vers == 3)
                delete argout_3;
            else
                delete argout;
        } else
            delete attr_reading_exception;
        work_.dev->get_poll_monitor().rel_monitor();
    }

}

