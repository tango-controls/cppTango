# cppTango
This repository is the result of svn2git migration of tango source code. It is combined from api/cppapi, api/log4tango, api/cpp_test_suite anf api/cpp_test_ds

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

# How to build and install using cmake

`mkdir build; cd build; cmake .. [-DCMAKE_INSTALL_PREFIX=<desired installation path>] [-DOMNI_BASE=<omniORB4 home folder>] [-DZMQ_BASE=<zmq home folder>] [-DCMAKE_BUILD_TYPE=RELEASE|DEBUG] [-DCMAKE_VERBOSE_MAKEFILE=true]; make; make install`

# Links

[Tango Controls web page](http://tango-controls.org)