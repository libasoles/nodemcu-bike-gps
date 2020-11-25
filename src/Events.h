#pragma once

#include <vector>

using namespace std;

enum class Event {
    BOOT,
    IDLE,
    BLE_SCANNING_ADVERTISERS,
    BLE_PAIRED,
    BLE_CONNECTION_LOST,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP_LEFT,
    DIRECTION_DOWN_LEFT,
    DIRECTION_UP_RIGHT,
    DIRECTION_DOWN_RIGHT,
    NO_DIRECTION,
    MODE_CHANGED,
    MODE_CHANGED_TO_SPEED,
    MODE_CHANGED_TO_NAVIGATION,
    SPEED_CHANGED,
    MAX_SPEED_UPDATED,
    COLOR_SCHEMA_CHANGED,
    DEBUG_RED,
    DEBUG_GREEN,
    DEBUG_BLUE,
    DEBUG_YELLOW,
};

class EventObserver {
public:
    virtual void reactTo(Event eventName, std::vector<uint8_t> args = {0}) = 0;
};

class EventBroadcaster {
public:
    void virtual subscribe(EventObserver *listener) {
        listeners.push_back(listener);
    }

protected:
    vector<EventObserver *> listeners;

    void virtual notify(Event event, std::vector<uint8_t> args = {0}) {
        for (int i = 0; i < listeners.size(); ++i)
            listeners[i]->reactTo(event, args);
    }
};
