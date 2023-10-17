#include <Servo.h>

Servo motor1;

int servoMotorPin = 5;
int potentioMeterPin = 0;
int motorAngle;
int motorAngleOld;

void setup()
{
  motor1.attach(servoMotorPin, 600, 2400);
  Serial.begin(115200);
}

void loop()
{
  int potentioMeter = analogRead(potentioMeterPin);
  motorAngle = map(potentioMeter, 0, 1023, 0, 180);
  motor1.write(motorAngle);
  if (motorAngle != motorAngleOld)
  {
    Serial.print("Servo motor Angle : ");
    Serial.println(motorAngle);
  }
  motorAngleOld = motorAngle;
  delay(20);
}