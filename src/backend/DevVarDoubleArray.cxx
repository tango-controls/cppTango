//
// Created by ingvord on 10/17/16.
//

#include <tango/frontend/DevVarDoubleArray.hxx>
//actual implementation
#include <idl/tango.h>

namespace Tango {
    template<>
    DevVarArray::DevVarArray(size_t max):
    delegate_ (new backend::DevVarDoubleArray(max))
            {};


    template<>
    DevVarDoubleArray::DevVarArray(size_t  max,  size_t len,  double *data,  bool rel):
    delegate_(new backend::DevVarDoubleArray(max, len, data, rel))
            {};

    template<>
    size_t DevVarDoubleArray::length() const {
        return delegate_->length();
    }

    template<>
    void DevVarDoubleArray::length(size_t size) {
        delegate_->length((_CORBA_ULong) size);
    }

    template<>
    size_t DevVarDoubleArray::maximum() const {
        return delegate_->maximum();
    }

    template<>
    double &DevVarDoubleArray::operator[](size_t size) {
        return (*delegate_)[(_CORBA_ULong) size];
    }

    template<>
    const double &DevVarDoubleArray::operator[](size_t size) const {
        return (*delegate_)[(_CORBA_ULong) size];
    }

    template<>
    void DevVarDoubleArray::replace(size_t size, size_t size1, double *t, bool b) {
        delegate_->replace((_CORBA_ULong) size, (_CORBA_ULong) size1, t, b);
    }

    template<>
    const double *DevVarDoubleArray::get_buffer() const {
        return delegate_->get_buffer();
    }

    template<>
    double *DevVarDoubleArray::get_buffer(bool b) {
        return delegate_->get_buffer(b);
    }

    template<>
    bool DevVarDoubleArray::release() const {
        return delegate_->release();
    }
}//Tango

