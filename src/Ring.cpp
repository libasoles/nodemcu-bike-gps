#include "Ring.h"
#include "Animation.h"

#define UPDATES_PER_SECOND 500

Ring::Ring(uint8_t brightness) {
    FastLED.addLeds<RING_LED_TYPE, RING_LED_PIN>
            (leds, RING_NUMBER_OF_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);

    useColorSchema(Schema::CLASSIC);
}

void Ring::useColorSchema(Schema schema) {
    colorSchema = colorSchemas.get(schema);
}

void Ring::clear() {
    FastLED.clear();
}

void Ring::draw(Animation *animation) {
    animation->execute();

    show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void Ring::show() {
    mapRingColorsToSchema();

    FastLED.show();
}

void Ring::mapRingColorsToSchema() {
    for (int i = 0; i < numberOfLeds(); i++) {
        if (!leds[i]) {
            continue;
        }
        mapLedColorToSchema(leds[i]);
    }
}

void Ring::mapLedColorToSchema(Color &led) {
    bool wasConversionAlreadyApplied = colorSchema.find(led) == colorSchema.end();
    if (wasConversionAlreadyApplied)
        return;

    try {
        led = colorSchema.at(led);
    } catch (const std::exception &e) {
        Serial.println("Error mapping color to schema number");
    }
}

void Ring::wait(int time) {
    unsigned long start = millis();
    while (millis() - start < time) {
        show();
    }
}

void Ring::fill(Color aColor) {
    fill_solid(leds, numberOfLeds(), aColor);
    show();
}

void Ring::forEachLed(Executable callback) {
    for (int i = 0; i < numberOfLeds(); i++) {
        callback([this, i](Color color) {
            leds[i] = colorSchema.at(color);
        }, i);
    }
}

int Ring::numberOfLeds() {
    return FastLED.size();
}

