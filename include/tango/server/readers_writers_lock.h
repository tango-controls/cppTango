// -*- Mode: C++; -*-
//
// ReadersWritersLock.h     Author    : Tristan Richardson (tjr)
//										Jens Meyer
//										Emmanuel Taurel
//
// Copyright (C) :      1997-1999 AT&T Laboratories Cambridge
//						2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _ReadersWritersLock_h_
#define _ReadersWritersLock_h_

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

class ReadersWritersLock {
    using thread = std::thread;
    using Lock = unique_lock<recursive_mutex>;

    struct ReadLock {
        ReadLock(ReadersWritersLock &parent) noexcept
                : parent(parent) {}

        void lock() {
            Lock self{parent.condition_mutex_, try_to_lock};
            if (!self.owns_lock())
                while (parent.is_writer_) {
                    //we are not the writer so wait when writer notifies us
                    Lock lock{parent.condition_mutex_};
                    parent.condition_.wait(lock);
                }//release lock
            parent.readers_++;
        }

        void unlock() noexcept {
            if (parent.readers_-- == 0) {
                parent.condition_.notify_one();//notify one writer
            }
        }

        ReadersWritersLock &parent;
    };

    struct WriteLock {
        WriteLock(ReadersWritersLock &parent) noexcept
                : parent(parent) {}

        void lock() {
            Lock lock{parent.condition_mutex_};
            while (parent.readers_.load() > 0) {
                parent.condition_.wait(lock);//unlock
            }
            parent.is_writer_ = true;
        }

        void unlock() noexcept {
            parent.is_writer_ = false;
            parent.condition_.notify_all();
        }

        ReadersWritersLock &parent;
    };

    recursive_mutex condition_mutex_;
    condition_variable_any condition_;

    atomic_int readers_;
    atomic_bool is_writer_{false};

    ReadLock reader_lock_;
    WriteLock writer_lock_;
public:
    ReadersWritersLock(void) noexcept
            : readers_(0), reader_lock_(*this), writer_lock_(*this) {}


    void readerIn(void) {
        reader_lock_.lock();
    }

    void readerOut(void) noexcept {
        reader_lock_.unlock();
    }

    void writerIn(void) {
        writer_lock_.lock();
    }

    void writerOut(void) noexcept {
        writer_lock_.unlock();
    }
};

//
// As an alternative to:
// {
//   lock.readerIn();
//   .....
//   lock.readerOut();
// }
//
// you can use a single instance of the ReaderLock class:
//
// {
//   ReaderLock r(lock);
//   ....
// }
//
// This has the advantage that lock.readerOut() will be called automatically
// when an exception is thrown.
//

class ReaderLock {
    ReadersWritersLock &rwl;
public:
    ReaderLock(ReadersWritersLock &l) : rwl(l) { rwl.readerIn(); }

    ~ReaderLock(void) { rwl.readerOut(); }
};


//
// Similarly the WriterLock class can be used instead of lock.writerIn() and
// lock.writerOut().
//

class WriterLock {
    ReadersWritersLock &rwl;
public:
    WriterLock(ReadersWritersLock &l) : rwl(l) { rwl.writerIn(); }

    ~WriterLock(void) { rwl.writerOut(); }
};

#endif