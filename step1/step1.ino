#include <AccelStepper.h>
#include <MultiStepper.h>

// 핀 설정을 위한 구조체 정의
struct MotorPins {
  int enablePin;
  int stepPin;
  int dirPin;
};

// 각 모터에 대한 핀 설정
MotorPins motorPins[] = {
  { 2, 3, 4 },  // Motor 1
  { 5, 6, 7 },  // Motor 2
  { 8, 9, 10 }  // Motor 3
};

// 스테퍼 객체 배열
AccelStepper steppers[3] = {
  AccelStepper(1, motorPins[0].stepPin, motorPins[0].dirPin),
  AccelStepper(1, motorPins[1].stepPin, motorPins[1].dirPin),
  AccelStepper(1, motorPins[2].stepPin, motorPins[2].dirPin)
};

// 스위치 핀 번호 설정
int switchPins[] = { 11, 12, 13 };

MultiStepper multiStepper;

void setup() {
  // 핀 모드 설정
  for (int i = 0; i < 3; i++) {
    pinMode(motorPins[i].enablePin, OUTPUT);
    pinMode(motorPins[i].stepPin, OUTPUT);
    pinMode(motorPins[i].dirPin, OUTPUT);

    // 스테퍼 모터 초기화
    steppers[i].setCurrentPosition(0);
    steppers[i].setMaxSpeed(3000);
    steppers[i].setAcceleration(500);

    // MultiStepper 객체에 스테퍼 추가
    multiStepper.addStepper(steppers[i]);

    // 스위치 핀 설정
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 3; i++) {
    homeStepper(steppers[i], switchPins[i]);
  }
}

void loop() {
  // 모든 스테퍼 모터의 목표 위치 설정
  long positions[] = { 1600, 3200, 4800 };
  for (int i = 0; i < 3; i++) {
    steppers[i].moveTo(positions[i]);
  }

  // 모든 모터가 목표 위치에 도달할 때까지 실행
  while (steppers[0].distanceToGo() != 0 || steppers[1].distanceToGo() != 0 || steppers[2].distanceToGo() != 0) {
    for (int i = 0; i < 3; i++) {
      if (steppers[i].distanceToGo() != 0) steppers[i].run();
    }
  }
  delay(100);

  // 모터 복귀 위치 설정
  long returnPositions[] = { 0, 0, 0 };
  for (int i = 0; i < 3; i++) {
    steppers[i].moveTo(returnPositions[i]);
  }

  // 모터 별로 다른 조건을 적용하여 멈춤
  while (steppers[0].distanceToGo() != 0 || steppers[1].distanceToGo() != 0 || steppers[2].distanceToGo() != 0) {
    for (int i = 0; i < 3; i++) {
      if (steppers[i].distanceToGo() != 0) steppers[i].run();
    }
  }
  delay(100);
}

void homeStepper(AccelStepper &stepper, int switchPin) {
  stepper.setSpeed(-200);                   // 음의 속도로 천천히 이동
  while (digitalRead(switchPin) == HIGH) {  // 스위치가 눌리지 않은 동안 계속 이동
    stepper.runSpeed();
  }
  stepper.setCurrentPosition(0);  // 현재 위치를 0으로 설정
  stepper.stop();                 // 모터 정지
}
