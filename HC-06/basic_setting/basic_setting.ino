#include <SoftwareSerial.h>

const int rx = 2;
const int tx = 3;

SoftwareSerial myBT(rx, tx); //모듈의 RX, TX와 교차해서 연결해야 한다.

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino is ready");
  myBT.begin(9600);
  Serial.println("BT serial started at 9600");
  Serial.println("Enter AT command:");
  
}

void loop() {
  if(myBT.available()){
    Serial.write(myBT.read());
  }
  if(Serial.available()){
    myBT.write(Serial.read());
  }
}