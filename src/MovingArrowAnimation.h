#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class MovingArrowAnimation : public Animation {
public:
    MovingArrowAnimation(
            Ring &aRing,
            const Color aColor = Color::Green,
            const Color aSecondaryColor = Color::LightGreen,
            const uint8_t aStartingPoint = 0
    ) : Animation(aRing), color(aColor), secondaryColor(aSecondaryColor), startingPoint(aStartingPoint) {}

    void execute() override {
        uint8_t numberOfLeds = ring.numberOfLeds();
        uint8_t currentStart = startingPoint;
        uint8_t currentRange = range;

        for (int i = 0; i < 7; i++) {
            ring.forEachLed(
                    [this, currentStart, currentRange, numberOfLeds](Colorable setColor, int i) {
                        Color backgroundColor = isPointWithinRange(i, startingPoint, range, numberOfLeds) ? secondaryColor : Color::Black;
                        setColor(backgroundColor);

                        if(isPointWithinRange(i, currentStart, currentRange, numberOfLeds)) {
                            setColor(color);
                        }
                    }
            );
            ring.wait(200);
            currentStart = (currentStart + 1) % numberOfLeds;
            currentRange -= 2;
        }
    }

private:
    Color color;
    Color secondaryColor;
    uint8_t startingPoint;
    uint8_t range = 13;

    bool isPointWithinRange(uint8_t point, uint8_t start, uint8_t range, uint8_t numLeds) {
        bool condition = (point >= start) && (point < start + range);
        bool condition2 =
                ((start + range) % numLeds <= range) && (point < (start + range) % numLeds);

        return condition || condition2;
    }
};
