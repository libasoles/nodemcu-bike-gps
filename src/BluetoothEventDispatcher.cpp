#include "BluetoothEventDispatcher.h"
#include "utils.h"

void BluetoothEventDispatcher::broadcastMode(uint8_t mode) {
    notify(Event::MODE_CHANGED, asVector(mode));
}

void BluetoothEventDispatcher::broadcastDirection(uint8_t direction) {
    Event event = directionsIndex.at(direction);
    notify(event);
}

void BluetoothEventDispatcher::broadcastSpeed(uint8_t speed) {
    notify(Event::SPEED_CHANGED, asVector(speed));
}

void BluetoothEventDispatcher::broadcastMaxSpeed(uint8_t speed) {
    notify(Event::MAX_SPEED_UPDATED, asVector(speed));
}

void BluetoothEventDispatcher::broadcastColorSchema(uint8_t schema) {
    notify(Event::COLOR_SCHEMA_CHANGED, asVector(schema));
}


