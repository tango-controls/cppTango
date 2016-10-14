//
// Created by ingvord on 10/14/16.
//

#pragma once

#include <cstdlib>
#include <string>
#include <vector>


namespace Tango {
    class DevVarString : public std::string {
    public:
        DevVarString(char *value) : std::string(value) {};

        DevVarString(const char *value) : std::string(value) {};

        DevVarString in();

        DevVarString out();

        DevVarString in() const;

        DevVarString out() const;

        DevVarString &operator=(char *value);

        operator char *() {
            return const_cast<char *>(c_str());
        }

        operator const char *() const {
            return c_str();
        }
    };

    class DevVarStringArray_var;

    class DevVarStringArray : public std::vector<DevVarString> {
    public:
        inline DevVarStringArray() {}

        inline DevVarStringArray(const DevVarStringArray &_s);

        inline DevVarStringArray(size_t _max);

        inline DevVarStringArray(size_t _max, size_t _len, DevVarString *_val, bool _rel = 0);

        inline DevVarStringArray(size_t _max, size_t _len, char **_val, bool _rel = 0);


        inline DevVarStringArray &operator=(const DevVarStringArray &_s) {
            //TODO copy content
            return *this;
        }

        /**
         *
         *
         * @return actual length
         */
        inline size_t length() {
            return size();
        }

        inline size_t length() const {
            return size();
        }

        //TODO initialize to default values
        void length(size_t);

        /**
         *
         * @return how many elements this array can have (was initialized with?)
         */
        size_t maximum();

        size_t maximum() const;


        void replace(size_t, size_t, char **, bool);


        //TODO replace with actual type -> fails in devapi_pipe.cpp:1423
        //TODO hide these methods
        static char **allocbuf(size_t);

        static void freebuf(void *);

        /**
        *
        * @return underlying array
        */
        char* *get_buffer(bool = false);

        /**
        *
        * @return underlying array
        */
        const char**get_buffer() const;


        bool release();

        bool release() const;
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

        inline DevVarString &operator[](size_t _s) {
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

        inline DevVarString &operator[](size_t _i) {
            return (*_data)[_i];
        }


        DevVarStringArray *&_data;

    private:
        DevVarStringArray_out();

        DevVarStringArray_out &operator=(const DevVarStringArray_var &);
    };

}//Tango