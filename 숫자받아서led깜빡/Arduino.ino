const int ledPin = 13;
int blinkNum = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  if (Serial.available())
  {
    char val = Serial.read();
    if (isDigit(val))
    {
      blinkNum = (val - '0');
    }
  }
  for (char i = 0; i < blinkNum; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  blinkNum = 0;
}
