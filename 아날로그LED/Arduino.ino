const int ledA = 3;
const int ledB = 5;
int brightness = 0;
int increment = 1;

void setup()
{
}

void loop()
{
  analogWrite(ledA, brightness);
  analogWrite(ledB, 255 - brightness);

  brightness += increment;
  if (brightness >= 255 || brightness <= 0)
    increment = -increment;

  delay(10);
}
