// https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal
// 앱 다운 받아서 블루투스 연결

// 초기값 설정하는 방법
// "/set 정수 정수 실수"
// ex) /set 0 10 1.5
// (기본위치, 당겼을 때 위치, 움직일 때 시간)

// 시작하는 방법
// "/start"

// 멈추는 방법
// "/start"

// HC-SR04
// TRG(D11), ECHO(D12)

// HC-06
// TXD(D2), RXD(D3)

#include <SoftwareSerial.h>

// HC-SR04 핀 정의
const int trigPin = 11;
const int echoPin = 12;

const int ledPin = 13;

// HC-06 블루투스 모듈을 위한 소프트웨어 시리얼
const int RX = 2;
const int TX = 3;
SoftwareSerial bluetooth(RX, TX);  // RX, TX

// 변수 설정
int distance_base = -1;
int distance_goal = -1;
float time_goal = -1;
unsigned long time_start;
unsigned long time_set_start;
bool measuring = false;
bool movingTowardsGoal = true;
int count = 0;
int fastCount = 0;
int slowCount = 0;
String command = "";
bool setDefaltValue = false;

void setup() {
  // 핀 초기화
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //블루투스 초기화
  bluetooth.begin(9600);

  //시리얼 초기화
  Serial.begin(9600);
}

void loop() {
  command = "";
  getData();
  if (command.startsWith("/init")) {
    unsigned long endTime = millis() + 10000;  // 현재 시간으로부터 10초 후
    int maxDistance = 0;                       // 최대 거리
    int minDistance = 300;                     // 최소 거리 (초음파 센서의 최대 범위를 고려하여 초기값 설정)

    while (millis() < endTime) {
      int distance = measureDistance();
      if (distance > maxDistance) {
        maxDistance = distance;
        bluetooth.print("기본위치변경: ");
        bluetooth.println(maxDistance);
      }
      if (distance < minDistance && distance != 0) {
        minDistance = distance;
        bluetooth.print("이동위치변경: ");
        bluetooth.println(minDistance);
      }
      delay(50);  // 센서 측정 간격
    }

    distance_base = maxDistance;
    distance_goal = minDistance;

    // 결과 출력
    bluetooth.print("기본 위치: ");
    bluetooth.println(distance_base);
    bluetooth.print("목표 위치: ");
    bluetooth.println(distance_goal);

    setDefaltValue = true;                   // 기본값 설정 완료 표시
  } else if (command.startsWith("/set ")) {  // "set 정수 정수 실수" 형식의 명령을 처리
    String paramStr = command.substring(5);  // "/set " 다음의 문자열 추출
    int firstSpace = paramStr.indexOf(' ');
    int secondSpace = paramStr.lastIndexOf(' ');
    if (secondSpace > firstSpace && firstSpace != -1) {
      distance_base = paramStr.substring(0, firstSpace).toInt();
      distance_goal = paramStr.substring(firstSpace + 1, secondSpace).toInt();
      time_goal = paramStr.substring(secondSpace + 1).toFloat();

      setDefaltValue = true;
    }
    delay(2);
  } else if (command.startsWith("/start")) {
    if (!setDefaltValue) {
      bluetooth.println("기본값을 설정해 주세요");
      bluetooth.println("/set 기본값 목표값 시간");
    } else {
      // 값 초기화
      count = 0;
      fastCount = 0;
      slowCount = 0;

      time_start = millis();
      time_set_start = millis();
      measuring = true;
    }
  } else if (command.startsWith("/stop")) {
    // 블루투스로 데이터 전송
    bluetooth.print("Count: ");
    bluetooth.println(count / 2);
    bluetooth.print(fastCount);
    bluetooth.println("번 빨랐습니다.");
    bluetooth.print(slowCount);
    bluetooth.println("번 느렸습니다.");

    measuring = false;
    unsigned long totalMillis = millis() - time_set_start;
    int hours = totalMillis / 3600000;
    int mins = (totalMillis % 3600000) / 60000;
    int secs = ((totalMillis % 3600000) % 60000) / 1000;

    // 운동 시간 출력
    bluetooth.print("운동시간: ");
    bluetooth.print(hours);
    bluetooth.print("시간 ");
    bluetooth.print(mins);
    bluetooth.print("분 ");
    bluetooth.print(secs);
    bluetooth.println("초");
  }

  // 거리 업데이트
  if (measuring) {
    int distance = measureDistance();
    checkDistanceAndTime(distance);
  }
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  if (distance >= 300 || distance <= 1) {
    return 0;
  }
  return distance;
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
void checkDistanceAndTime(int distance) {
  if ((movingTowardsGoal && distance <= distance_goal) || (!movingTowardsGoal && distance >= distance_base)) {

    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);

    unsigned long time_taken = millis() - time_start;
    if (time_taken > time_goal * 1000 + 500) {  // 시간이 목표보다 느림
      slowCount++;
      bluetooth.print("더 빠르게 하세요");
    } else if (time_taken < time_goal * 1000 - 500) {  // 시간이 목표보다 빠름
      fastCount++;
      bluetooth.print("조금 천천히 하세요");
    } else {  // 시간이 적당함
      bluetooth.print("잘 하고 있습니다");
    }
    bluetooth.print(" --> ");

    movingTowardsGoal = !movingTowardsGoal;
    time_start = millis();
    count++;
    bluetooth.print(count / 2);
    bluetooth.print("개 - ");
    bluetooth.print(time_taken / 1000);
    bluetooth.print(".");
    bluetooth.print(time_taken % 1000);
    bluetooth.println("초");
  }
}
