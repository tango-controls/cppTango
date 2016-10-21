//
// Created by ingvord on 10/17/16.
//

#pragma once

#include <cstdlib>

namespace Tango {
    /**
     * Interface that mimics CORBA::UnboundedSequence
     *
     */
    //TODO copy or move
    template <typename T, typename DELEGATE>
    class DevVarArray {
    public://ctrs, dtrs etc
        ~DevVarArray();

        DevVarArray() = default;

        DevVarArray(const DevVarArray &_s) = default;

        //TODO perfect forwarding ctrs
        DevVarArray(size_t);

        DevVarArray(size_t, size_t, T, bool);

        //TODO from vector

        DevVarArray &operator=(const DevVarArray &_s) = default;
    public://methods

        size_t length() const;

        void length(size_t);

        size_t maximum() const;

        template <typename U = typename std::remove_pointer<T>::type>
        U&operator[](size_t);
        template <typename U = typename std::remove_pointer<T>::type>
        const U&operator[](size_t) const;

        void replace(size_t, size_t, T, bool);

        //TODO Buffer related stuff must be hidden from client/server code, but can be useful in the library
        static T allocbuf(size_t);
        static void freebuf(T);

        template <typename U = typename std::add_const<T>::type>
        U get_buffer() const;
        T get_buffer(bool);

        bool release() const;
    private:
        DELEGATE* delegate_;
    };
}//Tango