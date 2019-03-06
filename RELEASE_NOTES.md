
# Tango C++ library 9.3.3 Release Notes
March 2019

Table of Contents
=================
  * [What's new (since version 9.2.5)?](#whats-new-since-version-925)
  * [Changes which might have an impact on users](#changes-which-might-have-an-impact-on-users)
  * [Bug fixes](#bug-fixes)
    * [Event related bug fixes](#event-related-bug-fixes)
    * [Other bug fixes](#other-bug-fixes)
  * [Changelog](#changelog)
  * [Source code comparison with version 9.2.5](#source-code-comparison-with-version-925)
  * [Feedback](#feedback)
  * [Contributions](#contributions)
  * [Acknowledgement](#acknowledgement)

## What's new (since version 9.2.5)?
* cppTango source code repository has been moved from subversion on 
[Sourceforge.net](https://sourceforge.net/projects/tango-cs/) to git on 
[GitHub.com](https://github.com/tango-controls/cppTango). 
The tango-cs subversion repository on SourceForge has been frozen at a temperature near 0 K. 
The [SourceForge tickets](https://sourceforge.net/p/tango-cs/_list/tickets) have been converted to 
[GitHub issues](https://github.com/tango-controls/cppTango/issues) so we keep track of all the old problems which have 
been solved... as well as all the old problems which are still there and that we should fix at some point.
* cppTango now uses [CMake](https://cmake.org) (version 2.8.12 or more recent) as build system because we think it's 
cool and much easier to use than the autotools... And we can even use it on Windows! 
Please refer to [INSTALL.md](https://github.com/tango-controls/cppTango/blob/tango-9-lts/INSTALL.md) file to get more 
details if you want to build and install from the source code.
* Integration tests are now public and built automatically using [Travis CI](https://travis-ci.org) for Linux.
* cppTango is now using [appveyor](https://www.appveyor.com) to automatically build the Tango C++ library on Windows, 
as well as Windows installers for the Tango C++ library for different Windows compilers 
(from MSVC9 to MSVC15, 32 bits and 64 bits). 
Anyone who already tried to compile the Tango C++ library on Windows can understand how this is great news!
* A map was added in MultiAttribute object to improve performances when searching for an attribute by name. 
We should have thought about it before, it is much faster to find something with a good map! 
This should greatly improve the performances in the case where a device has a large number of attributes and should 
produce less heat on your CPU. A small contribution to reduce the impact on the climate.
* An error message is now printed with the exception description if the event callback provided by the user 
(push_event() method) is throwing an exception. Yes, the user code can sometimes throw an exception!
* Support for NaN and Inf was added in float and double properties.
So now you can use the keywords _NaN_, _-NaN_, _inf_, _-inf_ and _+inf_ when writing double and float scalar and array 
properties from jive.
It is not case sensitive, so _nan_, _-NAN_ , _nAn_, _iNF_, _-Inf_ and _+INF_ are also supported for instance.
A negative NaN is extracted as NaN.
* DevVarBooleanArray DeviceData insertion and extraction operators have been added.
* It is now possible to create dynamic forwarded attributes. This works well when the device server exports only one 
device per class. Please refer to [#342][pr-342] for more details about the limitations.
* Development Debian packages for the Tango C++ library are now deployed automatically on 
[Bintray](https://bintray.com/tango-controls/debian/cppTango) when a git tag is created so you can play with it easily 
after the creation of a new git tag. But be careful here. 
This Debian package provides only libtango library and is currently not compatible with the official libtango package 
provided by Debian.
* [Codacy](https://www.codacy.com) and [Sonar](https://sonarcloud.io) tools have been integrated. 
So it is now possible to monitor the tests coverage and to benefit from static analysis and automatic code reviews from 
these tools to detect potential issues. You can click on the GitHub badges in 
[cppTango README.md](https://github.com/tango-controls/cppTango/blob/tango-9-lts/README.md) file to get more details. 
Of course, we will use these results as a good basis to improve the tests coverage and fix the issues reported by these 
tools, in the next releases.
* cppTango can now be compiled with [Clang](https://clang.llvm.org) and recent g++ versions
* A search bar has been added to Doxygen generated documentation, which is now generated and saved in 
[cppTango-docs](https://github.com/tango-controls/cppTango-docs) repository. 
The Doxygen documentation is now available directly at this URL: https://tango-controls.github.io/cppTango-docs or 
indirectly on the 
[official Tango documentation](https://tango-controls.readthedocs.io/en/latest/development/cpp-api/index.html).
* The device server reliability has been improved when removing dynamic attributes from _delete_device()_ method.
* It is now possible to insert a const C string in a DeviceData

```
Tango::DeviceData din;
din << "My Wonderful C string";
```
* We improved the integration tests by splitting one of our big monolithic tests into several executables, by adding 
new tests and by making them more reliable. Still we are far from 100% coverage.
* As python 2.x is reaching EOL soon (PEP 373) and some distributions already switched to use python 3.x as a default 
python interpreter, we follow PEP 394 and use `python2` command/link instead of `python` when invoking `cxxtestgen`.
If `python2` is missing, CMake will fall-back to using `python` (if it's on `$PATH`).

## Changes which might have an impact on users
* log4tango is no longer a separate library. It is now part of libtango library. This means that it is no longer 
required to link Tango C++ programs with liblog4tango. 
This also means that programs compiled with a previous Tango 9 release of the Tango C++ library will still require 
log4tango library at execution time until they are recompiled with the newer cppTango version and until their Makefile 
is updated to no longer link with log4tango library.
* zmq.hpp file has been removed. This adds a dependency to [cppzmq](https://github.com/zeromq/cppzmq).
* tango.h file is now generated during the build process from the tango.idl file. This adds a dependency to 
[tango-idl](https://github.com/tango-controls/tango-idl) repository. 
[omniidl](http://omniorb.sourceforge.net/omni42/omniidl.html) is also required to be able to generate tango.h file.
* The include files are now installed under _<install_prefix>_/include/tango instead of _<install_prefix>_/include. 
You may have to update your Makefiles in order to match this new include install location.
* log4tango include files are now installed under _<install_prefix>_/include/tango/log4tango. 
You may have to update your Makefiles in order to match this new include install location.
* `DeviceAttribute::get_type()` no longer returns `API_EmptyDeviceAttribute` exception when the DeviceAttribute is empty.
  It returns `DATA_TYPE_UNKNOWN` instead.
   
  **Warning! This change could lead to Segmentation fault** in code doing something like:
```  
    DeviceAttribute da;
    da = device->read_attribute("AnAttributeWithINVALIDQualityFactor");
    cout << "Type = " << Tango::CmdArgTypeName[da.get_type()] << endl;
```
   Please refer to [#510][i-510] for more details.
* 2 consecutive change events having possibly the same attribute value (but different timestamps) might be sent now 
during the Device Server startup phase. 
This was necessary to fix a bug where a client would possibly miss some important change events after a device server 
restart. Please refer to [#359](i-359) and [#503](pr-503) to get more details.
* We reduced the sleep time during the event subscription phase. 
This should speed up the startup of some GUI applications which are subscribing to events for a huge number of 
attributes. Sadly, in some hopefully rare situations (slow network, busy computers), this could lead to a situation 
where some events might be missed if they occur between the subscribe_event() call and the ZMQ subscription reception 
(which is asynchronous) on the ZMQ publisher side, potentially leading to situations where a client application might 
show out of date/incorrect values until the next event is received. 
It was already the case in the previous versions but reducing the sleep time during the event subscription will increase
the probability of this situation to happen. We are working on this topic to try to solve this edge case issue. 
We are sorry for the inconvenience. In any case, we hope this change will provide more comfort than problems.
* `Tango::string_free()` method has been added. 
The users are strongly encouraged to use this method together with `Tango::string_dup()` when dealing with 
`Tango::DevString` variables instead of using `CORBA::string_free()` and `CORBA::string_dup()`.
* We added -pedantic flag when compiling with g++ or Clang in debug mode. 
This change can generate build errors with very old compilers (e.g. g++ 3.4.6) when compiling the library debug version.
 Please remove manually -pedantic flag in configure/CMakeLists.txt if you need to compile the debug version on a very 
 old compiler.

## Bug fixes
### Event related bug fixes
* We think we found a way to get rid of heartbeat events problems! 
You know, when you were subscribing to events and receiving an _API_EventTimeout_ error event every 10 seconds, followed
by a valid event (which was corresponding to the synchronous read_attribute call occurring during the event reconnection
mechanism)?
This problem was occurring because the client and the server were computing different heartbeat event channel names in 
some situations (for instance when the TANGO_HOST was not exactly the same string on the client side and server side or 
when the DNS was not well configured. At least not configured as Tango would have expected it). 
So the client was subscribing to heartbeat events with a name and the device server was sending heartbeat events with 
another channel name. 
Since the key for a good relationship in a couple is communication, we managed to make them talk better to each other so
they now agree on a channel name during the subscription phase.
The new solution works only between Tango 9 clients using the new version of the C++ library and Tango 9 device servers
also using this new version of the library. For the older clients and device servers, the behaviour will be as before. 
This sounds like a good reason to update, isn't it? 
* A bug in ZMQ 4.2.0 and ZMQ 4.2.1 was preventing clients to receive more than 1000 events. We found a work-around so if
you use this new Tango C++ library version, you should not be impacted by this ZMQ bug unless you are 
[tuning the event system high water mark](https://tango-controls.readthedocs.io/en/latest/development/advanced/reference.html#tuning-the-event-system-buffers-hwm)
with the `CtrlSystem/EventBufferHwm` free property.
And even in this case, if you restart all your clients after changing the value of this free property, you should be 
fine until the next time you change the value of this advanced free property, which should not be the case for 
99.9999999% of our users (estimation which is not 100% accurate).
* When you push events by code, you would like in some cases to be able to push error events containing an exception so 
that the clients get notified that something wrong happened. You could do that in the previous version but the Tango 9 
clients were not receiving the error events because the device server was sending these error events using a wrong 
channel name. Then the Tango 9 clients were receiving nasty _API_MissedEvents_ errors events afterwards. We fixed that. 
This problem was on the server side.
* We also fixed a bug related to heartbeat events which could occur when a device server runs on a host with multiple 
network interfaces.
* Tango 9 device servers sometimes didn't like when a Tango 8 client had subscribed to events for one of its attributes 
and this attribute became invalid (INVALID quality factor). In this case, the device server was expressing its 
dissatisfaction by stopping activity with a segmentation fault. We managed to reconcile Tango 8 clients and Tango 9 
device servers in this use case.
* We fixed a leak in the pipes! There was indeed a memory leak when a pipe event was sent and no client had subscribed 
to this pipe event.
* If you are using ZMQ 4.2.3, peer disconnection triggers zmq_disconnect on a ZMQ socket. 
Further zmq_disconnect calls for such a socket and endpoint will fail with errno set to ENOENT.
The ZMQ Event consumer now silently ignores ENOENT failures when disconnecting the socket.
The result is that the user no longer gets useless exceptions related to zmq socket disconnection when the DeviceProxy 
destructor is invoked for instance. 
DeviceProxy destructor now also catches all the exceptions which could be thrown from unsubscribe_event().

### Other bug fixes
* When you specify a polling period for a dynamic attribute from POGO, you expect the polling to start up automatically 
for these attributes. 
The device servers used to be lazy in previous versions and didn't start the polling in this specific case. 
We found the right words to motivate them to do what you expect.
* DeviceAttribute::get_type() is now returning the correct type when the DeviceAttribute object is initialized with a 
short or enum value. 
The bug which was not present in Tango 8 was a side effect of the first implementations of the DevEnum feature.
* The class list from the Util object was not cleaned up and reassigned with the reconstructed classes during the 
execution of the RestartServer admin command. Now this is clean.
* [Device server with a user defined event loop](https://tango-controls.readthedocs.io/en/latest/development/advanced/user-loop.html#device-server-with-user-defined-event-loop)
now works as well on Windows.
* A device server had the power to remotely eradicate a client (with a spell producing a client segmentation fault) when
the client was trying to write asynchronously an attribute on the device server as the first action immediately after 
the device server restart.
We decided to reinforce the defensive skills of the client which now manages to survive in this case and it even manages
to write the remote attribute.
* We fixed a bug in Attribute::get_att_device_class() method which was generating an exception when a device server was 
trying to set an attribute property (e.g.: set_max_value) when a memorized attribute was written during the device init 
phase (write hardware at init feature).
If you didn't understand anything to this explanation, you were probably not affected by this bug.
* We fixed a memory leak in get_device_property() which was occurring only on Windows and only when using Visual Studio 
10 compiler or an older MSVC compiler version.
* We fixed many compilation warnings and build errors with recent compilers and with Doxygen

## Changelog
Please refer to [CHANGELOG.md](https://github.com/tango-controls/cppTango/blob/tango-9-lts/CHANGELOG.md) for a detailed 
list of changes and references to corresponding Github issues and pull requests.

## Source code comparison with version 9.2.5
You can see the source code comparison with Tango 9.2.5, as well as the commits and contributors on this page:
https://github.com/tango-controls/cppTango/compare/cppapi_Release_9_2_5...9.3.3

## Feedback
You can report issues on https://github.com/tango-controls/cppTango/issues or 
https://github.com/tango-controls/TangoTickets/issues for problems which may affect several repositories or when you 
don't know where to create the issue (well, now, you know!).

## Contributions
Contributions are welcome. Please do not hesitate to 
[create new Pull requests](https://help.github.com/en/articles/creating-a-pull-request) in 
[cppTango GitHub repository](https://github.com/tango-controls/cppTango).

## Acknowledgement
Many thanks to all the persons who contributed to this release, to the Tango kernel team and to the Tango community for 
its feedback, bug reports and tests.

[pr-342]: https://github.com/tango-controls/cppTango/pull/342
[i-359]: https://github.com/tango-controls/cppTango/issues/359
[pr-503]: https://github.com/tango-controls/cppTango/pull/503
[i-510]: https://github.com/tango-controls/cppTango/issues/510