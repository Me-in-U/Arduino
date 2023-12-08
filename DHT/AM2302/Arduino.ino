#include <DHT22.h>
#include <TM1637TinyDisplay6.h>

// DHT22 센서 설정
#define pinDATA 11
DHT22 dht22(pinDATA);

// TM1637 디스플레이 설정
#define CLK 13 // 클럭 핀
#define DIO 12 // 데이터 핀
TM1637TinyDisplay6 display(CLK, DIO);

float t;
float h;
int level;
char buff[7];

void setup()
{
  // Serial.begin(115200);
  // Serial.println("\ntest capteur DTH22");

  // TM1637 디스플레이 초기화
  display.begin();
  display.setBrightness(0x0f);
  for (int i = 1; i <= 10; i++)
  {
    pinMode(i, OUTPUT);
  }
  for (int i = 1; i <= 10; i++)
  {
    digitalWrite(i, HIGH);
    delay(100);
    digitalWrite(i, LOW);
  }
}

void loop()
{
  // DHT22 센서에서 데이터 읽기
  t = dht22.getTemperature();
  h = dht22.getHumidity();
  // 온도마다 led 설정
  for (int i = 1; i <= 10; i++)
    digitalWrite(i, LOW);
  if (t <= 0)
    level = 1;
  else if (t <= 26)
    level = (t - 1) / 3 + 2;
  else
    level = 10;
  for (int i = 1; i <= level; i++)
  {
    digitalWrite(i, HIGH);
  }
  // 세그먼트 표시
  if (dht22.getLastError() != dht22.OK)
  {
    // Serial.print("last error :");
    // Serial.println(dht22.getLastError());
    display.showString("ERROR");
  }
  else
  {
    // 시리얼 모니터에 출력
    // Serial.print("level=" + String(level) + ", Humi=" + String(h, 1) + ", Temp=" + String(t, 1) + "\n");
    // TM1637 디스플레이에 습도와 온도 표시
    // 온도가 10도 미만일 경우 소수점 첫째 자리까지 표시
    if (t < 0)
    {
      sprintf(buff, "%02dP-%01dC", (int)h, (int)t);
    }
    else if (t < 10.0)
      sprintf(buff, "%02dP %01dC", (int)h, (int)t);
    else
      sprintf(buff, "%02dP%02dC", (int)h, (int)t);
    display.showString(buff); // 버퍼 내용을 디스플레이에 표시
  }
  delay(2000);
}
