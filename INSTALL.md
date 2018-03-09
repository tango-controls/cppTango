[DRAFT]

# Prerequisites

* [tango-idl](https://github.com/tango-controls/tango-idl)
* docker and docker-engine (for tests)

# How to build and install using cmake

- clone
- cd into cloned repo
- mkdir build
- cd build
- cmake .. 
- make [-j NUMBER_OF_CPUS]
- sudo make install

cmake options are: `[-DCMAKE_INSTALL_PREFIX=<desired installation path>] [-DOMNI_BASE=<omniORB4 home folder>] [-DCPPZMQ_BASE=<cppzmq home folder>] [-DZMQ_BASE=<zmq home folder>] [-DIDL_BASE=<tango-idl installation folder>] [-DCMAKE_BUILD_TYPE=Release|Debug] [-DCMAKE_VERBOSE_MAKEFILE=true]`

Typical output:

```
-- Install configuration: "Debug"
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/lib/libtango.so.9.2.5
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/lib/libtango.so.9
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/lib/libtango.so
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/lib/pkgconfig/tango.pc
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/include/log4tango/Appender.hh
<snip>
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/include/subdev_diag.h
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/include/encoded_attribute.h
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/include/encoded_format.h
-- Installing: /storage/Projects/org.tango/git/cppTango/build/install/include/idl/tango.h
```

## Ubuntu 16.04 compilation problem

When compiling on Ubuntu 16.04 or on Debian stretch, the following error can occur:

```
[ 17%] Building CXX object cppapi/client/CMakeFiles/client_objects.dir/zmqeventconsumer.cpp.o
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp: In member function ‘virtual void* Tango::ZmqEventConsumer::run_undetached(void*)’:
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp:186:18: error: cannot convert ‘zmq::socket_t’ to ‘void*’ in assignment
  items[0].socket = *control_sock;
                  ^
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp:187:18: error: cannot convert ‘zmq::socket_t’ to ‘void*’ in assignment
  items[1].socket = *heartbeat_sub_sock;
                  ^
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp:188:18: error: cannot convert ‘zmq::socket_t’ to ‘void*’ in assignment
  items[2].socket = *event_sub_sock;
                  ^
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp: In member function ‘bool Tango::ZmqEventConsumer::process_ctrl(zmq::message_t&, zmq::pollitem_t*, int&)’:
/home/ingvord/Projects/org.tango/git/cppTango/cppapi/client/zmqeventconsumer.cpp:1063:47: error: cannot convert ‘zmq::socket_t’ to ‘void*’ in assignment
                 poll_list[old_poll_nb].socket = *tmp_sock;
```

This is due to incompatibility of zmq.hpp file provided in libzmq3-dev:4.1.7 (ubuntu 16.04), i.e. it is not possible to compile cppTango using zmq.hpp file provided by libzmq3-dev:4.1.7 (ubuntu 16.04).

The following workaround can be applied:

Download and install [cppzmq](https://github.com/zeromq/cppzmq) (version 4.2.2 for instance). Install it in some folder.

Build cppTango using installed cppzmq, using the following cmake command during the build process:

`cmake .. -DCPPZMQ_BASE=<cppzmq_install_folder>`

This problem is addressed in issue #273 and Pull Request #421.

# How to setup tests

Using provided docker based TANGO environment:

### Build the library:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Start TANGO environment

```
$ make start-tango
Setup test environment
557e3c8a3daa2b75aac4fe04562bac32570db0ace08edd06a23cebaa7fd86f5e
CONTAINER=27bad3659305155c33d99505c4836b616d9c2a6de3431229e79b71a020f18455
TANGO_HOST=172.17.0.3:10000
Create tango_host file
Wait till tango-cs is online
```

This process takes ~30 s

### Run tests

```
$ make run-tests
TANGO_HOST=172.17.0.3:10000
Run conf_devtest
Added test server : DevTest/test -> test/debian8/10, class : DevTest
Added test server : DevTest/test -> test/debian8/11, class : DevTest
Added test server : DevTest/test -> test/debian8/12, class : DevTest

Added test server : FwdTest/test -> test/fwd_debian8/10, class : FwdTest

Added pseudo server : DsCache/test -> test/cache1/1, class : CacheTest1
Added pseudo server : DsCache/test -> test/cache1/2, class : CacheTest1
Added pseudo server : DsCache/test -> test/cache2/1, class : CacheTest2

<snip>

Constructing a list of tests
Done constructing a list of tests
Checking test dependency graph...
Checking test dependency graph end
test 1
      Start  1: log4tango_test
      
<snip>
      
100% tests passed, 0 tests failed out of 59

Total Test time (real) = 843.30 sec

Run command: /home/tango/src/build/cpp_test_suite/environment/post_test.sh      
```

The whole test suite takes ~ 15 min

### Setup and run individual tests

To run individual tests use TANGO_HOST provided by the `start-tango` target.

Make sure TANGO_HOST is set correctly:

```
$ cd build
$ cat tango_host
#!/bin/bash
export TANGO_HOST=172.17.0.3:10000
```

TANGO_HOST must be the same as what `start-tango` has returned.

```
$ . tango_host
```

Now run some test:

```
$ ctest -R attr_misc -V
UpdateCTestConfiguration  from :/storage/Projects/org.tango/git/cppTango/build/DartConfiguration.tcl
Parse Config file:/storage/Projects/org.tango/git/cppTango/build/DartConfiguration.tcl
 Add coverage exclude regular expressions.
UpdateCTestConfiguration  from :/storage/Projects/org.tango/git/cppTango/build/DartConfiguration.tcl
Parse Config file:/storage/Projects/org.tango/git/cppTango/build/DartConfiguration.tcl
Test project /storage/Projects/org.tango/git/cppTango/build
Run command: /storage/Projects/org.tango/git/cppTango/build/cpp_test_suite/environment/pre_test.sh
TANGO_HOST=172.17.0.3:10000
Run conf_devtest
<snip>
Constructing a list of tests
Done constructing a list of tests
Checking test dependency graph...
Checking test dependency graph end
test 12
    Start 12: old_tests::attr_misc

12: Test command: /storage/Projects/org.tango/git/cppTango/build/cpp_test_suite/old_tests/attr_misc "test/debian8/10"
12: Test timeout computed to be: 1500
12: 
12: new DeviceProxy(test/debian8/10) returned
12: 
12:    Setting/Getting attribute info --> OK
12:    Writing outside attribute limits --> OK
12:    Min alarm detection (on a float spectrum) --> OK
12:    Reset min alarm detection --> OK
12:    Max alarm detection (on a float spectrum) --> OK
12:    Reset max alarm detection --> OK
12:    Min alarm detection (on a unsigned short spectrum) --> OK
12:    Reset min alarm detection --> OK
12:    Max alarm detection (on a unsigned short spectrum) --> OK
12:    Reset max alarm detection --> OK
12:    Setting/Getting V5 attribute info --> OK
12:    Alarm, Warning level detection --> OK
12:    Exception when trying to change "hard coded" properties --> OK
1/1 Test #12: old_tests::attr_misc .............   Passed    0.29 sec

The following tests passed:
        old_tests::attr_misc

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.33 sec
```

See [CTest guide](https://cmake.org/Wiki/CMake/Testing_With_CTest)

### Stop TANGO environment

```
$ make stop-tango
```