//
// Created by ingvord on 10/13/16.
//

#pragma once

#include <cstdlib>
#include <cstdint>
#include <string>
#include <omnithread.h>
#include <vector>

#include <tango/frontend/dev_types.hxx>

//TODO template instead of Any
//TODO DevVarXXXArray -> std::array
//TODO enum -> enum class
//TODO omnimutex -> std
namespace Tango {

    class Any{};

    using CORBA::Any = Any;

    //TODO more elegant solution?
    template<typename T>
    class DevVarArray {
    public:
        size_t length();

        T& operator[](size_t);
    };

    class DevVarBooleanArray_var;

    class DevVarBooleanArray : public DevVarArray<bool> {
    public:
        typedef DevVarBooleanArray_var _var_type;

        inline DevVarBooleanArray();

        inline DevVarBooleanArray(const DevVarBooleanArray &_s);

        inline DevVarBooleanArray(size_t _max);

        inline DevVarBooleanArray(size_t _max, size_t _len, bool *_val, bool _rel = 0);


        inline DevVarBooleanArray &operator=(const DevVarBooleanArray &_s);
    };

    class DevVarBooleanArray_out;


    class DevVarDoubleArray_var;

    class DevVarDoubleArray: public DevVarArray<double> {
    public:
        typedef DevVarDoubleArray_var _var_type;

        inline DevVarDoubleArray() {}

        inline DevVarDoubleArray(const DevVarDoubleArray &_s);

        inline DevVarDoubleArray(size_t _max);

        inline DevVarDoubleArray(size_t _max, size_t _len, double *_val, bool _rel = 0);


        inline DevVarDoubleArray &operator=(const DevVarDoubleArray &_s);
    };

    class DevVarDoubleArray_out;

    class DevVarFloatArray_var;

    class DevVarFloatArray : public DevVarArray<float>{
    public:
        typedef DevVarFloatArray_var _var_type;

        inline DevVarFloatArray() {}

        inline DevVarFloatArray(const DevVarFloatArray &_s);

        inline DevVarFloatArray(size_t _max);

        inline DevVarFloatArray(size_t _max, size_t _len, float *_val, bool _rel = 0);


        inline DevVarFloatArray &operator=(const DevVarFloatArray &_s);
    };

    class DevVarFloatArray_out;

    class  DevVarShortArray_var;

    class DevVarShortArray : public DevVarArray<short>{
    public:
        typedef DevVarShortArray_var _var_type;

        inline DevVarShortArray();

        inline DevVarShortArray(const DevVarShortArray &_s);

        inline DevVarShortArray(size_t _max);

        inline DevVarShortArray(size_t _max, size_t _len, short *_val, bool _rel = 0);


        inline DevVarShortArray &operator=(const DevVarShortArray &_s);
    };

    class DevVarShortArray_out;

    class DevVarLongArray_var;

    class DevVarLongArray : public DevVarArray<int32_t>{
    public:
        typedef DevVarLongArray_var _var_type;

        inline DevVarLongArray();

        inline DevVarLongArray(const DevVarLongArray &_s);

        inline DevVarLongArray(size_t _max);

        inline DevVarLongArray(size_t _max, size_t _len, int32_t *_val, bool _rel = 0);


        inline DevVarLongArray &operator=(const DevVarLongArray &_s);
    };

    class DevVarLongArray_out;

    class DevVarLong64Array_var;

    class DevVarLong64Array : public DevVarArray<int64_t>{
    public:
        typedef DevVarLong64Array_var _var_type;

        inline DevVarLong64Array();

        inline DevVarLong64Array(const DevVarLong64Array &_s);

        inline DevVarLong64Array(size_t _max);

        inline DevVarLong64Array(size_t _max, size_t _len, int64_t *_val, bool _rel = 0);


        inline DevVarLong64Array &operator=(const DevVarLong64Array &_s);
    };

    class DevVarLong64Array_out;

    class DevVarCharArray_var;

    class DevVarCharArray: public DevVarArray<char> {
    public:
        typedef DevVarCharArray_var _var_type;

        inline DevVarCharArray();

        inline DevVarCharArray(const DevVarCharArray &_s);

        inline DevVarCharArray(size_t _max);

        inline DevVarCharArray(size_t _max, size_t _len, char *_val, bool _rel = 0);


        inline DevVarCharArray &operator=(const DevVarCharArray &_s);
    };

    class DevVarCharArray_out;

    class DevVarStringArray_var;

    class DevVarStringArray: public DevVarArray<char*> {
    public:
        typedef DevVarStringArray_var _var_type;

        inline DevVarStringArray();

        inline DevVarStringArray(const DevVarStringArray &_s);

        inline DevVarStringArray(size_t _max);

        inline DevVarStringArray(size_t _max, size_t _len, char **_val, bool _rel = 0);


        inline DevVarStringArray &operator=(const DevVarStringArray &_s);
    };

    class DevVarStringArray_out;

    class DevVarUShortArray_var;

    class DevVarUShortArray : public DevVarArray<uint16_t>{
    public:
        typedef DevVarUShortArray_var _var_type;

        inline DevVarUShortArray();

        inline DevVarUShortArray(const DevVarUShortArray &_s);

        inline DevVarUShortArray(size_t _max);

        inline DevVarUShortArray(size_t _max, size_t _len, uint16_t *_val, bool _rel = 0);


        inline DevVarUShortArray &operator=(const DevVarUShortArray &_s);
    };

    class DevVarUShortArray_out;

    class DevVarULongArray_var;

    class DevVarULongArray : public DevVarArray<uint32_t>{
    public:
        typedef DevVarULongArray_var _var_type;

        inline DevVarULongArray();

        inline DevVarULongArray(const DevVarULongArray &_s);

        inline DevVarULongArray(size_t _max);

        inline DevVarULongArray(size_t _max, size_t _len, uint32_t *_val, bool _rel = 0);


        inline DevVarULongArray &operator=(const DevVarULongArray &_s);
    };

    class DevVarULongArray_out;

    class DevVarULong64Array_var;

    class DevVarULong64Array : public DevVarArray<uint64_t>{
    public:
        typedef DevVarULong64Array_var _var_type;

        inline DevVarULong64Array();

        inline DevVarULong64Array(const DevVarULong64Array &_s);

        inline DevVarULong64Array(size_t _max);

        inline DevVarULong64Array(size_t _max, size_t _len, uint64_t *_val,
                                  bool _rel = 0);


        inline DevVarULong64Array &operator=(const DevVarULong64Array &_s);
    };

    class DevVarULong64Array_out;

    struct DevVarDoubleStringArray {
        DevVarDoubleArray dvalue;

        DevVarStringArray svalue;
    };

    struct DevEncoded {
        std::string encoded_format;

        DevVarCharArray encoded_data;
    };

    class DevVarEncodedArray_var;

    class DevVarEncodedArray {
    public:
        typedef DevVarEncodedArray_var _var_type;

        inline DevVarEncodedArray();

        inline DevVarEncodedArray(const DevVarEncodedArray &_s);

        inline DevVarEncodedArray(size_t _max);

        inline DevVarEncodedArray(size_t _max, size_t _len, DevEncoded *_val, bool _rel = 0);


        inline DevVarEncodedArray &operator=(const DevVarEncodedArray &_s);
    };

    class DevVarEncodedArray_out;

    typedef uint64_t JavaUUID[2];
    typedef uint64_t JavaUUID_slice;

    inline JavaUUID_slice *JavaUUID_alloc() {
        return new JavaUUID_slice[2];
    }

    inline JavaUUID_slice *JavaUUID_dup(const JavaUUID_slice *_s) {
        if (!_s) return 0;
        JavaUUID_slice *_data = JavaUUID_alloc();
        if (_data) {
            for (size_t _0i0 = 0; _0i0 < 2; _0i0++) {

                _data[_0i0] = _s[_0i0];

            }

        }
        return _data;
    }

    inline void JavaUUID_copy(JavaUUID_slice *_to, const JavaUUID_slice *_from) {
        for (size_t _0i0 = 0; _0i0 < 2; _0i0++) {

            _to[_0i0] = _from[_0i0];

        }

    }

    inline void JavaUUID_free(JavaUUID_slice *_s) {
        delete[] _s;
    }

    class JavaUUID_copyHelper {
    public:
        static inline JavaUUID_slice *alloc() { return ::Tango::JavaUUID_alloc(); }

        static inline JavaUUID_slice *dup(const JavaUUID_slice *p) { return ::Tango::JavaUUID_dup(p); }

        static inline void free(JavaUUID_slice *p) { ::Tango::JavaUUID_free(p); }
    };

    typedef JavaUUID_slice *JavaUUID_out;

    typedef uint32_t CppClntIdent;

    struct JavaClntIdent {
        std::string MainClass;

        JavaUUID uuid;
    };

    enum LockerLanguage {
        CPP, JAVA /*, __max_LockerLanguage=0xffffffff */ };

    class ClntIdent {
    public:

        ClntIdent();

        ClntIdent(const ClntIdent &_value) {
            _pd__initialised = _value._pd__initialised;
            switch (_value._pd__d) {
                case CPP:
                    cpp_clnt(_value._pd_cpp_clnt);
                    break;

                case JAVA:
                    java_clnt(_value._pd_java_clnt);
                    break;

                default:
                    break;


            }
            _pd__d = _value._pd__d;

        }

        ~ClntIdent();

        ClntIdent &operator=(const ClntIdent &_value) {
            _pd__initialised = _value._pd__initialised;
            switch (_value._pd__d) {
                case CPP:
                    cpp_clnt(_value._pd_cpp_clnt);
                    break;

                case JAVA:
                    java_clnt(_value._pd_java_clnt);
                    break;

                default:
                    break;


            }
            _pd__d = _value._pd__d;

            return *this;
        }

        LockerLanguage _d() const { return _pd__d; }

        void _d(LockerLanguage _value);


        CppClntIdent cpp_clnt() const { return _pd_cpp_clnt; }

        void cpp_clnt(CppClntIdent _value) {
            _pd__initialised = 1;
            _pd__d = CPP;
            _pd__default = 0;
            _pd_cpp_clnt = _value;
        }

        const JavaClntIdent &java_clnt() const { return _pd_java_clnt; }

        JavaClntIdent &java_clnt() { return _pd_java_clnt; }

        void java_clnt(const JavaClntIdent &_value) {
            _pd__initialised = 1;
            _pd__d = JAVA;
            _pd__default = 0;
            _pd_java_clnt = _value;
        }

    private:
        LockerLanguage _pd__d;
        bool _pd__default;
        bool _pd__initialised;

        union {
            CppClntIdent _pd_cpp_clnt;


        };


        JavaClntIdent _pd_java_clnt;


    };

    enum AttrQuality {
        ATTR_VALID, ATTR_INVALID, ATTR_ALARM, ATTR_CHANGING, ATTR_WARNING /*, __max_AttrQuality=0xffffffff */ };
    typedef AttrQuality &AttrQuality_out;

    enum AttrWriteType {
        READ, READ_WITH_WRITE, WRITE, READ_WRITE, WT_UNKNOWN /*, __max_AttrWriteType=0xffffffff */ };
    typedef AttrWriteType &AttrWriteType_out;

    enum AttrDataFormat {
        SCALAR, SPECTRUM, IMAGE, FMT_UNKNOWN /*, __max_AttrDataFormat=0xffffffff */ };
    typedef AttrDataFormat &AttrDataFormat_out;

    enum DevSource {
        DEV, CACHE, CACHE_DEV /*, __max_DevSource=0xffffffff */ };
    typedef DevSource &DevSource_out;

    enum ErrSeverity {
        WARN, ERR, PANIC /*, __max_ErrSeverity=0xffffffff */ };
    typedef ErrSeverity &ErrSeverity_out;

    enum DevState {
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
        UNKNOWN /*, __max_DevState=0xffffffff */ };
    typedef DevState &DevState_out;

    enum DispLevel {
        OPERATOR, EXPERT, DL_UNKNOWN /*, __max_DispLevel=0xffffffff */ };
    typedef DispLevel &DispLevel_out;

    class DevVarStateArray_var;

    class DevVarStateArray {
    public:
        typedef DevVarStateArray_var _var_type;

        inline DevVarStateArray();

        inline DevVarStateArray(const DevVarStateArray &_s);

        inline DevVarStateArray(size_t _max);

        inline DevVarStateArray(size_t _max, size_t _len, DevState *_val, bool _rel = 0);


        inline DevVarStateArray &operator=(const DevVarStateArray &_s);
    };

    class DevVarStateArray_out;

    struct TimeVal {
        int32_t tv_sec;

        int32_t tv_usec;

        int32_t tv_nsec;
    };

    typedef TimeVal &TimeVal_out;

    struct DevCmdInfo {
        std::string cmd_name;

        int32_t cmd_tag;

        int32_t in_type;

        int32_t out_type;

        std::string in_type_desc;

        std::string out_type_desc;
    };

    struct DevCmdInfo_2 {
        std::string cmd_name;

        DispLevel level;

        int32_t cmd_tag;

        int32_t in_type;

        int32_t out_type;

        std::string in_type_desc;

        std::string out_type_desc;
    };

    class DevCmdInfoList_var;

    class DevCmdInfoList {
    public:
        typedef DevCmdInfoList_var _var_type;

        inline DevCmdInfoList();

        inline DevCmdInfoList(const DevCmdInfoList &_s);

        inline DevCmdInfoList(size_t _max);

        inline DevCmdInfoList(size_t _max, size_t _len, DevCmdInfo *_val, bool _rel = 0);


        inline DevCmdInfoList &operator=(const DevCmdInfoList &_s);
    };

    class DevCmdInfoList_out;

    class DevCmdInfoList_2_var;

    class DevCmdInfoList_2 {
    public:
        typedef DevCmdInfoList_2_var _var_type;

        inline DevCmdInfoList_2();

        inline DevCmdInfoList_2(const DevCmdInfoList_2 &_s);

        inline DevCmdInfoList_2(size_t _max);

        inline DevCmdInfoList_2(size_t _max, size_t _len, DevCmdInfo_2 *_val, bool _rel = 0);


        inline DevCmdInfoList_2 &operator=(const DevCmdInfoList_2 &_s);
    };

    class DevCmdInfoList_2_out;

    struct DevError {
        std::string reason;

        ErrSeverity severity;

        std::string desc;

        std::string origin;
    };

    class DevErrorList_var;

    class DevErrorList : DevVarArray<DevError> {
    public:
        typedef DevErrorList_var _var_type;

        inline DevErrorList();

        inline DevErrorList(const DevErrorList &_s);

        inline DevErrorList(size_t _max);

        inline DevErrorList(size_t _max, size_t _len, DevError *_val, bool _rel = 0);


        inline DevErrorList &operator=(const DevErrorList &_s);
    };

    class DevErrorList_out;

    struct NamedDevError {
        std::string name;

        int32_t index_in_call;

        DevErrorList err_list;
    };

    class NamedDevErrorList_var;

    class NamedDevErrorList {
    public:
        typedef NamedDevErrorList_var _var_type;

        inline NamedDevErrorList();

        inline NamedDevErrorList(const NamedDevErrorList &_s);

        inline NamedDevErrorList(size_t _max);

        inline NamedDevErrorList(size_t _max, size_t _len, NamedDevError *_val, bool _rel = 0);


        inline NamedDevErrorList &operator=(const NamedDevErrorList &_s);
    };

    class NamedDevErrorList_out;

    class DevFailed : public std::exception {
    public:

        DevErrorList errors;


        inline DevFailed();

        DevFailed(const DevFailed &);

        DevFailed(const DevErrorList i_errors);

        DevFailed &operator=(const DevFailed &);

        virtual ~DevFailed();

        virtual void _raise() const;

        static DevFailed *_downcast(std::exception *);

        static const DevFailed *_downcast(const std::exception *);

        static inline DevFailed *_narrow(std::exception *_e) {
            return _downcast(_e);
        }

        virtual std::exception *_NP_duplicate() const;

    private:
        virtual const char *_NP_typeId() const;

        virtual const char *_NP_repoId(int *) const;
    };

    class MultiDevFailed : public std::exception {
    public:

        NamedDevErrorList errors;


        inline MultiDevFailed();

        MultiDevFailed(const MultiDevFailed &);

        MultiDevFailed(const NamedDevErrorList i_errors);

        MultiDevFailed &operator=(const MultiDevFailed &);

        virtual ~MultiDevFailed();

        virtual void _raise() const;

        static MultiDevFailed *_downcast(std::exception *);

        static const MultiDevFailed *_downcast(const std::exception *);

        static inline MultiDevFailed *_narrow(std::exception *_e) {
            return _downcast(_e);
        }

        virtual std::exception *_NP_duplicate() const;

    private:
        virtual const char *_NP_typeId() const;

        virtual const char *_NP_repoId(int *) const;
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
    };

    struct AttributeValue {
        Tango::Any value;

        AttrQuality quality;

        TimeVal time;

        std::string name;

        int32_t dim_x;

        int32_t dim_y;
    };

    struct AttributeDim {
        int32_t dim_x;

        int32_t dim_y;
    };

    struct AttributeValue_3 {
        Tango::Any value;

        AttrQuality quality;

        TimeVal time;

        std::string name;

        AttributeDim r_dim;

        AttributeDim w_dim;

        DevErrorList err_list;
    };

    enum AttributeDataType {
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
        ATT_NO_DATA /*, __max_AttributeDataType=0xffffffff */ };
    typedef AttributeDataType &AttributeDataType_out;

    class AttrValUnion {
    public:
        AttrValUnion() : _pd__initialised(0) {

        }

        AttrValUnion(const AttrValUnion &_value) {
            _pd__initialised = _value._pd__initialised;
            switch (_value._pd__d) {
                case ATT_BOOL:
                    bool_att_value(_value._pd_bool_att_value);
                    break;

                case ATT_SHORT:
                    short_att_value(_value._pd_short_att_value);
                    break;

                case ATT_LONG:
                    long_att_value(_value._pd_long_att_value);
                    break;

                case ATT_LONG64:
                    long64_att_value(_value._pd_long64_att_value);
                    break;

                case ATT_FLOAT:
                    float_att_value(_value._pd_float_att_value);
                    break;

                case ATT_DOUBLE:
                    double_att_value(_value._pd_double_att_value);
                    break;

                case ATT_UCHAR:
                    uchar_att_value(_value._pd_uchar_att_value);
                    break;

                case ATT_USHORT:
                    ushort_att_value(_value._pd_ushort_att_value);
                    break;

                case ATT_ULONG:
                    ulong_att_value(_value._pd_ulong_att_value);
                    break;

                case ATT_ULONG64:
                    ulong64_att_value(_value._pd_ulong64_att_value);
                    break;

                case ATT_STRING:
                    string_att_value(_value._pd_string_att_value);
                    break;

                case ATT_STATE:
                    state_att_value(_value._pd_state_att_value);
                    break;

                case DEVICE_STATE:
                    dev_state_att(_value._pd_dev_state_att);
                    break;

                case ATT_ENCODED:
                    encoded_att_value(_value._pd_encoded_att_value);
                    break;

                case ATT_NO_DATA:
                    union_no_data(_value._pd_union_no_data);
                    break;

                default:
                    break;


            }
            _pd__d = _value._pd__d;

        }

        ~AttrValUnion() {}

        AttrValUnion &operator=(const AttrValUnion &_value) {
            _pd__initialised = _value._pd__initialised;
            switch (_value._pd__d) {
                case ATT_BOOL:
                    bool_att_value(_value._pd_bool_att_value);
                    break;

                case ATT_SHORT:
                    short_att_value(_value._pd_short_att_value);
                    break;

                case ATT_LONG:
                    long_att_value(_value._pd_long_att_value);
                    break;

                case ATT_LONG64:
                    long64_att_value(_value._pd_long64_att_value);
                    break;

                case ATT_FLOAT:
                    float_att_value(_value._pd_float_att_value);
                    break;

                case ATT_DOUBLE:
                    double_att_value(_value._pd_double_att_value);
                    break;

                case ATT_UCHAR:
                    uchar_att_value(_value._pd_uchar_att_value);
                    break;

                case ATT_USHORT:
                    ushort_att_value(_value._pd_ushort_att_value);
                    break;

                case ATT_ULONG:
                    ulong_att_value(_value._pd_ulong_att_value);
                    break;

                case ATT_ULONG64:
                    ulong64_att_value(_value._pd_ulong64_att_value);
                    break;

                case ATT_STRING:
                    string_att_value(_value._pd_string_att_value);
                    break;

                case ATT_STATE:
                    state_att_value(_value._pd_state_att_value);
                    break;

                case DEVICE_STATE:
                    dev_state_att(_value._pd_dev_state_att);
                    break;

                case ATT_ENCODED:
                    encoded_att_value(_value._pd_encoded_att_value);
                    break;

                case ATT_NO_DATA:
                    union_no_data(_value._pd_union_no_data);
                    break;

                default:
                    break;


            }
            _pd__d = _value._pd__d;

            return *this;
        }

        AttributeDataType _d() const { return _pd__d; }

        void _d(AttributeDataType _value) {
            if (_value == _pd__d) return; // no change
        }


        const DevVarBooleanArray &bool_att_value() const { return _pd_bool_att_value; }

        DevVarBooleanArray &bool_att_value() { return _pd_bool_att_value; }

        void bool_att_value(const DevVarBooleanArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_BOOL;
            _pd__default = 0;
            _pd_bool_att_value = _value;
        }

        const DevVarShortArray &short_att_value() const { return _pd_short_att_value; }

        DevVarShortArray &short_att_value() { return _pd_short_att_value; }

        void short_att_value(const DevVarShortArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_SHORT;
            _pd__default = 0;
            _pd_short_att_value = _value;
        }

        const DevVarLongArray &long_att_value() const { return _pd_long_att_value; }

        DevVarLongArray &long_att_value() { return _pd_long_att_value; }

        void long_att_value(const DevVarLongArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_LONG;
            _pd__default = 0;
            _pd_long_att_value = _value;
        }

        const DevVarLong64Array &long64_att_value() const { return _pd_long64_att_value; }

        DevVarLong64Array &long64_att_value() { return _pd_long64_att_value; }

        void long64_att_value(const DevVarLong64Array &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_LONG64;
            _pd__default = 0;
            _pd_long64_att_value = _value;
        }

        const DevVarFloatArray &float_att_value() const { return _pd_float_att_value; }

        DevVarFloatArray &float_att_value() { return _pd_float_att_value; }

        void float_att_value(const DevVarFloatArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_FLOAT;
            _pd__default = 0;
            _pd_float_att_value = _value;
        }

        const DevVarDoubleArray &double_att_value() const { return _pd_double_att_value; }

        DevVarDoubleArray &double_att_value() { return _pd_double_att_value; }

        void double_att_value(const DevVarDoubleArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_DOUBLE;
            _pd__default = 0;
            _pd_double_att_value = _value;
        }

        const DevVarCharArray &uchar_att_value() const { return _pd_uchar_att_value; }

        DevVarCharArray &uchar_att_value() { return _pd_uchar_att_value; }

        void uchar_att_value(const DevVarCharArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_UCHAR;
            _pd__default = 0;
            _pd_uchar_att_value = _value;
        }

        const DevVarUShortArray &ushort_att_value() const { return _pd_ushort_att_value; }

        DevVarUShortArray &ushort_att_value() { return _pd_ushort_att_value; }

        void ushort_att_value(const DevVarUShortArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_USHORT;
            _pd__default = 0;
            _pd_ushort_att_value = _value;
        }

        const DevVarULongArray &ulong_att_value() const { return _pd_ulong_att_value; }

        DevVarULongArray &ulong_att_value() { return _pd_ulong_att_value; }

        void ulong_att_value(const DevVarULongArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_ULONG;
            _pd__default = 0;
            _pd_ulong_att_value = _value;
        }

        const DevVarULong64Array &ulong64_att_value() const { return _pd_ulong64_att_value; }

        DevVarULong64Array &ulong64_att_value() { return _pd_ulong64_att_value; }

        void ulong64_att_value(const DevVarULong64Array &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_ULONG64;
            _pd__default = 0;
            _pd_ulong64_att_value = _value;
        }

        const DevVarStringArray &string_att_value() const { return _pd_string_att_value; }

        DevVarStringArray &string_att_value() { return _pd_string_att_value; }

        void string_att_value(const DevVarStringArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_STRING;
            _pd__default = 0;
            _pd_string_att_value = _value;
        }

        const DevVarStateArray &state_att_value() const { return _pd_state_att_value; }

        DevVarStateArray &state_att_value() { return _pd_state_att_value; }

        void state_att_value(const DevVarStateArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_STATE;
            _pd__default = 0;
            _pd_state_att_value = _value;
        }

        DevState dev_state_att() const { return _pd_dev_state_att; }

        void dev_state_att(DevState _value) {
            _pd__initialised = 1;
            _pd__d = DEVICE_STATE;
            _pd__default = 0;
            _pd_dev_state_att = _value;
        }

        const DevVarEncodedArray &encoded_att_value() const { return _pd_encoded_att_value; }

        DevVarEncodedArray &encoded_att_value() { return _pd_encoded_att_value; }

        void encoded_att_value(const DevVarEncodedArray &_value) {
            _pd__initialised = 1;
            _pd__d = ATT_ENCODED;
            _pd__default = 0;
            _pd_encoded_att_value = _value;
        }

        DevBoolean union_no_data() const { return _pd_union_no_data; }

        void union_no_data(DevBoolean _value) {
            _pd__initialised = 1;
            _pd__d = ATT_NO_DATA;
            _pd__default = 0;
            _pd_union_no_data = _value;
        }

    private:
        AttributeDataType _pd__d;
        bool _pd__default;
        bool _pd__initialised;

        union {
            DevState _pd_dev_state_att;

            DevBoolean _pd_union_no_data;


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

        void set_attr_mutex(omni_mutex *ptr) { mut_ptr = ptr; }

        void rel_attr_mutex() {
            if (mut_ptr != NULL) {
                mut_ptr->unlock();
                mut_ptr = NULL;
            }
        }

        omni_mutex *get_attr_mutex() { return mut_ptr; }

        omni_mutex *mut_ptr;


        AttrValUnion value;

        AttrQuality quality;

        AttrDataFormat data_format;

        TimeVal time;

        std::string name;

        AttributeDim r_dim;

        AttributeDim w_dim;

        DevErrorList err_list;
    };

    struct AttributeValue_5 {
//Added by Tango team
        virtual ~AttributeValue_5() { if (mut_ptr != NULL)mut_ptr->unlock(); }

        AttributeValue_5() { mut_ptr = NULL; }

        void set_attr_mutex(omni_mutex *ptr) { mut_ptr = ptr; }

        void rel_attr_mutex() {
            if (mut_ptr != NULL) {
                mut_ptr->unlock();
                mut_ptr = NULL;
            }
        }

        omni_mutex *get_attr_mutex() { return mut_ptr; }

        omni_mutex *mut_ptr;


        AttrValUnion value;

        AttrQuality quality;

        AttrDataFormat data_format;

        int32_t data_type;

        TimeVal time;

        std::string name;

        AttributeDim r_dim;

        AttributeDim w_dim;

        DevErrorList err_list;
    };

    struct ChangeEventProp {
        std::string rel_change;

        std::string abs_change;

        DevVarStringArray extensions;
    };

    struct PeriodicEventProp {
        std::string period;

        DevVarStringArray extensions;
    };

    struct ArchiveEventProp {
        std::string rel_change;

        std::string abs_change;

        std::string period;

        DevVarStringArray extensions;
    };

    struct EventProperties {
        ChangeEventProp ch_event;

        PeriodicEventProp per_event;

        ArchiveEventProp arch_event;
    };

    struct AttributeAlarm {
        std::string min_alarm;

        std::string max_alarm;

        std::string min_warning;

        std::string max_warning;

        std::string delta_t;

        std::string delta_val;

        DevVarStringArray extensions;
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
    };

    class AttributeConfigList_var;

    class AttributeConfigList {
    public:
        typedef AttributeConfigList_var _var_type;

        inline AttributeConfigList();

        inline AttributeConfigList(const AttributeConfigList &_s);

        inline AttributeConfigList(size_t _max);

        inline AttributeConfigList(size_t _max, size_t _len, AttributeConfig *_val, bool _rel = 0);


        inline AttributeConfigList &operator=(const AttributeConfigList &_s);
    };

    class AttributeConfigList_out;

    class AttributeConfigList_2_var;

    class AttributeConfigList_2 {
    public:
        typedef AttributeConfigList_2_var _var_type;

        inline AttributeConfigList_2();

        inline AttributeConfigList_2(const AttributeConfigList_2 &_s);

        inline AttributeConfigList_2(size_t _max);

        inline AttributeConfigList_2(size_t _max, size_t _len, AttributeConfig_2 *_val,
                                     bool _rel = 0);


        inline AttributeConfigList_2 &operator=(const AttributeConfigList_2 &_s);
    };

    class AttributeConfigList_2_out;

    class AttributeConfigList_3_var;

    class AttributeConfigList_3 {
    public:
        typedef AttributeConfigList_3_var _var_type;

        inline AttributeConfigList_3();

        inline AttributeConfigList_3(const AttributeConfigList_3 &_s);

        inline AttributeConfigList_3(size_t _max);

        inline AttributeConfigList_3(size_t _max, size_t _len, AttributeConfig_3 *_val,
                                     bool _rel = 0);


        inline AttributeConfigList_3 &operator=(const AttributeConfigList_3 &_s);
    };

    class AttributeConfigList_3_out;

    class AttributeConfigList_5_var;

    class AttributeConfigList_5 {
    public:
        typedef AttributeConfigList_5_var _var_type;

        inline AttributeConfigList_5();

        inline AttributeConfigList_5(const AttributeConfigList_5 &_s);

        inline AttributeConfigList_5(size_t _max);

        inline AttributeConfigList_5(size_t _max, size_t _len, AttributeConfig_5 *_val,
                                     bool _rel = 0);

        inline AttributeConfigList_5 &operator=(const AttributeConfigList_5 &_s);
    };

    class AttributeConfigList_5_out;

    class AttributeValueList_var;

    class AttributeValueList {
    public:
        typedef AttributeValueList_var _var_type;

        inline AttributeValueList();

        inline AttributeValueList(const AttributeValueList &_s);

        inline AttributeValueList(size_t _max);

        inline AttributeValueList(size_t _max, size_t _len, AttributeValue *_val, bool _rel = 0);


        inline AttributeValueList &operator=(const AttributeValueList &_s);
    };

    class AttributeValueList_out;

    class AttributeValueList_3_var;

    class AttributeValueList_3 {
    public:
        typedef AttributeValueList_3_var _var_type;

        inline AttributeValueList_3();

        inline AttributeValueList_3(const AttributeValueList_3 &_s);

        inline AttributeValueList_3(size_t _max);

        inline AttributeValueList_3(size_t _max, size_t _len, AttributeValue_3 *_val,
                                    bool _rel = 0);


        inline AttributeValueList_3 &operator=(const AttributeValueList_3 &_s);
    };

    class AttributeValueList_3_out;

    class AttributeValueList_4_var;

    class AttributeValueList_4 {
    public:
        typedef AttributeValueList_4_var _var_type;

        inline AttributeValueList_4();

        inline AttributeValueList_4(const AttributeValueList_4 &_s);

        inline AttributeValueList_4(size_t _max);

        inline AttributeValueList_4(size_t _max, size_t _len, AttributeValue_4 *_val,
                                    bool _rel = 0);

        inline AttributeValueList_4 &operator=(const AttributeValueList_4 &_s);
    };

    class AttributeValueList_4_out;

    class AttributeValueList_5_var;

    class AttributeValueList_5 {
    public:
        typedef AttributeValueList_5_var _var_type;

        inline AttributeValueList_5();

        inline AttributeValueList_5(const AttributeValueList_5 &_s);

        inline AttributeValueList_5(size_t _max);

        inline AttributeValueList_5(size_t _max, size_t _len, AttributeValue_5 *_val,
                                    bool _rel = 0);


        inline AttributeValueList_5 &operator=(const AttributeValueList_5 &_s);
    };

    class AttributeValueList_5_out;

    enum PipeWriteType {
        PIPE_READ, PIPE_READ_WRITE, PIPE_WT_UNKNOWN /*, __max_PipeWriteType=0xffffffff */ };
    typedef PipeWriteType &PipeWriteType_out;

    struct PipeConfig {
        std::string name;

        std::string description;

        std::string label;

        DispLevel level;

        PipeWriteType writable;

        DevVarStringArray extensions;
    };

    class PipeConfigList_var;

    class PipeConfigList {
    public:
        typedef PipeConfigList_var _var_type;

        inline PipeConfigList();

        inline PipeConfigList(const PipeConfigList &_s);

        inline PipeConfigList(size_t _max);

        inline PipeConfigList(size_t _max, size_t _len, PipeConfig *_val, bool _rel = 0);


        inline PipeConfigList &operator=(const PipeConfigList &_s);
    };

    class PipeConfigList_out;

    struct DevPipeDataElt;

    class DevVarPipeDataEltArray_var;

    class DevVarPipeDataEltArray {
    public:
        typedef DevVarPipeDataEltArray_var _var_type;

        inline DevVarPipeDataEltArray();

        DevVarPipeDataEltArray(const DevVarPipeDataEltArray &_s);

        DevVarPipeDataEltArray &operator=(const DevVarPipeDataEltArray &_s);

        inline DevVarPipeDataEltArray(size_t _max);

        inline DevVarPipeDataEltArray(size_t _max, size_t _len, DevPipeDataElt *_val,
                                      bool _rel = 0);

        virtual ~DevVarPipeDataEltArray();

        DevPipeDataElt &operator[](size_t _index);

        const DevPipeDataElt &operator[](size_t _index) const;

        static DevPipeDataElt *allocbuf(size_t _nelems);

        static void freebuf(DevPipeDataElt *_b);

    protected:
        void NP_copybuffer(size_t _newmax);

        void NP_freebuf();
    };

    class DevVarPipeDataEltArray_out;

    struct DevPipeDataElt {
        std::string name;

        AttrValUnion value;

        DevVarPipeDataEltArray inner_blob;

        std::string inner_blob_name;
    };

    struct DevPipeBlob {
        std::string name;

        DevVarPipeDataEltArray blob_data;
    };

    struct DevPipeData {

//Added by Tango team
        virtual ~DevPipeData() { if (mut_ptr != NULL)mut_ptr->unlock(); }

        DevPipeData() { mut_ptr = NULL; }

        void set_pipe_mutex(omni_mutex *ptr) { mut_ptr = ptr; }

        void rel_pipe_mutex() {
            if (mut_ptr != NULL) {
                mut_ptr->unlock();
                mut_ptr = NULL;
            }
        }

        omni_mutex *mut_ptr;


        std::string name;

        TimeVal time;

        DevPipeBlob data_blob;
    };

    struct AttDataReady {
        std::string name;

        int32_t data_type;

        int32_t ctr;
    };

    struct DevIntrChange {
        bool dev_started;

        DevCmdInfoList_2 cmds;

        AttributeConfigList_5 atts;
    };

    struct DevInfo {
        std::string dev_class;

        std::string server_id;

        std::string server_host;

        int32_t server_version;

        std::string doc_url;
    };

    struct DevInfo_3 {
        std::string dev_class;

        std::string server_id;

        std::string server_host;

        int32_t server_version;

        std::string doc_url;

        std::string dev_type;
    };

    struct DevCmdHistory {
        TimeVal time;

        bool cmd_failed;

        Tango::Any value;

        DevErrorList errors;
    };

    class DevCmdHistoryList_var;

    class DevCmdHistoryList {
    public:
        typedef DevCmdHistoryList_var _var_type;

        inline DevCmdHistoryList();

        inline DevCmdHistoryList(const DevCmdHistoryList &_s);

        inline DevCmdHistoryList(size_t _max);

        inline DevCmdHistoryList(size_t _max, size_t _len, DevCmdHistory *_val, bool _rel = 0);


        inline DevCmdHistoryList &operator=(const DevCmdHistoryList &_s);
    };

    class DevCmdHistoryList_out;

    struct DevAttrHistory {
        bool attr_failed;

        AttributeValue value;

        DevErrorList errors;
    };

    struct DevAttrHistory_3 {
        bool attr_failed;

        AttributeValue_3 value;
    };

    struct EltInArray {
        int32_t start;

        int32_t nb_elt;
    };

    class EltInArrayList_var;

    class EltInArrayList {
    public:
        typedef EltInArrayList_var _var_type;

        inline EltInArrayList();

        inline EltInArrayList(const EltInArrayList &_s);

        inline EltInArrayList(size_t _max);

        inline EltInArrayList(size_t _max, size_t _len, EltInArray *_val, bool _rel = 0);


        inline EltInArrayList &operator=(const EltInArrayList &_s);
    };

    class EltInArrayList_out;

    class TimeValList_var;

    class TimeValList {
    public:
        typedef TimeValList_var _var_type;

        inline TimeValList();

        inline TimeValList(const TimeValList &_s);

        inline TimeValList(size_t _max);

        inline TimeValList(size_t _max, size_t _len, TimeVal *_val, bool _rel = 0);


        inline TimeValList &operator=(const TimeValList &_s);
    };

    class TimeValList_out;

    class AttrQualityList_var;

    class AttrQualityList {
    public:
        typedef AttrQualityList_var _var_type;

        inline AttrQualityList() {}

        inline AttrQualityList(const AttrQualityList &_s);

        inline AttrQualityList(size_t _max);

        inline AttrQualityList(size_t _max, size_t _len, AttrQuality *_val, bool _rel = 0);


        inline AttrQualityList &operator=(const AttrQualityList &_s);
    };

    class AttrQualityList_out;

    class AttributeDimList_var;

    class AttributeDimList {
    public:
        typedef AttributeDimList_var _var_type;

        inline AttributeDimList() {}

        inline AttributeDimList(const AttributeDimList &_s);

        inline AttributeDimList(size_t _max);

        inline AttributeDimList(size_t _max, size_t _len, AttributeDim *_val, bool _rel = 0);


        inline AttributeDimList &operator=(const AttributeDimList &_s);
    };

    class AttributeDimList_out;

    class DevErrorListList {
    public:
        typedef DevErrorListList_var _var_type;

        inline DevErrorListList() {}

        inline DevErrorListList(const DevErrorListList &_s);

        inline DevErrorListList(size_t _max);

        inline DevErrorListList(size_t _max, size_t _len, DevErrorList *_val, bool _rel = 0);


        inline DevErrorListList &operator=(const DevErrorListList &_s);
    };

    class DevErrorListList_out;

    struct DevAttrHistory_4 {
        std::string name;

        TimeValList dates;

        Tango::Any value;

        AttrQualityList quals;

        EltInArrayList quals_array;

        AttributeDimList r_dims;

        EltInArrayList r_dims_array;

        AttributeDimList w_dims;

        EltInArrayList w_dims_array;

        DevErrorListList errors;

        EltInArrayList errors_array;
    };

    struct DevAttrHistory_5 {
        std::string name;

        AttrDataFormat data_format;

        int32_t data_type;

        TimeValList dates;

        Tango::Any value;

        AttrQualityList quals;

        EltInArrayList quals_array;

        AttributeDimList r_dims;

        EltInArrayList r_dims_array;

        AttributeDimList w_dims;

        EltInArrayList w_dims_array;

        DevErrorListList errors;

        EltInArrayList errors_array;
    };

    struct DevCmdHistory_4 {
        TimeValList dates;

        Tango::Any value;

        AttributeDimList dims;

        EltInArrayList dims_array;

        DevErrorListList errors;

        EltInArrayList errors_array;

        int32_t cmd_type;
    };

    class DevAttrHistoryList_var;

    class DevAttrHistoryList {
    public:
        typedef DevAttrHistoryList_var _var_type;

        inline DevAttrHistoryList() {}

        inline DevAttrHistoryList(const DevAttrHistoryList &_s);

        inline DevAttrHistoryList(size_t _max);

        inline DevAttrHistoryList(size_t _max, size_t _len, DevAttrHistory *_val, bool _rel = 0);


        inline DevAttrHistoryList &operator=(const DevAttrHistoryList &_s);
    };

    class DevAttrHistoryList_out;

    class DevAttrHistoryList_3_var;

    class DevAttrHistoryList_3 {
    public:
        typedef DevAttrHistoryList_3_var _var_type;

        inline DevAttrHistoryList_3() {}

        inline DevAttrHistoryList_3(const DevAttrHistoryList_3 &_s);

        inline DevAttrHistoryList_3(size_t _max);

        inline DevAttrHistoryList_3(size_t _max, size_t _len, DevAttrHistory_3 *_val,
                                    bool _rel = 0);


        inline DevAttrHistoryList_3 &operator=(const DevAttrHistoryList_3 &_s);
    };

    class DevAttrHistoryList_3_out;

    struct ZmqCallInfo {
        int32_t version;

        uint32_t ctr;

        std::string method_name;

        DevVarCharArray oid;

        bool call_is_except;
    };


// interface Device
    class Device {
    public:
        virtual ~_impl_Device();

        virtual Tango::Any *command_inout(const char *command, const Tango::Any &argin) = 0;

        virtual AttributeConfigList *get_attribute_config(const ::Tango::DevVarStringArray &names) = 0;

        virtual void set_attribute_config(const ::Tango::AttributeConfigList &new_conf) = 0;

        virtual AttributeValueList *read_attributes(const ::Tango::DevVarStringArray &names) = 0;

        virtual void write_attributes(const ::Tango::AttributeValueList &values) = 0;

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

// interface Device_2
    class Device_2 : public Device {
    public:
        // Declarations for this interface type.
        virtual ~_impl_Device_2();

        virtual Tango::Any *
        command_inout_2(const char *command, const Tango::Any &argin, ::Tango::DevSource source) = 0;

        virtual AttributeValueList *
        read_attributes_2(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source) = 0;

        virtual AttributeConfigList_2 *get_attribute_config_2(const ::Tango::DevVarStringArray &names) = 0;

        virtual DevCmdInfoList_2 *command_list_query_2() = 0;

        virtual DevCmdInfo_2 *command_query_2(const char *command) = 0;

        virtual DevCmdHistoryList *command_inout_history_2(const char *command, int32_t n) = 0;

        virtual DevAttrHistoryList *read_attribute_history_2(const char *name, int32_t n) = 0;
    };


// interface Device_3
    class Device_3 : public Device_2 {
    public:
        // Declarations for this interface type.
        virtual ~_impl_Device_3();

        virtual AttributeValueList_3 *
        read_attributes_3(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source) = 0;

        virtual void write_attributes_3(const ::Tango::AttributeValueList &values) = 0;

        virtual DevAttrHistoryList_3 *read_attribute_history_3(const char *name, int32_t n) = 0;

        virtual DevInfo_3 *info_3() = 0;

        virtual AttributeConfigList_3 *get_attribute_config_3(const ::Tango::DevVarStringArray &names) = 0;

        virtual void set_attribute_config_3(const ::Tango::AttributeConfigList_3 &new_conf) = 0;

    public:  // Really protected, workaround for xlC
        virtual bool _dispatch(omniCallHandle &);

    private:
        virtual void *_ptrToInterface(const char *);

        virtual const char *_mostDerivedRepoId();

    };


// interface Device_4
    class Device_4 : public Device_3 {
    public:
        // Declarations for this interface type.
        virtual ~_impl_Device_4();

        virtual DevAttrHistory_4 *read_attribute_history_4(const char *name, int32_t n) = 0;

        virtual DevCmdHistory_4 *command_inout_history_4(const char *command, int32_t n) = 0;

        virtual Tango::Any *command_inout_4(const char *command, const Tango::Any &argin, ::Tango::DevSource source,
                                            const ::Tango::ClntIdent &cl_ident) = 0;

        virtual AttributeValueList_4 *
        read_attributes_4(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                          const ::Tango::ClntIdent &cl_ident) = 0;

        virtual void
        write_attributes_4(const ::Tango::AttributeValueList_4 &values, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual void
        set_attribute_config_4(const ::Tango::AttributeConfigList_3 &new_conf, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual AttributeValueList_4 *
        write_read_attributes_4(const ::Tango::AttributeValueList_4 &values, const ::Tango::ClntIdent &cl_ident) = 0;
    };


// interface Device_5
    class Device_5 : public Device_4 {
    public:
        // Declarations for this interface type.

        virtual ~_impl_Device_5();

        virtual AttributeConfigList_5 *get_attribute_config_5(const ::Tango::DevVarStringArray &names) = 0;

        virtual void
        set_attribute_config_5(const ::Tango::AttributeConfigList_5 &new_conf, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual AttributeValueList_5 *
        read_attributes_5(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                          const ::Tango::ClntIdent &cl_ident) = 0;

        virtual AttributeValueList_5 *
        write_read_attributes_5(const ::Tango::AttributeValueList_4 &values, const ::Tango::DevVarStringArray &r_names,
                                const ::Tango::ClntIdent &cl_ident) = 0;

        virtual DevAttrHistory_5 *read_attribute_history_5(const char *name, int32_t n) = 0;

        virtual PipeConfigList *get_pipe_config_5(const ::Tango::DevVarStringArray &names) = 0;

        virtual void set_pipe_config_5(const ::Tango::PipeConfigList &new_conf, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual DevPipeData *read_pipe_5(const char *name, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual void write_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident) = 0;

        virtual DevPipeData *
        write_read_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident) = 0;

    };

}//Tango


namespace POA_Tango {

    class Device :
            public virtual Tango::Device {
    public:
        virtual ~Device();
    };

    class Device_2 :
            public virtual Tango::Device_2,
            public virtual Device {
    public:
        virtual ~Device_2();
    };

    class Device_3 :
            public virtual Tango::Device_3,
            public virtual Device_2 {
    public:
        virtual ~Device_3();
    };

    class Device_4 :
            public virtual Tango::Device_4,
            public virtual Device_3 {
    public:
        virtual ~Device_4();
    };

    class Device_5 :
            public virtual Tango::Device_5,
            public virtual Device_4 {
    public:
        virtual ~Device_5();
    };

}//POA_Tango

