int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;

int potentioMeterPin = 0;

int stopRange = 100;

int motorSpeed;
int motorSpeedPercent;

int steps[] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001, B0000};

void setup()
{
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  Serial.begin(115200);
}

void loop()
{
  int potentioMeter = analogRead(potentioMeterPin);

  if (potentioMeter >= 512 + (stopRange / 2))
  {
    motorSpeed = map(potentioMeter, 512 + (stopRange / 2), 1023, 4500, 1000);
    motorSpeedPercent = map(motorSpeed, 4500, 1000, 1, 100);
    Serial.print("CW Motor Speed : ");
    Serial.print(motorSpeedPercent);
    Serial.println("%");
    clockwise();
  }
  else if (potentioMeter <= 512 - (stopRange / 2))
  {
    motorSpeed = map(potentioMeter, 512 - (stopRange / 2), 0, 4500, 1000);
    motorSpeedPercent = map(motorSpeed, 4500, 1000, 1, 100);
    Serial.print("CCW Motor Speed : ");
    Serial.print(motorSpeedPercent);
    Serial.println("%");
    counterClockwise();
  }
  else
  {
    Serial.println("Motor Stop");
    motorStop();
  }
}

void counterClockwise()
{
  for (int i = 0; i < 8; i++)
  {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}
void clockwise()
{
  for (int i = 7; i >= 0; i--)
  {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}
void motorStop()
{
  motorSignalOutput(8);
}
void motorSignalOutput(int out)
{
  digitalWrite(motorPin1, bitRead(steps[out], 0));
  digitalWrite(motorPin2, bitRead(steps[out], 1));
  digitalWrite(motorPin3, bitRead(steps[out], 2));
  digitalWrite(motorPin4, bitRead(steps[out], 3));
}