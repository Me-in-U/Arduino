#include <IRremote.hpp>

#define IR_RECEIVE_PIN 11

void setup()
{
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
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
      IrReceiver.resume();  // Enable receiving of the next value
    }
  }
}