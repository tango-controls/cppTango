//
// Created by ingvord on 11/1/16.
//

#include "poll_task.hxx"
#include "helpers.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>

Tango::polling::PollTask::PollTask(Tango::WorkItem &work) : work_(work) {}

std::future<void> Tango::polling::PollTask::operator()() {
    auto future = std::async(std::launch::async,&Tango::polling::PollTask::execute, this);
    return future;
}

void Tango::polling::PollTask::execute() {
    if(work_.type == Tango::POLL_CMD)
        poll_cmd();
    else
        poll_attr();
}

void Tango::polling::PollTask::poll_cmd() {


        cout5 << "----------> Time = "
              << chrono::system_clock::now().time_since_epoch().count()
              << " Dev name = " << work_.dev->get_name()
              << ", Cmd name = " << work_.name[0] << endl;

        CORBA::Any *argout = NULL;
        Tango::DevFailed *save_except = NULL;
        struct timeval before_cmd, after_cmd;
#ifdef _TG_WINDOWS_
        struct _timeb before_win,after_win;
        LARGE_INTEGER before,after;
#endif

        vector<PollObj *>::iterator ite;
        bool cmd_failed = false;
        try {
#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
                QueryPerformanceCounter(&before);
            _ftime(&before_win);
            before_cmd.tv_sec = (unsigned long)before_win.time;
            before_cmd.tv_usec = (long)before_win.millitm * 1000;
#else
            gettimeofday(&before_cmd, NULL);
#endif
            before_cmd.tv_sec = before_cmd.tv_sec - DELTA_T;

//
// Execute the command
//

            argout = work_.dev->command_inout(work_.name[0].c_str(), CORBA::Any{});

#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                work_.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                work_.needed_time = needed_time;
            }
#else
            gettimeofday(&after_cmd, NULL);
            after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
            work_.needed_time = time_diff(before_cmd, after_cmd);
#endif
        }
        catch (Tango::DevFailed &e) {
            cmd_failed = true;
#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                work_.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                work_.needed_time = needed_time;
            }
#else
            gettimeofday(&after_cmd, NULL);
            after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
            work_.needed_time = time_diff(before_cmd, after_cmd);
#endif
            save_except = new Tango::DevFailed(e);
        }

//
// Insert result in polling buffer and simply forget this command if it is not possible to insert the result in
// polling buffer
//

        try {
            work_.dev->get_poll_monitor().get_monitor();
            ite = work_.dev->get_polled_obj_by_type_name(work_.type, work_.name[0]);
            if (cmd_failed == false)
                (*ite)->insert_data(argout, before_cmd, work_.needed_time);
            else
                (*ite)->insert_except(save_except, before_cmd, work_.needed_time);
            work_.dev->get_poll_monitor().rel_monitor();
        }
        catch (Tango::DevFailed &) {
            if (cmd_failed == false)
                delete argout;
            else
                delete save_except;
            work_.dev->get_poll_monitor().rel_monitor();
        }
}



template <typename T>
void Tango::polling::PollTask::steal_data(T &attr_value,T &new_attr_value)
{
    switch (attr_value.value._d())
    {
        case ATT_BOOL:
        {
            DevVarBooleanArray &union_seq = attr_value.value.bool_att_value();
            DevVarBooleanArray tmp_seq(1);
            new_attr_value.value.bool_att_value(tmp_seq);
            DevVarBooleanArray &new_union_seq = new_attr_value.value.bool_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_SHORT:
        {
            DevVarShortArray &union_seq = attr_value.value.short_att_value();
            DevVarShortArray tmp_seq(1);
            new_attr_value.value.short_att_value(tmp_seq);
            DevVarShortArray &new_union_seq = new_attr_value.value.short_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_LONG:
        {
            DevVarLongArray &union_seq = attr_value.value.long_att_value();
            DevVarLongArray tmp_seq(1);
            new_attr_value.value.long_att_value(tmp_seq);
            DevVarLongArray &new_union_seq = new_attr_value.value.long_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_LONG64:
        {
            DevVarLong64Array &union_seq = attr_value.value.long64_att_value();
            DevVarLong64Array tmp_seq(1);
            new_attr_value.value.long64_att_value(tmp_seq);
            DevVarLong64Array &new_union_seq = new_attr_value.value.long64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_FLOAT:
        {
            DevVarFloatArray &union_seq = attr_value.value.float_att_value();
            DevVarFloatArray tmp_seq(1);
            new_attr_value.value.float_att_value(tmp_seq);
            DevVarFloatArray &new_union_seq = new_attr_value.value.float_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_DOUBLE:
        {
            DevVarDoubleArray &union_seq = attr_value.value.double_att_value();
            DevVarDoubleArray tmp_seq(1);
            new_attr_value.value.double_att_value(tmp_seq);
            DevVarDoubleArray &new_union_seq = new_attr_value.value.double_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_UCHAR:
        {
            DevVarUCharArray &union_seq = attr_value.value.uchar_att_value();
            DevVarUCharArray tmp_seq(1);
            new_attr_value.value.uchar_att_value(tmp_seq);
            DevVarUCharArray &new_union_seq = new_attr_value.value.uchar_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_USHORT:
        {
            DevVarUShortArray &union_seq = attr_value.value.ushort_att_value();
            DevVarUShortArray tmp_seq(1);
            new_attr_value.value.ushort_att_value(tmp_seq);
            DevVarUShortArray &new_union_seq = new_attr_value.value.ushort_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_ULONG:
        {
            DevVarULongArray &union_seq = attr_value.value.ulong_att_value();
            DevVarULongArray tmp_seq(1);
            new_attr_value.value.ulong_att_value(tmp_seq);
            DevVarULongArray &new_union_seq = new_attr_value.value.ulong_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_ULONG64:
        {
            DevVarULong64Array &union_seq = attr_value.value.ulong64_att_value();
            DevVarULong64Array tmp_seq(1);
            new_attr_value.value.ulong64_att_value(tmp_seq);
            DevVarULong64Array &new_union_seq = new_attr_value.value.ulong64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_STRING:
        {
            DevVarStringArray &union_seq = attr_value.value.string_att_value();
            DevVarStringArray tmp_seq(1);
            new_attr_value.value.string_att_value(tmp_seq);
            DevVarStringArray &new_union_seq = new_attr_value.value.string_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case ATT_STATE:
        {
            DevVarStateArray &union_seq = attr_value.value.state_att_value();
            DevVarStateArray tmp_seq(1);
            new_attr_value.value.state_att_value(tmp_seq);
            DevVarStateArray &new_union_seq = new_attr_value.value.state_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;

        case DEVICE_STATE:
        {
            DevState union_state = attr_value.value.dev_state_att();
            new_attr_value.value.dev_state_att(union_state);
        }
            break;

        case ATT_NO_DATA:
        {
            new_attr_value.value.union_no_data(true);
        }
            break;

        case ATT_ENCODED:
        {
            DevVarEncodedArray &union_seq = attr_value.value.encoded_att_value();
            DevVarEncodedArray tmp_seq(1);
            new_attr_value.value.encoded_att_value(tmp_seq);
            DevVarEncodedArray &new_union_seq = new_attr_value.value.encoded_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
            break;
    }
}



template <typename T>
void Tango::polling::PollTask::copy_remaining(T &old_attr_value,T &new_attr_value)
{
    new_attr_value.quality = old_attr_value.quality;
    new_attr_value.data_format = old_attr_value.data_format;
    new_attr_value.time = old_attr_value.time;
    new_attr_value.r_dim = old_attr_value.r_dim;
    new_attr_value.w_dim = old_attr_value.w_dim;
    new_attr_value.err_list = old_attr_value.err_list;

    char *tmp_ptr = old_attr_value.name._retn();
    new_attr_value.name = tmp_ptr;

    omni_mutex *tmp_mut = old_attr_value.get_attr_mutex();
    if (tmp_mut != NULL)
    {
        new_attr_value.set_attr_mutex(tmp_mut);
        old_attr_value.set_attr_mutex(NULL);
    }

}


void Tango::polling::PollTask::poll_attr() {
    size_t nb_obj = work_.name.size();
    string att_list;
    for (size_t ctr = 0; ctr < nb_obj; ctr++) {
        att_list = att_list + work_.name[ctr];
        if (ctr < (nb_obj - 1))
            att_list = att_list + ", ";
    }

    cout5 << "----------> Time = "
          << chrono::system_clock::now().time_since_epoch().count()
          << " Dev name = " << work_.dev->get_name()
          << ", Attr name = " << att_list << endl;

    struct timeval before_cmd, after_cmd;
#ifdef _TG_WINDOWS_
    struct _timeb before_win,after_win;
        LARGE_INTEGER before,after;
#endif
    Tango::AttributeValueList *argout = NULL;
    Tango::AttributeValueList_3 *argout_3 = NULL;
    Tango::AttributeValueList_4 *argout_4 = NULL;
    Tango::AttributeValueList_5 *argout_5 = NULL;
    Tango::DevFailed *saved_exception = NULL;
    bool attr_failed = false;
    vector<PollObj *>::iterator ite;
    map<size_t, Tango::DevFailed *> map_except;

    long idl_vers = work_.dev->get_dev_idl_version();
    try {
#ifdef _TG_WINDOWS_
        if (ctr_frequency != 0)
                QueryPerformanceCounter(&before);
            _ftime(&before_win);
            before_cmd.tv_sec = (unsigned long)before_win.time;
            before_cmd.tv_usec = (long)before_win.millitm * 1000;
#else
        gettimeofday(&before_cmd, NULL);
#endif
        before_cmd.tv_sec = before_cmd.tv_sec - DELTA_T;

//
// Read the attributes
//

        DevVarStringArray attr_names{nb_obj};
        for (size_t ctr = 0; ctr < nb_obj; ctr++)
            attr_names[ctr] = work_.name[ctr].c_str();

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

#ifdef _TG_WINDOWS_
        if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                work_.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm  * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                work_.needed_time = needed_time;
            }
#else
        gettimeofday(&after_cmd, NULL);
        after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
        work_.needed_time = time_diff(before_cmd, after_cmd);
#endif
    }
    catch (Tango::DevFailed &e) {
        attr_failed = true;
#ifdef _TG_WINDOWS_
        if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                work_.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                work_.needed_time = needed_time;
            }
#else
        gettimeofday(&after_cmd, NULL);
        after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
        work_.needed_time = time_diff(before_cmd, after_cmd);
#endif

        saved_exception = new Tango::DevFailed(e);
    }

//
// Starting with IDl release 3, an attribute in error is not an exception any more. Re-create one.
// Don't forget that it is still possible to receive classical exception (in case of Monitor timeout for instance)
//

    if (idl_vers >= 3) {
        if (idl_vers >= 5) {
            if (nb_obj == 1) {
                if ((attr_failed == false) && ((*argout_5)[0].err_list.length() != 0)) {
                    attr_failed = true;
                    saved_exception = new Tango::DevFailed((*argout_5)[0].err_list);
                    delete argout_5;
                }
            } else {
                for (size_t ctr = 0; ctr < nb_obj; ctr++) {
                    if ((attr_failed == false) && ((*argout_5)[ctr].err_list.length() != 0)) {
                        Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_5)[ctr].err_list);
                        map_except.insert(pair<size_t, Tango::DevFailed *>(ctr, tmp_except));
                    }
                }
            }
        } else if (idl_vers == 4) {
            if (nb_obj == 1) {
                if ((attr_failed == false) && ((*argout_4)[0].err_list.length() != 0)) {
                    attr_failed = true;
                    saved_exception = new Tango::DevFailed((*argout_4)[0].err_list);
                    delete argout_4;
                }
            } else {
                for (size_t ctr = 0; ctr < nb_obj; ctr++) {
                    if ((attr_failed == false) && ((*argout_4)[ctr].err_list.length() != 0)) {
                        Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_4)[ctr].err_list);
                        map_except.insert(pair<size_t, Tango::DevFailed *>(ctr, tmp_except));
                    }
                }
            }
        } else {
            if ((attr_failed == false) && ((*argout_3)[0].err_list.length() != 0)) {
                attr_failed = true;
                saved_exception = new Tango::DevFailed((*argout_3)[0].err_list);
                delete argout_3;
            }
        }
    }

//
// Events - for each event call the detect_and_push() method this method will fire events if there are clients
// registered and if there is an event (on_change, on_alarm or periodic)
// We also have to retrieve which kind of clients made the subscription (zmq or notifd) and send the event accordingly
//

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


                AttributeValue		dummy_att{};
                AttributeValue_3	dummy_att3{};
                AttributeValue_4 	dummy_att4{};
                AttributeValue_5	dummy_att5{};

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
                    send_event = event_supplier_nd->detect_and_push_events(work_.dev, ad, saved_exception,
                                                                           work_.name[ctr], &before_cmd);
                if (event_supplier_zmq != NULL) {
                    if (event_supplier_nd != NULL) {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(work_.dev, CHANGE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, saved_exception);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(work_.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, saved_exception);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(work_.dev, PERIODIC_EVENT, f_names, f_data,
                                                                f_names_lg, f_data_lg, ad, att, saved_exception);
                    } else
                        event_supplier_zmq->detect_and_push_events(work_.dev, ad, saved_exception, work_.name[ctr],
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
                    tmp_except = saved_exception;
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
                    (*ite)->insert_except(saved_exception, before_cmd, work_.needed_time);
                else {
                    if (ctr != nb_obj - 1) {
                        Tango::DevFailed *dup_except = new Tango::DevFailed(saved_exception->errors);
                        (*ite)->insert_except(dup_except, before_cmd, work_.needed_time);
                    } else
                        (*ite)->insert_except(saved_exception, before_cmd, work_.needed_time);
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
            delete saved_exception;
        work_.dev->get_poll_monitor().rel_monitor();
    }

}

