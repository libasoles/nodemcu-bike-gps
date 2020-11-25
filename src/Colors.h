#pragma once

#include <pixeltypes.h>
#include <map>
#include <unordered_map>

struct EnumClassHash {
    template<typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

/**
 * Just an Adapter, to detach from the third party library.
 * Notice we are overriding just the constructors and operators we use.
 */
struct Color : public CRGB {
    inline Color() __attribute__((always_inline)) {
    }

    inline Color &operator=(const uint32_t colorCode) __attribute__((always_inline)) {
        CRGB::operator=(colorCode);
        return *this;
    }

    inline Color(uint32_t colorCode)  __attribute__((always_inline))
            : CRGB(colorCode) {
    }

    inline Color(const CRGB &rhs) __attribute__((always_inline))
            : CRGB(rhs) {
    }

    /**
     * Think of accessibility.
     * You should register your colors here
     * for consistency. Because colors not present here
     * and not mapped latter in a particular color schema
     * will not be converted, and some people may not
     * distinguish them right.
     */
    typedef enum {
        Red = 0xFF0000,
        Green = 0x00FF00,
        Blue = 0x0000FF,
        Yellow = 0xFFFF00,
        White = 0xFFFFFF,
        Black = 0x000000,
        LightGreen = 0x002000,
        LightRed = 0x200000,
        LightYellow = 0x202000,
    } HTMLColorCode;
};

enum class Schema {
    CLASSIC = 0,
    DEUTERANOPIA = 1,
    PROTANOPIA = 2,
    TRITANOPIA = 3,
};

typedef std::unordered_map<Color, Color, EnumClassHash> ColorSchema;

class ColorSchemas {
public:
    ColorSchema get(Schema schema) {
        return (ColorSchema) schemas.at(schema);
    }

private:
    const ColorSchema classic = {
            {Color::Red,         Color::Red},
            {Color::Green,       Color::Green},
            {Color::Blue,        Color::Blue},
            {Color::Yellow,      Color::Yellow},
            {Color::White,       Color::White},
            {Color::Black,       Color::Black},
            {Color::LightGreen,  Color::LightGreen},
            {Color::LightRed,    Color::LightRed},
            {Color::LightYellow, Color::LightYellow},
    };

    const ColorSchema deuteranopia = {
            {Color::Red,         CRGB(0xA89432)},
            {Color::Green,       CRGB(0x6584C2)},
            {Color::Blue,        CRGB(0x233E98)},
            {Color::Yellow,      CRGB(0xF8EC22)},
            {Color::White,       Color::White},
            {Color::Black,       Color::Black},
            {Color::LightGreen,  Color::LightGreen}, // TODO: update
            {Color::LightRed,    Color::LightRed}, // TODO: update
            {Color::LightYellow, Color::LightYellow}, // TODO: update
    };

    const ColorSchema protanopia = {
            {Color::Red,         CRGB(0x726401)},
            {Color::Green,       CRGB(0xBCAC31)},
            {Color::Blue,        CRGB(0x283895)},
            {Color::Yellow,      CRGB(0xF8EC20)},
            {Color::White,       Color::White},
            {Color::Black,       Color::Black},
            {Color::LightGreen,  Color::LightGreen}, // TODO: update
            {Color::LightRed,    Color::LightRed}, // TODO: update
            {Color::LightYellow, Color::LightYellow}, // TODO: update
    };

    const ColorSchema tritanopia = {
            {Color::Red,         CRGB(0xEE2B6A)},
            {Color::Green,       CRGB(0xFFC7D4)},
            {Color::Blue,        CRGB(0x5384A4)},
            {Color::Yellow,      CRGB(0x44C6EB)},
            {Color::White,       Color::White},
            {Color::Black,       Color::Black},
            {Color::LightGreen,  Color::LightGreen}, // TODO: update
            {Color::LightRed,    Color::LightRed}, // TODO: update
            {Color::LightYellow, Color::LightYellow}, // TODO: update
    };

    typedef std::map<Schema, ColorSchema> Schemas;
    const Schemas schemas = {
            {Schema::CLASSIC,      classic},
            {Schema::DEUTERANOPIA, deuteranopia},
            {Schema::PROTANOPIA,   protanopia},
            {Schema::TRITANOPIA,   tritanopia},
    };
};
