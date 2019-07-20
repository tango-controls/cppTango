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
    long attribute_id;

    EventClientLibVersions change;
    EventClientLibVersions archive;
    EventClientLibVersions periodic;
    EventClientLibVersions user;
    EventClientLibVersions att_conf;

    bool has_quality_event_clients;
    bool has_data_ready_event_clients;

    bool is_notifd_transport;
    bool is_zeromq_transport;
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
