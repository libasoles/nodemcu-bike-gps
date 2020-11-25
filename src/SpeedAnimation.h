#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "Ring.h"
#include "Colors.h"

class SpeedAnimation : public Animation {
public:
    SpeedAnimation(Ring &aRing, uint8_t currentSpeed, uint8_t maxSpeed)
            : Animation(aRing), speed(currentSpeed), maxSpeedAllowed(maxSpeed) {}

    void execute() override {
        ring.clear();

        drawPlaceholder();

        if (maxSpeedAllowed == 0)
            return;

        uint8_t activeLeds = calculateActiveLeds();
        drawActiveLeds(activeLeds);

        if (speed >= maxSpeedAllowed) {
            blinkAlert();
        }
    }

    void blinkAlert() {
        ring.forEachLed([this](Colorable setColor, int i) {
            bool isPointOutsideSpeedometer = isPointWithinRange(i, 6, speedometerSize, ring.numberOfLeds());
            if (isPointOutsideSpeedometer) {
                setColor(Color::LightRed);
            }
        });

        ring.wait(1000);

        ring.forEachLed([this](Colorable setColor, int i) {
            bool isPointOutsideSpeedometer = isPointWithinRange(i, 6, speedometerSize, ring.numberOfLeds());
            if (isPointOutsideSpeedometer) {
                setColor(Color::Black);
            }
        });

        ring.wait(1000);
    }

    uint8_t calculateActiveLeds() const {
        uint8_t range = speed * 100 / maxSpeedAllowed;
        uint8_t activeLeds = round(range * speedometerSize / 100);

        return activeLeds;
    }

    void drawActiveLeds(int activeLeds) {
        ring.forEachLed([this, activeLeds](Colorable setColor, int i) {
            bool isPointOutsideSpeedometer = isPointWithinRange(i, 6, speedometerSize, ring.numberOfLeds());
            bool isPointWithinSpeedRange = isPointWithinRange(i, startingPoint, activeLeds, ring.numberOfLeds());
            if (isPointOutsideSpeedometer || !isPointWithinSpeedRange) {
                return;
            }

            drawInSpeedometer(i, Color::Green, Color::Yellow, Color::Red, setColor);
        });
    }

private:
    uint8_t speed;
    uint8_t maxSpeedAllowed;
    uint8_t startingPoint = 18;
    uint8_t speedometerSize = 12;

    void drawPlaceholder() {
        ring.forEachLed(
                [this](Colorable setColor, int i) {
                    drawInSpeedometer(i, Color::LightGreen, Color::LightYellow, Color::LightRed, setColor);
                }
        );
    }

    void drawInSpeedometer(int position, Color ok, Color warning, Color danger, Colorable &setColor) {
        if (isPointWithinRange(position, startingPoint, 7, ring.numberOfLeds())) {
            setColor(ok);
        } else if (isPointWithinRange(position, 1, 3, ring.numberOfLeds())) {
            setColor(warning);
        } else if (isPointWithinRange(position, 4, 2, ring.numberOfLeds())) {
            setColor(danger);
        }
    }

    bool isPointWithinRange(uint8_t point, uint8_t start, uint8_t range, uint8_t numLeds) {
        bool condition = (point >= start) && (point < start + range);
        bool condition2 =
                ((start + range) % numLeds <= range) && (point < (start + range) % numLeds);

        return condition || condition2;
    }
};
