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
#define FOTORESISTOR_CHARACTERISTIC_UUID LIGHT_BASE_UUID "2"

#define LED_PIN 13
#define FOTORESISTOR_PIN 36

uint16_t fotoResistorValue=0;
BLECharacteristic* pFOTORESISTORCharacteristic = NULL;


class LEDCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        uint8_t value = *pCharacteristic->getData();
        if (value > 0) {
          digitalWrite(LED_PIN, HIGH);
        }else{
          digitalWrite(LED_PIN, LOW);
          }
    }
};

class FOTORESISTORCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        uint8_t value = *pCharacteristic->getData();
        if (value > 0) {
          digitalWrite(LED_PIN, HIGH);
        }else{
          digitalWrite(LED_PIN, LOW);
          }
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
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );

  pLEDCharacteristic->setCallbacks(new LEDCallbacks());
  uint8_t init_value=1;
  pLEDCharacteristic->setValue(&init_value,1);// value=0 and length=1 NOTE: That 0 is uint8 (8 bits= 1 byte)
//----------------------------
  pFOTORESISTORCharacteristic = pService->createCharacteristic(
                                         FOTORESISTOR_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ   |
                                         BLECharacteristic::PROPERTY_WRITE  |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
 pFOTORESISTORCharacteristic->addDescriptor(new BLE2902());




  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(LIGHT_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  fotoResistorValue = analogRead(FOTORESISTOR_PIN);
  pFOTORESISTORCharacteristic->setValue(fotoResistorValue);
  pFOTORESISTORCharacteristic->notify();
  delay(2000);
}
