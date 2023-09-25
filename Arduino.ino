const int cdsPin = 1;

int adcValue;
int illuminance;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  adcValue = analogRead(cdsPin);
  illuminance = map(adcValue, 0, 1023, 100, 0);
  Serial.println(illuminance);
}