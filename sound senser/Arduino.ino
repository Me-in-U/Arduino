char soundInputPin = 0;

char ledLevel[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

void setup()
{
  for (int i = 0; i <= 10; i++)
  {
    pinMode(ledLevel[i], OUTPUT);
  }
}

void loop()
{
  int soundInput = analogRead(soundInputPin);
  int soundLevel = map(soundInput, 50, 900, 0, 9);

  for (int i = 0; i < 10; i++)
  {
    digitalWrite(ledLevel[i], LOW);
  }
  for (int i = 0; i < soundLevel; i++)
  {
    digitalWrite(ledLevel[i], HIGH);
  }
  delay(50);
}