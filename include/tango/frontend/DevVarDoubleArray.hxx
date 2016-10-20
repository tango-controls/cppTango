//
// Created by ingvord on 10/17/16.
//

#pragma once

#include <memory>
#include "DevVarArray.hxx"

namespace Tango {


    class DevVarDoubleArray : public DevVarArray<double> {
    public:
        inline DevVarDoubleArray() {}

        inline DevVarDoubleArray(const DevVarDoubleArray &_s) = default;

        inline DevVarDoubleArray(size_t _max);

        inline DevVarDoubleArray(size_t _max, size_t _len, double *_val, bool _rel = 0);

        //TODO from vector

        inline DevVarDoubleArray &operator=(const DevVarDoubleArray &_s) = default;

        ~DevVarDoubleArray();

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

    private:
        void* impl;
    };

    using DevVarDoubleArray_var = std::unique_ptr<DevVarDoubleArray>;
}//Tango