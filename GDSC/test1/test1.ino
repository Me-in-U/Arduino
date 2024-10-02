#include <MPU6050.h>

// https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal
// 앱 다운 받아서 블루투스 연결

// 초기값 설정하는 방법

// "/setdist"
// 10초간 기본거리 당겼을때 거리 자동 측정

// "/settime 1.5"
// 편도 이동 시간 설정

// 시작하는 방법
// "/start"

// 멈추는 방법
// "/stop"

// HC-SR04
// TRG(D11), ECHO(D12)

// HC-06
// RXD(D2), TXD(D3)

//data 0 (거리)
//data 1 (빠르기) (시간차)
//data 2 (빨랐던수) (느렸던수)

#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// HC-SR04 핀 정의
const int trigPin = 11;
const int echoPin = 12;

const int ledPin = 9;

// LiquidCrystal I2C 설정 (필요한 경우)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 주소 0x27, 16자 2줄

// HC-06 블루투스 모듈을 위한 소프트웨어 시리얼
const int RX = 2;
const int TX = 3;
SoftwareSerial bluetooth(RX, TX);  // RX, TX

// 변수 설정
float distance_base = -1;
float distance_goal = -1;
float time_goal = -1;
unsigned long time_start;
bool measuring = false;
bool movingTowardsGoal = true;
int count = 0;
int fastCount = 0;
int slowCount = 0;
bool setDefaltDistance = false;
bool setDefaltTime = false;
String command = "";
int threshold =500;

void setup() {
  // 핀 초기화
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //블루투스 초기화
  bluetooth.begin(9600);

  //시리얼 초기화
  Serial.begin(9600);

  // LiquidCrystal I2C 초기화 (필요한 경우)
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Lets start");

  bluetooth.println("Lets start");
}

void loop() {
  command = "";
  getData();

  if (command.startsWith("/setdist")) {
    unsigned long endTime = millis() + 3000;  // 현재 시간으로부터 3초 후
    float maxDistance = 0;                     // 최대 거리
    float minDistance = 300;                   // 최소 거리

    // LCD 초기화
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    while (millis() < endTime) {
      float distance = measureDistance();
      lcd.setCursor(6, 0);
      lcd.print(distance);
      lcd.print(" cm    ");
      if (distance > maxDistance) {
        maxDistance = distance;
        // bluetooth.print("기본 위치 변경: ");
        // bluetooth.println(maxDistance);
        lcd.setCursor(0, 1);
        lcd.print("max: ");
        lcd.print(maxDistance);
      }
      if (distance < minDistance && distance > -1) {
        minDistance = distance;
        // bluetooth.print("이동 위치 변경: ");
        // bluetooth.println(minDistance);
        lcd.setCursor(8, 1);
        lcd.print("min: ");
        lcd.print(minDistance);
      }
      bluetooth.print("기본위치:");
      bluetooth.print(maxDistance);
      bluetooth.print(", 이동위치:");
      bluetooth.println(minDistance);

      delay(100);  // 센서 측정 간격
    }
    bluetooth.println();

    distance_base = maxDistance;
    distance_goal = minDistance;

    // 결과 출력
    bluetooth.print("기본 위치: ");
    bluetooth.println(distance_base);
    bluetooth.print("목표 위치: ");
    bluetooth.println(distance_goal);
    bluetooth.println();

    setDefaltDistance = true;
  } else if (command.startsWith("/settime ")) {
    String paramStr = command.substring(9);  // "/settime " 다음의 문자열 추출
    time_goal = paramStr.toFloat();

    // 결과 출력
    bluetooth.print("목표 편도 시간: ");
    bluetooth.println(time_goal);
    bluetooth.println();
    bluetooth.print("data ");
    bluetooth.print(time_goal);
    bluetooth.println();

    setDefaltTime = true;
  } else if (command.startsWith("/start")) {
    delay(200);

    lcd.clear();
    if (!setDefaltDistance && !setDefaltTime) {
      bluetooth.println("기본값을 설정해 주세요");
      // LCD 초기화
      lcd.setCursor(0, 0);
      lcd.print("Please Set");
      lcd.setCursor(0, 1);
      lcd.print("Defalut Value");
    } else {
      // 값 초기화
      count = 0;
      fastCount = 0;
      slowCount = 0;

      // LCD 초기화
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      lcd.setCursor(0, 1);
      lcd.print("Time: ");

      time_start = millis();
      measuring = true;
    }
  } else if (command.startsWith("/stop")) {
    delay(200);

    // LCD에 결과 표시
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Total Count: ");
    lcd.print(count / 2);
    lcd.setCursor(0, 1);
    lcd.print("Fast:");
    lcd.print(fastCount);
    lcd.setCursor(8, 1);
    lcd.print("Slow:");
    lcd.print(slowCount);

    // 블루투스로 데이터 전송
    bluetooth.print("data 2 ");
    bluetooth.print(fastCount);
    bluetooth.print(" ");
    bluetooth.println(slowCount);

    measuring = false;
  }

  // 거리 업데이트
  if (measuring) {
    float distance = measureDistance();
    lcd.setCursor(6, 0);
    lcd.print(distance);
    lcd.print(" cm    ");

// 블루투스로 데이터 전송
    bluetooth.print("data 0 ");
    bluetooth.println(distance);

    checkDistanceAndTime(distance);

    delay(50);
  }
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  if (distance >= 300 || distance <= 0.5) {
    return 0;
  }
  return distance;
}

void checkDistanceAndTime(float distance) {
  if ((movingTowardsGoal && distance <= distance_goal) || (!movingTowardsGoal && distance >= distance_base)) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    lcd.setCursor(6, 1);

    // 블루투스로 데이터 전송
    bluetooth.print("data 1 ");

    unsigned long time_taken = millis() - time_start;
    if (time_taken > time_goal * 1000 + threshold) {  // 시간이 목표보다 느림
      slowCount++;
      lcd.print("FASTER");
      bluetooth.print("0 ");
    } else if (time_taken < time_goal * 1000 - threshold) {  // 시간이 목표보다 빠름
      fastCount++;
      lcd.print("SLOWER");
      bluetooth.print("1 ");
    } else {  // 시간이 적당함
      lcd.print("GOOD  ");
      bluetooth.print("2 ");
    }

    movingTowardsGoal = !movingTowardsGoal;  // 목표 방향 전환
    time_start = millis();                   // 시간 재설정
    count++;
    bluetooth.print(time_taken / 1000);
    bluetooth.print(".");
    bluetooth.println(time_taken % 1000);
  }
}

void getData() {
  if (bluetooth.available()) {
    command = bluetooth.readStringUntil('\n');
    Serial.print("블루투스 입력 : ");
  }
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    Serial.print("시리얼 입력 : ");
  }
  if (command != "") {
    command.trim();
    Serial.println(command);
  }
}
