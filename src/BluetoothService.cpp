#pragma once

#include "BluetoothService.h"

/**
 * This bluetooth event callback must be static.
 * That's not cool for our OOP design.
 * So, we were forced to instantiate eventDispatcher
 * outside of Bluetooth class.
 */
BluetoothEventDispatcher eventDispatcher{};


static void logNotification(BLERemoteCharacteristic *aCharacteristic, uint8_t *data, const char *label);

static void logNotification(BLERemoteCharacteristic *aCharacteristic, uint8_t *data, const char *label) {
    Serial.println(aCharacteristic->getUUID().toString().c_str());

    Serial.print("data: ");
    Serial.println(*data);

    Serial.print(label);
    Serial.println(" ^");
    Serial.println("-----------");
}

static void notifyCallback(
        BLERemoteCharacteristic *aCharacteristic,
        uint8_t *data,
        std::size_t length,
        bool isNotify) {

    if (aCharacteristic->getUUID().equals(directionUUID)) {
        logNotification(aCharacteristic, data, "Direction");

        eventDispatcher.broadcastDirection(*data);
    } else if (aCharacteristic->getUUID().equals(speedUUID)) {
        logNotification(aCharacteristic, data, "Speed");

        eventDispatcher.broadcastSpeed(*data);
    } else if (aCharacteristic->getUUID().equals(modeUUID)) {
        logNotification(aCharacteristic, data, "Mode");

        eventDispatcher.broadcastMode(*data);
    } else if (aCharacteristic->getUUID().equals(maxSpeedUUID)) {
        logNotification(aCharacteristic, data, "Max Speed");

        eventDispatcher.broadcastMaxSpeed(*data);
    } else if (aCharacteristic->getUUID().equals(colorSchemaUUID)) {
        logNotification(aCharacteristic, data, "Color Schema");

        eventDispatcher.broadcastColorSchema(*data);
    }
}

void Bluetooth::subscribe(EventObserver *listener) {
    EventBroadcaster::subscribe(listener);
    eventDispatcher.subscribe(listener);
}

void Bluetooth::init() {
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init(BLE_DEVICE_NAME);

    advertiserScanner = BLEDevice::getScan();
    advertiserScanner->setAdvertisedDeviceCallbacks(this);
    advertiserScanner->setInterval(1349);
    advertiserScanner->setWindow(449);
    advertiserScanner->setActiveScan(true);
    scanAdvertisers();
}

void Bluetooth::scanAdvertisers(uint32_t duration) {
    Serial.println("Scanning advertisers...");
    notify(Event::BLE_SCANNING_ADVERTISERS);
    advertiserScanner->start(duration, false);
}

void Bluetooth::keepConnectionAlive() {
    bool someSecondsPassed = (millis() - lastTimeChecked) > 2000;

    if (shouldConnectToServer()) {
        connectToServer();
    } else if (someSecondsPassed && !wasPhoneDeviceFound()) {
        scanAdvertisers();
    } else if (someSecondsPassed && !isPhoneStillThere()) {
        notify(Event::BLE_CONNECTION_LOST);
        delay(500);
        onDisconnect();
    }

    if (someSecondsPassed) {
        lastTimeChecked = millis();
    }
}

bool Bluetooth::shouldConnectToServer() {
    return wasPhoneDeviceFound() && (!client || !client->isConnected());
}

bool Bluetooth::wasPhoneDeviceFound() {
    return phoneDevice;
}

void Bluetooth::onResult(BLEAdvertisedDevice advertisedDevice) {
    bool isOurPhoneApp = advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID);
    if (!isOurPhoneApp) {
        return;
    }

    Serial.print("Service UUID: ");
    Serial.println(advertisedDevice.getServiceUUID().toString().c_str());

    advertiserScanner->erase(advertisedDevice.getAddress());
    onPhoneDeviceFound(advertisedDevice);
}

void Bluetooth::onPhoneDeviceFound(const BLEAdvertisedDevice &advertisedDevice) {
    advertiserScanner->stop();
    phoneDevice = new BLEAdvertisedDevice(advertisedDevice);
}

void Bluetooth::connectToServer() {
    try {
        createClient();
        client->connect(phoneDevice);
        Serial.println("* Connected to server");
    } catch (const std::exception &e) {
        Serial.println("ERROR: Server connection failed!");
        Serial.println(e.what());

        if (client && client->isConnected()) {
            onDisconnect();
        }
    }

    try {
        initServiceConnection();
        registerCallbacks();

        Serial.println("> Service available. We can read and get notifications.");

        notify(Event::BLE_PAIRED);

        readSettings();
    } catch (const std::exception &e) {
        Serial.println("Service connection error!");
        Serial.println(e.what());

        if (client && client->isConnected()) {
            onDisconnect();
        }
    }
}

void Bluetooth::registerCallbacks() {
    service->getCharacteristic(directionUUID)->registerForNotify(notifyCallback);
    service->getCharacteristic(speedUUID)->registerForNotify(notifyCallback);
    service->getCharacteristic(maxSpeedUUID)->registerForNotify(notifyCallback);
    service->getCharacteristic(modeUUID)->registerForNotify(notifyCallback);
    service->getCharacteristic(colorSchemaUUID)->registerForNotify(notifyCallback);
}

void Bluetooth::createClient() {
    Serial.print("Forming a connection to ");
    Serial.println(phoneDevice->getAddress().toString().c_str());

    client = BLEDevice::createClient();
    Serial.println("Created client");

    client->setClientCallbacks(this);
}

/**
 * On client<>server connected
 */
void Bluetooth::onConnect(BLEClient *theClient) {
    Serial.println("Client<>Server paired");
    client = theClient;
}

BLERemoteService *Bluetooth::initServiceConnection() {
    service = client->getService(serviceUUID);

    if (service == nullptr) {
        onDisconnect();

        throw "ERROR: Failed to find BLE service";
    }

    checkConnection();
}

void Bluetooth::checkConnection() {
    checkIsAliveCharacteristic(service);

    if (!isAliveCharacteristic->canNotify()) {
        throw "ERROR: BLE service won't notify characteristic updates";
    }
}

void Bluetooth::checkIsAliveCharacteristic(BLERemoteService *remoteService) {
    isAliveCharacteristic = remoteService->getCharacteristic(isAliveUUID);
    if (isAliveCharacteristic == nullptr) {
        onDisconnect();

        throw "ERROR: Failed to find location characteristic";
    }
}

void Bluetooth::readSettings() {
    readCurrentMode(service);
    readMaxSpeedAllowed(service);
    readColorSchema(service);
}

void Bluetooth::readCurrentMode(BLERemoteService *remoteService) {
    BLERemoteCharacteristic *currentMode = remoteService->getCharacteristic(modeUUID);
    if (currentMode != nullptr && currentMode->canRead()) {
        eventDispatcher.broadcastMode(currentMode->readUInt8());
    }
}

void Bluetooth::readMaxSpeedAllowed(BLERemoteService *remoteService) {
    BLERemoteCharacteristic *maxSpeed = remoteService->getCharacteristic(maxSpeedUUID);
    if (maxSpeed != nullptr && maxSpeed->canRead()) {
        eventDispatcher.broadcastMaxSpeed(maxSpeed->readUInt8());
    }
}

void Bluetooth::readColorSchema(BLERemoteService *remoteService) {
    BLERemoteCharacteristic *schema = remoteService->getCharacteristic(colorSchemaUUID);
    if (schema != nullptr && schema->canRead()) {
        eventDispatcher.broadcastColorSchema(schema->readUInt8());
    }
}

void Bluetooth::onDisconnect(BLEClient *theClient) {
    if (phoneDevice) {
        delete phoneDevice;
    }

    phoneDevice = nullptr;

    Serial.println("* Disconnected from server");
}

void Bluetooth::onDisconnect() {
    client->disconnect();
    onDisconnect(client);
}

bool Bluetooth::isPhoneStillThere() {
    uint8_t isAlive = isAliveCharacteristic->readUInt8() == 1;

    return isAlive;
}
