#include <Servo.h>

Servo motor1;

int servoMotorPin = 5;

void setup()
{
  motor1.attach(servoMotorPin, 600, 2400);
  motor1.write(90);
  Serial.begin(115100);
}

void loop()
{
  // 0 => 시계방향 최대속도
  // 90 => 정지
  // 180 => 반시계방향 최대속도
  for (int i = 90; i >= 0; i--)
  {
    motor1.write(i);
    delay(100);
    Serial.println(i);
  }
  for (int i = 0; i <= 90; i++)
  {
    motor1.write(i);
    delay(100);
    Serial.println(i);
  }
  for (int i = 90; i <= 180; i++)
  {
    motor1.write(i);
    delay(100);
    Serial.println(i);
  }
  for (int i = 180; i >= 90; i--)
  {
    motor1.write(i);
    delay(100);
    Serial.println(i);
  }
}