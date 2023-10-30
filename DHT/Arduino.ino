#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  Serial.println("DHT TEST");
  dht.begin();
}

void loop()
{
  unsigned char t = dht.readTemperature();
  unsigned char h = dht.readHumidity();

  Serial.print("Temperature is ");
  Serial.print(t);
  Serial.println("C");
  Serial.print("Humidity    is ");
  Serial.print(h);
  Serial.println("%");
  delay(2000);
}
