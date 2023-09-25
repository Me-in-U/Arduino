const int potentialMeterPin = 0;
const int ledPin = 13;

int adcValue;
int duty;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  adcValue = analogRead(potentialMeterPin);
  duty = map(adcValue, 0, 1023, 0, 100);

  digitalWrite(ledPin, HIGH);
  delay(duty);

  digitalWrite(ledPin, LOW);
  delay(100 - duty);

  Serial.print("ADC Value is ");
  Serial.print(adcValue);
  Serial.print(". Duty Cycle is ");
  Serial.print(duty);
  Serial.println("%");
}