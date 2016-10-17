//
// Created by ingvord on 10/17/16.
//

#pragma once

#include "DevVarArray.hxx"

namespace Tango {
    class DevVarDoubleArray_var;

    class DevVarDoubleArray : public DevVarArray<double> {
    public:
        inline DevVarDoubleArray() {}

        inline DevVarDoubleArray(const DevVarDoubleArray &_s);

        inline DevVarDoubleArray(size_t _max);

        inline DevVarDoubleArray(size_t _max, size_t _len, double *_val, bool _rel = 0);


        inline DevVarDoubleArray &operator=(const DevVarDoubleArray &_s);

    public://DeVarArray implementation
        virtual size_t length() const override;

        virtual void length(size_t size) override;

        virtual size_t maximum() const override;

        virtual double &operator[](size_t size) override;

        virtual const double &operator[](size_t size) const override;

        virtual void replace(size_t size, size_t size1, double *t, bool b) override;

        virtual double *get_buffer() const override;

        virtual double *get_buffer(bool b) override;

        virtual bool release() const override;
    };

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

        inline double &operator[](size_t _s) {
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

    private:
        DevVarDoubleArray *_pd_seq;
    };

}//Tango