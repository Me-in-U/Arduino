const int cdsPin = 0;
const int waterFullAdcValue = 600;

int adcValue;
int waterLevel;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  adcValue = analogRead(cdsPin);
  waterLevel = map(adcValue, 0, waterFullAdcValue, 0, 100);
  Serial.print(adcValue);
  Serial.print(' ');
  Serial.println(waterLevel);
  delay(50);
}