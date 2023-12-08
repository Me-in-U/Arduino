#include <Servo.h>

#define trigPin 2
#define echoPin 3
#define buzzerPin 4

const int buzzerDuration = 100;
bool buzzerActive = false;
unsigned long buzzerTimer;

int pulseWidth;
int distance;
int minDistance = 300;
void setup()
{
    Serial.begin(9600);
    pinMode(buzzerPin, OUTPUT);

    // 초음파
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);

    initDistance();
}

void loop()
{
    // 초음파 거리, 서보 모터 상태 업데이트(초 단위)
    updateDistance();
    Serial.println();
}
void initDistance()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        pulseWidth = pulseIn(echoPin, HIGH);
        distance = pulseWidth / 58;
        Serial.print("Distance : ");
        Serial.println(distance);
        if (minDistance > distance)
        {
            minDistance = distance;
        }
        delay(1000);
    }
    Serial.print("설정완료 : ");
    Serial.print(minDistance - 5);
    Serial.println("cm 이내면 부저 울립니다.");
    delay(1000);
}
void updateDistance()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pulseWidth = pulseIn(echoPin, HIGH);
    distance = pulseWidth / 58;
    if (distance >= 0)
    {
        Serial.print(distance);
        Serial.print(" cm\t");
        // 부저 상태 업데이트
        if (distance <= minDistance - 5)
        {
            if (!buzzerActive)
            {
                tone(buzzerPin, 262); // 부저 켜기
                buzzerActive = true;
                buzzerTimer = millis();
            }
        }
        else if (buzzerActive && millis() - buzzerTimer > buzzerDuration)
        {
            noTone(buzzerPin); // 부저 끄기
            buzzerActive = false;
        }
    }
}