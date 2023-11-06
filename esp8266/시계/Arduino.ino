#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>

const char *ssid = "healing 201";
const char *password = "0553213247";

int previousMin = 0;

WiFiUDP udp;
NTPClient timeClient(udp, "kr.pool.ntp.org", 32400, 3600000);
// 32400을 더해 kr 시간에 맞게 조정

const int CLK_PIN = 16;
const int DIO_PIN = 5;
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
TM1637Display display(CLK_PIN, DIO_PIN);

void setup()
{
    display.setBrightness(7, true);
    display.setSegments(data);
    delay(1000);
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    display.setBrightness(2, true);
    timeClient.begin();
}

void loop()
{
    timeClient.update();
    int currentMin = timeClient.getMinutes();
    if (previousMin != currentMin)
    {
        previousMin = currentMin;
        int currentTime = timeClient.getHours() * 100 + currentMin;
    }

    // display.showNumberDecEx(timeClient.getHours(), 0b11100000, false, 2, 0);
    // display.showNumberDecEx(timeClient.getMinutes(), 0b11100000, false, 2, 2);
    // 출력값, Dot표현위치 (왼쪽0x80, 0x40, 0x20, 0x10) 순으로 지정하면됨. 쉽죠??

    // Blink colon every second
    bool colon = timeClient.getSeconds() % 2 == 0; // 짝수 초에는 콜론이 켜지고, 홀수 초에는 꺼집니다.
    display.showNumberDecEx(timeClient.getHours(), (colon ? 0x40 : 0x00), true, 2, 0); // 콜론 토글
    display.showNumberDecEx(timeClient.getMinutes(), 0, true, 2, 2);

    Serial.println(timeClient.getFormattedTime());
    delay(1000);
}