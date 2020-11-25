#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class ArrowAnimation : public Animation {
public:
    ArrowAnimation(Ring &aRing, Color aColor = Color::White, uint8_t aStartingPoint = 0)
            : Animation(aRing), color(aColor), startingPoint(aStartingPoint) {}

    void execute() override {
        uint8_t numberOfLeds = ring.numberOfLeds();

        ring.forEachLed(
                [this, numberOfLeds](Colorable setColor, int i) {
                    Color color = isPointWithinRange(i, numberOfLeds) ? color : Color::Black;
                    setColor(color);
                }
        );
    }

private:
    Color color;
    uint8_t startingPoint;
    uint8_t range = 13;

    bool isPointWithinRange(uint8_t point, uint8_t numberOfLeds) {
        bool condition = point >= startingPoint && point < startingPoint + range;
        bool condition2 =
                (startingPoint + range) % numberOfLeds <= range && point < (startingPoint + range) % numberOfLeds;

        return condition || condition2;
    }
};
