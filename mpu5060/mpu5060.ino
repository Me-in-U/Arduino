#include <Wire.h>
#include <MPU6050.h>
#include <TM1637Display.h>

// TM1637 설정
TM1637Display display(D7, D6);

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

int count = 0;
bool isLifting = false;

void setup() {
  Serial.begin(115200);

  // MPU6050 초기화
  Wire.begin(D2, D1); // NodeMCU의 SDA(D2), SCL(D1) 핀 설정
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
  Serial.println("MPU6050 connection successful");

  // TM1637 디스플레이 초기화
  display.setBrightness(0x0f);
  display.showNumberDec(0); // 시작 시 0으로 초기화
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("AX: ");
  Serial.print(ax);
  Serial.print(", AY: ");
  Serial.print(ay);
  Serial.print(", AZ: ");
  Serial.print(az);

  Serial.print("- GX: ");
  Serial.print(gx);
  Serial.print(", GY: ");
  Serial.print(gy);
  Serial.print(", GZ: ");
  Serial.println(gz);
  // 아령 운동 감지 알고리즘
  if (az > 8000 && !isLifting) { // 아령이 올라가는 것을 감지
    isLifting = true;
  } else if (az < -8000 && isLifting) { // 아령이 내려가는 것을 감지
    isLifting = false;
    count++; // 운동 횟수 증가
    display.showNumberDec(count, false); // 디스플레이 업데이트
    Serial.print("운동 횟수: ");
    Serial.println(count);
  }

  delay(100);
}
