const char trigPin = 13;
const char echoPin = 12;

int pulseWidth;
int distance;
int distanceOld;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pulseWidth = pulseIn(echoPin, HIGH);
  distance = pulseWidth / 58;

  if (distance < 200 || distance >= 2) {
    if (distance != distanceOld) {
      Serial.print(distance);
      Serial.println(" cm");
    }
  }
  distanceOld = distance;
  delay(100);
}