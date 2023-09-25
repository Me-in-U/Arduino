const int ledPin = 13;
const int inputPin = 2;
void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT_PULLUP);
}

void loop()
{
  digitalWrite(ledPin, (digitalRead(inputPin) == LOW) ? HIGH : LOW);
}
