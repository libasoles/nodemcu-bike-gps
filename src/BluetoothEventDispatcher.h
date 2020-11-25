#pragma once

#include <unordered_map>
#include "Events.h"

class BluetoothEventDispatcher: public EventBroadcaster {
public:
    void broadcastMode(uint8_t mode);
    void broadcastDirection(uint8_t direction);
    void broadcastSpeed(uint8_t speed);
    void broadcastMaxSpeed(uint8_t speed);
    void broadcastColorSchema(uint8_t schema);

private:
    typedef std::unordered_map<uint8_t, Event> DirectionsIndex;
    DirectionsIndex directionsIndex = {
            { 8, Event::DIRECTION_UP },
            { 2, Event::DIRECTION_DOWN },
            { 6, Event::DIRECTION_RIGHT },
            { 4, Event::DIRECTION_LEFT },
            { 7, Event::DIRECTION_UP_LEFT },
            { 9, Event::DIRECTION_UP_RIGHT },
            { 1, Event::DIRECTION_DOWN_LEFT },
            { 3, Event::DIRECTION_DOWN_RIGHT },
            { 5, Event::NO_DIRECTION },
    };
};
