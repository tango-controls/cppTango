//
// Created by ingvord on 10/10/16.
//

#pragma once

#include <cstdint>

namespace tango {
    namespace backend {
        //TODO clean out CORBA stuff
        class DevVarBooleanArray_var;

        struct DevVarBooleanArray {
            DevVarBooleanArray() = default;

            DevVarBooleanArray(const DevVarBooleanArray&) = default;

            DevVarBooleanArray &operator=(const DevVarBooleanArray&) = default;

            DevVarBooleanArray(DevVarBooleanArray&&) = default;

            DevVarBooleanArray& operator= (DevVarBooleanArray&&) = default;

            ~DevVarBooleanArray() = default;
            
            // non default
            DevVarBooleanArray(uint32_t _max);

            DevVarBooleanArray(uint32_t _max, uint32_t _len, bool *_val,
                               bool _rel = 0);
        };

        struct DevVarDoubleArray {
            DevVarDoubleArray() = default;

            DevVarDoubleArray(const DevVarDoubleArray &) = default;

            DevVarDoubleArray &operator=(const DevVarDoubleArray &_s)=default;

            DevVarDoubleArray(DevVarDoubleArray &&) = default;

            DevVarDoubleArray &operator=(DevVarDoubleArray &&)=default;
            
            ~DevVarDoubleArray()=default;
            //non default
            DevVarDoubleArray(uint32_t _max);

            DevVarDoubleArray(uint32_t _max, uint32_t _len, float64_t *_val,
                                     bool _rel = 0);


            
        };

        class DevVarFloatArray {
        public:
            DevVarFloatArray()=default;

            DevVarFloatArray(const DevVarFloatArray &_s)=default;
            
            DevVarFloatArray &operator=(const DevVarFloatArray &_s)=default;

            DevVarFloatArray(DevVarFloatArray &&)=default;

            DevVarFloatArray &operator=(DevVarFloatArray &&)=default;
            
            ~DevVarFloatArray()=default;
            
            //non default
             DevVarFloatArray(uint32_t _max);

             DevVarFloatArray(uint32_t _max, uint32_t _len, float32_t *_val, bool _rel = 0);


             
        };

        struct DevVarShortArray {
            DevVarShortArray()=default;

            DevVarShortArray(const DevVarShortArray &_s)=default;

            DevVarShortArray &operator=(const DevVarShortArray &_s)=default;

            DevVarShortArray(const DevVarShortArray &&)=default

            DevVarShortArray &operator=(const DevVarShortArray &&)=default;
            
            ~DevVarShortArray()=default;

            //non default
            DevVarShortArray(uint32_t _max);

            DevVarShortArray(uint32_t _max, uint32_t _len, int16_t *_val, bool _rel = 0);
        };

        struct DevVarLongArray {
             DevVarLongArray()=default;

             DevVarLongArray(const DevVarLongArray &_s)=default;

            DevVarLongArray &operator=(const DevVarLongArray &_s)=default;

            DevVarLongArray(const DevVarLongArray &&)=default;

            DevVarLongArray &operator=(const DevVarLongArray &&)=default;

            ~DevVarLongArray()=default;
            
            //non default
             DevVarLongArray(uint32_t _max);

             DevVarLongArray(uint32_t _max, uint32_t _len, int32_t *_val, bool _rel = 0);
        };

        struct DevVarLong64Array {
            DevVarLong64Array()=default;

            DevVarLong64Array(const DevVarLong64Array &_s)=default;

            DevVarLong64Array &operator=(const DevVarLong64Array &_s) =default;

            DevVarLong64Array(const DevVarLong64Array &&)=default;

            DevVarLong64Array &operator=(const DevVarLong64Array &&) =default;

            ~DevVarLong64Array()=default;

            //non default
            DevVarLong64Array(uint32_t _max);

            DevVarLong64Array(uint32_t _max, uint32_t _len, int64_t *_val,
                                     bool _rel = 0);
        };

//TODO!!!
        class DevVarCharArray : public _CORBA_Unbounded_Sequence_Octet {
        public:
            typedef DevVarCharArray_var _var_type;

            DevVarCharArray() {}

            DevVarCharArray(const DevVarCharArray &_s)
                    : _CORBA_Unbounded_Sequence_Octet(_s) {}

            DevVarCharArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_Octet(_max) {}

            DevVarCharArray(uint32_t _max, uint32_t _len, ::CORBA::Octet *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_Octet(_max, _len, _val, _rel) {}


            DevVarCharArray &operator=(const DevVarCharArray &_s) {
                _CORBA_Unbounded_Sequence_Octet::operator=(_s);
                return *this;
            }
        };

        class DevVarCharArray_out;

        class DevVarCharArray_var {
        public:
            DevVarCharArray_var() : _pd_seq(0) {}

            DevVarCharArray_var(DevVarCharArray *_s) : _pd_seq(_s) {}

            DevVarCharArray_var(const DevVarCharArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarCharArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarCharArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarCharArray_var &operator=(DevVarCharArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarCharArray_var &operator=(const DevVarCharArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarCharArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            ::CORBA::Octet &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarCharArray *operator->() { return _pd_seq; }

            const DevVarCharArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarCharArray& () const { return *_pd_seq; }
#else

            operator const DevVarCharArray &() const { return *_pd_seq; }

            operator DevVarCharArray &() { return *_pd_seq; }

#endif

            const DevVarCharArray &in() const { return *_pd_seq; }

            DevVarCharArray &inout() { return *_pd_seq; }

            DevVarCharArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarCharArray *_retn() {
                DevVarCharArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarCharArray_out;

        private:
            DevVarCharArray *_pd_seq;
        };

        class DevVarCharArray_out {
        public:
            DevVarCharArray_out(DevVarCharArray *&_s) : _data(_s) { _data = 0; }

            DevVarCharArray_out(DevVarCharArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarCharArray *) 0; }

            DevVarCharArray_out(const DevVarCharArray_out &_s) : _data(_s._data) {}

            DevVarCharArray_out &operator=(const DevVarCharArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarCharArray_out &operator=(DevVarCharArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarCharArray *&() { return _data; }

            DevVarCharArray *&ptr() { return _data; }

            DevVarCharArray *operator->() { return _data; }

            ::CORBA::Octet &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarCharArray *&_data;

        private:
            DevVarCharArray_out();

            DevVarCharArray_out &operator=(const DevVarCharArray_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarStringArray;

        class DevVarStringArray_var;

        class DevVarStringArray : public _CORBA_Unbounded_Sequence_String {
        public:
            typedef DevVarStringArray_var _var_type;

            DevVarStringArray() {}

            DevVarStringArray(const DevVarStringArray &_s)
                    : _CORBA_Unbounded_Sequence_String(_s) {}

            DevVarStringArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_String(_max) {}

            DevVarStringArray(uint32_t _max, uint32_t _len, char **_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_String(_max, _len, _val, _rel) {}


            DevVarStringArray &operator=(const DevVarStringArray &_s) {
                _CORBA_Unbounded_Sequence_String::operator=(_s);
                return *this;
            }
        };

        class DevVarStringArray_out;

        class DevVarStringArray_var {
        public:
            DevVarStringArray_var() : _pd_seq(0) {}

            DevVarStringArray_var(DevVarStringArray *_s) : _pd_seq(_s) {}

            DevVarStringArray_var(const DevVarStringArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarStringArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarStringArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarStringArray_var &operator=(DevVarStringArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarStringArray_var &operator=(const DevVarStringArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarStringArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            _CORBA_String_element operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarStringArray *operator->() { return _pd_seq; }

            const DevVarStringArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarStringArray& () const { return *_pd_seq; }
#else

            operator const DevVarStringArray &() const { return *_pd_seq; }

            operator DevVarStringArray &() { return *_pd_seq; }

#endif

            const DevVarStringArray &in() const { return *_pd_seq; }

            DevVarStringArray &inout() { return *_pd_seq; }

            DevVarStringArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarStringArray *_retn() {
                DevVarStringArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarStringArray_out;

        private:
            DevVarStringArray *_pd_seq;
        };

        class DevVarStringArray_out {
        public:
            DevVarStringArray_out(DevVarStringArray *&_s) : _data(_s) { _data = 0; }

            DevVarStringArray_out(DevVarStringArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarStringArray *) 0; }

            DevVarStringArray_out(const DevVarStringArray_out &_s) : _data(_s._data) {}

            DevVarStringArray_out &operator=(const DevVarStringArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarStringArray_out &operator=(DevVarStringArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarStringArray *&() { return _data; }

            DevVarStringArray *&ptr() { return _data; }

            DevVarStringArray *operator->() { return _data; }

            _CORBA_String_element operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarStringArray *&_data;

        private:
            DevVarStringArray_out();

            DevVarStringArray_out &operator=(const DevVarStringArray_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarUShortArray;

        class DevVarUShortArray_var;

        class DevVarUShortArray : public _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::UShort, 2, 2> {
        public:
            typedef DevVarUShortArray_var _var_type;

            DevVarUShortArray() {}

            DevVarUShortArray(const DevVarUShortArray &_s)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::UShort, 2, 2>(_s) {}

            DevVarUShortArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::UShort, 2, 2>(_max) {}

            DevVarUShortArray(uint32_t _max, uint32_t _len, ::CORBA::UShort *_val,
                                     bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::UShort, 2, 2>(_max, _len, _val, _rel) {}


            DevVarUShortArray &operator=(const DevVarUShortArray &_s) {
                _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::UShort, 2, 2>::operator=(_s);
                return *this;
            }
        };

        class DevVarUShortArray_out;

        class DevVarUShortArray_var {
        public:
            DevVarUShortArray_var() : _pd_seq(0) {}

            DevVarUShortArray_var(DevVarUShortArray *_s) : _pd_seq(_s) {}

            DevVarUShortArray_var(const DevVarUShortArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarUShortArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarUShortArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarUShortArray_var &operator=(DevVarUShortArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarUShortArray_var &operator=(const DevVarUShortArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarUShortArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            ::CORBA::UShort &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarUShortArray *operator->() { return _pd_seq; }

            const DevVarUShortArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarUShortArray& () const { return *_pd_seq; }
#else

            operator const DevVarUShortArray &() const { return *_pd_seq; }

            operator DevVarUShortArray &() { return *_pd_seq; }

#endif

            const DevVarUShortArray &in() const { return *_pd_seq; }

            DevVarUShortArray &inout() { return *_pd_seq; }

            DevVarUShortArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarUShortArray *_retn() {
                DevVarUShortArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarUShortArray_out;

        private:
            DevVarUShortArray *_pd_seq;
        };

        class DevVarUShortArray_out {
        public:
            DevVarUShortArray_out(DevVarUShortArray *&_s) : _data(_s) { _data = 0; }

            DevVarUShortArray_out(DevVarUShortArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarUShortArray *) 0; }

            DevVarUShortArray_out(const DevVarUShortArray_out &_s) : _data(_s._data) {}

            DevVarUShortArray_out &operator=(const DevVarUShortArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarUShortArray_out &operator=(DevVarUShortArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarUShortArray *&() { return _data; }

            DevVarUShortArray *&ptr() { return _data; }

            DevVarUShortArray *operator->() { return _data; }

            ::CORBA::UShort &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarUShortArray *&_data;

        private:
            DevVarUShortArray_out();

            DevVarUShortArray_out &operator=(const DevVarUShortArray_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarULongArray;

        class DevVarULongArray_var;

        class DevVarULongArray : public _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULong, 4, 4> {
        public:
            typedef DevVarULongArray_var _var_type;

            DevVarULongArray() {}

            DevVarULongArray(const DevVarULongArray &_s)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULong, 4, 4>(_s) {}

            DevVarULongArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULong, 4, 4>(_max) {}

            DevVarULongArray(uint32_t _max, uint32_t _len, ::CORBA::ULong *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULong, 4, 4>(_max, _len, _val, _rel) {}


            DevVarULongArray &operator=(const DevVarULongArray &_s) {
                _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULong, 4, 4>::operator=(_s);
                return *this;
            }
        };

        class DevVarULongArray_out;

        class DevVarULongArray_var {
        public:
            DevVarULongArray_var() : _pd_seq(0) {}

            DevVarULongArray_var(DevVarULongArray *_s) : _pd_seq(_s) {}

            DevVarULongArray_var(const DevVarULongArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarULongArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarULongArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarULongArray_var &operator=(DevVarULongArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarULongArray_var &operator=(const DevVarULongArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarULongArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            ::CORBA::ULong &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarULongArray *operator->() { return _pd_seq; }

            const DevVarULongArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarULongArray& () const { return *_pd_seq; }
#else

            operator const DevVarULongArray &() const { return *_pd_seq; }

            operator DevVarULongArray &() { return *_pd_seq; }

#endif

            const DevVarULongArray &in() const { return *_pd_seq; }

            DevVarULongArray &inout() { return *_pd_seq; }

            DevVarULongArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarULongArray *_retn() {
                DevVarULongArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarULongArray_out;

        private:
            DevVarULongArray *_pd_seq;
        };

        class DevVarULongArray_out {
        public:
            DevVarULongArray_out(DevVarULongArray *&_s) : _data(_s) { _data = 0; }

            DevVarULongArray_out(DevVarULongArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarULongArray *) 0; }

            DevVarULongArray_out(const DevVarULongArray_out &_s) : _data(_s._data) {}

            DevVarULongArray_out &operator=(const DevVarULongArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarULongArray_out &operator=(DevVarULongArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarULongArray *&() { return _data; }

            DevVarULongArray *&ptr() { return _data; }

            DevVarULongArray *operator->() { return _data; }

            ::CORBA::ULong &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarULongArray *&_data;

        private:
            DevVarULongArray_out();

            DevVarULongArray_out &operator=(const DevVarULongArray_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarULong64Array;

        class DevVarULong64Array_var;

        class DevVarULong64Array : public _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULongLong, 8, 8> {
        public:
            typedef DevVarULong64Array_var _var_type;

            DevVarULong64Array() {}

            DevVarULong64Array(const DevVarULong64Array &_s)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULongLong, 8, 8>(_s) {}

            DevVarULong64Array(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULongLong, 8, 8>(_max) {}

            DevVarULong64Array(uint32_t _max, uint32_t _len, ::CORBA::ULongLong *_val,
                                      bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULongLong, 8, 8>(_max, _len, _val, _rel) {}


            DevVarULong64Array &operator=(const DevVarULong64Array &_s) {
                _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::ULongLong, 8, 8>::operator=(_s);
                return *this;
            }
        };

        class DevVarULong64Array_out;

        class DevVarULong64Array_var {
        public:
            DevVarULong64Array_var() : _pd_seq(0) {}

            DevVarULong64Array_var(DevVarULong64Array *_s) : _pd_seq(_s) {}

            DevVarULong64Array_var(const DevVarULong64Array_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarULong64Array(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarULong64Array_var() { if (_pd_seq) delete _pd_seq; }

            DevVarULong64Array_var &operator=(DevVarULong64Array *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarULong64Array_var &operator=(const DevVarULong64Array_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarULong64Array;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            ::CORBA::ULongLong &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarULong64Array *operator->() { return _pd_seq; }

            const DevVarULong64Array *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarULong64Array& () const { return *_pd_seq; }
#else

            operator const DevVarULong64Array &() const { return *_pd_seq; }

            operator DevVarULong64Array &() { return *_pd_seq; }

#endif

            const DevVarULong64Array &in() const { return *_pd_seq; }

            DevVarULong64Array &inout() { return *_pd_seq; }

            DevVarULong64Array *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarULong64Array *_retn() {
                DevVarULong64Array *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarULong64Array_out;

        private:
            DevVarULong64Array *_pd_seq;
        };

        class DevVarULong64Array_out {
        public:
            DevVarULong64Array_out(DevVarULong64Array *&_s) : _data(_s) { _data = 0; }

            DevVarULong64Array_out(DevVarULong64Array_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarULong64Array *) 0; }

            DevVarULong64Array_out(const DevVarULong64Array_out &_s) : _data(_s._data) {}

            DevVarULong64Array_out &operator=(const DevVarULong64Array_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarULong64Array_out &operator=(DevVarULong64Array *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarULong64Array *&() { return _data; }

            DevVarULong64Array *&ptr() { return _data; }

            DevVarULong64Array *operator->() { return _data; }

            ::CORBA::ULongLong &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarULong64Array *&_data;

        private:
            DevVarULong64Array_out();

            DevVarULong64Array_out &operator=(const DevVarULong64Array_var &);
        };

        struct DevVarLongStringArray {
            typedef _CORBA_ConstrType_Variable_Var <DevVarLongStringArray> _var_type;


            DevVarLongArray lvalue;

            DevVarStringArray svalue;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevVarLongStringArray::_var_type DevVarLongStringArray_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevVarLongStringArray, DevVarLongStringArray_var> DevVarLongStringArray_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarLongStringArray;

        struct DevVarDoubleStringArray {
            typedef _CORBA_ConstrType_Variable_Var <DevVarDoubleStringArray> _var_type;


            DevVarDoubleArray dvalue;

            DevVarStringArray svalue;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevVarDoubleStringArray::_var_type DevVarDoubleStringArray_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevVarDoubleStringArray, DevVarDoubleStringArray_var> DevVarDoubleStringArray_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarDoubleStringArray;

        struct DevEncoded {
            typedef _CORBA_ConstrType_Variable_Var <DevEncoded> _var_type;


            ::CORBA::String_member encoded_format;

            DevVarCharArray encoded_data;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevEncoded::_var_type DevEncoded_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevEncoded, DevEncoded_var> DevEncoded_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevEncoded;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarEncodedArray;

        class DevVarEncodedArray_var;

        class DevVarEncodedArray : public _CORBA_Unbounded_Sequence<DevEncoded> {
        public:
            typedef DevVarEncodedArray_var _var_type;

            DevVarEncodedArray() {}

            DevVarEncodedArray(const DevVarEncodedArray &_s)
                    : _CORBA_Unbounded_Sequence<DevEncoded>(_s) {}

            DevVarEncodedArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevEncoded>(_max) {}

            DevVarEncodedArray(uint32_t _max, uint32_t _len, DevEncoded *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevEncoded>(_max, _len, _val, _rel) {}


            DevVarEncodedArray &operator=(const DevVarEncodedArray &_s) {
                _CORBA_Unbounded_Sequence<DevEncoded>::operator=(_s);
                return *this;
            }
        };

        class DevVarEncodedArray_out;

        class DevVarEncodedArray_var {
        public:
            DevVarEncodedArray_var() : _pd_seq(0) {}

            DevVarEncodedArray_var(DevVarEncodedArray *_s) : _pd_seq(_s) {}

            DevVarEncodedArray_var(const DevVarEncodedArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarEncodedArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarEncodedArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarEncodedArray_var &operator=(DevVarEncodedArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarEncodedArray_var &operator=(const DevVarEncodedArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarEncodedArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevEncoded &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarEncodedArray *operator->() { return _pd_seq; }

            const DevVarEncodedArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarEncodedArray& () const { return *_pd_seq; }
#else

            operator const DevVarEncodedArray &() const { return *_pd_seq; }

            operator DevVarEncodedArray &() { return *_pd_seq; }

#endif

            const DevVarEncodedArray &in() const { return *_pd_seq; }

            DevVarEncodedArray &inout() { return *_pd_seq; }

            DevVarEncodedArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarEncodedArray *_retn() {
                DevVarEncodedArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarEncodedArray_out;

        private:
            DevVarEncodedArray *_pd_seq;
        };

        class DevVarEncodedArray_out {
        public:
            DevVarEncodedArray_out(DevVarEncodedArray *&_s) : _data(_s) { _data = 0; }

            DevVarEncodedArray_out(DevVarEncodedArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarEncodedArray *) 0; }

            DevVarEncodedArray_out(const DevVarEncodedArray_out &_s) : _data(_s._data) {}

            DevVarEncodedArray_out &operator=(const DevVarEncodedArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarEncodedArray_out &operator=(DevVarEncodedArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarEncodedArray *&() { return _data; }

            DevVarEncodedArray *&ptr() { return _data; }

            DevVarEncodedArray *operator->() { return _data; }

            DevEncoded &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarEncodedArray *&_data;

        private:
            DevVarEncodedArray_out();

            DevVarEncodedArray_out &operator=(const DevVarEncodedArray_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_JavaUUID;

        typedef ::CORBA::ULongLong JavaUUID[2];
        typedef ::CORBA::ULongLong JavaUUID_slice;

        _CORBA_MODULE_INLINE JavaUUID_slice
        *

        JavaUUID_alloc() {
            return new JavaUUID_slice[2];
        }

        _CORBA_MODULE_INLINE JavaUUID_slice
        *

        JavaUUID_dup(const JavaUUID_slice *_s) {
            if (!_s) return 0;
            JavaUUID_slice * _data = JavaUUID_alloc();
            if (_data) {
                for (uint32_t _0i0 = 0; _0i0 < 2; _0i0++) {

                    _data[_0i0] = _s[_0i0];

                }

            }
            return _data;
        }

        _CORBA_MODULE_INLINE void JavaUUID_copy(JavaUUID_slice *_to, const JavaUUID_slice *_from) {
            for (uint32_t _0i0 = 0; _0i0 < 2; _0i0++) {

                _to[_0i0] = _from[_0i0];

            }

        }

        _CORBA_MODULE_INLINE void JavaUUID_free(JavaUUID_slice *_s) {
            delete[] _s;
        }

        class JavaUUID_copyHelper {
        public:
            static JavaUUID_slice *alloc() { return ::Tango::JavaUUID_alloc(); }

            static JavaUUID_slice *dup(const JavaUUID_slice *p) { return ::Tango::JavaUUID_dup(p); }

            static void free(JavaUUID_slice *p) { ::Tango::JavaUUID_free(p); }
        };

        typedef _CORBA_Array_Fix_Var <JavaUUID_copyHelper, JavaUUID_slice> JavaUUID_var;
        typedef _CORBA_Array_Fix_Forany <JavaUUID_copyHelper, JavaUUID_slice> JavaUUID_forany;

        typedef JavaUUID_slice *JavaUUID_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_CppClntIdent;

        typedef ::CORBA::ULong CppClntIdent;
        typedef ::CORBA::ULong_out CppClntIdent_out;

        struct JavaClntIdent {
            typedef _CORBA_ConstrType_Variable_Var <JavaClntIdent> _var_type;


            ::CORBA::String_member MainClass;

            JavaUUID uuid;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef JavaClntIdent::_var_type JavaClntIdent_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <JavaClntIdent, JavaClntIdent_var> JavaClntIdent_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_JavaClntIdent;

        enum LockerLanguage {
            CPP, JAVA /*, __max_LockerLanguage=0xffffffff */ };
        typedef LockerLanguage &LockerLanguage_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_LockerLanguage;

        class ClntIdent {
        public:

            typedef _CORBA_ConstrType_Variable_Var <ClntIdent> _var_type;


            ClntIdent() : _pd__initialised(0) {

            }

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

            ~ClntIdent() {}

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

            void _d(LockerLanguage _value) {
                // illegal to set discriminator before making a member active
                if (!_pd__initialised)
                    OMNIORB_THROW(BAD_PARAM, _OMNI_NS(BAD_PARAM_InvalidUnionDiscValue), ::CORBA::COMPLETED_NO);

                if (_value == _pd__d) return; // no change

                switch (_pd__d) {
                    case CPP:
                        goto fail;
                    case JAVA:
                        goto fail;
                    default:
                        goto fail;

                };


                fail:
                OMNIORB_THROW(BAD_PARAM, _OMNI_NS(BAD_PARAM_InvalidUnionDiscValue), ::CORBA::COMPLETED_NO);


            }


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


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);

        private:
            LockerLanguage _pd__d;
            bool _pd__default;
            bool _pd__initialised;

            union {
                CppClntIdent _pd_cpp_clnt;


            };


            JavaClntIdent _pd_java_clnt;


        };

        typedef ClntIdent::_var_type ClntIdent_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <ClntIdent, ClntIdent_var> ClntIdent_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_ClntIdent;

        enum AttrQuality {
            ATTR_VALID, ATTR_INVALID, ATTR_ALARM, ATTR_CHANGING, ATTR_WARNING /*, __max_AttrQuality=0xffffffff */ };
        typedef AttrQuality &AttrQuality_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttrQuality;

        enum AttrWriteType {
            READ, READ_WITH_WRITE, WRITE, READ_WRITE, WT_UNKNOWN /*, __max_AttrWriteType=0xffffffff */ };
        typedef AttrWriteType &AttrWriteType_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttrWriteType;

        enum AttrDataFormat {
            SCALAR, SPECTRUM, IMAGE, FMT_UNKNOWN /*, __max_AttrDataFormat=0xffffffff */ };
        typedef AttrDataFormat &AttrDataFormat_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttrDataFormat;

        enum DevSource {
            DEV, CACHE, CACHE_DEV /*, __max_DevSource=0xffffffff */ };
        typedef DevSource &DevSource_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevSource;

        enum ErrSeverity {
            WARN, ERR, PANIC /*, __max_ErrSeverity=0xffffffff */ };
        typedef ErrSeverity &ErrSeverity_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_ErrSeverity;

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

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevState;

        enum DispLevel {
            OPERATOR, EXPERT, DL_UNKNOWN /*, __max_DispLevel=0xffffffff */ };
        typedef DispLevel &DispLevel_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DispLevel;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarStateArray;

// Need to declare <<= for elem type, as GCC expands templates early
#if defined(__GNUG__) && __GNUG__ == 2 && __GNUC_MINOR__ == 7
                                                                                                                                friend void operator >>= (DevState, cdrStream&);
   friend void operator <<= (DevState&, cdrStream&);
#endif

        class DevVarStateArray_var;

        class DevVarStateArray : public _CORBA_Unbounded_Sequence_w_FixSizeElement<DevState, 4, 4> {
        public:
            typedef DevVarStateArray_var _var_type;

            DevVarStateArray() {}

            DevVarStateArray(const DevVarStateArray &_s)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<DevState, 4, 4>(_s) {}

            DevVarStateArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<DevState, 4, 4>(_max) {}

            DevVarStateArray(uint32_t _max, uint32_t _len, DevState *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<DevState, 4, 4>(_max, _len, _val, _rel) {}


            DevVarStateArray &operator=(const DevVarStateArray &_s) {
                _CORBA_Unbounded_Sequence_w_FixSizeElement<DevState, 4, 4>::operator=(_s);
                return *this;
            }
        };

        class DevVarStateArray_out;

        class DevVarStateArray_var {
        public:
            DevVarStateArray_var() : _pd_seq(0) {}

            DevVarStateArray_var(DevVarStateArray *_s) : _pd_seq(_s) {}

            DevVarStateArray_var(const DevVarStateArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarStateArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarStateArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarStateArray_var &operator=(DevVarStateArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarStateArray_var &operator=(const DevVarStateArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarStateArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevState &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarStateArray *operator->() { return _pd_seq; }

            const DevVarStateArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarStateArray& () const { return *_pd_seq; }
#else

            operator const DevVarStateArray &() const { return *_pd_seq; }

            operator DevVarStateArray &() { return *_pd_seq; }

#endif

            const DevVarStateArray &in() const { return *_pd_seq; }

            DevVarStateArray &inout() { return *_pd_seq; }

            DevVarStateArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarStateArray *_retn() {
                DevVarStateArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarStateArray_out;

        private:
            DevVarStateArray *_pd_seq;
        };

        class DevVarStateArray_out {
        public:
            DevVarStateArray_out(DevVarStateArray *&_s) : _data(_s) { _data = 0; }

            DevVarStateArray_out(DevVarStateArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarStateArray *) 0; }

            DevVarStateArray_out(const DevVarStateArray_out &_s) : _data(_s._data) {}

            DevVarStateArray_out &operator=(const DevVarStateArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarStateArray_out &operator=(DevVarStateArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarStateArray *&() { return _data; }

            DevVarStateArray *&ptr() { return _data; }

            DevVarStateArray *operator->() { return _data; }

            DevState &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarStateArray *&_data;

        private:
            DevVarStateArray_out();

            DevVarStateArray_out &operator=(const DevVarStateArray_var &);
        };

        struct TimeVal {
            typedef _CORBA_ConstrType_Fix_Var <TimeVal> _var_type;


            int32_t tv_sec;

            int32_t tv_usec;

            int32_t tv_nsec;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef TimeVal::_var_type TimeVal_var;

        typedef TimeVal &TimeVal_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_TimeVal;

        struct DevCmdInfo {
            typedef _CORBA_ConstrType_Variable_Var <DevCmdInfo> _var_type;


            ::CORBA::String_member cmd_name;

            int32_t cmd_tag;

            int32_t in_type;

            int32_t out_type;

            ::CORBA::String_member in_type_desc;

            ::CORBA::String_member out_type_desc;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevCmdInfo::_var_type DevCmdInfo_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevCmdInfo, DevCmdInfo_var> DevCmdInfo_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdInfo;

        struct DevCmdInfo_2 {
            typedef _CORBA_ConstrType_Variable_Var <DevCmdInfo_2> _var_type;


            ::CORBA::String_member cmd_name;

            DispLevel level;

            int32_t cmd_tag;

            int32_t in_type;

            int32_t out_type;

            ::CORBA::String_member in_type_desc;

            ::CORBA::String_member out_type_desc;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevCmdInfo_2::_var_type DevCmdInfo_2_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevCmdInfo_2, DevCmdInfo_2_var> DevCmdInfo_2_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdInfo_2;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdInfoList;

        class DevCmdInfoList_var;

        class DevCmdInfoList : public _CORBA_Unbounded_Sequence<DevCmdInfo> {
        public:
            typedef DevCmdInfoList_var _var_type;

            DevCmdInfoList() {}

            DevCmdInfoList(const DevCmdInfoList &_s)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo>(_s) {}

            DevCmdInfoList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo>(_max) {}

            DevCmdInfoList(uint32_t _max, uint32_t _len, DevCmdInfo *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo>(_max, _len, _val, _rel) {}


            DevCmdInfoList &operator=(const DevCmdInfoList &_s) {
                _CORBA_Unbounded_Sequence<DevCmdInfo>::operator=(_s);
                return *this;
            }
        };

        class DevCmdInfoList_out;

        class DevCmdInfoList_var {
        public:
            DevCmdInfoList_var() : _pd_seq(0) {}

            DevCmdInfoList_var(DevCmdInfoList *_s) : _pd_seq(_s) {}

            DevCmdInfoList_var(const DevCmdInfoList_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevCmdInfoList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevCmdInfoList_var() { if (_pd_seq) delete _pd_seq; }

            DevCmdInfoList_var &operator=(DevCmdInfoList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevCmdInfoList_var &operator=(const DevCmdInfoList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevCmdInfoList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevCmdInfo &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevCmdInfoList *operator->() { return _pd_seq; }

            const DevCmdInfoList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevCmdInfoList& () const { return *_pd_seq; }
#else

            operator const DevCmdInfoList &() const { return *_pd_seq; }

            operator DevCmdInfoList &() { return *_pd_seq; }

#endif

            const DevCmdInfoList &in() const { return *_pd_seq; }

            DevCmdInfoList &inout() { return *_pd_seq; }

            DevCmdInfoList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevCmdInfoList *_retn() {
                DevCmdInfoList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevCmdInfoList_out;

        private:
            DevCmdInfoList *_pd_seq;
        };

        class DevCmdInfoList_out {
        public:
            DevCmdInfoList_out(DevCmdInfoList *&_s) : _data(_s) { _data = 0; }

            DevCmdInfoList_out(DevCmdInfoList_var &_s)
                    : _data(_s._pd_seq) { _s = (DevCmdInfoList *) 0; }

            DevCmdInfoList_out(const DevCmdInfoList_out &_s) : _data(_s._data) {}

            DevCmdInfoList_out &operator=(const DevCmdInfoList_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevCmdInfoList_out &operator=(DevCmdInfoList *_s) {
                _data = _s;
                return *this;
            }

            operator DevCmdInfoList *&() { return _data; }

            DevCmdInfoList *&ptr() { return _data; }

            DevCmdInfoList *operator->() { return _data; }

            DevCmdInfo &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevCmdInfoList *&_data;

        private:
            DevCmdInfoList_out();

            DevCmdInfoList_out &operator=(const DevCmdInfoList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdInfoList_2;

        class DevCmdInfoList_2_var;

        class DevCmdInfoList_2 : public _CORBA_Unbounded_Sequence<DevCmdInfo_2> {
        public:
            typedef DevCmdInfoList_2_var _var_type;

            DevCmdInfoList_2() {}

            DevCmdInfoList_2(const DevCmdInfoList_2 &_s)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo_2>(_s) {}

            DevCmdInfoList_2(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo_2>(_max) {}

            DevCmdInfoList_2(uint32_t _max, uint32_t _len, DevCmdInfo_2 *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevCmdInfo_2>(_max, _len, _val, _rel) {}


            DevCmdInfoList_2 &operator=(const DevCmdInfoList_2 &_s) {
                _CORBA_Unbounded_Sequence<DevCmdInfo_2>::operator=(_s);
                return *this;
            }
        };

        class DevCmdInfoList_2_out;

        class DevCmdInfoList_2_var {
        public:
            DevCmdInfoList_2_var() : _pd_seq(0) {}

            DevCmdInfoList_2_var(DevCmdInfoList_2 *_s) : _pd_seq(_s) {}

            DevCmdInfoList_2_var(const DevCmdInfoList_2_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevCmdInfoList_2(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevCmdInfoList_2_var() { if (_pd_seq) delete _pd_seq; }

            DevCmdInfoList_2_var &operator=(DevCmdInfoList_2 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevCmdInfoList_2_var &operator=(const DevCmdInfoList_2_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevCmdInfoList_2;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevCmdInfo_2 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevCmdInfoList_2 *operator->() { return _pd_seq; }

            const DevCmdInfoList_2 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevCmdInfoList_2& () const { return *_pd_seq; }
#else

            operator const DevCmdInfoList_2 &() const { return *_pd_seq; }

            operator DevCmdInfoList_2 &() { return *_pd_seq; }

#endif

            const DevCmdInfoList_2 &in() const { return *_pd_seq; }

            DevCmdInfoList_2 &inout() { return *_pd_seq; }

            DevCmdInfoList_2 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevCmdInfoList_2 *_retn() {
                DevCmdInfoList_2 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevCmdInfoList_2_out;

        private:
            DevCmdInfoList_2 *_pd_seq;
        };

        class DevCmdInfoList_2_out {
        public:
            DevCmdInfoList_2_out(DevCmdInfoList_2 *&_s) : _data(_s) { _data = 0; }

            DevCmdInfoList_2_out(DevCmdInfoList_2_var &_s)
                    : _data(_s._pd_seq) { _s = (DevCmdInfoList_2 *) 0; }

            DevCmdInfoList_2_out(const DevCmdInfoList_2_out &_s) : _data(_s._data) {}

            DevCmdInfoList_2_out &operator=(const DevCmdInfoList_2_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevCmdInfoList_2_out &operator=(DevCmdInfoList_2 *_s) {
                _data = _s;
                return *this;
            }

            operator DevCmdInfoList_2 *&() { return _data; }

            DevCmdInfoList_2 *&ptr() { return _data; }

            DevCmdInfoList_2 *operator->() { return _data; }

            DevCmdInfo_2 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevCmdInfoList_2 *&_data;

        private:
            DevCmdInfoList_2_out();

            DevCmdInfoList_2_out &operator=(const DevCmdInfoList_2_var &);
        };

        struct DevError {
            typedef _CORBA_ConstrType_Variable_Var <DevError> _var_type;


            ::CORBA::String_member reason;

            ErrSeverity severity;

            ::CORBA::String_member desc;

            ::CORBA::String_member origin;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevError::_var_type DevError_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevError, DevError_var> DevError_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevError;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevErrorList;

        class DevErrorList_var;

        class DevErrorList : public _CORBA_Unbounded_Sequence<DevError> {
        public:
            typedef DevErrorList_var _var_type;

            DevErrorList() {}

            DevErrorList(const DevErrorList &_s)
                    : _CORBA_Unbounded_Sequence<DevError>(_s) {}

            DevErrorList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevError>(_max) {}

            DevErrorList(uint32_t _max, uint32_t _len, DevError *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevError>(_max, _len, _val, _rel) {}


            DevErrorList &operator=(const DevErrorList &_s) {
                _CORBA_Unbounded_Sequence<DevError>::operator=(_s);
                return *this;
            }
        };

        class DevErrorList_out;

        class DevErrorList_var {
        public:
            DevErrorList_var() : _pd_seq(0) {}

            DevErrorList_var(DevErrorList *_s) : _pd_seq(_s) {}

            DevErrorList_var(const DevErrorList_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevErrorList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevErrorList_var() { if (_pd_seq) delete _pd_seq; }

            DevErrorList_var &operator=(DevErrorList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevErrorList_var &operator=(const DevErrorList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevErrorList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevError &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevErrorList *operator->() { return _pd_seq; }

            const DevErrorList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevErrorList& () const { return *_pd_seq; }
#else

            operator const DevErrorList &() const { return *_pd_seq; }

            operator DevErrorList &() { return *_pd_seq; }

#endif

            const DevErrorList &in() const { return *_pd_seq; }

            DevErrorList &inout() { return *_pd_seq; }

            DevErrorList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevErrorList *_retn() {
                DevErrorList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevErrorList_out;

        private:
            DevErrorList *_pd_seq;
        };

        class DevErrorList_out {
        public:
            DevErrorList_out(DevErrorList *&_s) : _data(_s) { _data = 0; }

            DevErrorList_out(DevErrorList_var &_s)
                    : _data(_s._pd_seq) { _s = (DevErrorList *) 0; }

            DevErrorList_out(const DevErrorList_out &_s) : _data(_s._data) {}

            DevErrorList_out &operator=(const DevErrorList_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevErrorList_out &operator=(DevErrorList *_s) {
                _data = _s;
                return *this;
            }

            operator DevErrorList *&() { return _data; }

            DevErrorList *&ptr() { return _data; }

            DevErrorList *operator->() { return _data; }

            DevError &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevErrorList *&_data;

        private:
            DevErrorList_out();

            DevErrorList_out &operator=(const DevErrorList_var &);
        };

        struct NamedDevError {
            typedef _CORBA_ConstrType_Variable_Var <NamedDevError> _var_type;


            ::CORBA::String_member name;

            int32_t index_in_call;

            DevErrorList err_list;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef NamedDevError::_var_type NamedDevError_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <NamedDevError, NamedDevError_var> NamedDevError_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_NamedDevError;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_NamedDevErrorList;

        class NamedDevErrorList_var;

        class NamedDevErrorList : public _CORBA_Unbounded_Sequence<NamedDevError> {
        public:
            typedef NamedDevErrorList_var _var_type;

            NamedDevErrorList() {}

            NamedDevErrorList(const NamedDevErrorList &_s)
                    : _CORBA_Unbounded_Sequence<NamedDevError>(_s) {}

            NamedDevErrorList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<NamedDevError>(_max) {}

            NamedDevErrorList(uint32_t _max, uint32_t _len, NamedDevError *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<NamedDevError>(_max, _len, _val, _rel) {}


            NamedDevErrorList &operator=(const NamedDevErrorList &_s) {
                _CORBA_Unbounded_Sequence<NamedDevError>::operator=(_s);
                return *this;
            }
        };

        class NamedDevErrorList_out;

        class NamedDevErrorList_var {
        public:
            NamedDevErrorList_var() : _pd_seq(0) {}

            NamedDevErrorList_var(NamedDevErrorList *_s) : _pd_seq(_s) {}

            NamedDevErrorList_var(const NamedDevErrorList_var &_s) {
                if (_s._pd_seq) _pd_seq = new NamedDevErrorList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~NamedDevErrorList_var() { if (_pd_seq) delete _pd_seq; }

            NamedDevErrorList_var &operator=(NamedDevErrorList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            NamedDevErrorList_var &operator=(const NamedDevErrorList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new NamedDevErrorList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            NamedDevError &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            NamedDevErrorList *operator->() { return _pd_seq; }

            const NamedDevErrorList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator NamedDevErrorList& () const { return *_pd_seq; }
#else

            operator const NamedDevErrorList &() const { return *_pd_seq; }

            operator NamedDevErrorList &() { return *_pd_seq; }

#endif

            const NamedDevErrorList &in() const { return *_pd_seq; }

            NamedDevErrorList &inout() { return *_pd_seq; }

            NamedDevErrorList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            NamedDevErrorList *_retn() {
                NamedDevErrorList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class NamedDevErrorList_out;

        private:
            NamedDevErrorList *_pd_seq;
        };

        class NamedDevErrorList_out {
        public:
            NamedDevErrorList_out(NamedDevErrorList *&_s) : _data(_s) { _data = 0; }

            NamedDevErrorList_out(NamedDevErrorList_var &_s)
                    : _data(_s._pd_seq) { _s = (NamedDevErrorList *) 0; }

            NamedDevErrorList_out(const NamedDevErrorList_out &_s) : _data(_s._data) {}

            NamedDevErrorList_out &operator=(const NamedDevErrorList_out &_s) {
                _data = _s._data;
                return *this;
            }

            NamedDevErrorList_out &operator=(NamedDevErrorList *_s) {
                _data = _s;
                return *this;
            }

            operator NamedDevErrorList *&() { return _data; }

            NamedDevErrorList *&ptr() { return _data; }

            NamedDevErrorList *operator->() { return _data; }

            NamedDevError &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            NamedDevErrorList *&_data;

        private:
            NamedDevErrorList_out();

            NamedDevErrorList_out &operator=(const NamedDevErrorList_var &);
        };

        class DevFailed : public ::CORBA::UserException {
        public:

            DevErrorList errors;


            DevFailed() {
                pd_insertToAnyFn = insertToAnyFn;
                pd_insertToAnyFnNCP = insertToAnyFnNCP;
            }

            DevFailed(const DevFailed &);

            DevFailed(const DevErrorList i_errors);

            DevFailed &operator=(const DevFailed &);

            virtual ~DevFailed();

            virtual void _raise() const;

            static DevFailed *_downcast(::CORBA::Exception *);

            static const DevFailed *_downcast(const ::CORBA::Exception *);

            static DevFailed *_narrow(::CORBA::Exception *_e) {
                return _downcast(_e);
            }

            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);

            static _core_attr insertExceptionToAny
            insertToAnyFn;
            static _core_attr insertExceptionToAnyNCP
            insertToAnyFnNCP;

            virtual ::CORBA::Exception *_NP_duplicate() const;

            static _core_attr const char *_PD_repoId;
            static _core_attr const char *_PD_typeId;

        private:
            virtual const char *_NP_typeId() const;

            virtual const char *_NP_repoId(int *) const;

            virtual void _NP_marshal(cdrStream &) const;
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevFailed;

        class MultiDevFailed : public ::CORBA::UserException {
        public:

            NamedDevErrorList errors;


            MultiDevFailed() {
                pd_insertToAnyFn = insertToAnyFn;
                pd_insertToAnyFnNCP = insertToAnyFnNCP;
            }

            MultiDevFailed(const MultiDevFailed &);

            MultiDevFailed(const NamedDevErrorList i_errors);

            MultiDevFailed &operator=(const MultiDevFailed &);

            virtual ~MultiDevFailed();

            virtual void _raise() const;

            static MultiDevFailed *_downcast(::CORBA::Exception *);

            static const MultiDevFailed *_downcast(const ::CORBA::Exception *);

            static MultiDevFailed *_narrow(::CORBA::Exception *_e) {
                return _downcast(_e);
            }

            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);

            static _core_attr insertExceptionToAny
            insertToAnyFn;
            static _core_attr insertExceptionToAnyNCP
            insertToAnyFnNCP;

            virtual ::CORBA::Exception *_NP_duplicate() const;

            static _core_attr const char *_PD_repoId;
            static _core_attr const char *_PD_typeId;

        private:
            virtual const char *_NP_typeId() const;

            virtual const char *_NP_repoId(int *) const;

            virtual void _NP_marshal(cdrStream &) const;
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_MultiDevFailed;

        struct AttributeConfig {
            typedef _CORBA_ConstrType_Variable_Var <AttributeConfig> _var_type;


            ::CORBA::String_member name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            ::CORBA::String_member description;

            ::CORBA::String_member label;

            ::CORBA::String_member unit;

            ::CORBA::String_member standard_unit;

            ::CORBA::String_member display_unit;

            ::CORBA::String_member format;

            ::CORBA::String_member min_value;

            ::CORBA::String_member max_value;

            ::CORBA::String_member min_alarm;

            ::CORBA::String_member max_alarm;

            ::CORBA::String_member writable_attr_name;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeConfig::_var_type AttributeConfig_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeConfig, AttributeConfig_var> AttributeConfig_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfig;

        struct AttributeConfig_2 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeConfig_2> _var_type;


            ::CORBA::String_member name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            ::CORBA::String_member description;

            ::CORBA::String_member label;

            ::CORBA::String_member unit;

            ::CORBA::String_member standard_unit;

            ::CORBA::String_member display_unit;

            ::CORBA::String_member format;

            ::CORBA::String_member min_value;

            ::CORBA::String_member max_value;

            ::CORBA::String_member min_alarm;

            ::CORBA::String_member max_alarm;

            ::CORBA::String_member writable_attr_name;

            DispLevel level;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeConfig_2::_var_type AttributeConfig_2_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeConfig_2, AttributeConfig_2_var> AttributeConfig_2_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfig_2;

        struct AttributeValue {
            typedef _CORBA_ConstrType_Variable_Var <AttributeValue> _var_type;


            ::CORBA::Any value;

            AttrQuality quality;

            TimeVal time;

            ::CORBA::String_member name;

            int32_t dim_x;

            int32_t dim_y;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeValue::_var_type AttributeValue_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeValue, AttributeValue_var> AttributeValue_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValue;

        struct AttributeDim {
            typedef _CORBA_ConstrType_Fix_Var <AttributeDim> _var_type;


            int32_t dim_x;

            int32_t dim_y;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeDim::_var_type AttributeDim_var;

        typedef AttributeDim &AttributeDim_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeDim;

        struct AttributeValue_3 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeValue_3> _var_type;


            ::CORBA::Any value;

            AttrQuality quality;

            TimeVal time;

            ::CORBA::String_member name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeValue_3::_var_type AttributeValue_3_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeValue_3, AttributeValue_3_var> AttributeValue_3_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValue_3;

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

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeDataType;

        class AttrValUnion {
        public:

            typedef _CORBA_ConstrType_Variable_Var <AttrValUnion> _var_type;


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
                // illegal to set discriminator before making a member active
                if (!_pd__initialised)
                    OMNIORB_THROW(BAD_PARAM, _OMNI_NS(BAD_PARAM_InvalidUnionDiscValue), ::CORBA::COMPLETED_NO);

                if (_value == _pd__d) return; // no change

                switch (_pd__d) {
                    case ATT_BOOL:
                        goto fail;
                    case ATT_SHORT:
                        goto fail;
                    case ATT_LONG:
                        goto fail;
                    case ATT_LONG64:
                        goto fail;
                    case ATT_FLOAT:
                        goto fail;
                    case ATT_DOUBLE:
                        goto fail;
                    case ATT_UCHAR:
                        goto fail;
                    case ATT_USHORT:
                        goto fail;
                    case ATT_ULONG:
                        goto fail;
                    case ATT_ULONG64:
                        goto fail;
                    case ATT_STRING:
                        goto fail;
                    case ATT_STATE:
                        goto fail;
                    case DEVICE_STATE:
                        goto fail;
                    case ATT_ENCODED:
                        goto fail;
                    case ATT_NO_DATA:
                        goto fail;
                    default:
                        goto fail;

                };


                fail:
                OMNIORB_THROW(BAD_PARAM, _OMNI_NS(BAD_PARAM_InvalidUnionDiscValue), ::CORBA::COMPLETED_NO);


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


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);

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

        typedef AttrValUnion::_var_type AttrValUnion_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttrValUnion, AttrValUnion_var> AttrValUnion_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttrValUnion;

        struct AttributeValue_4 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeValue_4> _var_type;

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

            ::CORBA::String_member name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeValue_4::_var_type AttributeValue_4_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeValue_4, AttributeValue_4_var> AttributeValue_4_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValue_4;

        struct AttributeValue_5 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeValue_5> _var_type;

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

            ::CORBA::String_member name;

            AttributeDim r_dim;

            AttributeDim w_dim;

            DevErrorList err_list;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeValue_5::_var_type AttributeValue_5_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeValue_5, AttributeValue_5_var> AttributeValue_5_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValue_5;

        struct ChangeEventProp {
            typedef _CORBA_ConstrType_Variable_Var <ChangeEventProp> _var_type;


            ::CORBA::String_member rel_change;

            ::CORBA::String_member abs_change;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef ChangeEventProp::_var_type ChangeEventProp_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <ChangeEventProp, ChangeEventProp_var> ChangeEventProp_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_ChangeEventProp;

        struct PeriodicEventProp {
            typedef _CORBA_ConstrType_Variable_Var <PeriodicEventProp> _var_type;


            ::CORBA::String_member period;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef PeriodicEventProp::_var_type PeriodicEventProp_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <PeriodicEventProp, PeriodicEventProp_var> PeriodicEventProp_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_PeriodicEventProp;

        struct ArchiveEventProp {
            typedef _CORBA_ConstrType_Variable_Var <ArchiveEventProp> _var_type;


            ::CORBA::String_member rel_change;

            ::CORBA::String_member abs_change;

            ::CORBA::String_member period;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef ArchiveEventProp::_var_type ArchiveEventProp_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <ArchiveEventProp, ArchiveEventProp_var> ArchiveEventProp_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_ArchiveEventProp;

        struct EventProperties {
            typedef _CORBA_ConstrType_Variable_Var <EventProperties> _var_type;


            ChangeEventProp ch_event;

            PeriodicEventProp per_event;

            ArchiveEventProp arch_event;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef EventProperties::_var_type EventProperties_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <EventProperties, EventProperties_var> EventProperties_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_EventProperties;

        struct AttributeAlarm {
            typedef _CORBA_ConstrType_Variable_Var <AttributeAlarm> _var_type;


            ::CORBA::String_member min_alarm;

            ::CORBA::String_member max_alarm;

            ::CORBA::String_member min_warning;

            ::CORBA::String_member max_warning;

            ::CORBA::String_member delta_t;

            ::CORBA::String_member delta_val;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeAlarm::_var_type AttributeAlarm_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeAlarm, AttributeAlarm_var> AttributeAlarm_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeAlarm;

        struct AttributeConfig_3 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeConfig_3> _var_type;


            ::CORBA::String_member name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            int32_t max_dim_x;

            int32_t max_dim_y;

            ::CORBA::String_member description;

            ::CORBA::String_member label;

            ::CORBA::String_member unit;

            ::CORBA::String_member standard_unit;

            ::CORBA::String_member display_unit;

            ::CORBA::String_member format;

            ::CORBA::String_member min_value;

            ::CORBA::String_member max_value;

            ::CORBA::String_member writable_attr_name;

            DispLevel level;

            AttributeAlarm att_alarm;

            EventProperties event_prop;

            DevVarStringArray extensions;

            DevVarStringArray sys_extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeConfig_3::_var_type AttributeConfig_3_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeConfig_3, AttributeConfig_3_var> AttributeConfig_3_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfig_3;

        struct AttributeConfig_5 {
            typedef _CORBA_ConstrType_Variable_Var <AttributeConfig_5> _var_type;


            ::CORBA::String_member name;

            AttrWriteType writable;

            AttrDataFormat data_format;

            int32_t data_type;

            ::CORBA::Boolean memorized;

            ::CORBA::Boolean mem_init;

            int32_t max_dim_x;

            int32_t max_dim_y;

            ::CORBA::String_member description;

            ::CORBA::String_member label;

            ::CORBA::String_member unit;

            ::CORBA::String_member standard_unit;

            ::CORBA::String_member display_unit;

            ::CORBA::String_member format;

            ::CORBA::String_member min_value;

            ::CORBA::String_member max_value;

            ::CORBA::String_member writable_attr_name;

            DispLevel level;

            ::CORBA::String_member root_attr_name;

            DevVarStringArray enum_labels;

            AttributeAlarm att_alarm;

            EventProperties event_prop;

            DevVarStringArray extensions;

            DevVarStringArray sys_extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttributeConfig_5::_var_type AttributeConfig_5_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttributeConfig_5, AttributeConfig_5_var> AttributeConfig_5_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfig_5;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfigList;

        class AttributeConfigList_var;

        class AttributeConfigList : public _CORBA_Unbounded_Sequence<AttributeConfig> {
        public:
            typedef AttributeConfigList_var _var_type;

            AttributeConfigList() {}

            AttributeConfigList(const AttributeConfigList &_s)
                    : _CORBA_Unbounded_Sequence<AttributeConfig>(_s) {}

            AttributeConfigList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeConfig>(_max) {}

            AttributeConfigList(uint32_t _max, uint32_t _len, AttributeConfig *_val,
                                       bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeConfig>(_max, _len, _val, _rel) {}


            AttributeConfigList &operator=(const AttributeConfigList &_s) {
                _CORBA_Unbounded_Sequence<AttributeConfig>::operator=(_s);
                return *this;
            }
        };

        class AttributeConfigList_out;

        class AttributeConfigList_var {
        public:
            AttributeConfigList_var() : _pd_seq(0) {}

            AttributeConfigList_var(AttributeConfigList *_s) : _pd_seq(_s) {}

            AttributeConfigList_var(const AttributeConfigList_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeConfigList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeConfigList_var() { if (_pd_seq) delete _pd_seq; }

            AttributeConfigList_var &operator=(AttributeConfigList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeConfigList_var &operator=(const AttributeConfigList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeConfigList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeConfig &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeConfigList *operator->() { return _pd_seq; }

            const AttributeConfigList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeConfigList& () const { return *_pd_seq; }
#else

            operator const AttributeConfigList &() const { return *_pd_seq; }

            operator AttributeConfigList &() { return *_pd_seq; }

#endif

            const AttributeConfigList &in() const { return *_pd_seq; }

            AttributeConfigList &inout() { return *_pd_seq; }

            AttributeConfigList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeConfigList *_retn() {
                AttributeConfigList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeConfigList_out;

        private:
            AttributeConfigList *_pd_seq;
        };

        class AttributeConfigList_out {
        public:
            AttributeConfigList_out(AttributeConfigList *&_s) : _data(_s) { _data = 0; }

            AttributeConfigList_out(AttributeConfigList_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeConfigList *) 0; }

            AttributeConfigList_out(const AttributeConfigList_out &_s) : _data(_s._data) {}

            AttributeConfigList_out &operator=(const AttributeConfigList_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeConfigList_out &operator=(AttributeConfigList *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeConfigList *&() { return _data; }

            AttributeConfigList *&ptr() { return _data; }

            AttributeConfigList *operator->() { return _data; }

            AttributeConfig &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeConfigList *&_data;

        private:
            AttributeConfigList_out();

            AttributeConfigList_out &operator=(const AttributeConfigList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfigList_2;

        class AttributeConfigList_2_var;

        class AttributeConfigList_2 : public _CORBA_Unbounded_Sequence<AttributeConfig_2> {
        public:
            typedef AttributeConfigList_2_var _var_type;

            AttributeConfigList_2() {}

            AttributeConfigList_2(const AttributeConfigList_2 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_2>(_s) {}

            AttributeConfigList_2(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_2>(_max) {}

            AttributeConfigList_2(uint32_t _max, uint32_t _len, AttributeConfig_2 *_val,
                                         bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_2>(_max, _len, _val, _rel) {}


            AttributeConfigList_2 &operator=(const AttributeConfigList_2 &_s) {
                _CORBA_Unbounded_Sequence<AttributeConfig_2>::operator=(_s);
                return *this;
            }
        };

        class AttributeConfigList_2_out;

        class AttributeConfigList_2_var {
        public:
            AttributeConfigList_2_var() : _pd_seq(0) {}

            AttributeConfigList_2_var(AttributeConfigList_2 *_s) : _pd_seq(_s) {}

            AttributeConfigList_2_var(const AttributeConfigList_2_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeConfigList_2(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeConfigList_2_var() { if (_pd_seq) delete _pd_seq; }

            AttributeConfigList_2_var &operator=(AttributeConfigList_2 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeConfigList_2_var &operator=(const AttributeConfigList_2_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeConfigList_2;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeConfig_2 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeConfigList_2 *operator->() { return _pd_seq; }

            const AttributeConfigList_2 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeConfigList_2& () const { return *_pd_seq; }
#else

            operator const AttributeConfigList_2 &() const { return *_pd_seq; }

            operator AttributeConfigList_2 &() { return *_pd_seq; }

#endif

            const AttributeConfigList_2 &in() const { return *_pd_seq; }

            AttributeConfigList_2 &inout() { return *_pd_seq; }

            AttributeConfigList_2 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeConfigList_2 *_retn() {
                AttributeConfigList_2 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeConfigList_2_out;

        private:
            AttributeConfigList_2 *_pd_seq;
        };

        class AttributeConfigList_2_out {
        public:
            AttributeConfigList_2_out(AttributeConfigList_2 *&_s) : _data(_s) { _data = 0; }

            AttributeConfigList_2_out(AttributeConfigList_2_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeConfigList_2 *) 0; }

            AttributeConfigList_2_out(const AttributeConfigList_2_out &_s) : _data(_s._data) {}

            AttributeConfigList_2_out &operator=(const AttributeConfigList_2_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeConfigList_2_out &operator=(AttributeConfigList_2 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeConfigList_2 *&() { return _data; }

            AttributeConfigList_2 *&ptr() { return _data; }

            AttributeConfigList_2 *operator->() { return _data; }

            AttributeConfig_2 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeConfigList_2 *&_data;

        private:
            AttributeConfigList_2_out();

            AttributeConfigList_2_out &operator=(const AttributeConfigList_2_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfigList_3;

        class AttributeConfigList_3_var;

        class AttributeConfigList_3 : public _CORBA_Unbounded_Sequence<AttributeConfig_3> {
        public:
            typedef AttributeConfigList_3_var _var_type;

            AttributeConfigList_3() {}

            AttributeConfigList_3(const AttributeConfigList_3 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_3>(_s) {}

            AttributeConfigList_3(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_3>(_max) {}

            AttributeConfigList_3(uint32_t _max, uint32_t _len, AttributeConfig_3 *_val,
                                         bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_3>(_max, _len, _val, _rel) {}


            AttributeConfigList_3 &operator=(const AttributeConfigList_3 &_s) {
                _CORBA_Unbounded_Sequence<AttributeConfig_3>::operator=(_s);
                return *this;
            }
        };

        class AttributeConfigList_3_out;

        class AttributeConfigList_3_var {
        public:
            AttributeConfigList_3_var() : _pd_seq(0) {}

            AttributeConfigList_3_var(AttributeConfigList_3 *_s) : _pd_seq(_s) {}

            AttributeConfigList_3_var(const AttributeConfigList_3_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeConfigList_3(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeConfigList_3_var() { if (_pd_seq) delete _pd_seq; }

            AttributeConfigList_3_var &operator=(AttributeConfigList_3 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeConfigList_3_var &operator=(const AttributeConfigList_3_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeConfigList_3;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeConfig_3 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeConfigList_3 *operator->() { return _pd_seq; }

            const AttributeConfigList_3 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeConfigList_3& () const { return *_pd_seq; }
#else

            operator const AttributeConfigList_3 &() const { return *_pd_seq; }

            operator AttributeConfigList_3 &() { return *_pd_seq; }

#endif

            const AttributeConfigList_3 &in() const { return *_pd_seq; }

            AttributeConfigList_3 &inout() { return *_pd_seq; }

            AttributeConfigList_3 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeConfigList_3 *_retn() {
                AttributeConfigList_3 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeConfigList_3_out;

        private:
            AttributeConfigList_3 *_pd_seq;
        };

        class AttributeConfigList_3_out {
        public:
            AttributeConfigList_3_out(AttributeConfigList_3 *&_s) : _data(_s) { _data = 0; }

            AttributeConfigList_3_out(AttributeConfigList_3_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeConfigList_3 *) 0; }

            AttributeConfigList_3_out(const AttributeConfigList_3_out &_s) : _data(_s._data) {}

            AttributeConfigList_3_out &operator=(const AttributeConfigList_3_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeConfigList_3_out &operator=(AttributeConfigList_3 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeConfigList_3 *&() { return _data; }

            AttributeConfigList_3 *&ptr() { return _data; }

            AttributeConfigList_3 *operator->() { return _data; }

            AttributeConfig_3 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeConfigList_3 *&_data;

        private:
            AttributeConfigList_3_out();

            AttributeConfigList_3_out &operator=(const AttributeConfigList_3_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeConfigList_5;

        class AttributeConfigList_5_var;

        class AttributeConfigList_5 : public _CORBA_Unbounded_Sequence<AttributeConfig_5> {
        public:
            typedef AttributeConfigList_5_var _var_type;

            AttributeConfigList_5() {}

            AttributeConfigList_5(const AttributeConfigList_5 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_5>(_s) {}

            AttributeConfigList_5(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_5>(_max) {}

            AttributeConfigList_5(uint32_t _max, uint32_t _len, AttributeConfig_5 *_val,
                                         bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeConfig_5>(_max, _len, _val, _rel) {}


            AttributeConfigList_5 &operator=(const AttributeConfigList_5 &_s) {
                _CORBA_Unbounded_Sequence<AttributeConfig_5>::operator=(_s);
                return *this;
            }
        };

        class AttributeConfigList_5_out;

        class AttributeConfigList_5_var {
        public:
            AttributeConfigList_5_var() : _pd_seq(0) {}

            AttributeConfigList_5_var(AttributeConfigList_5 *_s) : _pd_seq(_s) {}

            AttributeConfigList_5_var(const AttributeConfigList_5_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeConfigList_5(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeConfigList_5_var() { if (_pd_seq) delete _pd_seq; }

            AttributeConfigList_5_var &operator=(AttributeConfigList_5 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeConfigList_5_var &operator=(const AttributeConfigList_5_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeConfigList_5;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeConfig_5 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeConfigList_5 *operator->() { return _pd_seq; }

            const AttributeConfigList_5 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeConfigList_5& () const { return *_pd_seq; }
#else

            operator const AttributeConfigList_5 &() const { return *_pd_seq; }

            operator AttributeConfigList_5 &() { return *_pd_seq; }

#endif

            const AttributeConfigList_5 &in() const { return *_pd_seq; }

            AttributeConfigList_5 &inout() { return *_pd_seq; }

            AttributeConfigList_5 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeConfigList_5 *_retn() {
                AttributeConfigList_5 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeConfigList_5_out;

        private:
            AttributeConfigList_5 *_pd_seq;
        };

        class AttributeConfigList_5_out {
        public:
            AttributeConfigList_5_out(AttributeConfigList_5 *&_s) : _data(_s) { _data = 0; }

            AttributeConfigList_5_out(AttributeConfigList_5_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeConfigList_5 *) 0; }

            AttributeConfigList_5_out(const AttributeConfigList_5_out &_s) : _data(_s._data) {}

            AttributeConfigList_5_out &operator=(const AttributeConfigList_5_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeConfigList_5_out &operator=(AttributeConfigList_5 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeConfigList_5 *&() { return _data; }

            AttributeConfigList_5 *&ptr() { return _data; }

            AttributeConfigList_5 *operator->() { return _data; }

            AttributeConfig_5 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeConfigList_5 *&_data;

        private:
            AttributeConfigList_5_out();

            AttributeConfigList_5_out &operator=(const AttributeConfigList_5_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValueList;

        class AttributeValueList_var;

        class AttributeValueList : public _CORBA_Unbounded_Sequence<AttributeValue> {
        public:
            typedef AttributeValueList_var _var_type;

            AttributeValueList() {}

            AttributeValueList(const AttributeValueList &_s)
                    : _CORBA_Unbounded_Sequence<AttributeValue>(_s) {}

            AttributeValueList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeValue>(_max) {}

            AttributeValueList(uint32_t _max, uint32_t _len, AttributeValue *_val,
                                      bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeValue>(_max, _len, _val, _rel) {}


            AttributeValueList &operator=(const AttributeValueList &_s) {
                _CORBA_Unbounded_Sequence<AttributeValue>::operator=(_s);
                return *this;
            }
        };

        class AttributeValueList_out;

        class AttributeValueList_var {
        public:
            AttributeValueList_var() : _pd_seq(0) {}

            AttributeValueList_var(AttributeValueList *_s) : _pd_seq(_s) {}

            AttributeValueList_var(const AttributeValueList_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeValueList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeValueList_var() { if (_pd_seq) delete _pd_seq; }

            AttributeValueList_var &operator=(AttributeValueList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeValueList_var &operator=(const AttributeValueList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeValueList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeValue &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeValueList *operator->() { return _pd_seq; }

            const AttributeValueList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeValueList& () const { return *_pd_seq; }
#else

            operator const AttributeValueList &() const { return *_pd_seq; }

            operator AttributeValueList &() { return *_pd_seq; }

#endif

            const AttributeValueList &in() const { return *_pd_seq; }

            AttributeValueList &inout() { return *_pd_seq; }

            AttributeValueList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeValueList *_retn() {
                AttributeValueList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeValueList_out;

        private:
            AttributeValueList *_pd_seq;
        };

        class AttributeValueList_out {
        public:
            AttributeValueList_out(AttributeValueList *&_s) : _data(_s) { _data = 0; }

            AttributeValueList_out(AttributeValueList_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeValueList *) 0; }

            AttributeValueList_out(const AttributeValueList_out &_s) : _data(_s._data) {}

            AttributeValueList_out &operator=(const AttributeValueList_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeValueList_out &operator=(AttributeValueList *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeValueList *&() { return _data; }

            AttributeValueList *&ptr() { return _data; }

            AttributeValueList *operator->() { return _data; }

            AttributeValue &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeValueList *&_data;

        private:
            AttributeValueList_out();

            AttributeValueList_out &operator=(const AttributeValueList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValueList_3;

        class AttributeValueList_3_var;

        class AttributeValueList_3 : public _CORBA_Unbounded_Sequence<AttributeValue_3> {
        public:
            typedef AttributeValueList_3_var _var_type;

            AttributeValueList_3() {}

            AttributeValueList_3(const AttributeValueList_3 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeValue_3>(_s) {}

            AttributeValueList_3(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeValue_3>(_max) {}

            AttributeValueList_3(uint32_t _max, uint32_t _len, AttributeValue_3 *_val,
                                        bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeValue_3>(_max, _len, _val, _rel) {}


            AttributeValueList_3 &operator=(const AttributeValueList_3 &_s) {
                _CORBA_Unbounded_Sequence<AttributeValue_3>::operator=(_s);
                return *this;
            }
        };

        class AttributeValueList_3_out;

        class AttributeValueList_3_var {
        public:
            AttributeValueList_3_var() : _pd_seq(0) {}

            AttributeValueList_3_var(AttributeValueList_3 *_s) : _pd_seq(_s) {}

            AttributeValueList_3_var(const AttributeValueList_3_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeValueList_3(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeValueList_3_var() { if (_pd_seq) delete _pd_seq; }

            AttributeValueList_3_var &operator=(AttributeValueList_3 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeValueList_3_var &operator=(const AttributeValueList_3_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeValueList_3;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeValue_3 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeValueList_3 *operator->() { return _pd_seq; }

            const AttributeValueList_3 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeValueList_3& () const { return *_pd_seq; }
#else

            operator const AttributeValueList_3 &() const { return *_pd_seq; }

            operator AttributeValueList_3 &() { return *_pd_seq; }

#endif

            const AttributeValueList_3 &in() const { return *_pd_seq; }

            AttributeValueList_3 &inout() { return *_pd_seq; }

            AttributeValueList_3 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeValueList_3 *_retn() {
                AttributeValueList_3 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeValueList_3_out;

        private:
            AttributeValueList_3 *_pd_seq;
        };

        class AttributeValueList_3_out {
        public:
            AttributeValueList_3_out(AttributeValueList_3 *&_s) : _data(_s) { _data = 0; }

            AttributeValueList_3_out(AttributeValueList_3_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeValueList_3 *) 0; }

            AttributeValueList_3_out(const AttributeValueList_3_out &_s) : _data(_s._data) {}

            AttributeValueList_3_out &operator=(const AttributeValueList_3_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeValueList_3_out &operator=(AttributeValueList_3 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeValueList_3 *&() { return _data; }

            AttributeValueList_3 *&ptr() { return _data; }

            AttributeValueList_3 *operator->() { return _data; }

            AttributeValue_3 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeValueList_3 *&_data;

        private:
            AttributeValueList_3_out();

            AttributeValueList_3_out &operator=(const AttributeValueList_3_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValueList_4;

        class AttributeValueList_4_var;

        class AttributeValueList_4 : public _CORBA_Unbounded_Sequence<AttributeValue_4> {
        public:
            typedef AttributeValueList_4_var _var_type;

            AttributeValueList_4() {}

            AttributeValueList_4(const AttributeValueList_4 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeValue_4>(_s) {}

            AttributeValueList_4(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeValue_4>(_max) {}

            AttributeValueList_4(uint32_t _max, uint32_t _len, AttributeValue_4 *_val,
                                        bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeValue_4>(_max, _len, _val, _rel) {}


            AttributeValueList_4 &operator=(const AttributeValueList_4 &_s) {
                _CORBA_Unbounded_Sequence<AttributeValue_4>::operator=(_s);
                return *this;
            }
        };

        class AttributeValueList_4_out;

        class AttributeValueList_4_var {
        public:
            AttributeValueList_4_var() : _pd_seq(0) {}

            AttributeValueList_4_var(AttributeValueList_4 *_s) : _pd_seq(_s) {}

            AttributeValueList_4_var(const AttributeValueList_4_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeValueList_4(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeValueList_4_var() { if (_pd_seq) delete _pd_seq; }

            AttributeValueList_4_var &operator=(AttributeValueList_4 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeValueList_4_var &operator=(const AttributeValueList_4_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeValueList_4;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeValue_4 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeValueList_4 *operator->() { return _pd_seq; }

            const AttributeValueList_4 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeValueList_4& () const { return *_pd_seq; }
#else

            operator const AttributeValueList_4 &() const { return *_pd_seq; }

            operator AttributeValueList_4 &() { return *_pd_seq; }

#endif

            const AttributeValueList_4 &in() const { return *_pd_seq; }

            AttributeValueList_4 &inout() { return *_pd_seq; }

            AttributeValueList_4 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeValueList_4 *_retn() {
                AttributeValueList_4 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeValueList_4_out;

        private:
            AttributeValueList_4 *_pd_seq;
        };

        class AttributeValueList_4_out {
        public:
            AttributeValueList_4_out(AttributeValueList_4 *&_s) : _data(_s) { _data = 0; }

            AttributeValueList_4_out(AttributeValueList_4_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeValueList_4 *) 0; }

            AttributeValueList_4_out(const AttributeValueList_4_out &_s) : _data(_s._data) {}

            AttributeValueList_4_out &operator=(const AttributeValueList_4_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeValueList_4_out &operator=(AttributeValueList_4 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeValueList_4 *&() { return _data; }

            AttributeValueList_4 *&ptr() { return _data; }

            AttributeValueList_4 *operator->() { return _data; }

            AttributeValue_4 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeValueList_4 *&_data;

        private:
            AttributeValueList_4_out();

            AttributeValueList_4_out &operator=(const AttributeValueList_4_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeValueList_5;

        class AttributeValueList_5_var;

        class AttributeValueList_5 : public _CORBA_Unbounded_Sequence<AttributeValue_5> {
        public:
            typedef AttributeValueList_5_var _var_type;

            AttributeValueList_5() {}

            AttributeValueList_5(const AttributeValueList_5 &_s)
                    : _CORBA_Unbounded_Sequence<AttributeValue_5>(_s) {}

            AttributeValueList_5(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeValue_5>(_max) {}

            AttributeValueList_5(uint32_t _max, uint32_t _len, AttributeValue_5 *_val,
                                        bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeValue_5>(_max, _len, _val, _rel) {}


            AttributeValueList_5 &operator=(const AttributeValueList_5 &_s) {
                _CORBA_Unbounded_Sequence<AttributeValue_5>::operator=(_s);
                return *this;
            }
        };

        class AttributeValueList_5_out;

        class AttributeValueList_5_var {
        public:
            AttributeValueList_5_var() : _pd_seq(0) {}

            AttributeValueList_5_var(AttributeValueList_5 *_s) : _pd_seq(_s) {}

            AttributeValueList_5_var(const AttributeValueList_5_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeValueList_5(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeValueList_5_var() { if (_pd_seq) delete _pd_seq; }

            AttributeValueList_5_var &operator=(AttributeValueList_5 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeValueList_5_var &operator=(const AttributeValueList_5_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeValueList_5;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeValue_5 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeValueList_5 *operator->() { return _pd_seq; }

            const AttributeValueList_5 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeValueList_5& () const { return *_pd_seq; }
#else

            operator const AttributeValueList_5 &() const { return *_pd_seq; }

            operator AttributeValueList_5 &() { return *_pd_seq; }

#endif

            const AttributeValueList_5 &in() const { return *_pd_seq; }

            AttributeValueList_5 &inout() { return *_pd_seq; }

            AttributeValueList_5 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeValueList_5 *_retn() {
                AttributeValueList_5 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeValueList_5_out;

        private:
            AttributeValueList_5 *_pd_seq;
        };

        class AttributeValueList_5_out {
        public:
            AttributeValueList_5_out(AttributeValueList_5 *&_s) : _data(_s) { _data = 0; }

            AttributeValueList_5_out(AttributeValueList_5_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeValueList_5 *) 0; }

            AttributeValueList_5_out(const AttributeValueList_5_out &_s) : _data(_s._data) {}

            AttributeValueList_5_out &operator=(const AttributeValueList_5_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeValueList_5_out &operator=(AttributeValueList_5 *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeValueList_5 *&() { return _data; }

            AttributeValueList_5 *&ptr() { return _data; }

            AttributeValueList_5 *operator->() { return _data; }

            AttributeValue_5 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeValueList_5 *&_data;

        private:
            AttributeValueList_5_out();

            AttributeValueList_5_out &operator=(const AttributeValueList_5_var &);
        };

        enum PipeWriteType {
            PIPE_READ, PIPE_READ_WRITE, PIPE_WT_UNKNOWN /*, __max_PipeWriteType=0xffffffff */ };
        typedef PipeWriteType &PipeWriteType_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_PipeWriteType;

        struct PipeConfig {
            typedef _CORBA_ConstrType_Variable_Var <PipeConfig> _var_type;


            ::CORBA::String_member name;

            ::CORBA::String_member description;

            ::CORBA::String_member label;

            DispLevel level;

            PipeWriteType writable;

            DevVarStringArray extensions;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef PipeConfig::_var_type PipeConfig_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <PipeConfig, PipeConfig_var> PipeConfig_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_PipeConfig;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_PipeConfigList;

        class PipeConfigList_var;

        class PipeConfigList : public _CORBA_Unbounded_Sequence<PipeConfig> {
        public:
            typedef PipeConfigList_var _var_type;

            PipeConfigList() {}

            PipeConfigList(const PipeConfigList &_s)
                    : _CORBA_Unbounded_Sequence<PipeConfig>(_s) {}

            PipeConfigList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<PipeConfig>(_max) {}

            PipeConfigList(uint32_t _max, uint32_t _len, PipeConfig *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<PipeConfig>(_max, _len, _val, _rel) {}


            PipeConfigList &operator=(const PipeConfigList &_s) {
                _CORBA_Unbounded_Sequence<PipeConfig>::operator=(_s);
                return *this;
            }
        };

        class PipeConfigList_out;

        class PipeConfigList_var {
        public:
            PipeConfigList_var() : _pd_seq(0) {}

            PipeConfigList_var(PipeConfigList *_s) : _pd_seq(_s) {}

            PipeConfigList_var(const PipeConfigList_var &_s) {
                if (_s._pd_seq) _pd_seq = new PipeConfigList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~PipeConfigList_var() { if (_pd_seq) delete _pd_seq; }

            PipeConfigList_var &operator=(PipeConfigList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            PipeConfigList_var &operator=(const PipeConfigList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new PipeConfigList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            PipeConfig &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            PipeConfigList *operator->() { return _pd_seq; }

            const PipeConfigList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator PipeConfigList& () const { return *_pd_seq; }
#else

            operator const PipeConfigList &() const { return *_pd_seq; }

            operator PipeConfigList &() { return *_pd_seq; }

#endif

            const PipeConfigList &in() const { return *_pd_seq; }

            PipeConfigList &inout() { return *_pd_seq; }

            PipeConfigList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            PipeConfigList *_retn() {
                PipeConfigList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class PipeConfigList_out;

        private:
            PipeConfigList *_pd_seq;
        };

        class PipeConfigList_out {
        public:
            PipeConfigList_out(PipeConfigList *&_s) : _data(_s) { _data = 0; }

            PipeConfigList_out(PipeConfigList_var &_s)
                    : _data(_s._pd_seq) { _s = (PipeConfigList *) 0; }

            PipeConfigList_out(const PipeConfigList_out &_s) : _data(_s._data) {}

            PipeConfigList_out &operator=(const PipeConfigList_out &_s) {
                _data = _s._data;
                return *this;
            }

            PipeConfigList_out &operator=(PipeConfigList *_s) {
                _data = _s;
                return *this;
            }

            operator PipeConfigList *&() { return _data; }

            PipeConfigList *&ptr() { return _data; }

            PipeConfigList *operator->() { return _data; }

            PipeConfig &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            PipeConfigList *&_data;

        private:
            PipeConfigList_out();

            PipeConfigList_out &operator=(const PipeConfigList_var &);
        };

        struct DevPipeDataElt;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevVarPipeDataEltArray;

        class DevVarPipeDataEltArray_var;

        class DevVarPipeDataEltArray : public _CORBA_Unbounded_Sequence_Forward<DevPipeDataElt> {
        public:
            typedef DevVarPipeDataEltArray_var _var_type;

            DevVarPipeDataEltArray() {}

            DevVarPipeDataEltArray(const DevVarPipeDataEltArray &_s);

            DevVarPipeDataEltArray &operator=(const DevVarPipeDataEltArray &_s);

            DevVarPipeDataEltArray(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_Forward<DevPipeDataElt>(_max) {}

            DevVarPipeDataEltArray(uint32_t _max, uint32_t _len, DevPipeDataElt *_val,
                                          bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_Forward<DevPipeDataElt>(_max, _len, _val, _rel) {}


            virtual ~DevVarPipeDataEltArray();

            DevPipeDataElt &operator[](uint32_t _index);

            const DevPipeDataElt &operator[](uint32_t _index) const;

            static DevPipeDataElt *allocbuf(uint32_t _nelems);

            static void freebuf(DevPipeDataElt *_b);

            void operator>>=(cdrStream &_s) const;

            void operator<<=(cdrStream &_s);

        protected:
            void NP_copybuffer(uint32_t _newmax);

            void NP_freebuf();
        };

        class DevVarPipeDataEltArray_out;

        class DevVarPipeDataEltArray_var {
        public:
            DevVarPipeDataEltArray_var() : _pd_seq(0) {}

            DevVarPipeDataEltArray_var(DevVarPipeDataEltArray *_s) : _pd_seq(_s) {}

            DevVarPipeDataEltArray_var(const DevVarPipeDataEltArray_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevVarPipeDataEltArray(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevVarPipeDataEltArray_var() { if (_pd_seq) delete _pd_seq; }

            DevVarPipeDataEltArray_var &operator=(DevVarPipeDataEltArray *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevVarPipeDataEltArray_var &operator=(const DevVarPipeDataEltArray_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevVarPipeDataEltArray;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevPipeDataElt &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevVarPipeDataEltArray *operator->() { return _pd_seq; }

            const DevVarPipeDataEltArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevVarPipeDataEltArray& () const { return *_pd_seq; }
#else

            operator const DevVarPipeDataEltArray &() const { return *_pd_seq; }

            operator DevVarPipeDataEltArray &() { return *_pd_seq; }

#endif

            const DevVarPipeDataEltArray &in() const { return *_pd_seq; }

            DevVarPipeDataEltArray &inout() { return *_pd_seq; }

            DevVarPipeDataEltArray *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevVarPipeDataEltArray *_retn() {
                DevVarPipeDataEltArray *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevVarPipeDataEltArray_out;

        private:
            DevVarPipeDataEltArray *_pd_seq;
        };

        class DevVarPipeDataEltArray_out {
        public:
            DevVarPipeDataEltArray_out(DevVarPipeDataEltArray *&_s) : _data(_s) { _data = 0; }

            DevVarPipeDataEltArray_out(DevVarPipeDataEltArray_var &_s)
                    : _data(_s._pd_seq) { _s = (DevVarPipeDataEltArray *) 0; }

            DevVarPipeDataEltArray_out(const DevVarPipeDataEltArray_out &_s) : _data(_s._data) {}

            DevVarPipeDataEltArray_out &operator=(const DevVarPipeDataEltArray_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevVarPipeDataEltArray_out &operator=(DevVarPipeDataEltArray *_s) {
                _data = _s;
                return *this;
            }

            operator DevVarPipeDataEltArray *&() { return _data; }

            DevVarPipeDataEltArray *&ptr() { return _data; }

            DevVarPipeDataEltArray *operator->() { return _data; }

            DevPipeDataElt &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevVarPipeDataEltArray *&_data;

        private:
            DevVarPipeDataEltArray_out();

            DevVarPipeDataEltArray_out &operator=(const DevVarPipeDataEltArray_var &);
        };

        struct DevPipeDataElt {
            typedef _CORBA_ConstrType_Variable_Var <DevPipeDataElt> _var_type;


            ::CORBA::String_member name;

            AttrValUnion value;

            DevVarPipeDataEltArray inner_blob;

            ::CORBA::String_member inner_blob_name;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevPipeDataElt::_var_type DevPipeDataElt_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevPipeDataElt, DevPipeDataElt_var> DevPipeDataElt_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevPipeDataElt;

        struct DevPipeBlob {
            typedef _CORBA_ConstrType_Variable_Var <DevPipeBlob> _var_type;


            ::CORBA::String_member name;

            DevVarPipeDataEltArray blob_data;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevPipeBlob::_var_type DevPipeBlob_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevPipeBlob, DevPipeBlob_var> DevPipeBlob_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevPipeBlob;

        struct DevPipeData {
            typedef _CORBA_ConstrType_Variable_Var <DevPipeData> _var_type;

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


            ::CORBA::String_member name;

            TimeVal time;

            DevPipeBlob data_blob;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevPipeData::_var_type DevPipeData_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevPipeData, DevPipeData_var> DevPipeData_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevPipeData;

        struct AttDataReady {
            typedef _CORBA_ConstrType_Variable_Var <AttDataReady> _var_type;


            ::CORBA::String_member name;

            int32_t data_type;

            int32_t ctr;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef AttDataReady::_var_type AttDataReady_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <AttDataReady, AttDataReady_var> AttDataReady_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttDataReady;

        struct DevIntrChange {
            typedef _CORBA_ConstrType_Variable_Var <DevIntrChange> _var_type;


            ::CORBA::Boolean dev_started;

            DevCmdInfoList_2 cmds;

            AttributeConfigList_5 atts;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevIntrChange::_var_type DevIntrChange_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevIntrChange, DevIntrChange_var> DevIntrChange_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevIntrChange;

        struct DevInfo {
            typedef _CORBA_ConstrType_Variable_Var <DevInfo> _var_type;


            ::CORBA::String_member dev_class;

            ::CORBA::String_member server_id;

            ::CORBA::String_member server_host;

            int32_t server_version;

            ::CORBA::String_member doc_url;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevInfo::_var_type DevInfo_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevInfo, DevInfo_var> DevInfo_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevInfo;

        struct DevInfo_3 {
            typedef _CORBA_ConstrType_Variable_Var <DevInfo_3> _var_type;


            ::CORBA::String_member dev_class;

            ::CORBA::String_member server_id;

            ::CORBA::String_member server_host;

            int32_t server_version;

            ::CORBA::String_member doc_url;

            ::CORBA::String_member dev_type;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevInfo_3::_var_type DevInfo_3_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevInfo_3, DevInfo_3_var> DevInfo_3_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevInfo_3;

        struct DevCmdHistory {
            typedef _CORBA_ConstrType_Variable_Var <DevCmdHistory> _var_type;


            TimeVal time;

            ::CORBA::Boolean cmd_failed;

            ::CORBA::Any value;

            DevErrorList errors;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevCmdHistory::_var_type DevCmdHistory_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevCmdHistory, DevCmdHistory_var> DevCmdHistory_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdHistory;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdHistoryList;

        class DevCmdHistoryList_var;

        class DevCmdHistoryList : public _CORBA_Unbounded_Sequence<DevCmdHistory> {
        public:
            typedef DevCmdHistoryList_var _var_type;

            DevCmdHistoryList() {}

            DevCmdHistoryList(const DevCmdHistoryList &_s)
                    : _CORBA_Unbounded_Sequence<DevCmdHistory>(_s) {}

            DevCmdHistoryList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevCmdHistory>(_max) {}

            DevCmdHistoryList(uint32_t _max, uint32_t _len, DevCmdHistory *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevCmdHistory>(_max, _len, _val, _rel) {}


            DevCmdHistoryList &operator=(const DevCmdHistoryList &_s) {
                _CORBA_Unbounded_Sequence<DevCmdHistory>::operator=(_s);
                return *this;
            }
        };

        class DevCmdHistoryList_out;

        class DevCmdHistoryList_var {
        public:
            DevCmdHistoryList_var() : _pd_seq(0) {}

            DevCmdHistoryList_var(DevCmdHistoryList *_s) : _pd_seq(_s) {}

            DevCmdHistoryList_var(const DevCmdHistoryList_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevCmdHistoryList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevCmdHistoryList_var() { if (_pd_seq) delete _pd_seq; }

            DevCmdHistoryList_var &operator=(DevCmdHistoryList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevCmdHistoryList_var &operator=(const DevCmdHistoryList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevCmdHistoryList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevCmdHistory &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevCmdHistoryList *operator->() { return _pd_seq; }

            const DevCmdHistoryList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevCmdHistoryList& () const { return *_pd_seq; }
#else

            operator const DevCmdHistoryList &() const { return *_pd_seq; }

            operator DevCmdHistoryList &() { return *_pd_seq; }

#endif

            const DevCmdHistoryList &in() const { return *_pd_seq; }

            DevCmdHistoryList &inout() { return *_pd_seq; }

            DevCmdHistoryList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevCmdHistoryList *_retn() {
                DevCmdHistoryList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevCmdHistoryList_out;

        private:
            DevCmdHistoryList *_pd_seq;
        };

        class DevCmdHistoryList_out {
        public:
            DevCmdHistoryList_out(DevCmdHistoryList *&_s) : _data(_s) { _data = 0; }

            DevCmdHistoryList_out(DevCmdHistoryList_var &_s)
                    : _data(_s._pd_seq) { _s = (DevCmdHistoryList *) 0; }

            DevCmdHistoryList_out(const DevCmdHistoryList_out &_s) : _data(_s._data) {}

            DevCmdHistoryList_out &operator=(const DevCmdHistoryList_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevCmdHistoryList_out &operator=(DevCmdHistoryList *_s) {
                _data = _s;
                return *this;
            }

            operator DevCmdHistoryList *&() { return _data; }

            DevCmdHistoryList *&ptr() { return _data; }

            DevCmdHistoryList *operator->() { return _data; }

            DevCmdHistory &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevCmdHistoryList *&_data;

        private:
            DevCmdHistoryList_out();

            DevCmdHistoryList_out &operator=(const DevCmdHistoryList_var &);
        };

        struct DevAttrHistory {
            typedef _CORBA_ConstrType_Variable_Var <DevAttrHistory> _var_type;


            ::CORBA::Boolean attr_failed;

            AttributeValue value;

            DevErrorList errors;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevAttrHistory::_var_type DevAttrHistory_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevAttrHistory, DevAttrHistory_var> DevAttrHistory_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistory;

        struct DevAttrHistory_3 {
            typedef _CORBA_ConstrType_Variable_Var <DevAttrHistory_3> _var_type;


            ::CORBA::Boolean attr_failed;

            AttributeValue_3 value;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevAttrHistory_3::_var_type DevAttrHistory_3_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevAttrHistory_3, DevAttrHistory_3_var> DevAttrHistory_3_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistory_3;

        struct EltInArray {
            typedef _CORBA_ConstrType_Fix_Var <EltInArray> _var_type;


            int32_t start;

            int32_t nb_elt;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef EltInArray::_var_type EltInArray_var;

        typedef EltInArray &EltInArray_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_EltInArray;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_EltInArrayList;

        class EltInArrayList_var;

        class EltInArrayList : public _CORBA_Unbounded_Sequence<EltInArray> {
        public:
            typedef EltInArrayList_var _var_type;

            EltInArrayList() {}

            EltInArrayList(const EltInArrayList &_s)
                    : _CORBA_Unbounded_Sequence<EltInArray>(_s) {}

            EltInArrayList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<EltInArray>(_max) {}

            EltInArrayList(uint32_t _max, uint32_t _len, EltInArray *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<EltInArray>(_max, _len, _val, _rel) {}


            EltInArrayList &operator=(const EltInArrayList &_s) {
                _CORBA_Unbounded_Sequence<EltInArray>::operator=(_s);
                return *this;
            }
        };

        class EltInArrayList_out;

        class EltInArrayList_var {
        public:
            EltInArrayList_var() : _pd_seq(0) {}

            EltInArrayList_var(EltInArrayList *_s) : _pd_seq(_s) {}

            EltInArrayList_var(const EltInArrayList_var &_s) {
                if (_s._pd_seq) _pd_seq = new EltInArrayList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~EltInArrayList_var() { if (_pd_seq) delete _pd_seq; }

            EltInArrayList_var &operator=(EltInArrayList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            EltInArrayList_var &operator=(const EltInArrayList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new EltInArrayList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            EltInArray &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            EltInArrayList *operator->() { return _pd_seq; }

            const EltInArrayList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator EltInArrayList& () const { return *_pd_seq; }
#else

            operator const EltInArrayList &() const { return *_pd_seq; }

            operator EltInArrayList &() { return *_pd_seq; }

#endif

            const EltInArrayList &in() const { return *_pd_seq; }

            EltInArrayList &inout() { return *_pd_seq; }

            EltInArrayList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            EltInArrayList *_retn() {
                EltInArrayList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class EltInArrayList_out;

        private:
            EltInArrayList *_pd_seq;
        };

        class EltInArrayList_out {
        public:
            EltInArrayList_out(EltInArrayList *&_s) : _data(_s) { _data = 0; }

            EltInArrayList_out(EltInArrayList_var &_s)
                    : _data(_s._pd_seq) { _s = (EltInArrayList *) 0; }

            EltInArrayList_out(const EltInArrayList_out &_s) : _data(_s._data) {}

            EltInArrayList_out &operator=(const EltInArrayList_out &_s) {
                _data = _s._data;
                return *this;
            }

            EltInArrayList_out &operator=(EltInArrayList *_s) {
                _data = _s;
                return *this;
            }

            operator EltInArrayList *&() { return _data; }

            EltInArrayList *&ptr() { return _data; }

            EltInArrayList *operator->() { return _data; }

            EltInArray &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            EltInArrayList *&_data;

        private:
            EltInArrayList_out();

            EltInArrayList_out &operator=(const EltInArrayList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_TimeValList;

        class TimeValList_var;

        class TimeValList : public _CORBA_Unbounded_Sequence<TimeVal> {
        public:
            typedef TimeValList_var _var_type;

            TimeValList() {}

            TimeValList(const TimeValList &_s)
                    : _CORBA_Unbounded_Sequence<TimeVal>(_s) {}

            TimeValList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<TimeVal>(_max) {}

            TimeValList(uint32_t _max, uint32_t _len, TimeVal *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<TimeVal>(_max, _len, _val, _rel) {}


            TimeValList &operator=(const TimeValList &_s) {
                _CORBA_Unbounded_Sequence<TimeVal>::operator=(_s);
                return *this;
            }
        };

        class TimeValList_out;

        class TimeValList_var {
        public:
            TimeValList_var() : _pd_seq(0) {}

            TimeValList_var(TimeValList *_s) : _pd_seq(_s) {}

            TimeValList_var(const TimeValList_var &_s) {
                if (_s._pd_seq) _pd_seq = new TimeValList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~TimeValList_var() { if (_pd_seq) delete _pd_seq; }

            TimeValList_var &operator=(TimeValList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            TimeValList_var &operator=(const TimeValList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new TimeValList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            TimeVal &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            TimeValList *operator->() { return _pd_seq; }

            const TimeValList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator TimeValList& () const { return *_pd_seq; }
#else

            operator const TimeValList &() const { return *_pd_seq; }

            operator TimeValList &() { return *_pd_seq; }

#endif

            const TimeValList &in() const { return *_pd_seq; }

            TimeValList &inout() { return *_pd_seq; }

            TimeValList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            TimeValList *_retn() {
                TimeValList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class TimeValList_out;

        private:
            TimeValList *_pd_seq;
        };

        class TimeValList_out {
        public:
            TimeValList_out(TimeValList *&_s) : _data(_s) { _data = 0; }

            TimeValList_out(TimeValList_var &_s)
                    : _data(_s._pd_seq) { _s = (TimeValList *) 0; }

            TimeValList_out(const TimeValList_out &_s) : _data(_s._data) {}

            TimeValList_out &operator=(const TimeValList_out &_s) {
                _data = _s._data;
                return *this;
            }

            TimeValList_out &operator=(TimeValList *_s) {
                _data = _s;
                return *this;
            }

            operator TimeValList *&() { return _data; }

            TimeValList *&ptr() { return _data; }

            TimeValList *operator->() { return _data; }

            TimeVal &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            TimeValList *&_data;

        private:
            TimeValList_out();

            TimeValList_out &operator=(const TimeValList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttrQualityList;

// Need to declare <<= for elem type, as GCC expands templates early
#if defined(__GNUG__) && __GNUG__ == 2 && __GNUC_MINOR__ == 7
                                                                                                                                friend void operator >>= (AttrQuality, cdrStream&);
   friend void operator <<= (AttrQuality&, cdrStream&);
#endif

        class AttrQualityList_var;

        class AttrQualityList : public _CORBA_Unbounded_Sequence_w_FixSizeElement<AttrQuality, 4, 4> {
        public:
            typedef AttrQualityList_var _var_type;

            AttrQualityList() {}

            AttrQualityList(const AttrQualityList &_s)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<AttrQuality, 4, 4>(_s) {}

            AttrQualityList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<AttrQuality, 4, 4>(_max) {}

            AttrQualityList(uint32_t _max, uint32_t _len, AttrQuality *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence_w_FixSizeElement<AttrQuality, 4, 4>(_max, _len, _val, _rel) {}


            AttrQualityList &operator=(const AttrQualityList &_s) {
                _CORBA_Unbounded_Sequence_w_FixSizeElement<AttrQuality, 4, 4>::operator=(_s);
                return *this;
            }
        };

        class AttrQualityList_out;

        class AttrQualityList_var {
        public:
            AttrQualityList_var() : _pd_seq(0) {}

            AttrQualityList_var(AttrQualityList *_s) : _pd_seq(_s) {}

            AttrQualityList_var(const AttrQualityList_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttrQualityList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttrQualityList_var() { if (_pd_seq) delete _pd_seq; }

            AttrQualityList_var &operator=(AttrQualityList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttrQualityList_var &operator=(const AttrQualityList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttrQualityList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttrQuality &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttrQualityList *operator->() { return _pd_seq; }

            const AttrQualityList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttrQualityList& () const { return *_pd_seq; }
#else

            operator const AttrQualityList &() const { return *_pd_seq; }

            operator AttrQualityList &() { return *_pd_seq; }

#endif

            const AttrQualityList &in() const { return *_pd_seq; }

            AttrQualityList &inout() { return *_pd_seq; }

            AttrQualityList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttrQualityList *_retn() {
                AttrQualityList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttrQualityList_out;

        private:
            AttrQualityList *_pd_seq;
        };

        class AttrQualityList_out {
        public:
            AttrQualityList_out(AttrQualityList *&_s) : _data(_s) { _data = 0; }

            AttrQualityList_out(AttrQualityList_var &_s)
                    : _data(_s._pd_seq) { _s = (AttrQualityList *) 0; }

            AttrQualityList_out(const AttrQualityList_out &_s) : _data(_s._data) {}

            AttrQualityList_out &operator=(const AttrQualityList_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttrQualityList_out &operator=(AttrQualityList *_s) {
                _data = _s;
                return *this;
            }

            operator AttrQualityList *&() { return _data; }

            AttrQualityList *&ptr() { return _data; }

            AttrQualityList *operator->() { return _data; }

            AttrQuality &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttrQualityList *&_data;

        private:
            AttrQualityList_out();

            AttrQualityList_out &operator=(const AttrQualityList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_AttributeDimList;

        class AttributeDimList_var;

        class AttributeDimList : public _CORBA_Unbounded_Sequence<AttributeDim> {
        public:
            typedef AttributeDimList_var _var_type;

            AttributeDimList() {}

            AttributeDimList(const AttributeDimList &_s)
                    : _CORBA_Unbounded_Sequence<AttributeDim>(_s) {}

            AttributeDimList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<AttributeDim>(_max) {}

            AttributeDimList(uint32_t _max, uint32_t _len, AttributeDim *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<AttributeDim>(_max, _len, _val, _rel) {}


            AttributeDimList &operator=(const AttributeDimList &_s) {
                _CORBA_Unbounded_Sequence<AttributeDim>::operator=(_s);
                return *this;
            }
        };

        class AttributeDimList_out;

        class AttributeDimList_var {
        public:
            AttributeDimList_var() : _pd_seq(0) {}

            AttributeDimList_var(AttributeDimList *_s) : _pd_seq(_s) {}

            AttributeDimList_var(const AttributeDimList_var &_s) {
                if (_s._pd_seq) _pd_seq = new AttributeDimList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~AttributeDimList_var() { if (_pd_seq) delete _pd_seq; }

            AttributeDimList_var &operator=(AttributeDimList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            AttributeDimList_var &operator=(const AttributeDimList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new AttributeDimList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            AttributeDim &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            AttributeDimList *operator->() { return _pd_seq; }

            const AttributeDimList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator AttributeDimList& () const { return *_pd_seq; }
#else

            operator const AttributeDimList &() const { return *_pd_seq; }

            operator AttributeDimList &() { return *_pd_seq; }

#endif

            const AttributeDimList &in() const { return *_pd_seq; }

            AttributeDimList &inout() { return *_pd_seq; }

            AttributeDimList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            AttributeDimList *_retn() {
                AttributeDimList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class AttributeDimList_out;

        private:
            AttributeDimList *_pd_seq;
        };

        class AttributeDimList_out {
        public:
            AttributeDimList_out(AttributeDimList *&_s) : _data(_s) { _data = 0; }

            AttributeDimList_out(AttributeDimList_var &_s)
                    : _data(_s._pd_seq) { _s = (AttributeDimList *) 0; }

            AttributeDimList_out(const AttributeDimList_out &_s) : _data(_s._data) {}

            AttributeDimList_out &operator=(const AttributeDimList_out &_s) {
                _data = _s._data;
                return *this;
            }

            AttributeDimList_out &operator=(AttributeDimList *_s) {
                _data = _s;
                return *this;
            }

            operator AttributeDimList *&() { return _data; }

            AttributeDimList *&ptr() { return _data; }

            AttributeDimList *operator->() { return _data; }

            AttributeDim &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            AttributeDimList *&_data;

        private:
            AttributeDimList_out();

            AttributeDimList_out &operator=(const AttributeDimList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevErrorListList;

        class DevErrorListList_var;

        class DevErrorListList : public _CORBA_Unbounded_Sequence<DevErrorList> {
        public:
            typedef DevErrorListList_var _var_type;

            DevErrorListList() {}

            DevErrorListList(const DevErrorListList &_s)
                    : _CORBA_Unbounded_Sequence<DevErrorList>(_s) {}

            DevErrorListList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevErrorList>(_max) {}

            DevErrorListList(uint32_t _max, uint32_t _len, DevErrorList *_val, bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevErrorList>(_max, _len, _val, _rel) {}


            DevErrorListList &operator=(const DevErrorListList &_s) {
                _CORBA_Unbounded_Sequence<DevErrorList>::operator=(_s);
                return *this;
            }
        };

        class DevErrorListList_out;

        class DevErrorListList_var {
        public:
            DevErrorListList_var() : _pd_seq(0) {}

            DevErrorListList_var(DevErrorListList *_s) : _pd_seq(_s) {}

            DevErrorListList_var(const DevErrorListList_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevErrorListList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevErrorListList_var() { if (_pd_seq) delete _pd_seq; }

            DevErrorListList_var &operator=(DevErrorListList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevErrorListList_var &operator=(const DevErrorListList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevErrorListList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevErrorList &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevErrorListList *operator->() { return _pd_seq; }

            const DevErrorListList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevErrorListList& () const { return *_pd_seq; }
#else

            operator const DevErrorListList &() const { return *_pd_seq; }

            operator DevErrorListList &() { return *_pd_seq; }

#endif

            const DevErrorListList &in() const { return *_pd_seq; }

            DevErrorListList &inout() { return *_pd_seq; }

            DevErrorListList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevErrorListList *_retn() {
                DevErrorListList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevErrorListList_out;

        private:
            DevErrorListList *_pd_seq;
        };

        class DevErrorListList_out {
        public:
            DevErrorListList_out(DevErrorListList *&_s) : _data(_s) { _data = 0; }

            DevErrorListList_out(DevErrorListList_var &_s)
                    : _data(_s._pd_seq) { _s = (DevErrorListList *) 0; }

            DevErrorListList_out(const DevErrorListList_out &_s) : _data(_s._data) {}

            DevErrorListList_out &operator=(const DevErrorListList_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevErrorListList_out &operator=(DevErrorListList *_s) {
                _data = _s;
                return *this;
            }

            operator DevErrorListList *&() { return _data; }

            DevErrorListList *&ptr() { return _data; }

            DevErrorListList *operator->() { return _data; }

            DevErrorList &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevErrorListList *&_data;

        private:
            DevErrorListList_out();

            DevErrorListList_out &operator=(const DevErrorListList_var &);
        };

        struct DevAttrHistory_4 {
            typedef _CORBA_ConstrType_Variable_Var <DevAttrHistory_4> _var_type;


            ::CORBA::String_member name;

            TimeValList dates;

            ::CORBA::Any value;

            AttrQualityList quals;

            EltInArrayList quals_array;

            AttributeDimList r_dims;

            EltInArrayList r_dims_array;

            AttributeDimList w_dims;

            EltInArrayList w_dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevAttrHistory_4::_var_type DevAttrHistory_4_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevAttrHistory_4, DevAttrHistory_4_var> DevAttrHistory_4_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistory_4;

        struct DevAttrHistory_5 {
            typedef _CORBA_ConstrType_Variable_Var <DevAttrHistory_5> _var_type;


            ::CORBA::String_member name;

            AttrDataFormat data_format;

            int32_t data_type;

            TimeValList dates;

            ::CORBA::Any value;

            AttrQualityList quals;

            EltInArrayList quals_array;

            AttributeDimList r_dims;

            EltInArrayList r_dims_array;

            AttributeDimList w_dims;

            EltInArrayList w_dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevAttrHistory_5::_var_type DevAttrHistory_5_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevAttrHistory_5, DevAttrHistory_5_var> DevAttrHistory_5_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistory_5;

        struct DevCmdHistory_4 {
            typedef _CORBA_ConstrType_Variable_Var <DevCmdHistory_4> _var_type;


            TimeValList dates;

            ::CORBA::Any value;

            AttributeDimList dims;

            EltInArrayList dims_array;

            DevErrorListList errors;

            EltInArrayList errors_array;

            int32_t cmd_type;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef DevCmdHistory_4::_var_type DevCmdHistory_4_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <DevCmdHistory_4, DevCmdHistory_4_var> DevCmdHistory_4_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevCmdHistory_4;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistoryList;

        class DevAttrHistoryList_var;

        class DevAttrHistoryList : public _CORBA_Unbounded_Sequence<DevAttrHistory> {
        public:
            typedef DevAttrHistoryList_var _var_type;

            DevAttrHistoryList() {}

            DevAttrHistoryList(const DevAttrHistoryList &_s)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory>(_s) {}

            DevAttrHistoryList(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory>(_max) {}

            DevAttrHistoryList(uint32_t _max, uint32_t _len, DevAttrHistory *_val,
                                      bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory>(_max, _len, _val, _rel) {}


            DevAttrHistoryList &operator=(const DevAttrHistoryList &_s) {
                _CORBA_Unbounded_Sequence<DevAttrHistory>::operator=(_s);
                return *this;
            }
        };

        class DevAttrHistoryList_out;

        class DevAttrHistoryList_var {
        public:
            DevAttrHistoryList_var() : _pd_seq(0) {}

            DevAttrHistoryList_var(DevAttrHistoryList *_s) : _pd_seq(_s) {}

            DevAttrHistoryList_var(const DevAttrHistoryList_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevAttrHistoryList(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevAttrHistoryList_var() { if (_pd_seq) delete _pd_seq; }

            DevAttrHistoryList_var &operator=(DevAttrHistoryList *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevAttrHistoryList_var &operator=(const DevAttrHistoryList_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevAttrHistoryList;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevAttrHistory &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevAttrHistoryList *operator->() { return _pd_seq; }

            const DevAttrHistoryList *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevAttrHistoryList& () const { return *_pd_seq; }
#else

            operator const DevAttrHistoryList &() const { return *_pd_seq; }

            operator DevAttrHistoryList &() { return *_pd_seq; }

#endif

            const DevAttrHistoryList &in() const { return *_pd_seq; }

            DevAttrHistoryList &inout() { return *_pd_seq; }

            DevAttrHistoryList *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevAttrHistoryList *_retn() {
                DevAttrHistoryList *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevAttrHistoryList_out;

        private:
            DevAttrHistoryList *_pd_seq;
        };

        class DevAttrHistoryList_out {
        public:
            DevAttrHistoryList_out(DevAttrHistoryList *&_s) : _data(_s) { _data = 0; }

            DevAttrHistoryList_out(DevAttrHistoryList_var &_s)
                    : _data(_s._pd_seq) { _s = (DevAttrHistoryList *) 0; }

            DevAttrHistoryList_out(const DevAttrHistoryList_out &_s) : _data(_s._data) {}

            DevAttrHistoryList_out &operator=(const DevAttrHistoryList_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevAttrHistoryList_out &operator=(DevAttrHistoryList *_s) {
                _data = _s;
                return *this;
            }

            operator DevAttrHistoryList *&() { return _data; }

            DevAttrHistoryList *&ptr() { return _data; }

            DevAttrHistoryList *operator->() { return _data; }

            DevAttrHistory &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevAttrHistoryList *&_data;

        private:
            DevAttrHistoryList_out();

            DevAttrHistoryList_out &operator=(const DevAttrHistoryList_var &);
        };

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_DevAttrHistoryList_3;

        class DevAttrHistoryList_3_var;

        class DevAttrHistoryList_3 : public _CORBA_Unbounded_Sequence<DevAttrHistory_3> {
        public:
            typedef DevAttrHistoryList_3_var _var_type;

            DevAttrHistoryList_3() {}

            DevAttrHistoryList_3(const DevAttrHistoryList_3 &_s)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory_3>(_s) {}

            DevAttrHistoryList_3(uint32_t _max)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory_3>(_max) {}

            DevAttrHistoryList_3(uint32_t _max, uint32_t _len, DevAttrHistory_3 *_val,
                                        bool _rel = 0)
                    : _CORBA_Unbounded_Sequence<DevAttrHistory_3>(_max, _len, _val, _rel) {}


            DevAttrHistoryList_3 &operator=(const DevAttrHistoryList_3 &_s) {
                _CORBA_Unbounded_Sequence<DevAttrHistory_3>::operator=(_s);
                return *this;
            }
        };

        class DevAttrHistoryList_3_out;

        class DevAttrHistoryList_3_var {
        public:
            DevAttrHistoryList_3_var() : _pd_seq(0) {}

            DevAttrHistoryList_3_var(DevAttrHistoryList_3 *_s) : _pd_seq(_s) {}

            DevAttrHistoryList_3_var(const DevAttrHistoryList_3_var &_s) {
                if (_s._pd_seq) _pd_seq = new DevAttrHistoryList_3(*_s._pd_seq);
                else _pd_seq = 0;
            }

            ~DevAttrHistoryList_3_var() { if (_pd_seq) delete _pd_seq; }

            DevAttrHistoryList_3_var &operator=(DevAttrHistoryList_3 *_s) {
                if (_pd_seq) delete _pd_seq;
                _pd_seq = _s;
                return *this;
            }

            DevAttrHistoryList_3_var &operator=(const DevAttrHistoryList_3_var &_s) {
                if (_s._pd_seq) {
                    if (!_pd_seq) _pd_seq = new DevAttrHistoryList_3;
                    *_pd_seq = *_s._pd_seq;
                } else if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return *this;
            }

            DevAttrHistory_3 &operator[](uint32_t _s) {
                return (*_pd_seq)[_s];
            }


            DevAttrHistoryList_3 *operator->() { return _pd_seq; }

            const DevAttrHistoryList_3 *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
            operator DevAttrHistoryList_3& () const { return *_pd_seq; }
#else

            operator const DevAttrHistoryList_3 &() const { return *_pd_seq; }

            operator DevAttrHistoryList_3 &() { return *_pd_seq; }

#endif

            const DevAttrHistoryList_3 &in() const { return *_pd_seq; }

            DevAttrHistoryList_3 &inout() { return *_pd_seq; }

            DevAttrHistoryList_3 *&out() {
                if (_pd_seq) {
                    delete _pd_seq;
                    _pd_seq = 0;
                }
                return _pd_seq;
            }

            DevAttrHistoryList_3 *_retn() {
                DevAttrHistoryList_3 *tmp = _pd_seq;
                _pd_seq = 0;
                return tmp;
            }

            friend class DevAttrHistoryList_3_out;

        private:
            DevAttrHistoryList_3 *_pd_seq;
        };

        class DevAttrHistoryList_3_out {
        public:
            DevAttrHistoryList_3_out(DevAttrHistoryList_3 *&_s) : _data(_s) { _data = 0; }

            DevAttrHistoryList_3_out(DevAttrHistoryList_3_var &_s)
                    : _data(_s._pd_seq) { _s = (DevAttrHistoryList_3 *) 0; }

            DevAttrHistoryList_3_out(const DevAttrHistoryList_3_out &_s) : _data(_s._data) {}

            DevAttrHistoryList_3_out &operator=(const DevAttrHistoryList_3_out &_s) {
                _data = _s._data;
                return *this;
            }

            DevAttrHistoryList_3_out &operator=(DevAttrHistoryList_3 *_s) {
                _data = _s;
                return *this;
            }

            operator DevAttrHistoryList_3 *&() { return _data; }

            DevAttrHistoryList_3 *&ptr() { return _data; }

            DevAttrHistoryList_3 *operator->() { return _data; }

            DevAttrHistory_3 &operator[](uint32_t _i) {
                return (*_data)[_i];
            }


            DevAttrHistoryList_3 *&_data;

        private:
            DevAttrHistoryList_3_out();

            DevAttrHistoryList_3_out &operator=(const DevAttrHistoryList_3_var &);
        };

        struct ZmqCallInfo {
            typedef _CORBA_ConstrType_Variable_Var <ZmqCallInfo> _var_type;


            int32_t version;

            ::CORBA::ULong ctr;

            ::CORBA::String_member method_name;

            DevVarCharArray oid;

            ::CORBA::Boolean call_is_except;


            void operator>>=(cdrStream &) const;

            void operator<<=(cdrStream &);
        };

        typedef ZmqCallInfo::_var_type ZmqCallInfo_var;

        typedef _CORBA_ConstrType_Variable_OUT_arg <ZmqCallInfo, ZmqCallInfo_var> ZmqCallInfo_out;

        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_ZmqCallInfo;

#ifndef __Tango_mDevice__
#define __Tango_mDevice__

        class Device;

        class _objref_Device;

        class _impl_Device;

        typedef _objref_Device *Device_ptr;
        typedef Device_ptr DeviceRef;

        class Device_Helper {
        public:
            typedef Device_ptr _ptr_type;

            static _ptr_type _nil();

            static bool is_nil(_ptr_type);

            static void release(_ptr_type);

            static void duplicate(_ptr_type);

            static void marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type unmarshalObjRef(cdrStream &);
        };

        typedef _CORBA_ObjRef_Var <_objref_Device, Device_Helper> Device_var;
        typedef _CORBA_ObjRef_OUT_arg <_objref_Device, Device_Helper> Device_out;

#endif

// interface Device
        class Device {
        public:
            // Declarations for this interface type.
            typedef Device_ptr _ptr_type;
            typedef Device_var _var_type;

            static _ptr_type _duplicate(_ptr_type);

            static _ptr_type _narrow(::CORBA::Object_ptr);

            static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);

            static _ptr_type _nil();

            static void _marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type _unmarshalObjRef(cdrStream &s) {
                omniObjRef *o = omniObjRef::_unMarshal(_PD_repoId, s);
                if (o)
                    return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
                else
                    return _nil();
            }

            static _core_attr const char *_PD_repoId;

            // Other IDL defined within this scope.

        };

        class _objref_Device :
                public virtual ::CORBA::Object,
                public virtual omniObjRef {
        public:
            ::CORBA::Any *command_inout(const char *command, const ::CORBA::Any &argin);

            AttributeConfigList *get_attribute_config(const ::Tango::DevVarStringArray &names);

            void set_attribute_config(const ::Tango::AttributeConfigList &new_conf);

            AttributeValueList *read_attributes(const ::Tango::DevVarStringArray &names);

            void write_attributes(const ::Tango::AttributeValueList &values);

            void ping();

            DevVarStringArray *black_box(int32_t n);

            DevInfo *info();

            DevCmdInfoList *command_list_query();

            DevCmdInfo *command_query(const char *command);

            char *name();

            char *description();

            DevState state();

            char *status();

            char *adm_name();

            _objref_Device() { _PR_setobj(0); }  // nil
            _objref_Device(omniIOR *, omniIdentity *);

        protected:
            virtual ~_objref_Device();


        private:
            virtual void *_ptrToObjRef(const char *);

            _objref_Device(const _objref_Device &);

            _objref_Device &operator=(const _objref_Device &);
            // not implemented

            friend class Device;
        };

        class _pof_Device : public _OMNI_NS(proxyObjectFactory) {
                public:
                _pof_Device() : _OMNI_NS(proxyObjectFactory)(Device::_PD_repoId) {}
                virtual ~_pof_Device();

                virtual omniObjRef* newObjRef(omniIOR*, omniIdentity*);
                virtual bool is_a(const char*) const;
        };

        class _impl_Device :
                public virtual omniServant {
        public:
            virtual ~_impl_Device();

            virtual ::CORBA::Any *command_inout(const char *command, const ::CORBA::Any &argin) = 0;

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

        public:  // Really protected, workaround for xlC
            virtual bool _dispatch(omniCallHandle &);

        private:
            virtual void *_ptrToInterface(const char *);

            virtual const char *_mostDerivedRepoId();

        };


        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_Device;

#ifndef __Tango_mDevice__2__
#define __Tango_mDevice__2__

        class Device_2;

        class _objref_Device_2;

        class _impl_Device_2;

        typedef _objref_Device_2 *Device_2_ptr;
        typedef Device_2_ptr Device_2Ref;

        class Device_2_Helper {
        public:
            typedef Device_2_ptr _ptr_type;

            static _ptr_type _nil();

            static bool is_nil(_ptr_type);

            static void release(_ptr_type);

            static void duplicate(_ptr_type);

            static void marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type unmarshalObjRef(cdrStream &);
        };

        typedef _CORBA_ObjRef_Var <_objref_Device_2, Device_2_Helper> Device_2_var;
        typedef _CORBA_ObjRef_OUT_arg <_objref_Device_2, Device_2_Helper> Device_2_out;

#endif

// interface Device_2
        class Device_2 {
        public:
            // Declarations for this interface type.
            typedef Device_2_ptr _ptr_type;
            typedef Device_2_var _var_type;

            static _ptr_type _duplicate(_ptr_type);

            static _ptr_type _narrow(::CORBA::Object_ptr);

            static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);

            static _ptr_type _nil();

            static void _marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type _unmarshalObjRef(cdrStream &s) {
                omniObjRef *o = omniObjRef::_unMarshal(_PD_repoId, s);
                if (o)
                    return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
                else
                    return _nil();
            }

            static _core_attr const char *_PD_repoId;

            // Other IDL defined within this scope.

        };

        class _objref_Device_2 :
                public virtual _objref_Device {
        public:
            ::CORBA::Any *command_inout_2(const char *command, const ::CORBA::Any &argin, ::Tango::DevSource source);

            AttributeValueList *read_attributes_2(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source);

            AttributeConfigList_2 *get_attribute_config_2(const ::Tango::DevVarStringArray &names);

            DevCmdInfoList_2 *command_list_query_2();

            DevCmdInfo_2 *command_query_2(const char *command);

            DevCmdHistoryList *command_inout_history_2(const char *command, int32_t n);

            DevAttrHistoryList *read_attribute_history_2(const char *name, int32_t n);

            _objref_Device_2() { _PR_setobj(0); }  // nil
            _objref_Device_2(omniIOR *, omniIdentity *);

        protected:
            virtual ~_objref_Device_2();


        private:
            virtual void *_ptrToObjRef(const char *);

            _objref_Device_2(const _objref_Device_2 &);

            _objref_Device_2 &operator=(const _objref_Device_2 &);
            // not implemented

            friend class Device_2;
        };

        class _pof_Device_2 : public _OMNI_NS(proxyObjectFactory) {
                public:
                _pof_Device_2() : _OMNI_NS(proxyObjectFactory)(Device_2::_PD_repoId) {}
                virtual ~_pof_Device_2();

                virtual omniObjRef* newObjRef(omniIOR*, omniIdentity*);
                virtual bool is_a(const char*) const;
        };

        class _impl_Device_2 :
                public virtual _impl_Device {
        public:
            virtual ~_impl_Device_2();

            virtual ::CORBA::Any *
            command_inout_2(const char *command, const ::CORBA::Any &argin, ::Tango::DevSource source) = 0;

            virtual AttributeValueList *
            read_attributes_2(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source) = 0;

            virtual AttributeConfigList_2 *get_attribute_config_2(const ::Tango::DevVarStringArray &names) = 0;

            virtual DevCmdInfoList_2 *command_list_query_2() = 0;

            virtual DevCmdInfo_2 *command_query_2(const char *command) = 0;

            virtual DevCmdHistoryList *command_inout_history_2(const char *command, int32_t n) = 0;

            virtual DevAttrHistoryList *read_attribute_history_2(const char *name, int32_t n) = 0;

        public:  // Really protected, workaround for xlC
            virtual bool _dispatch(omniCallHandle &);

        private:
            virtual void *_ptrToInterface(const char *);

            virtual const char *_mostDerivedRepoId();

        };


        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_Device_2;

#ifndef __Tango_mDevice__3__
#define __Tango_mDevice__3__

        class Device_3;

        class _objref_Device_3;

        class _impl_Device_3;

        typedef _objref_Device_3 *Device_3_ptr;
        typedef Device_3_ptr Device_3Ref;

        class Device_3_Helper {
        public:
            typedef Device_3_ptr _ptr_type;

            static _ptr_type _nil();

            static bool is_nil(_ptr_type);

            static void release(_ptr_type);

            static void duplicate(_ptr_type);

            static void marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type unmarshalObjRef(cdrStream &);
        };

        typedef _CORBA_ObjRef_Var <_objref_Device_3, Device_3_Helper> Device_3_var;
        typedef _CORBA_ObjRef_OUT_arg <_objref_Device_3, Device_3_Helper> Device_3_out;

#endif

// interface Device_3
        class Device_3 {
        public:
            // Declarations for this interface type.
            typedef Device_3_ptr _ptr_type;
            typedef Device_3_var _var_type;

            static _ptr_type _duplicate(_ptr_type);

            static _ptr_type _narrow(::CORBA::Object_ptr);

            static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);

            static _ptr_type _nil();

            static void _marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type _unmarshalObjRef(cdrStream &s) {
                omniObjRef *o = omniObjRef::_unMarshal(_PD_repoId, s);
                if (o)
                    return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
                else
                    return _nil();
            }

            static _core_attr const char *_PD_repoId;

            // Other IDL defined within this scope.

        };

        class _objref_Device_3 :
                public virtual _objref_Device_2 {
        public:
            AttributeValueList_3 *read_attributes_3(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source);

            void write_attributes_3(const ::Tango::AttributeValueList &values);

            DevAttrHistoryList_3 *read_attribute_history_3(const char *name, int32_t n);

            DevInfo_3 *info_3();

            AttributeConfigList_3 *get_attribute_config_3(const ::Tango::DevVarStringArray &names);

            void set_attribute_config_3(const ::Tango::AttributeConfigList_3 &new_conf);

            _objref_Device_3() { _PR_setobj(0); }  // nil
            _objref_Device_3(omniIOR *, omniIdentity *);

        protected:
            virtual ~_objref_Device_3();


        private:
            virtual void *_ptrToObjRef(const char *);

            _objref_Device_3(const _objref_Device_3 &);

            _objref_Device_3 &operator=(const _objref_Device_3 &);
            // not implemented

            friend class Device_3;
        };

        class _pof_Device_3 : public _OMNI_NS(proxyObjectFactory) {
                public:
                _pof_Device_3() : _OMNI_NS(proxyObjectFactory)(Device_3::_PD_repoId) {}
                virtual ~_pof_Device_3();

                virtual omniObjRef* newObjRef(omniIOR*, omniIdentity*);
                virtual bool is_a(const char*) const;
        };

        class _impl_Device_3 :
                public virtual _impl_Device_2 {
        public:
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


        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_Device_3;

#ifndef __Tango_mDevice__4__
#define __Tango_mDevice__4__

        class Device_4;

        class _objref_Device_4;

        class _impl_Device_4;

        typedef _objref_Device_4 *Device_4_ptr;
        typedef Device_4_ptr Device_4Ref;

        class Device_4_Helper {
        public:
            typedef Device_4_ptr _ptr_type;

            static _ptr_type _nil();

            static bool is_nil(_ptr_type);

            static void release(_ptr_type);

            static void duplicate(_ptr_type);

            static void marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type unmarshalObjRef(cdrStream &);
        };

        typedef _CORBA_ObjRef_Var <_objref_Device_4, Device_4_Helper> Device_4_var;
        typedef _CORBA_ObjRef_OUT_arg <_objref_Device_4, Device_4_Helper> Device_4_out;

#endif

// interface Device_4
        class Device_4 {
        public:
            // Declarations for this interface type.
            typedef Device_4_ptr _ptr_type;
            typedef Device_4_var _var_type;

            static _ptr_type _duplicate(_ptr_type);

            static _ptr_type _narrow(::CORBA::Object_ptr);

            static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);

            static _ptr_type _nil();

            static void _marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type _unmarshalObjRef(cdrStream &s) {
                omniObjRef *o = omniObjRef::_unMarshal(_PD_repoId, s);
                if (o)
                    return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
                else
                    return _nil();
            }

            static _core_attr const char *_PD_repoId;

            // Other IDL defined within this scope.

        };

        class _objref_Device_4 :
                public virtual _objref_Device_3 {
        public:
            DevAttrHistory_4 *read_attribute_history_4(const char *name, int32_t n);

            DevCmdHistory_4 *command_inout_history_4(const char *command, int32_t n);

            ::CORBA::Any *command_inout_4(const char *command, const ::CORBA::Any &argin, ::Tango::DevSource source,
                                          const ::Tango::ClntIdent &cl_ident);

            AttributeValueList_4 *read_attributes_4(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                                                    const ::Tango::ClntIdent &cl_ident);

            void write_attributes_4(const ::Tango::AttributeValueList_4 &values, const ::Tango::ClntIdent &cl_ident);

            void
            set_attribute_config_4(const ::Tango::AttributeConfigList_3 &new_conf, const ::Tango::ClntIdent &cl_ident);

            AttributeValueList_4 *
            write_read_attributes_4(const ::Tango::AttributeValueList_4 &values, const ::Tango::ClntIdent &cl_ident);

            _objref_Device_4() { _PR_setobj(0); }  // nil
            _objref_Device_4(omniIOR *, omniIdentity *);

        protected:
            virtual ~_objref_Device_4();


        private:
            virtual void *_ptrToObjRef(const char *);

            _objref_Device_4(const _objref_Device_4 &);

            _objref_Device_4 &operator=(const _objref_Device_4 &);
            // not implemented

            friend class Device_4;
        };

        class _pof_Device_4 : public _OMNI_NS(proxyObjectFactory) {
                public:
                _pof_Device_4() : _OMNI_NS(proxyObjectFactory)(Device_4::_PD_repoId) {}
                virtual ~_pof_Device_4();

                virtual omniObjRef* newObjRef(omniIOR*, omniIdentity*);
                virtual bool is_a(const char*) const;
        };

        class _impl_Device_4 :
                public virtual _impl_Device_3 {
        public:
            virtual ~_impl_Device_4();

            virtual DevAttrHistory_4 *read_attribute_history_4(const char *name, int32_t n) = 0;

            virtual DevCmdHistory_4 *command_inout_history_4(const char *command, int32_t n) = 0;

            virtual ::CORBA::Any *
            command_inout_4(const char *command, const ::CORBA::Any &argin, ::Tango::DevSource source,
                            const ::Tango::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_4 *
            read_attributes_4(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                              const ::Tango::ClntIdent &cl_ident) = 0;

            virtual void
            write_attributes_4(const ::Tango::AttributeValueList_4 &values, const ::Tango::ClntIdent &cl_ident) = 0;

            virtual void
            set_attribute_config_4(const ::Tango::AttributeConfigList_3 &new_conf,
                                   const ::Tango::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_4 *
            write_read_attributes_4(const ::Tango::AttributeValueList_4 &values,
                                    const ::Tango::ClntIdent &cl_ident) = 0;

        public:  // Really protected, workaround for xlC
            virtual bool _dispatch(omniCallHandle &);

        private:
            virtual void *_ptrToInterface(const char *);

            virtual const char *_mostDerivedRepoId();

        };


        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_Device_4;

#ifndef __Tango_mDevice__5__
#define __Tango_mDevice__5__

        class Device_5;

        class _objref_Device_5;

        class _impl_Device_5;

        typedef _objref_Device_5 *Device_5_ptr;
        typedef Device_5_ptr Device_5Ref;

        class Device_5_Helper {
        public:
            typedef Device_5_ptr _ptr_type;

            static _ptr_type _nil();

            static bool is_nil(_ptr_type);

            static void release(_ptr_type);

            static void duplicate(_ptr_type);

            static void marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type unmarshalObjRef(cdrStream &);
        };

        typedef _CORBA_ObjRef_Var <_objref_Device_5, Device_5_Helper> Device_5_var;
        typedef _CORBA_ObjRef_OUT_arg <_objref_Device_5, Device_5_Helper> Device_5_out;

#endif

// interface Device_5
        class Device_5 {
        public:
            // Declarations for this interface type.
            typedef Device_5_ptr _ptr_type;
            typedef Device_5_var _var_type;

            static _ptr_type _duplicate(_ptr_type);

            static _ptr_type _narrow(::CORBA::Object_ptr);

            static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);

            static _ptr_type _nil();

            static void _marshalObjRef(_ptr_type, cdrStream &);

            static _ptr_type _unmarshalObjRef(cdrStream &s) {
                omniObjRef *o = omniObjRef::_unMarshal(_PD_repoId, s);
                if (o)
                    return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
                else
                    return _nil();
            }

            static _core_attr const char *_PD_repoId;

            // Other IDL defined within this scope.

        };

        class _objref_Device_5 :
                public virtual _objref_Device_4 {
        public:
            AttributeConfigList_5 *get_attribute_config_5(const ::Tango::DevVarStringArray &names);

            void
            set_attribute_config_5(const ::Tango::AttributeConfigList_5 &new_conf, const ::Tango::ClntIdent &cl_ident);

            AttributeValueList_5 *read_attributes_5(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                                                    const ::Tango::ClntIdent &cl_ident);

            AttributeValueList_5 *
            write_read_attributes_5(const ::Tango::AttributeValueList_4 &values,
                                    const ::Tango::DevVarStringArray &r_names,
                                    const ::Tango::ClntIdent &cl_ident);

            DevAttrHistory_5 *read_attribute_history_5(const char *name, int32_t n);

            PipeConfigList *get_pipe_config_5(const ::Tango::DevVarStringArray &names);

            void set_pipe_config_5(const ::Tango::PipeConfigList &new_conf, const ::Tango::ClntIdent &cl_ident);

            DevPipeData *read_pipe_5(const char *name, const ::Tango::ClntIdent &cl_ident);

            void write_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident);

            DevPipeData *write_read_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident);

            _objref_Device_5() { _PR_setobj(0); }  // nil
            _objref_Device_5(omniIOR *, omniIdentity *);

        protected:
            virtual ~_objref_Device_5();


        private:
            virtual void *_ptrToObjRef(const char *);

            _objref_Device_5(const _objref_Device_5 &);

            _objref_Device_5 &operator=(const _objref_Device_5 &);
            // not implemented

            friend class Device_5;
        };

        class _pof_Device_5 : public _OMNI_NS(proxyObjectFactory) {
                public:
                _pof_Device_5() : _OMNI_NS(proxyObjectFactory)(Device_5::_PD_repoId) {}
                virtual ~_pof_Device_5();

                virtual omniObjRef* newObjRef(omniIOR*, omniIdentity*);
                virtual bool is_a(const char*) const;
        };

        class _impl_Device_5 :
                public virtual _impl_Device_4 {
        public:
            virtual ~_impl_Device_5();

            virtual AttributeConfigList_5 *get_attribute_config_5(const ::Tango::DevVarStringArray &names) = 0;

            virtual void
            set_attribute_config_5(const ::Tango::AttributeConfigList_5 &new_conf,
                                   const ::Tango::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_5 *
            read_attributes_5(const ::Tango::DevVarStringArray &names, ::Tango::DevSource source,
                              const ::Tango::ClntIdent &cl_ident) = 0;

            virtual AttributeValueList_5 *
            write_read_attributes_5(const ::Tango::AttributeValueList_4 &values,
                                    const ::Tango::DevVarStringArray &r_names,
                                    const ::Tango::ClntIdent &cl_ident) = 0;

            virtual DevAttrHistory_5 *read_attribute_history_5(const char *name, int32_t n) = 0;

            virtual PipeConfigList *get_pipe_config_5(const ::Tango::DevVarStringArray &names) = 0;

            virtual void
            set_pipe_config_5(const ::Tango::PipeConfigList &new_conf, const ::Tango::ClntIdent &cl_ident) = 0;

            virtual DevPipeData *read_pipe_5(const char *name, const ::Tango::ClntIdent &cl_ident) = 0;

            virtual void write_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident) = 0;

            virtual DevPipeData *
            write_read_pipe_5(const ::Tango::DevPipeData &value, const ::Tango::ClntIdent &cl_ident) = 0;

        public:  // Really protected, workaround for xlC
            virtual bool _dispatch(omniCallHandle &);

        private:
            virtual void *_ptrToInterface(const char *);

            virtual const char *_mostDerivedRepoId();

        };


        _CORBA_MODULE_VAR _dyn_attr
        const ::CORBA::TypeCode_ptr _tc_Device_5;

        _CORBA_MODULE_END


                _CORBA_MODULE
        POA_Tango
                _CORBA_MODULE_BEG

        class Device :
                public virtual Tango::_impl_Device,
                public virtual ::PortableServer::ServantBase {
        public:
            virtual ~Device();

            ::Tango::Device_ptr _this() {
                return (::Tango::Device_ptr) _do_this(::Tango::Device::_PD_repoId);
            }
        };

        class Device_2 :
                public virtual Tango::_impl_Device_2,
                public virtual Device {
        public:
            virtual ~Device_2();

            ::Tango::Device_2_ptr _this() {
                return (::Tango::Device_2_ptr) _do_this(::Tango::Device_2::_PD_repoId);
            }
        };

        class Device_3 :
                public virtual Tango::_impl_Device_3,
                public virtual Device_2 {
        public:
            virtual ~Device_3();

            ::Tango::Device_3_ptr _this() {
                return (::Tango::Device_3_ptr) _do_this(::Tango::Device_3::_PD_repoId);
            }
        };

        class Device_4 :
                public virtual Tango::_impl_Device_4,
                public virtual Device_3 {
        public:
            virtual ~Device_4();

            ::Tango::Device_4_ptr _this() {
                return (::Tango::Device_4_ptr) _do_this(::Tango::Device_4::_PD_repoId);
            }
        };

        class Device_5 :
                public virtual Tango::_impl_Device_5,
                public virtual Device_4 {
        public:
            virtual ~Device_5();

            ::Tango::Device_5_ptr _this() {
                return (::Tango::Device_5_ptr) _do_this(::Tango::Device_5::_PD_repoId);
            }
        };

        _CORBA_MODULE_END


                _CORBA_MODULE
        OBV_Tango
                _CORBA_MODULE_BEG

        _CORBA_MODULE_END


#undef _core_attr
#undef _dyn_attr

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarBooleanArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarBooleanArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarBooleanArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarBooleanArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarDoubleArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarDoubleArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarDoubleArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarDoubleArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarFloatArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarFloatArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarFloatArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarFloatArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarShortArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarShortArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarShortArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarShortArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarLongArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarLongArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarLongArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarLongArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarLong64Array &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarLong64Array *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarLong64Array *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarLong64Array *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarCharArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarCharArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarCharArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarCharArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarStringArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarStringArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarStringArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarStringArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarUShortArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarUShortArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarUShortArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarUShortArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarULongArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarULongArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarULongArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarULongArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarULong64Array &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarULong64Array *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarULong64Array *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarULong64Array *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevVarLongStringArray &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevVarLongStringArray *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarLongStringArray *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarLongStringArray *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevVarDoubleStringArray &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevVarDoubleStringArray *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarDoubleStringArray *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarDoubleStringArray *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevEncoded &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevEncoded *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevEncoded *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevEncoded *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarEncodedArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarEncodedArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarEncodedArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarEncodedArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::JavaUUID_forany &_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::JavaUUID_forany &_s);

        extern void operator<<=(::CORBA::Any &_a, const Tango::JavaClntIdent &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::JavaClntIdent *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::JavaClntIdent *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::JavaClntIdent *&_sp);

        void operator>>=(Tango::LockerLanguage _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::LockerLanguage &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::JAVA) {
                _e = (Tango::LockerLanguage) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::LockerLanguage _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::LockerLanguage &_s);

        void operator<<=(::CORBA::Any &_a, const Tango::ClntIdent &_s);

        void operator<<=(::CORBA::Any &_a, Tango::ClntIdent *_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::ClntIdent *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::ClntIdent *&_sp);

        void operator>>=(Tango::AttrQuality _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::AttrQuality &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::ATTR_WARNING) {
                _e = (Tango::AttrQuality) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::AttrQuality _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttrQuality &_s);

        void operator>>=(Tango::AttrWriteType _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::AttrWriteType &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::WT_UNKNOWN) {
                _e = (Tango::AttrWriteType) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::AttrWriteType _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttrWriteType &_s);

        void operator>>=(Tango::AttrDataFormat _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::AttrDataFormat &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::FMT_UNKNOWN) {
                _e = (Tango::AttrDataFormat) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::AttrDataFormat _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttrDataFormat &_s);

        void operator>>=(Tango::DevSource _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::DevSource &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::CACHE_DEV) {
                _e = (Tango::DevSource) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::DevSource _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevSource &_s);

        void operator>>=(Tango::ErrSeverity _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::ErrSeverity &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::PANIC) {
                _e = (Tango::ErrSeverity) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::ErrSeverity _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::ErrSeverity &_s);

        void operator>>=(Tango::DevState _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::DevState &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::UNKNOWN) {
                _e = (Tango::DevState) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::DevState _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevState &_s);

        void operator>>=(Tango::DispLevel _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::DispLevel &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::DL_UNKNOWN) {
                _e = (Tango::DispLevel) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::DispLevel _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DispLevel &_s);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarStateArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarStateArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarStateArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarStateArray *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::TimeVal &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::TimeVal *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::TimeVal *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::TimeVal *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevCmdInfo &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevCmdInfo *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdInfo *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdInfo *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevCmdInfo_2 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevCmdInfo_2 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdInfo_2 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdInfo_2 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevCmdInfoList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevCmdInfoList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdInfoList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdInfoList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevCmdInfoList_2 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevCmdInfoList_2 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdInfoList_2 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdInfoList_2 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevError &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevError *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevError *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevError *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevErrorList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevErrorList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevErrorList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevErrorList *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::NamedDevError &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::NamedDevError *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::NamedDevError *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::NamedDevError *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::NamedDevErrorList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::NamedDevErrorList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::NamedDevErrorList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::NamedDevErrorList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevFailed &_s);

        void operator<<=(::CORBA::Any &_a, const Tango::DevFailed *_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevFailed *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::MultiDevFailed &_s);

        void operator<<=(::CORBA::Any &_a, const Tango::MultiDevFailed *_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::MultiDevFailed *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfig &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeConfig *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfig *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfig *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfig_2 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeConfig_2 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfig_2 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfig_2 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeValue &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeValue *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValue *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValue *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeDim &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeDim *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeDim *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeDim *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeValue_3 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeValue_3 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValue_3 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValue_3 *&_sp);

        void operator>>=(Tango::AttributeDataType _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::AttributeDataType &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::ATT_NO_DATA) {
                _e = (Tango::AttributeDataType) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::AttributeDataType _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeDataType &_s);

        void operator<<=(::CORBA::Any &_a, const Tango::AttrValUnion &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttrValUnion *_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttrValUnion *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttrValUnion *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeValue_4 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeValue_4 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValue_4 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValue_4 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeValue_5 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeValue_5 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValue_5 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValue_5 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::ChangeEventProp &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::ChangeEventProp *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::ChangeEventProp *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::ChangeEventProp *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::PeriodicEventProp &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::PeriodicEventProp *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::PeriodicEventProp *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::PeriodicEventProp *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::ArchiveEventProp &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::ArchiveEventProp *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::ArchiveEventProp *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::ArchiveEventProp *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::EventProperties &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::EventProperties *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::EventProperties *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::EventProperties *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeAlarm &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeAlarm *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeAlarm *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeAlarm *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfig_3 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeConfig_3 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfig_3 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfig_3 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfig_5 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttributeConfig_5 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfig_5 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfig_5 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfigList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeConfigList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfigList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfigList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfigList_2 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeConfigList_2 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfigList_2 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfigList_2 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfigList_3 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeConfigList_3 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfigList_3 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfigList_3 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeConfigList_5 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeConfigList_5 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeConfigList_5 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeConfigList_5 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeValueList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeValueList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValueList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValueList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeValueList_3 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeValueList_3 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValueList_3 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValueList_3 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeValueList_4 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeValueList_4 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValueList_4 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValueList_4 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeValueList_5 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeValueList_5 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeValueList_5 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeValueList_5 *&_sp);

        void operator>>=(Tango::PipeWriteType _e, cdrStream &s) {
            ::operator>>=((::CORBA::ULong) _e, s);
        }

        void operator<<=(Tango::PipeWriteType &_e, cdrStream &s) {
            ::CORBA::ULong _0RL_e;
            ::operator<<=(_0RL_e, s);
            if (_0RL_e <= Tango::PIPE_WT_UNKNOWN) {
                _e = (Tango::PipeWriteType) _0RL_e;
            } else {
                OMNIORB_THROW(MARSHAL, _OMNI_NS(MARSHAL_InvalidEnumValue),
                              (::CORBA::CompletionStatus) s.completion());
            }
        }

        void operator<<=(::CORBA::Any &_a, Tango::PipeWriteType _s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::PipeWriteType &_s);

        extern void operator<<=(::CORBA::Any &_a, const Tango::PipeConfig &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::PipeConfig *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::PipeConfig *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::PipeConfig *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::PipeConfigList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::PipeConfigList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::PipeConfigList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::PipeConfigList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevVarPipeDataEltArray &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevVarPipeDataEltArray *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevVarPipeDataEltArray *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevVarPipeDataEltArray *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevPipeDataElt &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevPipeDataElt *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevPipeDataElt *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevPipeDataElt *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevPipeBlob &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevPipeBlob *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevPipeBlob *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevPipeBlob *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevPipeData &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevPipeData *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevPipeData *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevPipeData *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::AttDataReady &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::AttDataReady *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::AttDataReady *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::AttDataReady *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevIntrChange &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevIntrChange *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevIntrChange *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevIntrChange *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevInfo &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevInfo *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevInfo *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevInfo *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevInfo_3 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevInfo_3 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevInfo_3 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevInfo_3 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevCmdHistory &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevCmdHistory *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdHistory *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdHistory *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevCmdHistoryList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevCmdHistoryList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdHistoryList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdHistoryList *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistory &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistory *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistory *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistory *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistory_3 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistory_3 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistory_3 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistory_3 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::EltInArray &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::EltInArray *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::EltInArray *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::EltInArray *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::EltInArrayList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::EltInArrayList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::EltInArrayList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::EltInArrayList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::TimeValList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::TimeValList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::TimeValList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::TimeValList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttrQualityList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttrQualityList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttrQualityList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttrQualityList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::AttributeDimList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::AttributeDimList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::AttributeDimList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::AttributeDimList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevErrorListList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevErrorListList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevErrorListList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevErrorListList *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistory_4 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistory_4 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistory_4 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistory_4 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistory_5 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistory_5 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistory_5 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistory_5 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::DevCmdHistory_4 &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::DevCmdHistory_4 *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::DevCmdHistory_4 *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::DevCmdHistory_4 *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistoryList &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistoryList *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistoryList *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistoryList *&_sp);

        void operator<<=(::CORBA::Any &_a, const Tango::DevAttrHistoryList_3 &_s);

        void operator<<=(::CORBA::Any &_a, Tango::DevAttrHistoryList_3 *_sp);

        bool operator>>=(const ::CORBA::Any &_a, Tango::DevAttrHistoryList_3 *&_sp);

        bool operator>>=(const ::CORBA::Any &_a, const Tango::DevAttrHistoryList_3 *&_sp);

        extern void operator<<=(::CORBA::Any &_a, const Tango::ZmqCallInfo &_s);

        extern void operator<<=(::CORBA::Any &_a, Tango::ZmqCallInfo *_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, Tango::ZmqCallInfo *&_sp);

        extern bool operator>>=(const ::CORBA::Any &_a, const Tango::ZmqCallInfo *&_sp);

        void operator<<=(::CORBA::Any &_a, Tango::Device_ptr _s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_ptr *_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::Device_ptr &_s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_2_ptr _s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_2_ptr *_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::Device_2_ptr &_s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_3_ptr _s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_3_ptr *_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::Device_3_ptr &_s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_4_ptr _s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_4_ptr *_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::Device_4_ptr &_s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_5_ptr _s);

        void operator<<=(::CORBA::Any &_a, Tango::Device_5_ptr *_s);

        bool operator>>=(const ::CORBA::Any &_a, Tango::Device_5_ptr &_s);


        void
        Tango::Device::_marshalObjRef(::Tango::Device_ptr obj, cdrStream &s) {
            omniObjRef::_marshal(obj->_PR_getobj(), s);
        }


        void
        Tango::Device_2::_marshalObjRef(::Tango::Device_2_ptr obj, cdrStream &s) {
            omniObjRef::_marshal(obj->_PR_getobj(), s);
        }


        void
        Tango::Device_3::_marshalObjRef(::Tango::Device_3_ptr obj, cdrStream &s) {
            omniObjRef::_marshal(obj->_PR_getobj(), s);
        }


        void
        Tango::Device_4::_marshalObjRef(::Tango::Device_4_ptr obj, cdrStream &s) {
            omniObjRef::_marshal(obj->_PR_getobj(), s);
        }


        void
        Tango::Device_5::_marshalObjRef(::Tango::Device_5_ptr obj, cdrStream &s) {
            omniObjRef::_marshal(obj->_PR_getobj(), s);
        }

    }//backend
}//tango
