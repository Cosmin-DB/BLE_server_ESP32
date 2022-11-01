/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define LIGHT_BASE_UUID                  "6a9d6db8-7dbe-4ae1-a5bc-b4e55a2d73d"
#define LIGHT_SERVICE_UUID               LIGHT_BASE_UUID "0"
#define LED_CHARACTERISTIC_UUID          LIGHT_BASE_UUID "1"
#define COUNTER_CHARACTERISTIC_UUID      LIGHT_BASE_UUID "2"


#define LED_PIN 13

uint16_t counterValue=0;
BLECharacteristic* pCOUNTERCharacteristic = NULL;


class LEDCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        uint8_t value = *pCharacteristic->getData();
        if (value > 0) {
          digitalWrite(LED_PIN, HIGH);
        }else{
          digitalWrite(LED_PIN, LOW);
          }
        Serial.print("LED:");
        Serial.println(value);
    }
};


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32_Cosmin");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(LIGHT_SERVICE_UUID);
  BLECharacteristic *pLEDCharacteristic = pService->createCharacteristic(
                                           LED_CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_WRITE
                                         );

  pLEDCharacteristic->setCallbacks(new LEDCallbacks());
  uint8_t init_value=1;
  pLEDCharacteristic->setValue(&init_value,1);// value=0 and length=1 NOTE: That 0 is uint8 (8 bits= 1 byte)
//----------------------------
  pCOUNTERCharacteristic = pService->createCharacteristic(
                                         COUNTER_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ   |
                                         BLECharacteristic::PROPERTY_NOTIFY 
                                       );

  BLE2902 *pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);
  pCOUNTERCharacteristic->addDescriptor(pBLE2902);
  pCOUNTERCharacteristic->setValue(counterValue);

  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(LIGHT_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  counterValue++;
  pCOUNTERCharacteristic->setValue(counterValue);
  pCOUNTERCharacteristic->notify();
  Serial.print("Counter:");
  Serial.println(counterValue);
  delay(2000);
}
