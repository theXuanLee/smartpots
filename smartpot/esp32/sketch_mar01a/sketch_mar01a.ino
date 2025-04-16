#include <WiFi.h>
#include <HTTPClient.h>

// WiFi 连接信息
const char* ssid = "xiaoluo";
const char* password = "aaaaaaa@.1";
const char* serverUrl = "http://192.168.1.17:5000/data";  // Flask 服务器地址

void setup() {
    Serial.begin(115200);  // 调试信息
    Serial2.begin(9600, SERIAL_8N1, 16, 17);  // UART2, GPIO16 = RX, GPIO17 = TX（连接到 UNO）

    WiFi.begin(ssid, password);
    Serial.print("连接 WiFi 中");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 连接成功");
    Serial.print("ESP32 IP 地址: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (Serial2.available()) {
        String data = Serial2.readStringUntil('\n');  // 读取 UNO 发送的数据
        data.trim();  // 去除空格和换行
        Serial.println("收到 UNO 数据: " + data);

        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");

            int httpResponseCode = http.POST(data);  // 发送 JSON 数据
            Serial.print("HTTP 响应码: ");
            Serial.println(httpResponseCode);

            http.end();
        } else {
            Serial.println("WiFi 未连接，无法发送数据");
        }
    }
    delay(1000);
}
