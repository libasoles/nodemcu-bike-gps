#pragma once

#include <Arduino.h>
#include <vector>
#include <unordered_map>
#include "Events.h"
#include "utils.h"

using namespace std;

#define NUMBER_1 49
#define NUMBER_2 50
#define NUMBER_3 51
#define NUMBER_4 52
#define NUMBER_5 53
#define NUMBER_6 54
#define NUMBER_7 55
#define NUMBER_8 56
#define NUMBER_9 57
#define NUMBER_0 48
#define CHAR_Q 113
#define CHAR_W 119
#define CHAR_E 101
#define CHAR_A 97
#define CHAR_D 100
#define CHAR_Z 122
#define CHAR_X 120
#define CHAR_C 99
#define CHAR_R 114
#define CHAR_G 103
#define CHAR_B 98
#define CHAR_Y 121
#define CHAR_S 115
#define CHAR_M 109
#define CHAR_V 118
#define CHAR_N 110

const std::unordered_map<uint8_t, Event> actions = {
        {NUMBER_1, Event::IDLE},
        {NUMBER_2, Event::BOOT},
        {NUMBER_3, Event::BLE_SCANNING_ADVERTISERS},
        {NUMBER_4, Event::BLE_PAIRED},
        {NUMBER_5, Event::BLE_CONNECTION_LOST},
        {NUMBER_8, Event::SPEED_CHANGED},
        {NUMBER_9, Event::SPEED_CHANGED},
        {NUMBER_0, Event::SPEED_CHANGED},
        {CHAR_S,   Event::COLOR_SCHEMA_CHANGED},
        {CHAR_W,   Event::DIRECTION_UP},
        {CHAR_X,   Event::DIRECTION_DOWN},
        {CHAR_A,   Event::DIRECTION_LEFT},
        {CHAR_D,   Event::DIRECTION_RIGHT},
        {CHAR_Q,   Event::DIRECTION_UP_LEFT},
        {CHAR_E,   Event::DIRECTION_UP_RIGHT},
        {CHAR_Z,   Event::DIRECTION_DOWN_LEFT},
        {CHAR_C,   Event::DIRECTION_DOWN_RIGHT},
        {CHAR_R,   Event::DEBUG_RED},
        {CHAR_G,   Event::DEBUG_GREEN},
        {CHAR_B,   Event::DEBUG_BLUE},
        {CHAR_Y,   Event::DEBUG_YELLOW},
        {CHAR_M,   Event::MODE_CHANGED},
        {CHAR_V,   Event::MODE_CHANGED_TO_SPEED},
        {CHAR_N,   Event::MODE_CHANGED_TO_NAVIGATION},
};

class SerialPortCommunicationHandler : public EventBroadcaster {
public:
    SerialPortCommunicationHandler(unsigned long port) {
        Serial.begin(port);
        Serial.flush();
    }

    void checkInput() {
        if (Serial.available() == 0)
            return;

        key = Serial.read();

        if (actions.count(key) == 0)
            return;

        Event event = actions.at(key);

        if (event == Event::COLOR_SCHEMA_CHANGED) {
            short nextSchema = colorSchema++ % 4;
            notify(event, asVector(nextSchema));
            return;
        }

        if (event == Event::MODE_CHANGED) {
            short nextMode = mode++ % 2;
            notify(event, asVector(nextMode));
            return;
        }

        if (event == Event::MODE_CHANGED_TO_NAVIGATION) {
            notify(event, asVector(1));
            return;
        }

        if (event == Event::MODE_CHANGED_TO_SPEED) {
            notify(event, asVector(0));
            return;
        }

        if (event == Event::SPEED_CHANGED) {
            notify(Event::MAX_SPEED_UPDATED, asVector(20));
            if(key == NUMBER_8) {
                notify(event, asVector(10));
            } else if(key == NUMBER_9) {
                notify(event, asVector(30)); // more than max!
            } else if (key == NUMBER_0) {
                notify(event, asVector(0));
            }

            return;
        }

        notify(event);
    }

private:
    int key = 0;
    short colorSchema = 0;
    short mode = 0;
};
