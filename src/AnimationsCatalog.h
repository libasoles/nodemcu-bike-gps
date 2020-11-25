#pragma once

#include <map>

#include "Colors.h"
#include "Events.h"
#include "MovingArrowAnimation.h"
#include "BlinkAnimation.h"
#include "ColorCircleAnimation.h"
#include "NoAnimation.h"
#include "PlainColorAnimation.h"
#include "SpeedAnimation.h"

static std::vector<uint8_t> noArguments = {0};

typedef std::function<Animation *(std::vector<uint8_t> args)> Executable;
typedef std::function<void(Event, std::vector<uint8_t> args)> Reactive;
typedef std::function<void(Animation *)> StateChanger;

class AnimationsCatalog {
public:
    AnimationsCatalog(Ring &aRing, Reactive react, StateChanger setAnimation)
            : ring(aRing), reactTo(react), animate(setAnimation) {

        initNeuterAnimations();
        initDebugAnimations();
        initNavigationAnimations();
        initSpeedAnimations();

        mergeTypes();
    }

    Animation *createWith(Event name, std::vector<uint8_t> args) {
        return animations.at(name)(args);
    }

    bool isNavigationType(Event name) {
        return navigationAnimations.find(name) != navigationAnimations.end();
    }

    bool isSpeedType(Event name) {
        return speedAnimations.find(name) != speedAnimations.end();
    }

    bool isNeuterType(Event name) {
        return neuterAnimations.find(name) != neuterAnimations.end();
    }

private:
    Ring &ring;

    std::map<Event, Executable> animations;
    std::map<Event, Executable> neuterAnimations;
    std::map<Event, Executable> navigationAnimations;
    std::map<Event, Executable> speedAnimations;
    std::map<Event, Executable> debugAnimations;

    Reactive reactTo = nullptr;
    StateChanger animate = nullptr;


    void initSpeedAnimations() {
        speedAnimations = {
                {
                        Event::SPEED_CHANGED,
                        [this](std::vector<uint8_t> args) {
                            return new SpeedAnimation(ring, args[0], args[1]);
                        }
                }
        };
    }

    void initNavigationAnimations() {
        navigationAnimations = {
                {
                        Event::DIRECTION_UP,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(18, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_DOWN,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(6, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_LEFT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(12, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_RIGHT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(0, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_UP_LEFT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(16, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_UP_RIGHT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(21, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_DOWN_LEFT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(9, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::DIRECTION_DOWN_RIGHT,
                        [this](std::vector<uint8_t> wasMaxSpeedReached) {
                            return createDirectionAnimation(3, wasMaxSpeedReached[0]);
                        }
                },
                {
                        Event::NO_DIRECTION,
                        [this](std::vector<uint8_t> args) {
                            return createBlinkAnimation(Color::Green, 800, 2);
                        }
                },
        };
    }

    void initDebugAnimations() {
        debugAnimations = {
                {
                        Event::DEBUG_RED,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Red);
                        }
                },
                {
                        Event::DEBUG_GREEN,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Green);
                        }
                },
                {
                        Event::DEBUG_BLUE,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Blue);
                        }
                },
                {
                        Event::DEBUG_YELLOW,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Yellow);
                        }
                },
        };
    }

    void initNeuterAnimations() {
        neuterAnimations = {
                {
                        Event::BOOT,
                        [this](std::vector<uint8_t> args) { return createColorCircleAnimation(Color::Blue); }
                },
                {
                        Event::IDLE,
                        [this](std::vector<uint8_t> args) { return createIdleAnimation(); }
                },
                {
                        Event::BLE_SCANNING_ADVERTISERS,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Blue);
                        }
                },
                {
                        Event::BLE_PAIRED,
                        [this](std::vector<uint8_t> args) { return createColorCircleAnimation(Color::Green); }
                },
                {
                        Event::BLE_CONNECTION_LOST,
                        [this](std::vector<uint8_t> args) {
                            return createPlainColorAnimation(Color::Yellow);
                        }
                },
                {
                        Event::MODE_CHANGED,
                        [this](std::vector<uint8_t> args) {
                            return createBlinkAnimation(Color::Blue, 200, 2);
                        }
                },
                {
                        Event::MODE_CHANGED_TO_NAVIGATION,
                        [this](std::vector<uint8_t> args) {
                            return createModeChangedToNavigationAnimation();
                        }
                },
                {
                        Event::MODE_CHANGED_TO_SPEED,
                        [this](std::vector<uint8_t> args) {
                            return createModeChangedToSpeedAnimation();
                        }
                },
                {
                        Event::MAX_SPEED_UPDATED,
                        [this](std::vector<uint8_t> args) {
                            return createBlinkAnimation(Color::Blue, 200, 2);
                        }
                },
                {
                        Event::COLOR_SCHEMA_CHANGED,
                        [this](std::vector<uint8_t> args) {
                            return createBlinkAnimation(Color::Blue, 200, 2);
                        }
                },
        };
    }

    void mergeTypes() {
        neuterAnimations.insert(debugAnimations.begin(), debugAnimations.end());

        animations.insert(neuterAnimations.begin(), neuterAnimations.end());
        animations.insert(navigationAnimations.begin(), navigationAnimations.end());
        animations.insert(speedAnimations.begin(), speedAnimations.end());
    }

    NoAnimation *createIdleAnimation() { return new NoAnimation{ring}; }

    ColorCircleAnimation *createColorCircleAnimation(Color color) { return new ColorCircleAnimation{ring, color}; }

    PlainColorAnimation *createPlainColorAnimation(Color color) { return new PlainColorAnimation{ring, color}; }

    BlinkAnimation *createBlinkAnimation(Color color, uint16_t waitTime = 200, uint8_t times = 2) {
        return new BlinkAnimation{ring, color, waitTime, times, [this]() {
            reactTo(Event::IDLE, noArguments);
        }};
    }

    MovingArrowAnimation *createDirectionAnimation(uint8_t aStartingPoint, uint8_t wasMaxSpeedReached = 0) {
        if (wasMaxSpeedReached) {
            return new MovingArrowAnimation{ring, Color::Green, Color::LightRed, aStartingPoint};
        }
        return new MovingArrowAnimation{ring, Color::Green, Color::LightGreen, aStartingPoint};
    }

    BlinkAnimation *createModeChangedToNavigationAnimation() {
        return new BlinkAnimation{ring, Color::Green, 200, 2, [this]() {
            animate(createColorCircleAnimation(Color::Green));
        }};
    }

    BlinkAnimation *createModeChangedToSpeedAnimation() {
        return new BlinkAnimation{ring, Color::Green, 200, 2, [this]() {
            animate(new SpeedAnimation(ring, 0, 0));
        }};
    }
};