#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO
#define OLED_RESET 4
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//MSP프로토콜 설정//
enum {
  HEAD1,
  HEAD2,
  HEAD3,
  DATASIZE,
  CMD,
  ROLL,
  PICTH,
  YAW,
  THROTTLE,
  AUX,
  CRC,
  PACKETSIZE
};
uint8_t mspPacket[PACKETSIZE];

const int MPU_addr = 0x68;                  //MPU Address//
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;  //가속도 센서값, 자이로 센서값//

//세가지 형태의 Roll, Pitch, Yaw 각도를 저장하기 위한 변수//
float accel_angle_x, accel_angle_y, accel_angle_z;
// float gyro_angle_x, gyro_angle_y, gyro_angle_z;
float filtered_angle_x, filtered_angle_y, filtered_angle_z;

//보정형 변수//
float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;

//시간관련 값//
float dt;
unsigned long t_now;
unsigned long t_prev;

//자이로 센서를 이용한 각도구하기//
float gyro_x, gyro_y, gyro_z;

float roll_target_angle = 0.0;
float pitch_target_angle = 0.0;
float yaw_target_angle = 0.0;

float base_roll_target_angle;
float base_pitch_target_angle;
float base_yaw_target_angle;

static uint8_t caliCount = 100;
static uint8_t caliDelay = 50;

void setup() {
  // put your setup code here, to run once:
  initMPU6050();  //MPU6050초기화//

  display.begin(i2c_Address, true);  // 시작 주소와 reset 여부 설정, true 대신 false 사용 시 수동 리셋 필요
  display.display();
  display.clearDisplay();
  display.setTextSize(2);              // 텍스트 크기 설정
  display.setTextColor(SH110X_WHITE);  // 텍스트 색상 설정
  display.setCursor(0, 0);             // 텍스트 시작 위치 설정
  display.display();                   // 화면에 표시

  // Serial.begin(115200);
  // Serial.println("calibAccelGyro");
  displayPrintln("caliAcGy");
  calibAccelGyro();  //가속도 자이로 센서의 초기 평균값을 구한다.//

  // Serial.println("initDT");
  display.setCursor(0, 16);
  displayPrintln("initDT");
  initDT();  //시간 간격 초기화//

  // Serial.println("initYPR");
  display.setCursor(0, 32);
  displayPrintln("initYPR");
  initYPR();  //Roll, Pitch, Yaw의 초기각도 값을 설정(평균을 구해 초기 각도로 설정, 호버링을 위한 목표 각도로 사용)//

  display.setCursor(0, 48);
  displayPrintln("done");
  delay(300);
}
///////////////////////
void initMPU6050() {
  //MPU6050 초기화//
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);  //I2C의 제어권을 반환//
}
/////////////////////////
void calibAccelGyro() {
  float sumAcX = 0;
  float sumAcY = 0;
  float sumAcZ = 0;
  float sumGyX = 0;
  float sumGyY = 0;
  float sumGyZ = 0;

  readAccelGyro();

  //초기 보정값은 caliCount번의 가속도 자이로 센서의 값을 받아 해당 평균값을 가진다.//
  for (uint8_t i = 0; i < caliCount; i++) {
    readAccelGyro();

    sumAcX += AcX, sumAcY += AcY, sumAcZ += AcZ;
    sumGyX += GyX, sumGyY += GyY, sumGyZ += GyZ;

    displayi(i);
    delay(caliDelay);
  }

  baseAcX = sumAcX / caliCount;
  baseAcY = sumAcY / caliCount;
  baseAcZ = sumAcZ / caliCount;

  baseGyX = sumGyX / caliCount;
  baseGyY = sumGyY / caliCount;
  baseGyZ = sumGyZ / caliCount;
}
////////////////////////
void initDT() {
  t_prev = micros();  //초기 t_prev값은 근사값//
}
////////////////////////
void initYPR() {
  //초기 호버링의 각도를 잡아주기 위해서 Roll, Pitch, Yaw 상보필터 구하는 과정을 caliCount번 반복한다.//
  for (uint8_t i = 0; i < caliCount; i++) {
    readAccelGyro();
    calcDT();
    calcAccelYPR();
    calcGyroYPR();
    calcFilteredYPR();

    base_roll_target_angle += filtered_angle_y;
    base_pitch_target_angle += filtered_angle_x;
    base_yaw_target_angle += filtered_angle_z;

    displayi(i);
    delay(caliDelay);
  }

  //평균값을 구한다.//
  base_roll_target_angle /= caliCount;
  base_pitch_target_angle /= caliCount;
  base_yaw_target_angle /= caliCount;

  //초기 타겟 각도를 잡아준다.//
  roll_target_angle = base_roll_target_angle;
  pitch_target_angle = base_pitch_target_angle;
  yaw_target_angle = base_yaw_target_angle;
}
////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  readAccelGyro();
  calcDT();
  calcAccelYPR();     //가속도 센서 Roll, Pitch, Yaw의 각도를 구하는 루틴//
  calcGyroYPR();      //자이로 센서 Roll, Pitch, Yaw의 각도를 구하는 루틴//
  calcFilteredYPR();  //상보필터를 적용해 Roll, Pitch, Yaw의 각도를 구하는 루틴//
  static bool print;
  if (print) {
    // SendDataToProcessing(); //프로세싱으로 Roll, Pitch, Yaw값을 전송//
    display.clearDisplay();     // 디스플레이 클리어
    display.setCursor(0, 0);    // 커서 위치 설정
    display.println("Angles");  // 텍스트 출력
    display.print("X: ");
    display.println(filtered_angle_x);  // X 각도 출력
    display.print("Y: ");
    display.println(filtered_angle_y);  // Y 각도 출력
    display.print("Z: ");
    display.println(filtered_angle_z);  // Z 각도 출력
    display.display();                  // 변경 사항 디스플레이에 적용
    print = false;
  } else {
    print = true;
  }
}
void calcDT() {
  t_now = micros();
  dt = (t_now - t_prev) / 1000000.0;
  t_prev = t_now;
}
void calcAccelYPR() {
  float accel_x, accel_y, accel_z;
  float accel_xz, accel_yz;
  const float RADIANS_TO_DEGREES = 180 / 3.141592;

  accel_x = AcX - baseAcX;
  accel_y = AcY - baseAcY;
  accel_z = AcZ + (16384 - baseAcZ);

  //accel_angle_y는 Roll각을 의미//
  accel_yz = sqrt(pow(accel_y, 2) + pow(accel_z, 2));
  accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES;

  //accel_angle_x는 Pitch값을 의미//
  accel_xz = sqrt(pow(accel_x, 2) + pow(accel_z, 2));
  accel_angle_x = atan(accel_y / accel_xz) * RADIANS_TO_DEGREES;

  accel_angle_z = 0;  //중력 가속도(g)의 방향과 정반대의 방향을 가리키므로 가속도 센서를 이용해서는 회전각을 계산할 수 없다.//
}
void calcGyroYPR() {
  const float GYROXYZ_TO_DEGREES_PER_SEC = 131;

  gyro_x = (GyX - baseGyX) / GYROXYZ_TO_DEGREES_PER_SEC;
  gyro_y = (GyY - baseGyY) / GYROXYZ_TO_DEGREES_PER_SEC;
  gyro_z = (GyZ - baseGyZ) / GYROXYZ_TO_DEGREES_PER_SEC;

  //gyro_angle_x += gyro_x * dt;
  //gyro_angle_y += gyro_y * dt;
  //gyro_angle_z += gyro_z * dt;
}
void calcFilteredYPR() {
  const float ALPHA = 0.96;
  float tmp_angle_x, tmp_angle_y, tmp_angle_z;

  tmp_angle_x = filtered_angle_x + gyro_x * dt;
  tmp_angle_y = filtered_angle_y + gyro_y * dt;
  tmp_angle_z = filtered_angle_z + gyro_z * dt;

  //상보필터 값 구하기(가속도, 자이로 센서의 절충)//
  filtered_angle_x = ALPHA * tmp_angle_x + (1.0 - ALPHA) * accel_angle_x;
  filtered_angle_y = ALPHA * tmp_angle_y + (1.0 - ALPHA) * accel_angle_y;
  filtered_angle_z = tmp_angle_z;
}
void readAccelGyro() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);  //I2C의 제어권을 이어간다//
  Wire.requestFrom(MPU_addr, 14, true);

  //가속도, 자이로 센서의 값을 읽어온다.//
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}
// void SendDataToProcessing(){
//   // Serial.print(F("DEL: "));
//   // Serial.print(dt, DEC);
//   // Serial.print(F("#ACC: "));
//   // Serial.print(accel_angle_x, 2);
//   // Serial.print(F(", "));
//   // Serial.print(accel_angle_y, 2);
//   // Serial.print(F(", "));
//   // Serial.print(accel_angle_z, 2);
//   // Serial.print(F("#GYR:"));
//   // Serial.print(gyro_angle_x, 2);
//   // Serial.print(F(", "));
//   // Serial.print(gyro_angle_y, 2);
//   // Serial.print(F(", "));
//   // Serial.print(gyro_angle_z, 2);
//   // Serial.print(F("#FIL: "));
//   // Serial.print(filtered_angle_x);
//   // Serial.print(F(", "));
//   // Serial.print(filtered_angle_y);
//   // Serial.print(F(", "));
//   // Serial.print(filtered_angle_z);
//   // Serial.print(F("\n"));
//   delay(5);
// }

void displayPrint(String str) {
  display.print(str);  // 텍스트 출력
  display.display();   // 변경 사항 디스플레이에 적용
}

void displayPrintln(String str) {
  display.println(str);  // 텍스트 출력
  display.display();     // 변경 사항 디스플레이에 적용
}
void displayi(uint8_t i) {
  display.fillRect(100, 47, 28, 16, SH110X_BLACK);  // 사각형으로 이전 숫자 지우기
  display.setCursor(100, 47);                       // 커서 위치 설정
  display.print(i);                                 // 현재 인덱스 출력
  display.display();                                // 디스플레이 갱신
}