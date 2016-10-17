//
// Created by ingvord on 10/17/16.
//

#pragma once

namespace Tango {
    /**
     * Interface that mimics CORAB::UnboundedSequence
     *
     */
    //TODO copy or move
    template <typename T>
    struct DevVarArray {
        virtual size_t length() const = 0;

        virtual void length(size_t) = 0;

        virtual size_t maximum() const = 0;

        virtual T&operator[](size_t) = 0;
        virtual const T&operator[](size_t) const = 0;

        virtual void replace(size_t, size_t, T*,bool) = 0;

        //TODO Buffer related stuff must be hidden from client/server code, but can be useful in the library
        static T* allocbuf(size_t);
        static void freebuf(T*);

        virtual T* get_buffer() const = 0;
        virtual T* get_buffer(bool) = 0;

        virtual bool release() const = 0;
    };
}//Tango