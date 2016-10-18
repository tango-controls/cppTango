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

//TODO current implementation may lead to writer thread starvation, a lot of quick readers will always sneak while writer will wait till htey all gone
class ReadersWritersLock {
    using thread = std::thread;
    using Lock = unique_lock<mutex>;
public:
    mutex mut;
    condition_variable cond{};
    int n;    // 0 means no-one active, > 0 means n readers, < 0 means writer
    // (-n times).
    thread::id writerId;

    ReadersWritersLock(void) : n(0), writerId(nullptr) {}

    void readerIn(void) {
        thread::id threadId{this_thread::get_id()};

        {
            Lock lock{mut};
            if ((n < 0) && (writerId == threadId)) {
                // this thread already has lock as writer, simply decrement n
                n--;
                lock.unlock();
                return;
            }

            cond.wait(lock, [&]() { return n < 0; });
            n++;
        }//release lock
    }

    void readerOut(void) {
        int _n;//local copy of n
        {
            Lock lock{mut};
            if (n < 0) {
                // this thread already had lock as writer, simply increment n
                n++;
                lock.unlock();
                return;
            }
            _n = n--;
        }//release lock
        if (_n == 0)
            cond.notify_all();
    }

    void writerIn(void) {
        thread::id threadId{this_thread::get_id()};

        {
            Lock lock{mut};
            if ((n < 0) && (writerId == threadId)) {
                // this thread already has lock as writer, simply decrement n
                n--;
                lock.unlock();
                return;
            }
            cond.wait(lock, [&]() { return n == 0; });//wait until there is no more readers
            n--;

            writerId = threadId;
        }//release lock
    }

    void writerOut(void) {
        int _n;//local copy of n
        {
            Lock lock{mut};
            _n = n++;
        }//release lock
        if (_n == 0) {
            cond.notify_all();    // might as well wake up all readers
        }
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
