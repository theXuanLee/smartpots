#include <WiFi.h>
#include <HTTPClient.h>

// WiFi 连接信息
const char* ssid = "xiaoluo";  // WiFi 名称
const char* password = "aaaaaaa@.1";  // WiFi 密码
const char* serverUrl = "http://192.168.1.17:5000/data";  // Flask 服务器地址

void setup() {
    Serial.begin(115200);  
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Serial2.setTimeout(200);  // 增加读取超时时间，确保读取完整数据


    // 连接 WiFi
    WiFi.begin(ssid, password);
    Serial.print("正在连接 WiFi");
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {  // 最多尝试 20 次
        delay(500);
        Serial.print(".");
        retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi 连接成功！");
        Serial.print("ESP32 IP 地址: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ WiFi 连接失败，请检查 SSID 和密码！");
    }
}

void loop() {
    // 读取 Arduino UNO 发送的数据
    if (Serial2.available()) {
        String data = Serial2.readStringUntil('\n');  
        data.trim();  
        Serial.println("🟢 收到 UNO 数据: " + data);

        // 检查 JSON 格式是否正确
        if (!data.startsWith("{") || !data.endsWith("}")) {
            Serial.println("❌ 错误：无效的 JSON 格式！");
            return;
        }

        // 确保 WiFi 连接正常
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");

            Serial.println("📤 发送 JSON 数据: " + data);
            int httpResponseCode = http.POST(data);  // 发送数据到 Flask 服务器
            
            Serial.print("HTTP 响应码: ");
            Serial.println(httpResponseCode);

            http.end();
        } else {
            Serial.println("⚠️ WiFi 未连接，无法发送数据");
        }
    }
    delay(1000);  // 每秒检测一次数据
}
