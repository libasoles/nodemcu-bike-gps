#pragma once

#include <Arduino.h>
#include <iostream>
#include "Ring.h"
#include "AnimationStateHandler.h"
#include "SerialPortCommunicationHandler.h"
#include "BluetoothService.h"


class App {
public:
    void setup() {
        delay(3000); // power-up safety delay

        animationsHandler.reactTo(Event::BOOT);

        subscribeToBluetooth();
        subscribeToSerialPort();
    }

    void loop() {
        try {
            ring.draw(animationsHandler.currentAnimation());
            keyboard.checkInput();
            bluetooth.keepConnectionAlive();
        } catch (const std::exception &e) {
            Serial.print("Severe exception. Recovering from: ");
            Serial.println(e.what());
        }
    }

private:
    Ring ring{RING_BRIGHTNESS};
    AnimationStateHandler animationsHandler{ring};
    SerialPortCommunicationHandler keyboard{SERIAL_PORT};
    Bluetooth bluetooth{};

    void subscribeToBluetooth() {
        bluetooth.init();
        bluetooth.subscribe(&animationsHandler);
    }

    void subscribeToSerialPort() {
        keyboard.subscribe(&animationsHandler);
    }
};
