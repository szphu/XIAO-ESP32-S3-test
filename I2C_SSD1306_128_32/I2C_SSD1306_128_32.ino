#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// XIAO ESP32S3 I2C ピン
#define I2C_SDA 5
#define I2C_SCL 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// スクロールさせるメッセージ
const char* MESSAGE = "Hello SSD1306!!";
const uint8_t TEXT_SIZE = 2;
const uint8_t BASE_CHAR_WIDTH = 6;  // 5px + 1px space

int16_t textX;
int16_t textWidth;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 init failed..."));
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();

  // メッセージ幅
  int16_t len = strlen(MESSAGE);
  textWidth = len * BASE_CHAR_WIDTH * TEXT_SIZE;

  // 右端の外からスタート
  textX = SCREEN_WIDTH;

  // ここがポイント：自動改行オフ
  display.setTextWrap(false);
}

void loop() {
  display.clearDisplay();

  int16_t textY = (SCREEN_HEIGHT - 8 * TEXT_SIZE) / 2;  // 中央寄せ

  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(textX, textY);
  display.print(MESSAGE);

  // 枠
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

  display.display();

  // 左にスクロール
  textX--;

  // 画面外まで流れ切ったら右端から再スタート
  if (textX < -textWidth) {
    textX = SCREEN_WIDTH;
  }

  delay(25);
}
