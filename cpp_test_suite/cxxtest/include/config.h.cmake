#ifndef Config_h
#define Config_h

#include <string>

namespace Tango {
    const std::string kProjectBinaryDir{"@PROJECT_BINARY_DIR@"};
    const std::string kStartServerCmd{"@CMAKE_CURRENT_BINARY_DIR@/bin/start_server.sh "};
    const std::string kKillServerCmd{"@CMAKE_CURRENT_BINARY_DIR@/bin/kill_server.sh"};
}

#endif