# Prerequisites

* [tango-idl](https://github.com/tango-controls/tango-idl)
* Other required packages can be installed on apt-based linux systems via
  * add deb-src entries for main and contrib (debian) or main, restricted and universe (ubuntu) into /etc/apt/sources.list
  * `sudo apt-get update`
  * `sudo apt-get build-dep libtango9`
* docker and docker-engine (for tests)

# How to build and install

- `git clone https://github.com/tango-controls/cppTango`
- `cd cppTango`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make [-j NUMBER_OF_CPUS]`
- `sudo make install`

## Available are the following cmake options:

- `-DCMAKE_INSTALL_PREFIX=<desired installation path>`
- `-DIDL_BASE=<tango-idl installation folder>`
- `-DOMNI_BASE=<omniORB4 home folder>`
- `-DCPPZMQ_BASE=<cppzmq home folder>`
- `-DZMQ_BASE=<zmq home folder>`
- `-DCMAKE_BUILD_TYPE=<Release|Debug>`
- `-DCMAKE_VERBOSE_MAKEFILE=true`
- `-DTANGO_USE_USING_NAMESPACE=<ON|OFF>` choose `OFF` for modern builds

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

This is due to incompatibility of zmq.hpp file provided in libzmq3-dev:4.1.7
(ubuntu 16.04), i.e. it is not possible to compile cppTango using zmq.hpp file
provided by libzmq3-dev:4.1.7 (ubuntu 16.04).

This requires installing [cppzmq](https://github.com/zeromq/cppzmq) via:

```
git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake -DENABLE_DRAFTS=OFF -DCPPZMQ_BUILD_TESTS=OFF ..
sudo make install
```

# Using pkg-config

Once installed cppTango provides [pkg-config](https://en.wikipedia.org/wiki/Pkg-config) file `tango.pc`

One can use it to resolve libtango dependencies in his project, for instance using cmake:

```cmake
include(FindPkgConfig)
pkg_search_module(TANGO_PKG REQUIRED tango)

#...

link_directories(${TANGO_PKG_LIBRARY_DIRS})

#note TANGO_PKG_XXX usage
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${TANGO_PKG_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TANGO_PKG_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME} PUBLIC ${TANGO_PKG_LIBRARIES})
```

`tango.pc` provides default installation directory for all Tango devices linked against this libtango:

```bash
pkg-config --variable=tangodsdir tango
/usr/bin
```

# How to setup tests

Using provided docker based TANGO environment:

### Build the library:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Run tests

From `build/` directory run:

```
$ ctest --output-on-failure --parallel 4

Test project /home/tango-cs/Documents/cppTango/build
      Start 54: old_tests::ring_depth
      Start 66: asyn::asyn_cmd
      Start 77: event::per_event
      Start 83: event::user_event
 1/93 Test #83: event::user_event .................   Passed   22.65 sec

...

90/93 Test  #1: log4tango_test ....................   Passed    0.20 sec
91/93 Test #39: CXX::cxx_nan_inf_in_prop ..........   Passed   13.28 sec
92/93 Test #43: old_tests::attr_types .............   Passed   12.89 sec
93/93 Test #14: CXX::cxx_attr_conf ................   Passed   12.81 sec

100% tests passed, 0 tests failed out of 93

Total Test time (real) = 546.93 sec
```

Test output and device server logs are collected in `build/cpp_test_suite/test_results`.

### Run individual tests

```
$ ctest -R old_tests::attr_misc -V

UpdateCTestConfiguration  from :/home/tango-cs/Documents/cppTango/build/DartConfiguration.tcl
Parse Config file:/home/tango-cs/Documents/cppTango/build/DartConfiguration.tcl
UpdateCTestConfiguration  from :/home/tango-cs/Documents/cppTango/build/DartConfiguration.tcl
Parse Config file:/home/tango-cs/Documents/cppTango/build/DartConfiguration.tcl
Test project /home/tango-cs/Documents/cppTango/build
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 0 tests to meet fixture requirements
Checking test dependency graph...
Checking test dependency graph end
test 49
    Start 49: old_tests::attr_misc

49: Test command: /home/tango-cs/Documents/cppTango/build/cpp_test_suite/environment/run_with_fixture.sh "/home/tango-cs/Documents/cppTango/build/cpp_test_suite/old_tests/attr_misc" "test/debian8/10"
49: Test timeout computed to be: 1500
49:
49: new DeviceProxy(test/debian8/10) returned
49:
49:    Setting/Getting attribute info --> OK
49:    Writing outside attribute limits --> OK
49:    Min alarm detection (on a float spectrum) --> OK
49:    Reset min alarm detection --> OK
49:    Max alarm detection (on a float spectrum) --> OK
49:    Reset max alarm detection --> OK
49:    Min alarm detection (on a unsigned short spectrum) --> OK
49:    Reset min alarm detection --> OK
49:    Max alarm detection (on a unsigned short spectrum) --> OK
49:    Reset max alarm detection --> OK
49:    Setting/Getting V5 attribute info --> OK
49:    Alarm, Warning level detection --> OK
49:    Exception when trying to change "hard coded" properties --> OK
1/1 Test #49: old_tests::attr_misc .............   Passed    8.55 sec

The following tests passed:
        old_tests::attr_misc

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   8.56 sec
```

See [CTest guide](https://cmake.org/Wiki/CMake/Testing_With_CTest)
