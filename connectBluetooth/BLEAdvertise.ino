#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// ---- 設定ここから ----
const int LED_PIN = LED_BUILTIN; // XIAO ESP32S3 のユーザーLED

// XIAOのLEDは LOW で点灯する個体が多いと言われているので、まずはこう置いておく
// もし動かしてみて「HIGH で点灯する」なら、下の2行を入れ替えてください。
const int LED_ON_LEVEL  = LOW;
const int LED_OFF_LEVEL = HIGH;

// 任意のUUID（オンラインのUUIDジェネレータで作ったものでも何でもOK, アスタリスク部分要置換）
#define SERVICE_UUID        "********-****-****-****-************"
#define CHARACTERISTIC_UUID "********-****-****-****-************"
// ---- 設定ここまで ----

BLEServer*        pServer        = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected             = false;

// 接続状態のコールバック
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("BLE: device connected");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("BLE: device disconnected, restart advertising");
    // 切断されたら再度アドバタイズ開始
    pServer->getAdvertising()->start();
  }
};

// Characteristic に書き込みがあったときのコールバック
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      Serial.println(rxValue);
      Serial.println("*********");

      // 例：'1' が来たらLED ON、'0' ならOFF みたいな制御
      if (rxValue == "1") {
        digitalWrite(LED_BUILTIN, HIGH);
      } else if (rxValue == "0") {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(1500);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF_LEVEL);

  Serial.println("Booting BLE LED peripheral...");

  // デバイス名をセット（スマホから見える名前）
  BLEDevice::init("XIAO_S3_LED");

  // サーバ生成＆コールバック設定
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // サービスを作成
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Characteristic作成（READ + WRITE）
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("0"); // 初期値はLED OFF

  // サービス開始
  pService->start();

  // アドバタイズ設定
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // 接続性改善用おまじない
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();

  Serial.println("BLE: advertising started, ready.");
}

void loop() {
  // 今回は特にやることなし。将来ここに他の処理を足すイメージ。
  delay(10);
}