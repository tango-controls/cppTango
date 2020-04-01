# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## [9.3.4] - 2020-04-01

### Fixed
- Be compatible with all in-use cppzmq versions ([#530][pr-530], [#561][pr-561], [#273][i-273], [#499][i-499], [#535][i-535])
- Fix snprintf detection. Now the `LOG_*` macros work again ([#658][pr-658])
- Various spelling errors (found by lintian) ([#647][pr-647], [#621][pr-621])
- CMake fixes regarding `CMAKE_BUILD_TYPE` logic ([#629][pr-629])
- Fix race conditions between polling threads and user threads pushing events ([#665][pr-665], [#511][i-511])
- Avoid undefined interface warning msvc ([#664][pr-664])
- travis.yml: remove deprecated skip_cleanup ([#683][pr-683])
- Fix memory leak in Attribute::get_att_device_class() ([#677][pr-677])
- Fix crash during alarm state evaluation if attribute value is not set ([#330][i-330], [#681][pr-681])

### Added
- Document contributing process and use CODEOWNERS for easier review organization ([#639][pr-639])
- Add testing with debian buster ([#597][pr-597])
- Add CMake option to disable building the test suite ([#689][pr-689])
- Add CMake switch to disable building MMX ([#674][pr-674])
- appveyor.yml: Add inline deployment on tag push ([#690][pr-690])

## [9.3.3] - 2019-03-29

### Added
- Add asyn_reconnection test ([#502][pr-502])
- Add .gitignore ([#522][pr-522])
- Add Visual Studio 2017 compatibility. Appveyor now builds for Visual Studio 2017 in addition to all the previously
supported versions ([#533][pr-533])

### Changed
- Replace CORBA::string_dup calls with Tango::string_dup ([#474][pr-474])
- Replace CORBA::string_free calls with Tango::string_free ([#539][pr-539])
- Make it possible to remove dynamic attributes from delete_device() by moving the location where the device is removed
from the device list ([#518][pr-518])
- Use python2 (if available) to run cxxtestgen ([#523][pr-523])
- Switch to Travis Xenial infrastructure ([#516][pr-516])
- Catch exceptions from unsubscribe_event() in DeviceProxy destructor ([#521][pr-521])
- Improve install documentation ([#529][pr-529])
- Install log4tango include files under *<install_prefix>*/include/tango/log4tango ([#517][pr-517])
- Update Doxygen documentation ([#532][i-532], [#544][pr-544])
- Update Debian 7 and Debian 8 Travis Docker files after Debian Wheezy end of life and jessie-updates removal
  ([#546][i-546], [#549][pr-549])

### Fixed
- Fix memory leak in get_device_property() on Windows when code compiled with Visual Studio 10 or older
([#439][i-439], [#488][pr-488])
- Fix cxx_fwd_att occasional failure ([#384][i-384], [#428][i-428], [#493][pr-493])
- Fix "Change event subscription blind to change events right after device server restart" issue
([#359][i-359], [#503][pr-503]).
**This bug fix may trigger the reception of 2 consecutive events having possibly the same attribute value
(but different timestamps) during the Device Server startup phase.**
- Fix client crash in case of asynchronous write_attribute executed immediately after reconnection ([#502][pr-502])
- Fix issue with dynamic attributes when a device is restarted on a device server exporting several devices which are
creating dynamic attributes ([#458][i-458], [#508][pr-508])
- Fix uncaught DevFailed exception in DeviceProxy destructor in cxx_stateless_subscription test case when using
ZMQ 4.2.3 ([#514][i-514]). Since [zeromq/libzmq@edb4ca1][libzmq-c-edb4ca1], peer disconnection triggers zmq_disconnect
on a zmq socket.
Further zmq_disconnect calls for such socket and endpoint will fail with errno set to ENOENT.
The patch provided in [#520][pr-520] ignores such failure.
- Fixed a broken dependency on generated Debian package ([5c74e8d][c-5c74e8d])
- Fix appveyor.yml after appveyor windows image update ([#537][pr-537])
- Fix event compatibility issues with device servers exporting Device_4 devices. Fix bug with Pipe, Data Ready and
Device Interface Change events. Fix event channel name issues in these different cases. ([#492][i-492], [#531][pr-531])
- Fix WAttribute::rollback() for DevUshort WRITE attributes ([#541][i-541], [#542][pr-542])

## [9.3.2] - 2018-09-12 [unstable]
### Added
- Add Tango::string_free ([#460][pr-460])
- Integrate Coveralls and Sonar ([#369][i-369], [#478][pr-478])

### Changed
- Tests: Split runner executable into several executables ([#275][i-275], [#476][pr-476]).
- Doxygen: upgrade to doxygen 1.8.13

## Removed
- Disable log4tango benchmark tests.

### Fixed
- Fix memory leak in zmq_event_subscription_change() ([#457][i-457], [#460][pr-460])
- Fix bug impacting device servers using dynamic attributes ([#458][i-458], [#459][pr-459])
- Fix reconnection issues when a client subscribes to several events ([#455][i-455])
- Fix some event compatibility issues with device server <= Tango 8 ([#456][i-456])
- Fix event field in EventData structure passed to user's callback (broken in Tango 9.3.0 and 9.3.1)
- Fix attribute name in EventData structure passed to user's callback for Attribute Config events
- Fix a bug occurring when an event is pushed at the same time as an event (re)subscription occurs
([#484][i-484], [#485][pr-485])
- Fix event name (EventData.event) passed to client's Callback after Tango 9 attribute reconnection ([#486][pr-486])
- Fix some doxygen warnings

## [9.3.1] - 2018-05-23 [unstable]
### Changed
- Add -pedantic for GCC/Clang and debug mode ([#441][pr-441]). **This change can generate
  errors with very old compilers (e.g. gcc 3.4.6) when compiling the lib debug
  version.** Please remove manually -pedantic flag in configure/CMakeLists.txt if
  you need to compile the debug version on a very old compiler.
- Generate static library ([#17][i-17])

### Fixed
- DServerSignal: Include errno header instead of trying to manually get the declaration right ([#441][pr-441])
- Remove GNU_CXX definition for GCC/Clang ([#441][pr-441])
- filedatabase.cpp: Remove stray semicolon ([#441][pr-441])
- FileDatabase: Use correct format specifier for unsigned arguments to sscanf ([#441][pr-441])
- EventConsumer::connect_event: Fix misleading indentation ([#441][pr-441])
- PollThread: Fix multiline macro definitions ([#451][pr-451])
- Fix compilation definitions ([#437][pr-437])
- Replace asserts in EventConsumer::initialize_received_from_admin with exceptions ([#453][i-453])

## [9.3.0] - 2018-05-04 [unstable]
### Fixed
- Fix FQDN events ([#423][pr-423], [#315][i-315])
   Limitations:
     - in case DeviceProxy is used for event subscription, client will get incorrect value
	 in the event callback if short domain name is used in env.TANGO_HOST
	 e.g. hzgc103 -> hzgc103.desy.de
- Fix memory leak when pipe event is sent and there is no client ([#447][i-447],[#448][pr-448])
- Set the ZMQ Receive Buffer High Water Mark only if it changes ([#444][i-444],[#445][pr-445])
   to reduce the impact of a bug in ZMQ 4.2.0 and ZMQ 4.2.1 which was fixed in
   commit [zeromq/libzmq@edc770d][libzmq-c-edc770d].

## [9.2.8] - 2018-03-29 [unstable]
### Changed
- Cmake release targets ([#438][pr-438])
- **Install include files in <install_prefix>/include/tango directory instead of
   <install_prefix>/include**.

### Fixed
- Fix appveyor build

## [9.2.7] - 2018-02-27 [unstable]
### Added
- **Use CMake**
- Set CMake minimum requirement to CMake 2.8.12 ([#422][pr-422])
- Add DevVarBooleanArray operators ([#347][pr-347])
- Add support for NaN and inf in device properties ([#360][i-360], [#362][pr-362],[ #371][pr-371])
- Refactor DeviceAttribute code ([#394][pr-394]). DeviceAttribute::init_common_class_members() method is now private
- Forwarded dynamic attributes ([#342][pr-342])
- Build on Windows using CMake and setup appveyor CI ([#413][pr-413])
- Enable Travis CI ([#375][pr-375], [#417][pr-417], [#418][pr-418], [#419][pr-419])
- tango_config.h: Add clang detection ([#442][pr-442])
- Doxygen: Add searchbar ([#432][pr-432])

### Changed
- DeviceAttribute::get_type() no longer throws an API_EmptyDeviceAttribute exception when the DeviceAttribute is empty.
  It returns DATA_TYPE_UNKNOWN instead ([#393][pr-393], [#509][i-509]).

  **Warning! This change could lead to Segmentation fault** in code doing things like:
```
    DeviceAttribute da;
    da = device->read_attribute("AnAttributeWithINVALIDQualityFactor");
    cout << "Type = " << Tango::CmdArgTypeName[da.get_type()] << endl;
```
  Please refer to [#510][i-510] for more details.
- Improve ZmqEventSubscriptionChange command argin and argout descriptions ([355da96][c-355da96])
- Generate files from IDL (**add a dependency to tango-idl repository. omniidl is required to be able to generate some
  files from  CORBA IDL**)
- Get exception message from event callback ([#431][pr-431])
- Add a map in MultiAttribute object to improve performances ([#424][i-424], [#430][pr-430])
- A small Doxygen improvement - take version from cmake ([#436][pr-436])
- Reduce event subscription sleeps on linux ([#415][pr-415]).

**Warning**: There is no guarantee that after a successful call to subscribe_event() that a subscriber will receive all
the events it is interested in. In some situations, some important events might be missed if they occur between the
subscribe_event() call and the ZMQ subscription reception on the ZMQ publisher side, potentially leading to situations
where a client application might show out of date/incorrect values.
- Tango 9 LTS fix[#395][i-395] (Inserting const C string in DeviceData) ([#396][pr-396])

### Removed
- Remove zmq.hpp ([#266][i-266], [#421][pr-421]). **Add a dependency to https://github.com/zeromq/cppzmq**

### Fixed
- Add missing event implementation with \_TG_WINDOWS\_ ifdef ([#349][i-349])
- Fix [TangoTickets#3][tt-i3] (Events received with UNKNOWN data format and without values when subscribing from
  a Tango 9 client to a Tango 7 server DevShort attribute)
- Fix bug related to heartbeat endpoints which could eventually occur when the device server runs on a host with
  multiple network interfaces ([36aca17][c-36aca17])
- Fix bug after admin device RestartServer command ([#358][pr-358])
- Fix bug in Attribute::get_att_device_class() ([4a00c8e][c-4a00c8e])
- Fix RcsId warnings ([#374][pr-374])
- Fix warnings reported by gcc 6.3 related to misleading indentations ([#376][pr-376])
- Fix "unused parameter" warnings ([#379][pr-379])
- Fix shift-negative-value compilation warnings reported by g++ 6.3 ([#383][pr-383])
- Fix DeviceAttribute constructor for short and enum data types ([#392][i-392], [#393][pr-393])
- Fix bug leading occasionally to segmentation faults of Tango 9 device servers ([#398][pr-398])
- Fix bug in event name when pushing an error event to IDL5 clients ([#400][pr-400])
- Attribute: Don't treat an integer as an char* ([#403][pr-403], [#406][pr-406])
- pointer-cast-size-mismatch attrgetsetprop.cpp:200 ([#339][i-339], [#406][pr-406])
- Fix build errors when using C++17 ([#405][pr-405], [#425][pr-425])
- Fix issue with polling not starting automatically on dynamic attributes ([#427][pr-427])
- Add noreturn to remove some compilation warnings ([#435][pr-435])
- CPU load when device has large number of attributes ([#404][pr-404])
- push event performance issue ([#244][i-244])

[9.3.4]: https://github.com/tango-controls/cppTango/compare/9.3.3...9.3.4
[9.3.3]: https://github.com/tango-controls/cppTango/compare/9.3.2...9.3.3
[9.3.2]: https://github.com/tango-controls/cppTango/compare/9.3.1...9.3.2
[9.3.1]: https://github.com/tango-controls/cppTango/compare/9.3.0...9.3.1
[9.3.0]: https://github.com/tango-controls/cppTango/compare/test-auto-deploy...9.3.0
[9.2.8]: https://github.com/tango-controls/cppTango/compare/9.2.7...test-auto-deploy
[9.2.7]: https://github.com/tango-controls/cppTango/compare/cppapi_Release_9_2_5...9.2.7

[c-4a00c8e]: https://github.com/tango-controls/cppTango/commit/4a00c8e4203668c247a2ada6e309470cd53744d3
[c-36aca17]: https://github.com/tango-controls/cppTango/commit/36aca17d43a97ebe09fdc825941cb39f6c48289e
[c-355da96]: https://github.com/tango-controls/cppTango/commit/355da96ac7d82d4a7727f68f3b093c98f087fd98
[c-5c74e8d]: https://github.com/tango-controls/cppTango/commit/5c74e8d33ce85036c9348a09b894e05ac3e8cdf9

[libzmq-c-edc770d]: https://github.com/zeromq/libzmq/commit/edc770d680b2be5a633526bd2c724d66406c8360
[libzmq-c-edb4ca1]: https://github.com/zeromq/libzmq/commit/edb4ca1

[tt-i3]: https://github.com/tango-controls/TangoTickets/issues/3

[i-17]: https://github.com/tango-controls/cppTango/issues/17
[i-244]: https://github.com/tango-controls/cppTango/issues/244
[i-266]: https://github.com/tango-controls/cppTango/issues/266
[i-273]: https://github.com/tango-controls/cppTango/issues/273
[i-275]: https://github.com/tango-controls/cppTango/issues/275
[i-315]: https://github.com/tango-controls/cppTango/issues/315
[i-330]: https://github.com/tango-controls/cppTango/issues/330
[i-339]: https://github.com/tango-controls/cppTango/issues/339
[pr-342]: https://github.com/tango-controls/cppTango/pull/342
[pr-347]: https://github.com/tango-controls/cppTango/pull/347
[i-349]: https://github.com/tango-controls/cppTango/issues/349
[pr-358]: https://github.com/tango-controls/cppTango/pull/358
[i-359]: https://github.com/tango-controls/cppTango/issues/359
[i-360]: https://github.com/tango-controls/cppTango/issues/360
[i-369]: https://github.com/tango-controls/cppTango/issues/369
[pr-362]: https://github.com/tango-controls/cppTango/pull/362
[pr-371]: https://github.com/tango-controls/cppTango/pull/371
[pr-374]: https://github.com/tango-controls/cppTango/pull/374
[pr-375]: https://github.com/tango-controls/cppTango/pull/375
[pr-376]: https://github.com/tango-controls/cppTango/pull/376
[pr-379]: https://github.com/tango-controls/cppTango/pull/379
[pr-383]: https://github.com/tango-controls/cppTango/pull/383
[i-384]: https://github.com/tango-controls/cppTango/issues/384
[i-392]: https://github.com/tango-controls/cppTango/issues/392
[pr-393]: https://github.com/tango-controls/cppTango/pull/393
[pr-394]: https://github.com/tango-controls/cppTango/pull/394
[i-395]: https://github.com/tango-controls/cppTango/issues/395
[pr-396]: https://github.com/tango-controls/cppTango/pull/396
[pr-398]: https://github.com/tango-controls/cppTango/pull/398
[pr-400]: https://github.com/tango-controls/cppTango/pull/400
[pr-403]: https://github.com/tango-controls/cppTango/pull/403
[pr-404]: https://github.com/tango-controls/cppTango/pull/404
[pr-405]: https://github.com/tango-controls/cppTango/pull/405
[pr-406]: https://github.com/tango-controls/cppTango/pull/406
[pr-413]: https://github.com/tango-controls/cppTango/pull/413
[pr-415]: https://github.com/tango-controls/cppTango/pull/415
[pr-417]: https://github.com/tango-controls/cppTango/pull/417
[pr-418]: https://github.com/tango-controls/cppTango/pull/418
[pr-419]: https://github.com/tango-controls/cppTango/pull/419
[pr-421]: https://github.com/tango-controls/cppTango/pull/421
[pr-422]: https://github.com/tango-controls/cppTango/pull/422
[pr-423]: https://github.com/tango-controls/cppTango/pull/423
[i-424]: https://github.com/tango-controls/cppTango/issues/424
[pr-425]: https://github.com/tango-controls/cppTango/pull/425
[pr-427]: https://github.com/tango-controls/cppTango/pull/427
[i-428]: https://github.com/tango-controls/cppTango/issues/428
[pr-430]: https://github.com/tango-controls/cppTango/pull/430
[pr-431]: https://github.com/tango-controls/cppTango/pull/431
[pr-432]: https://github.com/tango-controls/cppTango/pull/432
[pr-435]: https://github.com/tango-controls/cppTango/pull/435
[pr-436]: https://github.com/tango-controls/cppTango/pull/436
[pr-437]: https://github.com/tango-controls/cppTango/pull/437
[pr-438]: https://github.com/tango-controls/cppTango/pull/438
[i-439]: https://github.com/tango-controls/cppTango/issues/439
[pr-441]: https://github.com/tango-controls/cppTango/pull/441
[pr-442]: https://github.com/tango-controls/cppTango/pull/442
[i-444]: https://github.com/tango-controls/cppTango/issues/444
[pr-445]: https://github.com/tango-controls/cppTango/pull/445
[i-447]: https://github.com/tango-controls/cppTango/issues/447
[pr-448]: https://github.com/tango-controls/cppTango/pull/448
[pr-451]: https://github.com/tango-controls/cppTango/pull/451
[i-453]: https://github.com/tango-controls/cppTango/issues/453
[i-455]: https://github.com/tango-controls/cppTango/issues/455
[i-456]: https://github.com/tango-controls/cppTango/issues/456
[i-457]: https://github.com/tango-controls/cppTango/issues/457
[i-458]: https://github.com/tango-controls/cppTango/issues/458
[pr-459]: https://github.com/tango-controls/cppTango/pull/459
[pr-460]: https://github.com/tango-controls/cppTango/pull/460
[pr-474]: https://github.com/tango-controls/cppTango/pull/474
[pr-476]: https://github.com/tango-controls/cppTango/pull/476
[pr-478]: https://github.com/tango-controls/cppTango/pull/478
[i-484]: https://github.com/tango-controls/cppTango/issues/484
[pr-485]: https://github.com/tango-controls/cppTango/pull/485
[pr-486]: https://github.com/tango-controls/cppTango/pull/486
[pr-488]: https://github.com/tango-controls/cppTango/pull/488
[i-492]: https://github.com/tango-controls/cppTango/issues/492
[pr-493]: https://github.com/tango-controls/cppTango/pull/493
[i-499]: https://github.com/tango-controls/cppTango/issues/499
[pr-502]: https://github.com/tango-controls/cppTango/pull/502
[pr-503]: https://github.com/tango-controls/cppTango/pull/503
[pr-508]: https://github.com/tango-controls/cppTango/pull/508
[i-509]: https://github.com/tango-controls/cppTango/issues/509
[i-510]: https://github.com/tango-controls/cppTango/issues/510
[i-511]: https://github.com/tango-controls/cppTango/issues/511
[i-514]: https://github.com/tango-controls/cppTango/issues/514
[pr-516]: https://github.com/tango-controls/cppTango/pull/516
[pr-517]: https://github.com/tango-controls/cppTango/pull/517
[pr-518]: https://github.com/tango-controls/cppTango/pull/518
[pr-520]: https://github.com/tango-controls/cppTango/pull/520
[pr-521]: https://github.com/tango-controls/cppTango/pull/521
[pr-522]: https://github.com/tango-controls/cppTango/pull/522
[pr-523]: https://github.com/tango-controls/cppTango/pull/523
[pr-529]: https://github.com/tango-controls/cppTango/pull/529
[pr-530]: https://github.com/tango-controls/cppTango/pull/530
[pr-531]: https://github.com/tango-controls/cppTango/pull/531
[i-532]: https://github.com/tango-controls/cppTango/issues/532
[pr-533]: https://github.com/tango-controls/cppTango/pull/533
[i-535]: https://github.com/tango-controls/cppTango/issues/535
[pr-537]: https://github.com/tango-controls/cppTango/pull/537
[pr-539]: https://github.com/tango-controls/cppTango/pull/539
[i-541]: https://github.com/tango-controls/cppTango/issues/541
[pr-542]: https://github.com/tango-controls/cppTango/pull/542
[pr-544]: https://github.com/tango-controls/cppTango/pull/544
[i-546]: https://github.com/tango-controls/cppTango/issues/546
[pr-549]: https://github.com/tango-controls/cppTango/pull/549
[pr-561]: https://github.com/tango-controls/cppTango/pull/561
[pr-597]: https://github.com/tango-controls/cppTango/pull/597
[pr-621]: https://github.com/tango-controls/cppTango/pull/621
[pr-629]: https://github.com/tango-controls/cppTango/pull/629
[pr-639]: https://github.com/tango-controls/cppTango/pull/639
[pr-647]: https://github.com/tango-controls/cppTango/pull/647
[pr-658]: https://github.com/tango-controls/cppTango/pull/658
[pr-664]: https://github.com/tango-controls/cppTango/pull/664
[pr-665]: https://github.com/tango-controls/cppTango/pull/665
[pr-674]: https://github.com/tango-controls/cppTango/pull/674
[pr-677]: https://github.com/tango-controls/cppTango/pull/677
[pr-681]: https://github.com/tango-controls/cppTango/pull/681
[pr-683]: https://github.com/tango-controls/cppTango/pull/683
[pr-689]: https://github.com/tango-controls/cppTango/pull/689
[pr-690]: https://github.com/tango-controls/cppTango/pull/690