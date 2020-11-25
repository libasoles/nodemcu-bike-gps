#pragma once

#include <BLEDevice.h>
#include "Events.h"
#include "config.h"
#include <Arduino.h>
#include "BluetoothEventDispatcher.h"

static BLEUUID serviceUUID(BLE_SERVICE_UUID);
static BLEUUID isAliveUUID(BLE_IS_ALIVE_UUID);
static BLEUUID directionUUID(BLE_DIRECTION_UUID);
static BLEUUID speedUUID(BLE_SPEED_UUID);
static BLEUUID maxSpeedUUID(BLE_MAX_SPEED_UUID);
static BLEUUID modeUUID(BLE_MODE_UUID);
static BLEUUID colorSchemaUUID(BLE_COLOR_SCHEMA_UUID);

class Bluetooth : public EventBroadcaster, BLEAdvertisedDeviceCallbacks, BLEClientCallbacks {
public:
    void subscribe(EventObserver *listener) override;

    void init();

    /**
     * If we don't have knows from the server, check it's still alive
     */
    void keepConnectionAlive();

    /**
    * Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice);

    /**
     * On connect to server
     * @param theClient
     */
    void onConnect(BLEClient *theClient);

    /**
     * Disconnection from BLE, not from server
     * @param theClient
     */
    void onDisconnect(BLEClient *theClient);

    void onDisconnect();

private:
    BLEScan *advertiserScanner;
    BLEAdvertisedDevice *phoneDevice = nullptr;
    BLEClient *client = nullptr;
    BLERemoteService *service = nullptr;
    BLERemoteCharacteristic *isAliveCharacteristic = nullptr;
    uint32_t lastTimeChecked = 0;

    void scanAdvertisers(uint32_t duration = 5);

    void onPhoneDeviceFound(const BLEAdvertisedDevice &advertisedDevice);

    bool wasPhoneDeviceFound();

    bool shouldConnectToServer();

    void connectToServer();

    void createClient();

    BLERemoteService *initServiceConnection();

    void checkConnection();

    void checkIsAliveCharacteristic(BLERemoteService *theRemoteService);

    void readSettings();

    void readCurrentMode(BLERemoteService *theRemoteService);

    void readMaxSpeedAllowed(BLERemoteService *theRemoteService);

    void readColorSchema(BLERemoteService *theRemoteService);

    bool isPhoneStillThere();

    void registerCallbacks();
};
