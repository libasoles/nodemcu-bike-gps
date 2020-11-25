#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class PlainColorAnimation : public Animation {
public:
    PlainColorAnimation(Ring &aRing, Color aColor = Color::White)
            : Animation(aRing), color(aColor) {}

    void execute() override {
        ring.clear();
        ring.fill(color);
    }

private:
    Color color;
};
