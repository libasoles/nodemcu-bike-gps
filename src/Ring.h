#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <FastLED.h>

#include "config.h"
#include "Colors.h"

typedef std::function<void(Color color)> Colorable;
class Ring {
public:
    Ring(uint8_t brightness = 64);

    void useColorSchema(Schema schema);

    void clear();

    void draw(class Animation *animation);

    void fill(Color aColor);

    typedef std::function<void(Colorable setColor, int i)> Executable;
    void forEachLed(Executable callback);

    void wait(int time);

    int numberOfLeds();
private:
    Color leds[RING_NUMBER_OF_LEDS];

    ColorSchemas colorSchemas{};
    ColorSchema colorSchema;

    void mapRingColorsToSchema();

    void mapLedColorToSchema(Color &led);

    void show();
};
