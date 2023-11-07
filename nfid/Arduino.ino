#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;
    
  for(byte i = 0; mfrc522.uid.size;i++){
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i] ,HEX);
  }
  Serial.print("  PICC type: ");
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print(mfrc522.PICC_GetTypeName(piccType));
  Serial.println();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}