# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## [UNRELEASED]
### Added
- Add asyn_reconnection test (#502)

### Changed
- Replace CORBA::string_dup with Tango::string_dup (#474)

### Fixed
- Fix memory leak on get_device_property() on Windows when code compiled with Visual Studio 10 or older (#439, #488)
- Fix cxx_fwd_att occasional failure (#384,#428,#493)
- Fix "Change event subscription blind to change events right after device server restart" issue (#359,#503). 
**This bug fix may trigger the reception of 2 consecutive events having possibly the same attribute value (but different
 timestamps) during the Device Server startup phase.**
- Fix client crash in case of asynchronous write_attribute executed immediately after reconnection (#502)

## [9.3.2] - 2018-09-12 [unstable]
### Added
- Add Tango::string_free (#460)
- Integrate Coveralls and Sonar (#369, #478)

### Changed
- Tests: Split runner executable into several executables (#275, #476).
- Doxygen: upgrade to doxygen 1.8.13

## Removed
- Disable log4tango benchmark tests.

### Fixed
- Fix memory leak in zmq_event_subscription_change() (#457,#460)
- Fix bug impacting device servers using dynamic attributes (#458,#459)
- Fix reconnection issues when a client subscribes to several events (#455)
- Fix some event compatibility issues with device server <= Tango 8 (#456)
- Fix event field in EventData structure passed to user's callback (broken in Tango 9.3.0 and 9.3.1)
- Fix attribute name in EventData structure passed to user's callback for Attribute Config events
- Fix a bug occurring when an event is pushed at the same time as an event (re)subscription occurs (#484, #485)
- Fix event name (EventData.event) passed to client's Callback after Tango 9 attribute reconnection (#486)
- Fix some doxygen warnings

## [9.3.1] - 2018-05-23 [unstable]
### Changed
- Add -pedantic for GCC/Clang and debug mode (#441). **This change can generate 
  errors with very old compilers (e.g. gcc 3.4.6) when compiling the lib debug 
  version.** Please remove manually -pedantic flag in configure/CMakeLists.txt if
  you need to compile the debug version on a very old compiler.
- Generate static library (#17)

### Fixed
- DServerSignal: Include errno header instead of trying to manually get the declaration right (#441)
- Remove GNU_CXX definition for GCC/Clang (#441)
- filedatabase.cpp: Remove stray semicolon (#441)
- FileDatabase: Use correct format specifier for unsigned arguments to sscanf (#441)
- EventConsumer::connect_event: Fix misleading indentation (#441)
- PollThread: Fix multiline macro definitions (#451)
- Fix compilation definitions (#437)
- Replace asserts in EventConsumer::initialize_received_from_admin with exceptions (#453)

## [9.3.0] - 2018-05-04 [unstable]
### Fixed
- Fix FQDN events (#423,#315)
   Limitations:
     - in case DeviceProxy is used for event subscription, client will get incorrect value 
	 in the event callback if short domain name is used in env.TANGO_HOST 
	 e.g. hzgc103 -> hzgc103.desy.de
- Fix memory leak when pipe event is sent and there is no client (#447,#448)
- Set the ZMQ Receive Buffer High Water Mark only if it changes (#444,#445) 
   to reduce the impact of a bug in ZMQ 4.2.0 and ZMQ 4.2.1 which was fixed in 
   commit https://github.com/zeromq/libzmq/commit/edc770d680b2be5a633526bd2c724d66406c8360

## [9.2.8] - 2018-03-29 [unstable]
### Changed
- Cmake release targets #438 
- **Install include files in <install_prefix>/include/tango directory instead of 
   <install_prefix>/include**.

### Fixed
- Fix appveyor build

## [9.2.7] - 2018-02-27 [unstable]
### Added
- **Use CMake** 
- Set CMake minimum requirement to CMake 2.8.12 (#422)
- Add DevVarBooleanArray operators (#347)
- Add support for NaN and inf in device properties (#360,#362,#371)
- Refactor DeviceAttribute code (#394). DeviceAttribute::init_common_class_members() method is now private
- Forwarded dynamic attributes (#342)
- Build on Windows using CMake and setup appveyor CI (#413)
- Enable Travis CI (#375, #417, #418, #419)
- tango_config.h: Add clang detection (#442)
- Doxygen: Add searchbar (#432)

### Changed
- DeviceAttribute::get_type() no longer returns API_EmptyDeviceAttribute exception when the DeviceAttribute is empty.
  It returns DATA_TYPE_UNKNOWN instead (#393, #509).
   
  **Warning! This change could lead to Segmentation fault** in code doing things like:
```  
    DeviceAttribute da;
    da = device->read_attribute("AnAttributeWithINVALIDQualityFactor");
    cout << "Type = " << Tango::CmdArgTypeName[da.get_type()] << endl;
```
  Please refer to #510 for more details.
- Improve ZmqEventSubscriptionChange command argin and argout descriptions (355da96ac7d82d4a7727f68f3b093c98f087fd98)
- Generate files from IDL (**add a dependency to tango-idl repository. omniidl is required to be able to generate some 
  files from  CORBA IDL**)
- Get exception message from event callback (#431)
- Add a map in MultiAttribute object to improve performances (#424, #430)
- A small Doxygen improvement - take version from cmake (#436)

### Removed
- Remove zmq.hpp (#421). **Add a dependency to https://github.com/zeromq/cppzmq**

### Fixed
- Add missing event implementation with _TG_WINDOWS_ ifdef (#349)
- Fix tango-controls/TangoTickets#3 (Events received with UNKNOWN data format and without values when subscribing from
  a Tango 9 client to a Tango 7 server DevShort attribute)
- Fix bug related to heartbeat endpoints which could eventually occur when the device server runs on a host with
  multiple network interfaces (36aca17d43a97ebe09fdc825941cb39f6c48289e)
- Fix bug after admin device RestartServer command (#358)
- Fix bug in Attribute::get_att_device_class() (4a00c8e4203668c247a2ada6e309470cd53744d3) 
- Fix RcsId warnings (#374)
- Fix warnings reported by gcc 6.3 related to misleading indentations (#376)
- Fix "unused parameter" warnings (#379)
- Fix shift-negative-value compilation warnings reported by g++ 6.3 (#383)
- Fix DeviceAttribute constructor for short and enum data types (#392,#393)
- Tango 9 LTS fix#395 (Inserting const C string in DeviceData) #396
- Fix bug leading occasionally to segmentation faults of Tango 9 device servers (#398)
- Fix bug in event name when pushing an error event to IDL5 clients (#400)
- Attribute: Don't treat an integer as an char* (#403, #406)
- pointer-cast-size-mismatch attrgetsetprop.cpp:200 (#339, #406)
- Fix build errors when using C++17 (#405, #425)
- Remove event subscription sleeps on linux (#415)
- Fix issue with polling not starting automatically on dynamic attributes (#427)
- Add noreturn to remove some compilation warnings (#435)
- CPU load when device has large number of attributes (#404) 
- push event performance issue (#244)

[Unreleased]: https://github.com/tango-controls/cppTango/compare/9.3.2...HEAD
[9.3.2]: https://github.com/tango-controls/cppTango/compare/9.3.1...9.3.2
[9.3.1]: https://github.com/tango-controls/cppTango/compare/9.3.0...9.3.1
[9.3.0]: https://github.com/tango-controls/cppTango/compare/test-auto-deploy...9.3.0
[9.2.8]: https://github.com/tango-controls/cppTango/compare/9.2.7...test-auto-deploy
[9.2.7]: https://github.com/tango-controls/cppTango/compare/cppapi_Release_9_2_5...9.2.7
