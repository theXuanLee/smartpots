#include <Wire.h>
#include <BH1750.h>
#include <LedControl.h>

// 点阵屏引脚定义
#define DATA_IN   8    // 数据输入引脚
#define LOAD       7   // CS 引脚（片选）
#define CLOCK      6   // 时钟引脚
#define MAX_DEVICES 1  // 使用的点阵屏数量

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
const unsigned long checkInterval = 1800000; 

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
    Serial.begin(115200);
    Wire.begin();
    lightMeter.begin();

    pinMode(ledPin, OUTPUT);
    pinMode(pumpPin, OUTPUT);

    // 初始化点阵屏
    for (int i = 0; i < MAX_DEVICES; i++) {
        lc.shutdown(i, false);  // 开启设备
        lc.setIntensity(i, 8);   // 设置亮度（1-15）
        lc.clearDisplay(i);      // 清空显示
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
    displayImage(smileyFace); // 初始显示笑脸
    delay(2000);
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - lastCheckTime >= checkInterval) {
        lastCheckTime = currentTime;

        sensorWaterLevelValue = analogRead(waterLevelPin);
        moistureSensorValue = analogRead(moistureSensorPin);
        lightLevel = analogRead(lightSensorPin);
        lux = lightMeter.readLightLevel();

        Serial.print("{\"water_level\":");
        Serial.print(sensorWaterLevelValue);
        Serial.print(", \"moisture\":");
        Serial.print(moistureSensorValue);
        Serial.print(", \"light\":");
        Serial.print(lightLevel);
        Serial.print(", \"lux\":");
        Serial.print(lux);
        Serial.println("}");

        // 低水位警报
        if (sensorWaterLevelValue < waterLevelThreshold) {
            displayImage(sadFace); // 显示撇嘴图像
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
            Serial.println("水泵启动");

            displayImage(smileyFace); // 显示笑脸图像

            delay(amountToPump);
            digitalWrite(pumpPin, LOW);
            Serial.println("水泵关闭");

            displayImage(sadFace); // 显示撇嘴图像
            delay(800);
        } else {
            displayImage(sadFace); // 显示撇嘴图像
        }
    }
}

// 显示图像到点阵屏
void displayImage(byte image[8]) {
    for (int i = 0; i < 8; i++) {
        lc.setRow(0, i, image[i]); // 在设备0的第i行显示图像
    }
}
