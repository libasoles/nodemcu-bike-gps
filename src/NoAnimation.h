#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"

class NoAnimation : public Animation {
public:
    NoAnimation(Ring &aRing) : Animation(aRing) {}

    void execute() override {
        ring.clear();
    }
};
