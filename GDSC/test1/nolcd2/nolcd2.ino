//data 0 (거리)
//data 1 (빠르기) (시간차)
//data 2 (빨랐던수) (느렸던수)

#include <SoftwareSerial.h>

// HC-SR04 핀 정의
const int trigPin = 11;
const int echoPin = 12;

// 블루투스 모듈을 위한 소프트웨어 시리얼
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
  //초기화
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  bluetooth.begin(9600);
  Serial.begin(9600);
}

void loop() {
  command = "";
  getData();

  if (command.startsWith("/setdist")) {
    unsigned long endTime = millis() + 5000;  // 현재 시간으로부터 5초 후
    float maxDistance = 0;                     // 최대 거리
    float minDistance = 300;                   // 최소 거리

    while (millis() < endTime) {
      float distance = measureDistance();
      if (distance > maxDistance) {
        maxDistance = distance;
      }
      if (distance < minDistance && distance > -1) {
        minDistance = distance;
      }
      delay(100);  // 센서 측정 간격
    }

    distance_base = maxDistance;
    distance_goal = minDistance;

    // 결과 출력
    bluetooth.print("기본 위치: ");
    bluetooth.println(distance_base);
    bluetooth.print("목표 위치: ");
    bluetooth.println(distance_goal);

    setDefaltDistance = true;
  } else if (command.startsWith("/settime ")) {
    String paramStr = command.substring(9);  // "/settime " 다음의 문자열 추출
    time_goal = paramStr.toFloat();

    setDefaltTime = true;
  } else if (command.startsWith("/start")) {
    delay(200);

    if (!setDefaltDistance && !setDefaltTime) {
      bluetooth.println("기본값을 설정해 주세요");
    } else {
      // 값 초기화 이후 다시 측정 시작
      count = 0;
      fastCount = 0;
      slowCount = 0;

      time_start = millis();
      measuring = true;
    }
  } else if (command.startsWith("/stop")) {
    delay(200);

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
  if ((movingTowardsGoal && distance <= distance_goal)
   || (!movingTowardsGoal && distance >= distance_base)) {
    // 블루투스로 데이터 전송
    bluetooth.print("data 1 ");image.png

    unsigned long time_taken = millis() - time_start;
    if (time_taken > time_goal * 1000 + threshold) {  // 시간이 목표보다 느림
      slowCount++;
      bluetooth.print("0 ");
    } else if (time_taken < time_goal * 1000 - threshold) {  // 시간이 목표보다 빠름
      fastCount++;
      bluetooth.print("1 ");
    } else {  // 시간이 적당함
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
