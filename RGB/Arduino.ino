const int RedLed = 3;
const int GreenLed = 5;
const int BlueLed = 6;

void setup()
{
  // 아무런 작업을 하지 않습니다.
}

void loop()
{
  for (int i = 0; i <= 255; i++)
  {
    ledOutPut(255 - i, i, 0);
    delay(10);
  }
  for (int i = 0; i <= 255; i++)
  {
    ledOutPut(0, 255 - i, i);
    delay(10);
  }
  for (int i = 0; i <= 255; i++)
  {
    ledOutPut(i, 0, 255 - i);
    delay(10);
  }
}

void ledOutPut(int red, int green, int blue)
{
  analogWrite(RedLed, red);
  analogWrite(GreenLed, green);
  analogWrite(BlueLed, blue);
}
