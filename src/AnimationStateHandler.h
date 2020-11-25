#pragma once

#include <Arduino.h>
#include "Animation.h"
#include "AnimationsCatalog.h"
#include "Events.h"

using namespace std;

class AnimationStateHandler : public EventObserver {
public:
    explicit AnimationStateHandler(Ring &aRing)
            : ring(aRing),
              animations{
                      aRing,
                      [this](Event event, std::vector<uint8_t> args) { reactTo(event, args); },
                      [this](Animation *animation) {
                          updateCurrentAnimation(animation);
                      }
              } {
        animate(Event::IDLE, noArguments);
    }

    void reactTo(Event eventName, std::vector<uint8_t> args = noArguments) override {
        if (modeChanged(eventName)) {
            Serial.print("Mode updated to: ");
            Serial.println(!args[0] ? "Navigation" : "Speed");
            updateCurrentMode(args[0]);
            reactToModeChange(args[0]);
            return;
        }

        if (maxSpeedChanged(eventName)) {
            Serial.print("Max Speed updated to: ");
            Serial.println(args[0]);
            updateMaxSpeedAllowed(args[0]);
            return;
        }

        if (colorSchemaChanged(eventName)) {
            Serial.print("Color Schema updated to: ");
            Serial.println(args[0]);
            ring.useColorSchema(static_cast<Schema>(args[0]));
            return;
        }

        if (shouldNotifySpeedLimit(eventName, args[0])) {
            Serial.println("Max Speed!! Slow down!");
            animate(lastDirection, maxSpeedFlag);
            return;
        }

        if (shouldReactToNavigationEvent(eventName)) {
            animate(eventName, args);
            lastDirection = eventName;
            return;
        }

        if (shouldReactToSpeedEvent(eventName)) {
            args.push_back(maxSpeedAllowed);
            animate(eventName, args);
            return;
        }
    }

    Animation *currentAnimation() {
        return current;
    }

private:
    Ring &ring;
    Animation *current = nullptr;

    enum class Mode {
        Navigation, Speed
    };
    Mode currentMode = Mode::Navigation;
    uint8_t maxSpeedAllowed = 0;
    std::vector<uint8_t> maxSpeedFlag = {1};
    Event lastDirection = Event::NO_DIRECTION;

    AnimationsCatalog animations;

    bool modeChanged(const Event &eventName) const {
        return eventName == Event::MODE_CHANGED ||
               eventName == Event::MODE_CHANGED_TO_SPEED ||
               eventName == Event::MODE_CHANGED_TO_NAVIGATION;
    }

    bool maxSpeedChanged(const Event &eventName) const {
        return eventName == Event::MAX_SPEED_UPDATED;
    }

    bool colorSchemaChanged(const Event &eventName) const {
        return eventName == Event::COLOR_SCHEMA_CHANGED;
    }

    void updateCurrentMode(uint8_t value) {
        currentMode = isNavigationMode(value) ? Mode::Navigation : Mode::Speed;
    }

    void updateMaxSpeedAllowed(uint8_t value) {
        maxSpeedAllowed = value;
    }

    void reactToModeChange(uint8_t mode) {
        if (isNavigationMode(mode)) {
            animate(Event::MODE_CHANGED_TO_NAVIGATION, noArguments);
        } else if (isCurrentModeSpeed()) {
            animate(Event::MODE_CHANGED_TO_SPEED, noArguments);
        }
    }

    void updateCurrentAnimation(Animation *animation) {
        if (current) {
            delete current;
        }
        current = animation;
    }

    void animate(Event eventName, std::vector<uint8_t> args) {
        updateCurrentAnimation(animations.createWith(eventName, args));
    }

    bool shouldReactToSpeedEvent(const Event &eventName) {
        if (animations.isNeuterType(eventName))
            return true;

        bool isSpeedEventAndModeIsSpeed = animations.isSpeedType(eventName) && isCurrentModeSpeed();

        return isSpeedEventAndModeIsSpeed;
    }


    bool shouldReactToNavigationEvent(const Event &eventName) {
        if (animations.isNeuterType(eventName))
            return true;

        bool isNavigationsEventAndModeIsNavigation =
                animations.isNavigationType(eventName) && isCurrentModeNavigation();

        return isNavigationsEventAndModeIsNavigation;
    }

    bool isNavigationMode(uint8_t value) {
        return value == static_cast<int>(Mode::Navigation);
    }

    bool isCurrentModeNavigation() const { return currentMode == Mode::Navigation; }

    bool isCurrentModeSpeed() const { return currentMode == Mode::Speed; }

    bool shouldNotifySpeedLimit(const Event &eventName, uint8_t value) {
        return isCurrentModeNavigation() && isSpeedChanging(eventName) && speedExceedsLimit(value);
    }

    bool isSpeedChanging(Event event) {
        return event == Event::SPEED_CHANGED;
    }

    bool speedExceedsLimit(uint8_t value) {
        bool isMaxSpeedConfigured = maxSpeedAllowed > 0;
        return isMaxSpeedConfigured && value >= maxSpeedAllowed;
    }
};
