//
// Created by ingvord on 11/1/16.
//

#include "poll_task.hxx"
#include "helpers.hxx"
#include "event_system.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>

using namespace std;

using chrono::high_resolution_clock;
using chrono::milliseconds;
using chrono::duration_cast;

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
    Tango::DevFailed *cmd_execution_exception = nullptr;

    auto start = high_resolution_clock::now();

    //
    // Execute the command
    //
    try {
        argout = work_.dev->command_inout(work_.name[0].c_str(), CORBA::Any{});
    }
    catch (const Tango::DevFailed &e) {
        cmd_execution_exception = new Tango::DevFailed(e);
    }

    auto stop = high_resolution_clock::now();

    work_.start_time = duration_cast<milliseconds>(start.time_since_epoch());
    work_.needed_time = stop - start;
    work_.stop_time = duration_cast<milliseconds>(stop.time_since_epoch());

//
// Insert result in polling buffer and simply forget this command if it is not possible to insert the result in
// polling buffer
//

    try {
        struct timeval start_as_tv = duration_to_timeval(start.time_since_epoch());
        struct timeval delta_tv = duration_to_timeval(work_.needed_time);
        work_.dev->get_poll_monitor().get_monitor();
        auto ite = work_.dev->get_polled_obj_by_type_name(work_.type, work_.name[0]);
        if (cmd_execution_exception != nullptr)
            (*ite)->insert_except(cmd_execution_exception, start_as_tv, delta_tv);
        else
            (*ite)->insert_data(argout, start_as_tv, delta_tv);
        work_.dev->get_poll_monitor().rel_monitor();
    }
    catch (Tango::DevFailed &) {
        //TODO if not thrown who will release? CORBA?
        if (cmd_execution_exception != nullptr)
            delete cmd_execution_exception;
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

template<typename AttributesList>
void put_exceptions_into_map(map<string, Tango::DevFailed *>& exceptions_map, AttributesList &attrs, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (attrs[i].err_list.length() != 0) {
            exceptions_map.emplace(attrs[i].name.in(), new Tango::DevFailed(attrs[i].err_list));
        }
    }
}

void Tango::polling::PollTask::poll_attr() {
    string att_list = sequence_to_string(work_.name);
    cout5 << "----------> Time = "
          << chrono::system_clock::now().time_since_epoch().count()
          << " Dev name = " << work_.dev->get_name()
          << ", Attr name = " << att_list << endl;

    Tango::DevFailed *attr_reading_exception = nullptr;

    long idl_vers{work_.dev->get_dev_idl_version()};
    auto start = high_resolution_clock::now();

//
// Read the attributes
//
    try {
        DevVarStringArray attr_names{};
        attr_names.length(work_.name.size());

        for (size_t i = 0, size = work_.name.size(); i < size; i++)
            attr_names[i] = work_.name[i].c_str();

        if (idl_vers >= 5)
            work_.values = (dynamic_cast<Device_5Impl *>(work_.dev))->read_attributes_5(attr_names, Tango::DEV,
                                                                                        Util::kDummyClientIdentity);
        else if (idl_vers == 4)
            work_.values = (static_cast<Device_4Impl *>(work_.dev))->read_attributes_4(attr_names, Tango::DEV,
                                                                                       Util::kDummyClientIdentity);
        else if (idl_vers == 3)
            work_.values = (static_cast<Device_3Impl *>(work_.dev))->read_attributes_3(attr_names, Tango::DEV);
        else
            work_.values = work_.dev->read_attributes(attr_names);
    }
    catch (Tango::DevFailed &e) {
        attr_reading_exception = new Tango::DevFailed(e);
    }

    auto stop = high_resolution_clock::now();

    //TODO update work and publish
    work_.start_time = duration_cast<milliseconds>(start.time_since_epoch());
    work_.needed_time = duration_cast<chrono::nanoseconds>(stop - start);
    work_.stop_time = duration_cast<milliseconds>(stop.time_since_epoch());

//
// Starting with IDl release 3, an attribute in error is not an exception any more. Re-create one.
// Don't forget that it is still possible to receive classical exception (in case of Monitor timeout for instance)
//


    auto argout_5 = reinterpret_cast<AttributeValueList_5 *>(work_.values);
    auto argout_4 = reinterpret_cast<AttributeValueList_4 *>(work_.values);
    if (idl_vers >= 3) {
        if (idl_vers >= 5) {
            put_exceptions_into_map(work_.errors, *argout_5,
                                    work_.name.size());
        } else if (idl_vers == 4) {
            put_exceptions_into_map(work_.errors, *argout_4,
                                    work_.name.size());
        }
    } else {
        auto err_list = (*reinterpret_cast<AttributeValueList_3 *>(work_.values))[0].err_list;
        if (err_list.length() != 0) {
            //TODO is it correct?
            work_.errors.emplace(work_.name[0], new Tango::DevFailed(err_list));
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
        struct timeval tv = duration_to_timeval(work_.start_time);
        struct timeval delta_tv = duration_to_timeval(work_.needed_time);
        work_.dev->get_poll_monitor().get_monitor();
        for (size_t i = 0, size = work_.name.size(); i < size; i++) {
            auto name = work_.name[i];
            auto ite = work_.dev->get_polled_obj_by_type_name(work_.type, name);
            if (attr_reading_exception == nullptr) {
                if (idl_vers >= 5) {
                    map<string, Tango::DevFailed *>::iterator ite2 = work_.errors.find(name);
                    if (ite2 == work_.errors.end()) {
                        Tango::AttributeValueList_5 *new_argout_5 = new Tango::AttributeValueList_5(1);
                        new_argout_5->length(1);
                        (*new_argout_5)[0].value.union_no_data(true);
                        steal_data((*argout_5)[i], (*new_argout_5)[0]);
                        copy_remaining((*argout_5)[i], (*new_argout_5)[0]);
                        (*new_argout_5)[0].data_type = (*argout_5)[i].data_type;
                        (*ite)->insert_data(new_argout_5, tv, delta_tv);
                    } else
                        (*ite)->insert_except(ite2->second, tv, delta_tv);
                } else {
                    map<string, Tango::DevFailed *>::iterator ite2 = work_.errors.find(name);
                    if (ite2 == work_.errors.end()) {
                        Tango::AttributeValueList_4 *new_argout_4 = new Tango::AttributeValueList_4(1);
                        new_argout_4->length(1);
                        (*new_argout_4)[0].value.union_no_data(true);
                        steal_data((*argout_4)[i], (*new_argout_4)[0]);
                        copy_remaining((*argout_4)[i], (*new_argout_4)[0]);
                        (*ite)->insert_data(new_argout_4, tv, delta_tv);
                    } else
                        (*ite)->insert_except(ite2->second, tv, delta_tv);
                }
            } else {
                Tango::DevFailed *dup_except = new Tango::DevFailed(attr_reading_exception->errors);
                (*ite)->insert_except(dup_except, tv, delta_tv);
            }
        }

        work_.dev->get_poll_monitor().rel_monitor();
    }
    catch (Tango::DevFailed &) {
        if (attr_reading_exception != nullptr) {
            delete attr_reading_exception;
            work_.dev->get_poll_monitor().rel_monitor();
        }
    }
}

