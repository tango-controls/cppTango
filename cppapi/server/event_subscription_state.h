#ifndef _EVENT_SUBSCRIPTION_STATE_H
#define _EVENT_SUBSCRIPTION_STATE_H

#include <map>
#include <vector>
#include <string>

namespace Tango
{

using EventClientLibVersion = int;
using EventClientLibVersions = std::vector<EventClientLibVersion>;

struct EventSubscriptionState
{
    std::string attribute_name;

    EventClientLibVersions change;
    EventClientLibVersions archive;
    EventClientLibVersions periodic;
    EventClientLibVersions user;
    EventClientLibVersions att_conf;

    bool quality;
    bool data_ready;
    bool dev_intr_change;

    bool notifd;
    bool zmq;
};

using EventSubscriptionStates = std::vector<EventSubscriptionState>;
using ServerEventSubscriptionState = std::map<std::string, EventSubscriptionStates>;

}

#endif
