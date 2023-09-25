const int inputPin = 2;

long startTime = 0;
long swCounterTimer = 0;

void setup()
{
  pinMode(inputPin, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop()
{
  if (digitalRead(inputPin) == LOW)
  {
    startTime = millis();
    while (digitalRead(inputPin) == LOW)
    {
      delay(1);
    }
    swCounterTimer = millis() - startTime;
    Serial.print(swCounterTimer);
    Serial.println(" ms");
  }
}
