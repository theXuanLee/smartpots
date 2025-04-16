#include <Wire.h>
#include <BH1750.h>
#include <LedControl.h>
#include <SoftwareSerial.h>

// 软串口设置（连接 ESP32）
SoftwareSerial mySerial(2, 3); // RX, TX（ESP32 TX 连接到 2，ESP32 RX 连接到 3）

// 点阵屏引脚定义
#define DATA_IN   8
#define LOAD      7
#define CLOCK     6
#define MAX_DEVICES 1

// 硬件引脚定义
const int ledPin = 2;
const int pumpPin = 12;
const int waterLevelPin = A2;
const int moistureSensorPin = A3;
const int lightSensorPin = A1;

// 传感器对象
BH1750 lightMeter;
LedControl lc = LedControl(DATA_IN, CLOCK, LOAD, MAX_DEVICES);

// 记录上次检查时间
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 30000;  // 每 30 秒检查一次

// 传感器阈值
const int waterLevelThreshold = 380;
const int emptyReservoirTimer = 90;
const int amountToPump = 300;
const int moistureThreshold = 500;

int sensorWaterLevelValue = 0;
int moistureSensorValue = 0;
int lightLevel = 0;
float lux = 0;

unsigned long lastPumpTime = 0;
const unsigned long pumpCooldown = 600000;

// 笑脸图像（8x8）
byte smileyFace[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// 撇嘴图像（8x8）
byte sadFace[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

void setup() {
    Serial.begin(9600);   // 调试串口
    mySerial.begin(9600); // 软串口，连接 ESP32

    Wire.begin();
    lightMeter.begin();

    pinMode(ledPin, OUTPUT);
    pinMode(pumpPin, OUTPUT);

    // 初始化点阵屏
    for (int i = 0; i < MAX_DEVICES; i++) {
        lc.shutdown(i, false);
        lc.setIntensity(i, 8);
        lc.clearDisplay(i);
    }

    // LED 闪烁 5 次，表示系统启动成功
    for (int i = 0; i < 5; i++) {
        digitalWrite(ledPin, HIGH);
        delay(300);
        digitalWrite(ledPin, LOW);
        delay(300);
    }
    delay(2000);
    digitalWrite(ledPin, HIGH);

    // 显示系统启动信息
    displayImage(smileyFace);
    delay(2000);
}

void loop() {
    unsigned long currentTime = millis();

    // 每 30 秒读取并发送数据
    if (currentTime - lastCheckTime >= checkInterval) {
        lastCheckTime = currentTime;

        // 读取传感器数据
        sensorWaterLevelValue = analogRead(waterLevelPin);
        moistureSensorValue = analogRead(moistureSensorPin);
        lightLevel = analogRead(lightSensorPin);
        lux = lightMeter.readLightLevel();

        // **确保数据格式正确**
        String jsonData = "{";
        jsonData += "\"water_level\":" + String(sensorWaterLevelValue);
        jsonData += ", \"moisture\":" + String(moistureSensorValue);
        jsonData += ", \"light\":" + String(lightLevel);
        jsonData += ", \"lux\":" + String(lux, 2);  // 保留 2 位小数
        jsonData += "}";

        // **确保 JSON 数据完整后再发送**
        if (jsonData.length() > 0) {
            mySerial.println(jsonData);
            Serial.println("📤 发送数据到 ESP32: " + jsonData);
        } else {
            Serial.println("❌ 错误：JSON 数据为空，未发送！");
        }

        // 低水位警报
        if (sensorWaterLevelValue < waterLevelThreshold) {
            displayImage(sadFace);
            for (int i = 0; i < emptyReservoirTimer; i++) {
                digitalWrite(ledPin, LOW);
                delay(1000);
                digitalWrite(ledPin, HIGH);
                delay(1000);
            }
        } else {
            digitalWrite(ledPin, HIGH);
        }

        // 检测土壤湿度并启动水泵
        if (moistureSensorValue < moistureThreshold && (currentTime - lastPumpTime >= pumpCooldown)) {
            lastPumpTime = currentTime;

            digitalWrite(pumpPin, HIGH);
            Serial.println("💧 水泵启动");

            displayImage(smileyFace);

            delay(amountToPump);
            digitalWrite(pumpPin, LOW);
            Serial.println("💧 水泵关闭");

            displayImage(sadFace);
            delay(800);
        } else {
            displayImage(sadFace);
        }
    }

    delay(100);  // 延迟以避免过于频繁的检查
}

// 显示图像到点阵屏
void displayImage(byte image[8]) {
    for (int i = 0; i < 8; i++) {
        lc.setRow(0, i, image[i]);
    }
}
