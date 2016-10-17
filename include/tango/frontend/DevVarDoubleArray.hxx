//
// Created by ingvord on 10/17/16.
//

#pragma once

#include <omniORB4/CORBA.h>

namespace Tango {
    class DevVarDoubleArray_var;

    class DevVarDoubleArray : public _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::Double, 8, 8> {
    public:
        typedef DevVarDoubleArray_var _var_type;

        inline DevVarDoubleArray() {}

        inline DevVarDoubleArray(const DevVarDoubleArray &_s)
                : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::Double, 8, 8>(_s) {}

        inline DevVarDoubleArray(_CORBA_ULong _max)
                : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::Double, 8, 8>(_max) {}

        inline DevVarDoubleArray(_CORBA_ULong _max, _CORBA_ULong _len, ::CORBA::Double *_val, _CORBA_Boolean _rel = 0)
                : _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::Double, 8, 8>(_max, _len, _val, _rel) {}


        inline DevVarDoubleArray &operator=(const DevVarDoubleArray &_s) {
            _CORBA_Unbounded_Sequence_w_FixSizeElement<::CORBA::Double, 8, 8>::operator=(_s);
            return *this;
        }
    };

    class DevVarDoubleArray_out;

    class DevVarDoubleArray_var {
    public:
        inline DevVarDoubleArray_var() : _pd_seq(0) {}

        inline DevVarDoubleArray_var(DevVarDoubleArray *_s) : _pd_seq(_s) {}

        inline DevVarDoubleArray_var(const DevVarDoubleArray_var &_s) {
            if (_s._pd_seq) _pd_seq = new DevVarDoubleArray(*_s._pd_seq);
            else _pd_seq = 0;
        }

        inline ~DevVarDoubleArray_var() { if (_pd_seq) delete _pd_seq; }

        inline DevVarDoubleArray_var &operator=(DevVarDoubleArray *_s) {
            if (_pd_seq) delete _pd_seq;
            _pd_seq = _s;
            return *this;
        }

        inline DevVarDoubleArray_var &operator=(const DevVarDoubleArray_var &_s) {
            if (_s._pd_seq) {
                if (!_pd_seq) _pd_seq = new DevVarDoubleArray;
                *_pd_seq = *_s._pd_seq;
            } else if (_pd_seq) {
                delete _pd_seq;
                _pd_seq = 0;
            }
            return *this;
        }

        inline ::CORBA::Double &operator[](_CORBA_ULong _s) {
            return (*_pd_seq)[_s];
        }


        inline DevVarDoubleArray *operator->() { return _pd_seq; }

        inline const DevVarDoubleArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)
        inline operator DevVarDoubleArray& () const { return *_pd_seq; }
#else

        inline operator const DevVarDoubleArray &() const { return *_pd_seq; }

        inline operator DevVarDoubleArray &() { return *_pd_seq; }

#endif

        inline const DevVarDoubleArray &in() const { return *_pd_seq; }

        inline DevVarDoubleArray &inout() { return *_pd_seq; }

        inline DevVarDoubleArray *&out() {
            if (_pd_seq) {
                delete _pd_seq;
                _pd_seq = 0;
            }
            return _pd_seq;
        }

        inline DevVarDoubleArray *_retn() {
            DevVarDoubleArray *tmp = _pd_seq;
            _pd_seq = 0;
            return tmp;
        }

        friend class DevVarDoubleArray_out;

    private:
        DevVarDoubleArray *_pd_seq;
    };

    class DevVarDoubleArray_out {
    public:
        inline DevVarDoubleArray_out(DevVarDoubleArray *&_s) : _data(_s) { _data = 0; }

        inline DevVarDoubleArray_out(DevVarDoubleArray_var &_s)
                : _data(_s._pd_seq) { _s = (DevVarDoubleArray *) 0; }

        inline DevVarDoubleArray_out(const DevVarDoubleArray_out &_s) : _data(_s._data) {}

        inline DevVarDoubleArray_out &operator=(const DevVarDoubleArray_out &_s) {
            _data = _s._data;
            return *this;
        }

        inline DevVarDoubleArray_out &operator=(DevVarDoubleArray *_s) {
            _data = _s;
            return *this;
        }

        inline operator DevVarDoubleArray *&() { return _data; }

        inline DevVarDoubleArray *&ptr() { return _data; }

        inline DevVarDoubleArray *operator->() { return _data; }

        inline ::CORBA::Double &operator[](_CORBA_ULong _i) {
            return (*_data)[_i];
        }


        DevVarDoubleArray *&_data;

    private:
        DevVarDoubleArray_out();

        DevVarDoubleArray_out &operator=(const DevVarDoubleArray_var &);
    };

}//Tango