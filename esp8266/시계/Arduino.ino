#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <RotaryEncoder.h>

// 로터리 엔코더
const int PIN_A = 4;   // 로터리 엔코더 A 핀
const int PIN_B = 14;  // 로터리 엔코더 B 핀
RotaryEncoder encoder(PIN_A, PIN_B);

// 버튼
const int BUTTON_PIN = 12;    // 버튼 핀 번호
bool displayOn = true;        // 디스플레이 상태 플래그
bool lastButtonState = HIGH;  // 버튼의 마지막 상태를 저장 (풀업 저항 사용)

//Wifi
WiFiUDP udp;
const char *ssid = "healing 201";
const char *password = "0553213247";

//NTP Client
// 32400을 더해 kr 시간에 맞게 조정
NTPClient timeClient(udp, "kr.pool.ntp.org", 32400, 3600000);

//Display
int brightness = 2;  // 초기 밝기 설정
const int CLK_PIN = 16;
const int DIO_PIN = 5;
const uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
TM1637Display display(CLK_PIN, DIO_PIN);

// 비블로킹 딜레이를 위한 타임스탬프
unsigned long lastNTPUpdate = 0;
const long NTPUpdateInterval = 3600000;
unsigned long lastDisplayUpdate = 0;
const long displayUpdateInterval = 1000;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // 내부 풀업 저항을 활성화하여 버튼 핀 설정
  display.setBrightness(brightness, true);
  display.setSegments(data);
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  display.setBrightness(brightness, true);
  timeClient.begin();
  timeClient.update();
}

void loop() {
  unsigned long currentMillis = millis();

  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // 버튼이 눌렸을 때만 상태 변경
    displayOn = !displayOn;  // 디스플레이 상태 토글
    if (displayOn) {
      display.setBrightness(brightness, true);  // 디스플레이 켜기
      Serial.println("display On");
    } else {
      display.setSegments(blank);  // 디스플레이 끄기
      Serial.println("display Off");
    }
    delay(50);  // 디바운싱을 위한 작은 지연
  }
  lastButtonState = currentButtonState;  // 버튼 상태 업데이트

  // 로터리 엔코더 상태 체크
  encoder.tick();
  int newPos = encoder.getPosition();
  if (newPos != brightness) {
    brightness = max(0, min(newPos, 7));
    display.setBrightness(brightness, true);
    encoder.setPosition(brightness);
    Serial.println(newPos);
  }
  
  // 디스플레이가 켜져있을 때만 시간 표시
  if (displayOn && currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
    bool colon = timeClient.getSeconds() % 2 == 0;                                      // 짝수 초에는 콜론이 켜지고, 홀수 초에는 꺼집니다.
    display.showNumberDecEx(timeClient.getHours(), (colon ? 0x40 : 0x00), true, 2, 0);  // 콜론 토글
    display.showNumberDecEx(timeClient.getMinutes(), 0, true, 2, 2);
    lastDisplayUpdate = currentMillis;
    Serial.println(timeClient.getFormattedTime());
    timeClient.update();
  }
}