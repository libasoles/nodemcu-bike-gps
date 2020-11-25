#pragma once

#include <Arduino.h>

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class ColorCircleAnimation : public Animation {
public:
    ColorCircleAnimation(Ring &aRing, Color aColor = Color::White, int waitTime = 50)
            : Animation(aRing), color(aColor), time(waitTime) {};

    void execute() override {
        ring.forEachLed(
                [this](Colorable setColor, int i) {
                    setColor(color);
                    ring.wait(time);
                }
        );
    }

private:
    Color color;
    int time;
};
