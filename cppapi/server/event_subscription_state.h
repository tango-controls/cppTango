#ifndef _EVENT_SUBSCRIPTION_STATE_H
#define _EVENT_SUBSCRIPTION_STATE_H

#include <set>
#include <map>
#include <vector>
#include <string>

namespace Tango
{

typedef int EventClientLibVersion;
typedef std::set<EventClientLibVersion> EventClientLibVersions;

struct AttributeEventSubscriptionState
{
    long attr_id;
    EventClientLibVersions change;
    EventClientLibVersions archive;
    bool quality;
    EventClientLibVersions periodic;
    EventClientLibVersions user;
    EventClientLibVersions att_conf;
    bool data_ready;
    bool notifd;
    bool zmq;
};

typedef std::vector<AttributeEventSubscriptionState> AttributeEventSubscriptionStates;

struct DeviceEventSubscriptionState
{
    bool has_interface_change_event_clients;
    AttributeEventSubscriptionStates attribute_events;
};

typedef std::map<std::string, DeviceEventSubscriptionState> EventSubscriptionState;

}

#endif
