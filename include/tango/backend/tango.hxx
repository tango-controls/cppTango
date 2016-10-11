//
// Created by ingvord on 10/10/16.
//
//TODO clean out CORBA stuff
#pragma once

#include <cstdlib>
#include <cstdint>
#include <string>
#include <array>
#include <mutex>

namespace tango {
    namespace backend {
        struct DevVarBooleanArray {
            DevVarBooleanArray() = default;

            DevVarBooleanArray(const DevVarBooleanArray&) = default;

            DevVarBooleanArray &operator=(const DevVarBooleanArray&) = default;

            DevVarBooleanArray(DevVarBooleanArray&&) = default;

            DevVarBooleanArray& operator= (DevVarBooleanArray&&) = default;

            ~DevVarBooleanArray() = default;
            
            // non default
            DevVarBooleanArray(size_t _max);

            DevVarBooleanArray(size_t _max, size_t _len, bool *_val,
                               bool _rel = 0);
        };

        struct DevVarDoubleArray {
            DevVarDoubleArray() = default;

            DevVarDoubleArray(const DevVarDoubleArray &) = default;

            DevVarDoubleArray &operator=(const DevVarDoubleArray &_s);

            DevVarDoubleArray(DevVarDoubleArray &&) = default;

            DevVarDoubleArray &operator=(DevVarDoubleArray &&);
            
            ~DevVarDoubleArray();
            //non default
            DevVarDoubleArray(size_t _max);

            DevVarDoubleArray(size_t _max, size_t _len, double *_val,
                                     bool _rel = 0);


            
        };

        class DevVarFloatArray {
        public:
            DevVarFloatArray();

            DevVarFloatArray(const DevVarFloatArray &_s);
            
            DevVarFloatArray &operator=(const DevVarFloatArray &_s);

            DevVarFloatArray(DevVarFloatArray &&);

            DevVarFloatArray &operator=(DevVarFloatArray &&);
            
            ~DevVarFloatArray();
            
            //non default
             DevVarFloatArray(size_t _max);

             DevVarFloatArray(size_t _max, size_t _len, float *_val, bool _rel = 0);


             
        };

        struct DevVarShortArray {
            DevVarShortArray();

            DevVarShortArray(const DevVarShortArray &_s);

            DevVarShortArray &operator=(const DevVarShortArray &_s);

            DevVarShortArray(const DevVarShortArray &&);

            DevVarShortArray &operator=(const DevVarShortArray &&);
            
            ~DevVarShortArray();

            //non default
            DevVarShortArray(size_t _max);

            DevVarShortArray(size_t _max, size_t _len, int16_t *_val, bool _rel = 0);
        };

        struct DevVarLongArray {
             DevVarLongArray();

             DevVarLongArray(const DevVarLongArray &_s);

            DevVarLongArray &operator=(const DevVarLongArray &_s);

            DevVarLongArray(const DevVarLongArray &&);

            DevVarLongArray &operator=(const DevVarLongArray &&);

            ~DevVarLongArray();
            
            //non default
             DevVarLongArray(size_t _max);

             DevVarLongArray(size_t _max, size_t _len, int32_t *_val, bool _rel = 0);
        };

        struct DevVarLong64Array {
            DevVarLong64Array();

            DevVarLong64Array(const DevVarLong64Array &_s);

            DevVarLong64Array &operator=(const DevVarLong64Array &_s) ;

            DevVarLong64Array(const DevVarLong64Array &&);

            DevVarLong64Array &operator=(const DevVarLong64Array &&) ;

            ~DevVarLong64Array();

            //non default
            DevVarLong64Array(size_t _max);

            DevVarLong64Array(size_t _max, size_t _len, int64_t *_val,
                                     bool _rel = 0);
        };

        struct DevVarCharArray {
            DevVarCharArray();

            DevVarCharArray(const DevVarCharArray &_s);

            DevVarCharArray &operator=(const DevVarCharArray &_s);

            DevVarCharArray(DevVarCharArray &&);

            DevVarCharArray &operator=(DevVarCharArray &&);
            
            ~DevVarCharArray();

            //non default
            DevVarCharArray(size_t _max);

            DevVarCharArray(size_t _max, size_t _len, char *_val, bool _rel = 0);
        };

        struct DevVarStringArray {
            DevVarStringArray();

            DevVarStringArray(const DevVarStringArray &_s);

            DevVarStringArray(size_t _max);

            DevVarStringArray(size_t _max, size_t _len, std::string*_val, bool _rel = 0);


            DevVarStringArray &operator=(const DevVarStringArray &_s);
        };

        struct DevVarUShortArray {
            DevVarUShortArray();

            DevVarUShortArray(const DevVarUShortArray &_s);

            DevVarUShortArray(size_t _max);

            DevVarUShortArray(size_t _max, size_t _len, uint16_t *_val,
                                     bool _rel = 0);


            DevVarUShortArray &operator=(const DevVarUShortArray &_s);
        };

        struct DevVarULongArray {
            DevVarULongArray();

            DevVarULongArray(const DevVarULongArray &_s);

            DevVarULongArray(size_t _max);

            DevVarULongArray(size_t _max, size_t _len, uint32_t *_val, bool _rel = 0);


            DevVarULongArray &operator=(const DevVarULongArray &_s);
        };

        struct DevVarULong64Array {
            DevVarULong64Array();

            DevVarULong64Array(const DevVarULong64Array &_s);

            DevVarULong64Array(size_t _max);

            DevVarULong64Array(size_t _max, size_t _len, uint64_t *_val,
                                      bool _rel = 0);


            DevVarULong64Array &operator=(const DevVarULong64Array &_s);
        };

        struct DevEncoded {
            std::string encoded_format;

            DevVarCharArray encoded_data;

//            void operator>>= (cdrStream &) const;
//            void operator<<= (cdrStream &);
        };
        
        struct DevVarEncodedArray {
            DevVarEncodedArray();

            DevVarEncodedArray(const DevVarEncodedArray &_s);

            DevVarEncodedArray(size_t _max);

            DevVarEncodedArray(size_t _max, size_t _len, DevEncoded* _val, bool _rel = 0);


            DevVarEncodedArray &operator=(const DevVarEncodedArray &_s);
        };


        struct DevVarLongStringArray {
            DevVarLongArray lvalue;

            DevVarStringArray svalue;


//            void operator>>=(cdrStream &) const;

//            void operator<<=(cdrStream &);
        };

        struct DevVarDoubleStringArray {
            DevVarDoubleArray dvalue;

            DevVarStringArray svalue;


//            void operator>>=(cdrStream &) const;

//            void operator<<=(cdrStream &);
        };

        typedef uint64_t JavaUUID[2];
        typedef uint64_t JavaUUID_slice;

        JavaUUID_slice* JavaUUID_alloc() {
            return new JavaUUID_slice[2];
        }

        JavaUUID_slice * JavaUUID_dup(const JavaUUID_slice *_s) {
            if (!_s) return 0;
            JavaUUID_slice * _data = JavaUUID_alloc();
            if (_data) {
                for (size_t _0i0 = 0; _0i0 < 2; _0i0++) {

                    _data[_0i0] = _s[_0i0];

                }

            }
            return _data;
        }

        void JavaUUID_copy(JavaUUID_slice *_to, const JavaUUID_slice *_from) {
            for (size_t _0i0 = 0; _0i0 < 2; _0i0++) {

                _to[_0i0] = _from[_0i0];

            }

        }

        void JavaUUID_free(JavaUUID_slice *_s) {
            delete[] _s;
        }

        class JavaUUID_copyHelper {
        public:
            static JavaUUID_slice *alloc() { return ::tango::backend::JavaUUID_alloc(); }

            static JavaUUID_slice *dup(const JavaUUID_slice *p) { return ::tango::backend::JavaUUID_dup(p); }

            static void free(JavaUUID_slice *p) { ::tango::backend::JavaUUID_free(p); }
        };

        typedef uint32_t CppClntIdent;

        struct JavaClntIdent {
            std::string MainClass;

            JavaUUID uuid;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        enum class LockerLanguage {
            CPP, JAVA, PYTHON
        };

        struct ClntIdent {
            ClntIdent();

            ClntIdent(const ClntIdent &_value);
            ~ClntIdent();

            ClntIdent &operator=(const ClntIdent &_value);

//            LockerLanguage _d() const { return _pd__d; }

            void _d(LockerLanguage _value);


            CppClntIdent cpp_clnt() const { return _pd_cpp_clnt; }

            void cpp_clnt(CppClntIdent _value);

            const JavaClntIdent &java_clnt() const;

            JavaClntIdent &java_clnt();

            void java_clnt(const JavaClntIdent &_value);


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);

        private:
            LockerLanguage _pd__d;
            bool _pd__default;
            bool _pd__initialised;

            union {
                CppClntIdent _pd_cpp_clnt;


            };


            JavaClntIdent _pd_java_clnt;


        };

        enum class AttrQuality {
            ATTR_VALID, ATTR_INVALID, ATTR_ALARM, ATTR_CHANGING, ATTR_WARNING 
        };

        enum class AttrWriteType {
            READ, READ_WITH_WRITE, WRITE, READ_WRITE, WT_UNKNOWN 
        };

        enum class AttrDataFormat {
            SCALAR, SPECTRUM, IMAGE, FMT_UNKNOWN
        };

        enum class DevSource {
            DEV, CACHE, CACHE_DEV
        };

        enum class ErrSeverity {
            WARN, ERR, PANIC
        };

        enum class DevState {
            ON,
            OFF,
            CLOSE,
            OPEN,
            INSERT,
            EXTRACT,
            MOVING,
            STANDBY,
            FAULT,
            INIT,
            RUNNING,
            ALARM,
            DISABLE,
            UNKNOWN
        };

        enum class DispLevel {
            OPERATOR, EXPERT, DL_UNKNOWN
        };

        struct DevVarStateArray {
            DevVarStateArray() {}

            DevVarStateArray(const DevVarStateArray &_s);
            DevVarStateArray(size_t _max);

            DevVarStateArray(size_t _max, size_t _len, DevState* _val, bool _rel = 0);


            DevVarStateArray &operator=(const DevVarStateArray &_s);
        };

        struct TimeVal {
            int32_t tv_sec;

            int32_t tv_usec;

            int32_t tv_nsec;


//            void operator>>=(cdrStream &) const;

//            void operator<<=(cdrStream &);
        };

        struct DevCmdInfo {
            std::string cmd_name;

            int32_t cmd_tag;

            int32_t in_type;

            int32_t out_type;

            std::string in_type_desc;

            std::string out_type_desc;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevCmdInfo_2 {
            std::string cmd_name;

            DispLevel level;

            int32_t cmd_tag;

            int32_t in_type;

            int32_t out_type;

            std::string in_type_desc;

            std::string out_type_desc;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevCmdInfoList {
            DevCmdInfoList() {}

            DevCmdInfoList(const DevCmdInfoList &_s);

            DevCmdInfoList(size_t _max);

            DevCmdInfoList(size_t _max, size_t _len, DevCmdInfo* _val, bool _rel = 0);


            DevCmdInfoList &operator=(const DevCmdInfoList &_s);
        };

        struct DevCmdInfoList_2 {
            DevCmdInfoList_2() {}

            DevCmdInfoList_2(const DevCmdInfoList_2 &_s);

            DevCmdInfoList_2(size_t _max);

            DevCmdInfoList_2(size_t _max, size_t _len, DevCmdInfo_2* _val, bool _rel = 0);


            DevCmdInfoList_2 &operator=(const DevCmdInfoList_2 &_s);
        };

        struct DevError {
            std::string reason;

            ErrSeverity severity;

            std::string desc;

            std::string origin;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevErrorList {
            DevErrorList();

            DevErrorList(const DevErrorList &_s);

            DevErrorList(size_t _max);

            DevErrorList(size_t _max, size_t _len, DevError* _val, bool _rel = 0);


            DevErrorList &operator=(const DevErrorList &_s);

            size_t length();
        };

        struct NamedDevError {
            std::string name;

            int32_t index_in_call;

            DevErrorList err_list;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct NamedDevErrorList {
            NamedDevErrorList();

            NamedDevErrorList(const NamedDevErrorList &_s);

            NamedDevErrorList(size_t _max);

            NamedDevErrorList(size_t _max, size_t _len,NamedDevError* _val, bool _rel = 0);


            NamedDevErrorList &operator=(const NamedDevErrorList &_s);
        };

        struct DevFailed : public std::exception {
            DevErrorList errors;


            DevFailed();

            DevFailed(const DevFailed &);

            DevFailed(const DevErrorList i_errors);

            DevFailed &operator=(const DevFailed &);

            virtual ~DevFailed();

            virtual void _raise() const;

            static DevFailed *_downcast(std::exception *);

            static const DevFailed *_downcast(const std::exception *);

            static DevFailed *_narrow(std::exception *_e) {
                return _downcast(_e);
            }

//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);

            virtual std::exception *_NP_duplicate() const;
        };

        struct MultiDevFailed : public std::exception {
            NamedDevErrorList errors;


            MultiDevFailed();

            MultiDevFailed(const MultiDevFailed &);

            MultiDevFailed(const NamedDevErrorList i_errors);

            MultiDevFailed &operator=(const MultiDevFailed &);

            virtual ~MultiDevFailed();

            virtual void _raise() const;

            static MultiDevFailed *_downcast(std::exception *);

            static const MultiDevFailed *_downcast(const std::exception *);

            static MultiDevFailed *_narrow(std::exception *_e) {
                return _downcast(_e);
            }

//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);

            virtual std::exception *_NP_duplicate() const;
        };

        struct AttributeConfig {
            std::string name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            std::string description;

            std::string label;

            std::string unit;

            std::string standard_unit;

            std::string display_unit;

            std::string format;

            std::string min_value;

            std::string max_value;

            std::string min_alarm;

            std::string max_alarm;

            std::string writable_attr_name;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeConfig_2 {
            std::string name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            std::string description;

            std::string label;

            std::string unit;

            std::string standard_unit;

            std::string display_unit;

            std::string format;

            std::string min_value;

            std::string max_value;

            std::string min_alarm;

            std::string max_alarm;

            std::string writable_attr_name;

            DispLevel level;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct AttributeValue {
            T value;

            AttrQuality quality;

            TimeVal time;

            std::string name;

            int32_t dim_x;

            int32_t dim_y;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeDim {
            int32_t dim_x;

            int32_t dim_y;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct AttributeValue_3 {
            T value;

            AttrQuality quality;

            TimeVal time;

            std::string name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        enum class AttributeDataType {
            ATT_BOOL,
            ATT_SHORT,
            ATT_LONG,
            ATT_LONG64,
            ATT_FLOAT,
            ATT_DOUBLE,
            ATT_UCHAR,
            ATT_USHORT,
            ATT_ULONG,
            ATT_ULONG64,
            ATT_STRING,
            ATT_STATE,
            DEVICE_STATE,
            ATT_ENCODED,
            ATT_NO_DATA
        };

        class AttrValUnion {
        public:

            AttrValUnion();

            AttrValUnion(const AttrValUnion &_value);

            ~AttrValUnion();

            AttrValUnion &operator=(const AttrValUnion &_value);

            AttributeDataType _d() const { return _pd__d; }

            void _d(AttributeDataType _value);

            const DevVarBooleanArray &bool_att_value() const;

            DevVarBooleanArray &bool_att_value();

            void bool_att_value(const DevVarBooleanArray &_value);

            const DevVarShortArray &short_att_value() const;

            DevVarShortArray &short_att_value();

            void short_att_value(const DevVarShortArray &_value);

            const DevVarLongArray &long_att_value() const;

            DevVarLongArray &long_att_value();

            void long_att_value(const DevVarLongArray &_value);

            const DevVarLong64Array &long64_att_value() const;

            DevVarLong64Array &long64_att_value();

            void long64_att_value(const DevVarLong64Array &_value);

            const DevVarFloatArray &float_att_value() const;

            DevVarFloatArray &float_att_value();

            void float_att_value(const DevVarFloatArray &_value);

            const DevVarDoubleArray &double_att_value() const;

            DevVarDoubleArray &double_att_value();

            void double_att_value(const DevVarDoubleArray &_value);

            const DevVarCharArray &uchar_att_value() const;

            DevVarCharArray &uchar_att_value();

            void uchar_att_value(const DevVarCharArray &_value);

            const DevVarUShortArray &ushort_att_value() const;

            DevVarUShortArray &ushort_att_value();

            void ushort_att_value(const DevVarUShortArray &_value);

            const DevVarULongArray &ulong_att_value() const;

            DevVarULongArray &ulong_att_value();

            void ulong_att_value(const DevVarULongArray &_value);

            const DevVarULong64Array &ulong64_att_value() const;

            DevVarULong64Array &ulong64_att_value();

            void ulong64_att_value(const DevVarULong64Array &_value);

            const DevVarStringArray &string_att_value() const;

            DevVarStringArray &string_att_value();

            void string_att_value(const DevVarStringArray &_value);

            const DevVarStateArray &state_att_value() const;

            DevVarStateArray &state_att_value();

            void state_att_value(const DevVarStateArray &_value);

            DevState dev_state_att() const;

            void dev_state_att(DevState _value);

            const DevVarEncodedArray &encoded_att_value() const;

            DevVarEncodedArray &encoded_att_value();

            void encoded_att_value(const DevVarEncodedArray &_value) ;

            bool union_no_data() const;

            void union_no_data(bool _value);


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);

        private:
            AttributeDataType _pd__d;
            bool _pd__default;
            bool _pd__initialised;

            union {
                DevState _pd_dev_state_att;

                bool _pd_union_no_data;


            };


            DevVarBooleanArray _pd_bool_att_value;

            DevVarShortArray _pd_short_att_value;

            DevVarLongArray _pd_long_att_value;

            DevVarLong64Array _pd_long64_att_value;

            DevVarFloatArray _pd_float_att_value;

            DevVarDoubleArray _pd_double_att_value;

            DevVarCharArray _pd_uchar_att_value;

            DevVarUShortArray _pd_ushort_att_value;

            DevVarULongArray _pd_ulong_att_value;

            DevVarULong64Array _pd_ulong64_att_value;

            DevVarStringArray _pd_string_att_value;

            DevVarStateArray _pd_state_att_value;

            DevVarEncodedArray _pd_encoded_att_value;


        };

        struct AttributeValue_4 {
//Added by Tango team
            virtual ~AttributeValue_4() { if (mut_ptr != NULL)mut_ptr->unlock(); }

            AttributeValue_4() { mut_ptr = NULL; }

            void set_attr_mutex(std::mutex *ptr) { mut_ptr = ptr; }

            void rel_attr_mutex() {
                if (mut_ptr != NULL) {
                    mut_ptr->unlock();
                    mut_ptr = NULL;
                }
            }

            std::mutex *get_attr_mutex() { return mut_ptr; }

            std::mutex *mut_ptr;


            AttrValUnion value;

            AttrQuality quality;

            AttrDataFormat data_format;

            TimeVal time;

            std::string name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeValue_5 {
//Added by Tango team
            virtual ~AttributeValue_5() { if (mut_ptr != NULL)mut_ptr->unlock(); }

            AttributeValue_5() { mut_ptr = NULL; }

            void set_attr_mutex(std::mutex *ptr) { mut_ptr = ptr; }

            void rel_attr_mutex() {
                if (mut_ptr != NULL) {
                    mut_ptr->unlock();
                    mut_ptr = NULL;
                }
            }

            std::mutex *get_attr_mutex() { return mut_ptr; }

            std::mutex *mut_ptr;


            AttrValUnion value;

            AttrQuality quality;

            AttrDataFormat data_format;

            int32_t data_type;

            TimeVal time;

            std::string name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct ChangeEventProp {
            std::string rel_change;

            std::string abs_change;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct PeriodicEventProp {
            std::string period;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct ArchiveEventProp {
            std::string rel_change;

            std::string abs_change;

            std::string period;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct EventProperties {
            ChangeEventProp ch_event;

            PeriodicEventProp per_event;

            ArchiveEventProp arch_event;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeAlarm {
            std::string min_alarm;

            std::string max_alarm;

            std::string min_warning;

            std::string max_warning;

            std::string delta_t;

            std::string delta_val;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeConfig_3 {
            std::string name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            std::string description;

            std::string label;

            std::string unit;

            std::string standard_unit;

            std::string display_unit;

            std::string format;

            std::string min_value;

            std::string max_value;

            std::string writable_attr_name;

            DispLevel level;

            AttributeAlarm att_alarm;

            EventProperties event_prop;

            DevVarStringArray extensions;

            DevVarStringArray sys_extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeConfig_5 {
            std::string name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            bool memorized;

            bool mem_init;

            int32_t max_dim_x;

            int32_t max_dim_y;

            std::string description;

            std::string label;

            std::string unit;

            std::string standard_unit;

            std::string display_unit;

            std::string format;

            std::string min_value;

            std::string max_value;

            std::string writable_attr_name;

            DispLevel level;

            std::string root_attr_name;

            DevVarStringArray enum_labels;

            AttributeAlarm att_alarm;

            EventProperties event_prop;

            DevVarStringArray extensions;

            DevVarStringArray sys_extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttributeConfigList {
            AttributeConfigList() {}

            AttributeConfigList(const AttributeConfigList &_s);

            AttributeConfigList(size_t _max);

            AttributeConfigList(size_t _max, size_t _len, AttributeConfig* _val,
                                       bool _rel = 0);


            AttributeConfigList &operator=(const AttributeConfigList &_s);
        };

        struct AttributeConfigList_2 {

            AttributeConfigList_2() {}

            AttributeConfigList_2(const AttributeConfigList_2 &_s);

            AttributeConfigList_2(size_t _max);

            AttributeConfigList_2(size_t _max, size_t _len, AttributeConfig_2* _val,
                                         bool _rel = 0);


            AttributeConfigList_2 &operator=(const AttributeConfigList_2 &_s);
        };

        struct AttributeConfigList_3 {

            AttributeConfigList_3() {}

            AttributeConfigList_3(const AttributeConfigList_3 &_s);

            AttributeConfigList_3(size_t _max);

            AttributeConfigList_3(size_t _max, size_t _len, AttributeConfig_3 *_val,
                                         bool _rel = 0);


            AttributeConfigList_3 &operator=(const AttributeConfigList_3 &_s);
        };

        struct AttributeConfigList_5 {

            AttributeConfigList_5() {}

            AttributeConfigList_5(const AttributeConfigList_5 &_s);

            AttributeConfigList_5(size_t _max);

            AttributeConfigList_5(size_t _max, size_t _len, AttributeConfig_5 *_val,
                                         bool _rel = 0);


            AttributeConfigList_5 &operator=(const AttributeConfigList_5 &_s);
        };

        struct AttributeValueList {

            AttributeValueList() {}

            AttributeValueList(const AttributeValueList &_s);

            AttributeValueList(size_t _max);

            template<typename T>
            AttributeValueList(size_t _max, size_t _len, AttributeValue<T> *_val,
                                      bool _rel = 0);


            AttributeValueList &operator=(const AttributeValueList &_s);
        };

        struct AttributeValueList_3 {

            AttributeValueList_3() {}

            AttributeValueList_3(const AttributeValueList_3 &_s);

            AttributeValueList_3(size_t _max);

            template<typename T>
            AttributeValueList_3(size_t _max, size_t _len, AttributeValue_3<T> *_val,
                                        bool _rel = 0);


            AttributeValueList_3 &operator=(const AttributeValueList_3 &_s);
        };

        struct AttributeValueList_4 {

            AttributeValueList_4();

            AttributeValueList_4(const AttributeValueList_4 &_s);

            AttributeValueList_4(size_t _max);

            AttributeValueList_4(size_t _max, size_t _len, AttributeValue_4 *_val,
                                        bool _rel = 0);


            AttributeValueList_4 &operator=(const AttributeValueList_4 &_s);
        };

        struct AttributeValueList_5 {

            AttributeValueList_5();

            AttributeValueList_5(const AttributeValueList_5 &_s);

            AttributeValueList_5(size_t _max);

            AttributeValueList_5(size_t _max, size_t _len, AttributeValue_5 *_val,
                                        bool _rel = 0);


            AttributeValueList_5 &operator=(const AttributeValueList_5 &_s);
        };

        enum class PipeWriteType {
            PIPE_READ, PIPE_READ_WRITE, PIPE_WT_UNKNOWN
            };

        struct PipeConfig {
            std::string name;

            std::string description;

            std::string label;

            DispLevel level;

            PipeWriteType writable;

            DevVarStringArray extensions;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct PipeConfigList {

            PipeConfigList();

            PipeConfigList(const PipeConfigList &_s);

            PipeConfigList(size_t _max);

            PipeConfigList(size_t _max, size_t _len, PipeConfig *_val, bool _rel = 0);


            PipeConfigList &operator=(const PipeConfigList &_s);
        };

        struct DevPipeDataElt;

        struct DevVarPipeDataEltArray {
            DevVarPipeDataEltArray();

            DevVarPipeDataEltArray(const DevVarPipeDataEltArray &_s);

            DevVarPipeDataEltArray &operator=(const DevVarPipeDataEltArray &_s);

            DevVarPipeDataEltArray(size_t _max);

            DevVarPipeDataEltArray(size_t _max, size_t _len, DevPipeDataElt *_val,
                                          bool _rel = 0);

            virtual ~DevVarPipeDataEltArray();

            DevPipeDataElt &operator[](size_t _index);

            const DevPipeDataElt &operator[](size_t _index) const;

            static DevPipeDataElt *allocbuf(size_t _nelems);

            static void freebuf(DevPipeDataElt *_b);

//            void operator>>=(cdrStream &_s) const;
//
//            void operator<<=(cdrStream &_s);
        };

        struct DevPipeDataElt {
            std::string name;

            AttrValUnion value;

            DevVarPipeDataEltArray inner_blob;

            std::string inner_blob_name;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevPipeBlob {
            std::string name;

            DevVarPipeDataEltArray blob_data;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevPipeData {
//Added by Tango team
            virtual ~DevPipeData() { if (mut_ptr != NULL)mut_ptr->unlock(); }

            DevPipeData() { mut_ptr = NULL; }

            void set_pipe_mutex(std::mutex *ptr) { mut_ptr = ptr; }

            void rel_pipe_mutex() {
                if (mut_ptr != NULL) {
                    mut_ptr->unlock();
                    mut_ptr = NULL;
                }
            }

            std::mutex *mut_ptr;


            std::string name;

            TimeVal time;

            DevPipeBlob data_blob;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct AttDataReady {
            std::string name;

            int32_t data_type;

            int32_t ctr;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevIntrChange {
            bool dev_started;

            DevCmdInfoList_2 cmds;

            AttributeConfigList_5 atts;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevInfo {
            std::string dev_class;

            std::string server_id;

            std::string server_host;

            int32_t server_version;

            std::string doc_url;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevInfo_3 {
            std::string dev_class;

            std::string server_id;

            std::string server_host;

            int32_t server_version;

            std::string doc_url;

            std::string dev_type;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct DevCmdHistory {
            TimeVal time;

            bool cmd_failed;

            T value;

            DevErrorList errors;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct DevCmdHistoryList {

            DevCmdHistoryList() {}

            DevCmdHistoryList(const DevCmdHistoryList &_s);

            DevCmdHistoryList(size_t _max);

            template<typename T>
            DevCmdHistoryList(size_t _max, size_t _len, DevCmdHistory<T> *_val, bool _rel = 0);


            DevCmdHistoryList &operator=(const DevCmdHistoryList &_s);
        };

        template<typename T>
        struct DevAttrHistory {
            bool attr_failed;

            AttributeValue<T> value;

            DevErrorList errors;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct DevAttrHistory_3 {
            bool attr_failed;

            AttributeValue_3<T> value;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct EltInArray {
            int32_t start;

            int32_t nb_elt;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        struct EltInArrayList {

            EltInArrayList() {}

            EltInArrayList(const EltInArrayList &_s);

            EltInArrayList(size_t _max);

            EltInArrayList(size_t _max, size_t _len, EltInArray *_val, bool _rel = 0);


            EltInArrayList &operator=(const EltInArrayList &_s);
        };

        struct TimeValList {

            TimeValList();

            TimeValList(const TimeValList &_s);

            TimeValList(size_t _max);

            TimeValList(size_t _max, size_t _len, TimeVal *_val, bool _rel = 0);


            TimeValList &operator=(const TimeValList &_s);
        };


// Need to declare <<= for elem type, as GCC expands templates early
#if defined(__GNUG__) && __GNUG__ == 2 && __GNUC_MINOR__ == 7
   friend void operator >>= (AttrQuality, cdrStream&);
   friend void operator <<= (AttrQuality&, cdrStream&);
#endif

        struct AttrQualityList {

            AttrQualityList();

            AttrQualityList(const AttrQualityList &_s);

            AttrQualityList(size_t _max);

            AttrQualityList(size_t _max, size_t _len, AttrQuality *_val, bool _rel = 0);


            AttrQualityList &operator=(const AttrQualityList &_s);
        };

        struct AttributeDimList {

            AttributeDimList() {}

            AttributeDimList(const AttributeDimList &_s);

            AttributeDimList(size_t _max);

            AttributeDimList(size_t _max, size_t _len, AttributeDim *_val, bool _rel = 0);


            AttributeDimList &operator=(const AttributeDimList &_s);
        };

        struct DevErrorListList {

            DevErrorListList();

            DevErrorListList(const DevErrorListList &_s);

            DevErrorListList(size_t _max);

            DevErrorListList(size_t _max, size_t _len, DevErrorList *_val, bool _rel = 0);


            DevErrorListList &operator=(const DevErrorListList &_s);
        };

        template<typename T>
        struct DevAttrHistory_4 {
            std::string name;

            TimeValList dates;

            T value;

            AttrQualityList quals;

            EltInArrayList quals_array;

            AttributeDimList r_dims;

            EltInArrayList r_dims_array;

            AttributeDimList w_dims;

            EltInArrayList w_dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct DevAttrHistory_5 {
            std::string name;

            AttrDataFormat data_format;

            int32_t data_type;

            TimeValList dates;

            T value;

            AttrQualityList quals;

            EltInArrayList quals_array;

            AttributeDimList r_dims;

            EltInArrayList r_dims_array;

            AttributeDimList w_dims;

            EltInArrayList w_dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template<typename T>
        struct DevCmdHistory_4 {
            TimeValList dates;

            T value;

            AttributeDimList dims;

            EltInArrayList dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;

            int32_t cmd_type;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        template <typename T>
        struct DevAttrHistoryList {

            DevAttrHistoryList();

            DevAttrHistoryList(const DevAttrHistoryList &_s);

            DevAttrHistoryList(size_t _max);


            DevAttrHistoryList(size_t _max, size_t _len, DevAttrHistory<T> *_val,
                                      bool _rel = 0);


            DevAttrHistoryList &operator=(const DevAttrHistoryList &_s);
        };

        template <typename T>
        struct DevAttrHistoryList_3 {

            DevAttrHistoryList_3();

            DevAttrHistoryList_3(const DevAttrHistoryList_3 &_s);

            DevAttrHistoryList_3(size_t _max);

            DevAttrHistoryList_3(size_t _max, size_t _len, DevAttrHistory_3<T> *_val,
                                        bool _rel = 0);


            DevAttrHistoryList_3 &operator=(const DevAttrHistoryList_3 &_s);
        };

        struct ZmqCallInfo {
            int32_t version;

            uint32_t ctr;

            std::string method_name;

            DevVarCharArray oid;

            bool call_is_except;


//            void operator>>=(cdrStream &) const;
//
//            void operator<<=(cdrStream &);
        };

        class Device  {
        protected:
            virtual ~Device();
        public:
            template<typename IN,
                    typename OUT>
            auto command_inout(const std::string& command, const IN &argin) -> decltype(OUT::value_type);

            virtual AttributeConfigList *get_attribute_config(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual void set_attribute_config(const ::tango::backend::AttributeConfigList &new_conf) = 0;

            virtual AttributeValueList *read_attributes(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual void write_attributes(const ::tango::backend::AttributeValueList &values) = 0;

            virtual void ping() = 0;

            virtual DevVarStringArray *black_box(int32_t n) = 0;

            virtual DevInfo *info() = 0;

            virtual DevCmdInfoList *command_list_query() = 0;

            virtual DevCmdInfo *command_query(const char *command) = 0;

            virtual char *name() = 0;

            virtual char *description() = 0;

            virtual DevState state() = 0;

            virtual char *status() = 0;

            virtual char *adm_name() = 0;
       };


        class Device_2 : public virtual Device{
        protected:
            virtual ~Device_2();
        public:
            template<typename IN, typename OUT>
            OUT* command_inout_2(const char *command, const IN& argin, ::tango::backend::DevSource source);

            virtual AttributeValueList *
            read_attributes_2(const ::tango::backend::DevVarStringArray &names, ::tango::backend::DevSource source) = 0;

            virtual AttributeConfigList_2 *get_attribute_config_2(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual DevCmdInfoList_2 *command_list_query_2() = 0;

            virtual DevCmdInfo_2 *command_query_2(const char *command) = 0;

            virtual DevCmdHistoryList *command_inout_history_2(const char *command, int32_t n) = 0;

            virtual DevAttrHistoryList *read_attribute_history_2(const char *name, int32_t n) = 0;
        };


        class Device_3 :
                public virtual Device_2 {
        protected:
            virtual ~Device_3();
        public:
            virtual AttributeValueList_3 *
            read_attributes_3(const ::tango::backend::DevVarStringArray &names, ::tango::backend::DevSource source) = 0;

            virtual void write_attributes_3(const ::tango::backend::AttributeValueList &values) = 0;

            virtual DevAttrHistoryList_3 *read_attribute_history_3(const char *name, int32_t n) = 0;

            virtual DevInfo_3 *info_3() = 0;

            virtual AttributeConfigList_3 *get_attribute_config_3(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual void set_attribute_config_3(const ::tango::backend::AttributeConfigList_3 &new_conf) = 0;
        };


        class Device_4  :
                public virtual Device_3 {
        protected:
            virtual ~Device_4();
        public:
            template<typename T>
            DevAttrHistory_4<T> *read_attribute_history_4(const std::string& name, int32_t n);

            template<typename T>
            DevCmdHistory_4<T> *command_inout_history_4(const std::string& command, int32_t n);

            template <typename IN, typename OUT>
            auto
            command_inout_4(const std::string& command, const IN &argin, ::tango::backend::DevSource source,
                            const ::tango::backend::ClntIdent &cl_ident) -> decltype(OUT::value_type);

            virtual AttributeValueList_4 *
            read_attributes_4(const ::tango::backend::DevVarStringArray &names, ::tango::backend::DevSource source,
                              const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual void
            write_attributes_4(const ::tango::backend::AttributeValueList_4 &values, const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual void
            set_attribute_config_4(const ::tango::backend::AttributeConfigList_3 &new_conf,
                                   const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_4 *
            write_read_attributes_4(const ::tango::backend::AttributeValueList_4 &values,
                                    const ::tango::backend::ClntIdent &cl_ident) = 0;

        };

        class Device_5  :
                public virtual Device_4 {
        protected:
            virtual ~Device_5();
        public:
            virtual AttributeConfigList_5 *get_attribute_config_5(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual void
            set_attribute_config_5(const ::tango::backend::AttributeConfigList_5 &new_conf,
                                   const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_5 *
            read_attributes_5(const ::tango::backend::DevVarStringArray &names, ::tango::backend::DevSource source,
                              const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_5 *
            write_read_attributes_5(const ::tango::backend::AttributeValueList_4 &values,
                                    const ::tango::backend::DevVarStringArray &r_names,
                                    const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual DevAttrHistory_5 *read_attribute_history_5(const char *name, int32_t n) = 0;

            virtual PipeConfigList *get_pipe_config_5(const ::tango::backend::DevVarStringArray &names) = 0;

            virtual void
            set_pipe_config_5(const ::tango::backend::PipeConfigList &new_conf, const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual DevPipeData *read_pipe_5(const char *name, const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual void write_pipe_5(const ::tango::backend::DevPipeData &value, const ::tango::backend::ClntIdent &cl_ident) = 0;

            virtual DevPipeData *
            write_read_pipe_5(const ::tango::backend::DevPipeData &value, const ::tango::backend::ClntIdent &cl_ident) = 0;
        };
    }//backend
}//tango
