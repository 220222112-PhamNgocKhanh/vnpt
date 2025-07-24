#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "esp_sleep.h"
#include <Arduino.h>

// BLE UUIDs
#define SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-5678-90ab-cdef-1234567890ab"

#define LED_PIN 5
#define WAKEUP_PIN GPIO_NUM_33
#define WAKE_DURATION_MS 30000 // 30s

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool ledState = false;
unsigned long wakeStartTime = 0;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Client disconnected");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value == "on")
    {
      ledState = true;
    }
    else if (value == "off")
    {
      ledState = false;
    }
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    Serial.print("Received: ");
    Serial.println(value.c_str());
  }
};

void setup()
{
  Serial.begin(9600);
  delay(100);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Kiểm tra lý do đánh thức
  esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();
  if (wakeupReason == ESP_SLEEP_WAKEUP_EXT0)
  {
    Serial.println("wakeup");
  }

  // Khởi tạo BLE
  BLEDevice::init("ESP32_BLE_LED");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_READ);
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE advertising started");

  wakeStartTime = millis(); // Đánh dấu thời gian thức

  // Cấu hình chân wakeup (mức cao kích hoạt)
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); // Wake when GPIO 33 == HIGH
  pinMode(WAKEUP_PIN, INPUT_PULLDOWN);
}

void loop()
{
  // Nếu quá 1 phút thì đi ngủ
  if (millis() - wakeStartTime > WAKE_DURATION_MS)
  {
    Serial.println("Going to deep sleep...");
    delay(100);

    BLEDevice::deinit(true); // Giải phóng BLE
    esp_deep_sleep_start();  // Bắt đầu deep sleep
  }

  // Nếu client đang kết nối, gửi trạng thái LED
  if (deviceConnected)
  {
    pCharacteristic->setValue(ledState ? "ON" : "OFF");
    pCharacteristic->notify();
    Serial.println(ledState ? "LED ON" : "LED OFF");
  }
  else
  {
    Serial.println("Waiting for BLE client...");
  }

  delay(1000); // Gửi mỗi giây
}
