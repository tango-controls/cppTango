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
- `-DBUILD_TESTING=<ON|OFF>` Build the test suite (`ON` by default)

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

## Installing required cppzmq version on older systems

On some debian versions, 7 (wheezy) and older, there is no compatible cppzmq version available.

This requires installing [cppzmq](https://github.com/zeromq/cppzmq) via:

```
git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake -DENABLE_DRAFTS=OFF -DCPPZMQ_BUILD_TESTS=OFF ..
sudo make install
```

And then pass `-DCPPZMQ_BASE=/usr/local` to cmake or use

```
mkdir build
cd build
configure CXXFLAGS="-I /usr/local/include" ..
```

when compiling via the tango source distribution.

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
