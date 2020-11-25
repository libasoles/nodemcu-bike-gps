#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class BlinkAnimation : public Animation {
public:
    typedef std::function<void()> Executable;

    BlinkAnimation(
            Ring &aRing,
            Color aColor = Color::White,
            uint16_t millis = 500,
            uint8_t repeatTimes = 2,
            Executable then = []() {}
    ) : Animation(aRing), color(aColor), until(millis), times(repeatTimes), callback(then) {}

    void execute() override {
        repeatTimes();
        callback();
    }

    void repeatTimes() const {
        uint8_t i = 0;
        while (i < times) {
            ring.fill(color);
            ring.wait(until);
            ring.clear();
            ring.wait(until);

            i++;
        }
    }

private:
    Color color;
    uint16_t until;
    uint8_t times;
    Executable callback;
};
