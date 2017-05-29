[![logo](http://www.tango-controls.org/static/tango/img/logo_tangocontrols.png)](http://www.tango-controls.org)

[![Build Status](https://travis-ci.org/tango-controls/cppTango.svg?branch=master)](https://travis-ci.org/tango-controls/cppTango)
[![Coverage Status](https://coveralls.io/repos/github/tango-controls/cppTango/badge.svg?branch=master)](https://coveralls.io/github/tango-controls/cppTango?branch=master)

[![CLion](https://img.shields.io/badge/-Developed%20using%20CLion-blue.svg?style=flat-square&logo=data%3Aimage%2Fpng%3Bbase64%2CiVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM%2FrhtAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAALEwAACxMBAJqcGAAAA%2FlJREFUWIXtWE2IHFUQ%2Fup1b3ZdFXo2Ih5UUCciGnQPKogIuxcRRM0iCPGkEsXDol5MQDaCsmd%2FIBcPXoIkp9WDiAGRHW9elAQX9NCra8hB3SwjQhJ7Zl59Hvp1b%2Ff2z%2Fw4Iwj7DcW84U1XffNV1evqAfaxj%2F8xjv%2B%2Bxui%2B9zjJGDLqhc1whQCwcWQOlwNip6F44PM3R%2FZXhaEdJsQSbByZw3YjJng5UHxx%2BrPZ9%2FHtterrT5L0APoQ6SA89G4tB%2F%2FfkEshgDp7%2BPVnrp4KnsbyO2%2BlgS9121zY%2BgCEB6oH0oUV7RtzIAUriQH4YWkO23OK7Qax3VDsBLGSOwFx5skIiVqAl65JD2K6EIkgJkLYrFaxVsG1v77niT8%2B7fsDiF0FrQGsB3zyhIXoTEwKfo4cIAAVgAfQq%2FVdq2BWubC52pfoMLjjxw8hpuNU7FSqaOqcqJ0COPbGdPAAGqSpr0AlwVd%2BXSOtD1V%2FIiRJA8KANAANmuHbpXVeSfDcn1ug%2BqD1YyUzCIIAJAu2Gzz%2FuRxJ%2FcVGllOpJEj1oY4cNd9L7XYbALC0tASREdWlicPTqYjyNFd2Ma0ftycE2dMqUSZLbDSSjpgYCAw2D50sdVJJUNWHuJeZUA2KxAr%2BfM%2Fx4c%2FBWEEBKNCJdLIANPjl3tdqnZfW4Mrmd1Q75WrQL9TgeGCqwudQGvmjiyHEJFsCRb7esl26uLiI9fX1dC%2BLhYWFdN1qtfJBaLB1%2BFjf1JR%2B4eDXaxRjIcbCmB7EWPz26At5%2FyXHSEKwbq9f7L0oVZDJHYQCJSAlvuo6d%2BSjZ1CCan0IBUKBgWCQWpkUyhVUH3EG4vrrXWmMPXDw5Tna7gxsdxpXn32kUvKCNKfCS0y71%2FroXGmAWj8SjQKqSW36zAX6p3%2Bi%2BXirULwFBU9cuAgxU7DdGQgIEYKm%2F%2BQ7NMHkFsd4mqH6pUIUFFTrw3ZnAI2HS9LkLiwbEhLLHit9CWrsN2cv31ZIdUFBtQcgovHzgoizSSgYz4NZgmUoEIyOzsv02fOuFtxgkNkf2xGiBppV79jtg0%2FU0dF5oXpAJg3jBukB6kHVi4fiCvSVY%2FrseYqoSzv3vCtECIjilsNfFUmoj150fWo2mkWvMwvrLF5fh8cOBmg9ddPwzyRAoqZTMq0Zs7t2nfgGXi1e7O5GiaVd65Sj9UDrV5IbiCAARM%2FPy%2FLdN2cIuc5mfI5tPP4glpu3SthczQUiADpiqSU%2F1k1JdekFRvjrI0l5kuZrzz1U6qMZrlDtAfT%2BvmHXosSStM9CX7pz%2BHmwDtkGqiIHAGFzVWIJMwqmTeEU3PMwVoZJPfSW4v7WN9xse07RG2FfvOs%2Fjb%2BPfYyCfwCkWUhIKg3GkwAAAABJRU5ErkJggg%3D%3D)](http://www.jetbrains.com/clion)

# TANGO distributed control system - shared library

TANGO is an object-oriented distributed control system. In TANGO all objects are representations of devices, which can be on the same computer or distributed over a network. Communication between devices can be synchronous, asynchronous or event driven.

The object model in TANGO supports methods, attributes and properties. TANGO provides an API which hides all the details of network access and provides object browsing, discovery, and security features. 

TANGO is being actively developed as a collaborative effort between the ESRF ([www.esrf.eu](http://www.esrf.eu)), Soleil ([synchrotron-soleil.fr](http://synchrotron-soleil.fr)), Alba ([www.cells.es](http://www.cells.es)), and Elettra institutes ([www.elettra.trieste.it](http://www.elettra.trieste.it)).

For more information please visit [www.tango-controls.org](http://www.tango-controls.org).

# Documentation

Generated: [tango-controls.github.io/cppTango-docs](https://tango-controls.github.io/cppTango-docs)

Manuals: [tango-controls.org/resources/documentation/kernel](http://www.tango-controls.org/resources/documentation/kernel/)

# How to build and install using cmake

```bash
mkdir build; 
cd build; 
cmake .. 
[-DCMAKE_INSTALL_PREFIX=<desired installation path>] 
[-DTANGO_DEVICE_SERVER_PATH=<installation folder for device servers, will be availabe via pkg-config>] 
[-DOMNI_BASE=<omniORB4 home folder>] 
[-DZMQ_BASE=<zmq home folder>] 
[-DCMAKE_BUILD_TYPE=RELEASE|DEBUG] 
[-DCMAKE_VERBOSE_MAKEFILE=true]; 
make; 
[sudo] make install
```

More information is in [INSTALL file](https://github.com/tango-controls/cppTango/blob/master/INSTALL.md) 

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

# How to test

### 1. First build everything

 `mkdir build; cd build; cmake ..; make`

### 2. Start TANGO environment

 `make start-tango`
 
 Typical output:
Note env.TANGO_HOST value: this is normal TANGO_HOST that can be used, for instance, in Jive

```
Setup test environment
 174fda8cad89ac457a6129c599368d2842547575eac05f1d9b4ad60966cf0018
 CONTAINER=6130013cf55d420552df9dddd488b7564d58f49d10831d4880774443faf1a22b
 TANGO_HOST=172.17.0.3:10000
 Create tango_host file
 Wait till tango-cs is online
```

### 3. Run tests
 `make run-tests`

 Before running tests CTest runs conf_devtest (located in cpp_test_suite/new_tests) and starts DevTest and FwdTest device servers (located in cpp_test_ds)

### 4. Shutdown TANGO environment
 `make stop-tango`

For information on how to run individual tests see [INSTALL file](https://github.com/tango-controls/cppTango/blob/master/INSTALL.md)

# How to contribute

See [CONTRIBUTE file](https://github.com/tango-controls/cppTango/blob/master/CONTRIBUTE.md)

# Usage workflow (draft)


1. In its most basic form, we adopt the githubflow : the default branch is master, which is always in a deployable state (i.e. must never be broken); work on new features is done using temporary feature branches (which may live in the same repo or in a fork)
2. Reporting issues: use github issues
3. Code contributions: use Pull requests. Pull requests can be associated with issues. Trivial fixes can even be done from the web (a temporary branch is created automatically to use the Pull request infrastructure)
4. Code review: most projects (all except single-developer ones) should only allow commits to the master branch after peer review. This can be enforced by the convention that all commits to master must be done via a Pull request and the pull request approved by a person different from the author of the commit.
5. Releases (named versions):
  * Simple projects may be ok with just tagging certain (ideally, all) commits done to the master branch with a version number. With this system, only one release is actively maintained simultaneously.
        
  * More complex projects may require to simultaneously maintain more than one release (e.g. Tango may choose to support bugfixes in Tango9 even after Tango10 is released). In this case, releases may be done on release branches starting from master (see APPENDIX I for an example)
6. Semantic versioning is recommended.
7. Public automatic testing/continuous integration (e.g., via Travis) is recommended
8. The main development should be done on the tango-controls hosted project (as opposed to using a private organization project and just pushing to the tango-controls repo from time to time). This allows for public visibility of the latest development and issues and encourages sharing and reuse. If a given organization needs special tweaks or has particular release/testing cycles, the recommendation is that the organization forks from the "canonical" repo


# Links

[Tango Controls web page](http://tango-controls.org)
