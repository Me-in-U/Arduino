#include <IRremote.hpp>

#define IR_RECEIVE_PIN 11

int led1 = 10;
int led2 = 9;
int led3 = 8;

long led1Off = -1169817856;
long led1On = -1153106176;
long led2Off = -1186529536;
long led2On = -1086259456;
long led3Off = -1203241216;
long led3On = -1136394496;

void setup()
{
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop()
{
  if (IrReceiver.decode())
  {
    long data = IrReceiver.decodedIRData.decodedRawData;
    if (data != 0)
    {
      IrReceiver.printIRResultShort(&Serial);
      Serial.println(data); // Print "old" raw data
      if (data == led1On)
        digitalWrite(led1, HIGH);
      if (data == led1Off)
        digitalWrite(led1, LOW);
      if (data == led2On)
        digitalWrite(led2, HIGH);
      if (data == led2Off)
        digitalWrite(led2, LOW);
      if (data == led3On)
        digitalWrite(led3, HIGH);
      if (data == led3Off)
        digitalWrite(led3, LOW);
    }
    IrReceiver.resume(); // Enable receiving of the next value
  }
}