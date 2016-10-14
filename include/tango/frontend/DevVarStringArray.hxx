//
// Created by ingvord on 10/14/16.
//

#pragma once

#include <omniORB4/CORBA.h>

namespace Tango {

    class DevVarStringArray_var;

    class DevVarStringArray : public _CORBA_Unbounded_Sequence_String {
    public:
        typedef DevVarStringArray_var _var_type;

        inline DevVarStringArray() {}

        inline DevVarStringArray(const DevVarStringArray &_s)
                : _CORBA_Unbounded_Sequence_String(_s) {}

        inline DevVarStringArray(_CORBA_ULong _max)
                : _CORBA_Unbounded_Sequence_String(_max) {}

        inline DevVarStringArray(_CORBA_ULong _max, _CORBA_ULong _len, char **_val, _CORBA_Boolean _rel = 0)
                : _CORBA_Unbounded_Sequence_String(_max, _len, _val, _rel) {}


        inline DevVarStringArray &operator=(const DevVarStringArray &_s) {
            _CORBA_Unbounded_Sequence_String::operator=(_s);
            return *this;
        }
    };

    class DevVarStringArray_out;

    class DevVarStringArray_var {
    public:
        inline DevVarStringArray_var() : _pd_seq(0) {}

        inline DevVarStringArray_var(DevVarStringArray *_s) : _pd_seq(_s) {}

        inline DevVarStringArray_var(const DevVarStringArray_var &_s) {
            if (_s._pd_seq) _pd_seq = new DevVarStringArray(*_s._pd_seq);
            else _pd_seq = 0;
        }

        inline ~DevVarStringArray_var() { if (_pd_seq) delete _pd_seq; }

        inline DevVarStringArray_var &operator=(DevVarStringArray *_s) {
            if (_pd_seq) delete _pd_seq;
            _pd_seq = _s;
            return *this;
        }

        inline DevVarStringArray_var &operator=(const DevVarStringArray_var &_s) {
            if (_s._pd_seq) {
                if (!_pd_seq) _pd_seq = new DevVarStringArray;
                *_pd_seq = *_s._pd_seq;
            } else if (_pd_seq) {
                delete _pd_seq;
                _pd_seq = 0;
            }
            return *this;
        }

        inline _CORBA_String_element operator[](_CORBA_ULong _s) {
            return (*_pd_seq)[_s];
        }


        inline DevVarStringArray *operator->() { return _pd_seq; }

        inline const DevVarStringArray *operator->() const { return _pd_seq; }

#if defined(__GNUG__)

        inline operator DevVarStringArray &() const { return *_pd_seq; }

#else
        inline operator const DevVarStringArray& () const { return *_pd_seq; }
        inline operator DevVarStringArray& () { return *_pd_seq; }
#endif

        inline const DevVarStringArray &in() const { return *_pd_seq; }

        inline DevVarStringArray &inout() { return *_pd_seq; }

        inline DevVarStringArray *&out() {
            if (_pd_seq) {
                delete _pd_seq;
                _pd_seq = 0;
            }
            return _pd_seq;
        }

        inline DevVarStringArray *_retn() {
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
        inline DevVarStringArray_out(DevVarStringArray *&_s) : _data(_s) { _data = 0; }

        inline DevVarStringArray_out(DevVarStringArray_var &_s)
                : _data(_s._pd_seq) { _s = (DevVarStringArray *) 0; }

        inline DevVarStringArray_out(const DevVarStringArray_out &_s) : _data(_s._data) {}

        inline DevVarStringArray_out &operator=(const DevVarStringArray_out &_s) {
            _data = _s._data;
            return *this;
        }

        inline DevVarStringArray_out &operator=(DevVarStringArray *_s) {
            _data = _s;
            return *this;
        }

        inline operator DevVarStringArray *&() { return _data; }

        inline DevVarStringArray *&ptr() { return _data; }

        inline DevVarStringArray *operator->() { return _data; }

        inline _CORBA_String_element operator[](_CORBA_ULong _i) {
            return (*_data)[_i];
        }


        DevVarStringArray *&_data;

    private:
        DevVarStringArray_out();

        DevVarStringArray_out &operator=(const DevVarStringArray_var &);
    };

}//Tango