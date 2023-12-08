/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 *
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

#define trigPin 2
#define echoPin 3
#define buzzerPin 4
const int buzzerDuration = 1000;
bool buzzerActive = false;
unsigned long buzzerTimer;

int pulseWidth;
int distance;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

// 서보 모터의 수를 정의합니다.
const int numServos = 2;
int servoPins[numServos] = {5, 6}; // 각 서보 모터의 핀 번호 배열
const int sustainTime = 2000;
// 서보 모터의 상태와 타이머를 관리하기 위한 구조체
struct ServoState
{
    Servo servo;
    unsigned long timer;
    bool isActive;
};
ServoState servos[numServos];

// UID를 위한 byte 배열 정의
byte TagUIDS[2][4] = {
    {0xEA, 0x58, 0x79, 0x15},
    {0x0B, 0x1C, 0x55, 0x1C}};

void setup()
{
    Serial.begin(9600);
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522

    pinMode(buzzerPin, OUTPUT);

    // 초음파
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);

    for (int i = 0; i < numServos; i++)
    {
        servos[i].servo.attach(servoPins[i], 600, 2400);
        servos[i].servo.write(0);
        servos[i].isActive = false;
    }

    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("This code scan the MIFARE Classsic NUID."));
    Serial.print(F("Using the following key:"));
    printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop()
{
    // 초음파 거리, 서보 모터 상태 업데이트(초 단위)
    updateDistance();
    updateServo();
    Serial.println();

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!rfid.PICC_IsNewCardPresent())
        return;
    // Verify if the NUID has been readed
    if (!rfid.PICC_ReadCardSerial())
        return;

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
    }

    if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3])
    {
        Serial.println(F("A new card has been detected."));
        // Store NUID into nuidPICC array
        for (byte i = 0; i < 4; i++)
        {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }
        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    }
    else
        Serial.println(F("Card read previously."));

    // 특정 태그 감지 시 서보 모터 배열을 사용하여 움직임
    for (int i = 0; i < numServos; i++)
    {
        if (isUIDEqual(rfid.uid.uidByte, TagUIDS[i]))
        {
            moveServo(i);
        }
    }

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}

void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

// UID 배열이 동일한지 확인하는 함수
bool isUIDEqual(byte *uid1, byte *uid2)
{
    for (byte i = 0; i < 4; i++)
    {
        if (uid1[i] != uid2[i])
        {
            return false;
        }
    }
    return true;
}

void updateDistance()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pulseWidth = pulseIn(echoPin, HIGH);
    distance = pulseWidth / 58;
    Serial.print(distance);
    Serial.print(" cm\t");
    // 부저 상태 업데이트
    if (distance < 10 && distance >= 2 && !buzzerActive)
    {
        tone(buzzerPin, 262, 2000); // 부저 켜기
        buzzerActive = true;
        buzzerTimer = millis();
    }
    if (buzzerActive && millis() - buzzerTimer > buzzerDuration)
    {
        digitalWrite(buzzerPin, LOW); // 부저 끄기
        noTone(buzzerPin);
        buzzerActive = false;
    }
}
void updateServo()
{
    for (int i = 0; i < numServos; i++)
    {
        Serial.print(i);
        Serial.print("번 서보 ");
        Serial.print(servos[i].isActive ? "Active " : "Not Act ");
        Serial.print((millis() - servos[i].timer) / 1000);
        Serial.print("초\t");
        if (servos[i].isActive && millis() - servos[i].timer > sustainTime)
        {
            servos[i].servo.write(0);
            servos[i].isActive = false;
        }
    }
}
void moveServo(int servoIndex)
{
    servos[servoIndex].servo.write(100);
    servos[servoIndex].timer = millis();
    servos[servoIndex].isActive = true;
}