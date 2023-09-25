const int potentialMeterPin = 0;
const int ledYellowPin = 10;

int adcValue;
int brightness;

void setup()
{
  pinMode(ledYellowPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  adcValue = analogRead(potentialMeterPin);
  brightness = map(adcValue, 0, 1023, 0, 255);
  analogWrite(ledYellowPin, brightness);
}