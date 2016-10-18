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

namespace {
    thread_local bool isWriter = false;
};
//TODO current implementation may lead to writer thread starvation, a lot of quick readers will always sneak while writer will wait till htey all gone
class ReadersWritersLock {
    using thread = std::thread;
    using Lock = unique_lock<mutex>;

    mutex readersLock;
    mutex writerLock;
    condition_variable readers;
    condition_variable writers;
    atomic_int totalReaders;
    atomic_int totalWriters;
public:
    ReadersWritersLock(void) : totalReaders(0), totalWriters(0) {}

    void readerIn(void) {
        if(totalWriters.load() > 0 && !::isWriter)
        {
            Lock lock{readersLock};
            readers.wait(lock, [&](){ return totalWriters.load() == 0;});
        }//release lock
        //TODO prevent writer starvation by limiting number of readers
        totalReaders++;
    }

    void readerOut(void) {
        //if this is the last reader notify writers
        if(totalReaders-- == 0 && !::isWriter){
            writers.notify_all();
        }
    }

    void writerIn(void) {
        if(totalReaders.load() > 0){
            Lock lock{writerLock};
            writers.wait(lock, [&](){ return totalReaders.load() == 0;});
        }//release lock
        totalWriters++;
        ::isWriter = true;
    }

    void writerOut(void) {
        if (totalWriters-- == 0) {
            ::isWriter = false;
            readers.notify_all();    // might as well wake up all readers
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
