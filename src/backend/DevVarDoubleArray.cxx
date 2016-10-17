//
// Created by ingvord on 10/17/16.
//

#include <tango/frontend/DevVarDoubleArray.hxx>
//actual implementation
#include <idl/tango.h>

Tango::DevVarDoubleArray::DevVarDoubleArray(size_t _max):
        impl(new backend::Tango::DevVarDoubleArray((_CORBA_ULong)_max))
{

}

Tango::DevVarDoubleArray::DevVarDoubleArray(size_t _max, size_t _len, double *_val, bool _rel):
        impl(new backend::Tango::DevVarDoubleArray((_CORBA_ULong)_max, (_CORBA_ULong) _len, _val, (_CORBA_Boolean) _rel))
{

}

#define IMPL static_cast<backend::Tango::DevVarDoubleArray*>(impl)

size_t Tango::DevVarDoubleArray::length() const {
    return IMPL->length();
}

void Tango::DevVarDoubleArray::length(size_t size) {
    IMPL->length((_CORBA_ULong)size);
}

size_t Tango::DevVarDoubleArray::maximum() const {
    return IMPL->maximum();
}

double &Tango::DevVarDoubleArray::operator[](size_t size) {
    return IMPL->operator[]((_CORBA_ULong) size);
}

const double &Tango::DevVarDoubleArray::operator[](size_t size) const {
    return IMPL->operator[]((_CORBA_ULong)size);
}

void Tango::DevVarDoubleArray::replace(size_t size, size_t size1, double *t, bool b) {
    IMPL->replace((_CORBA_ULong) size, (_CORBA_ULong) size1, t, b);
}

double *Tango::DevVarDoubleArray::get_buffer() const {
    return IMPL->get_buffer();
}

double *Tango::DevVarDoubleArray::get_buffer(bool b) {
    return IMPL->get_buffer(b);
}

bool Tango::DevVarDoubleArray::release() const {
    return IMPL->release();
}

Tango::DevVarDoubleArray::~DevVarDoubleArray(){
    delete impl;
}

