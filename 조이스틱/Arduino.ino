const int xAxisPin = 0;
const int yAxisPin = 1;
const int zAxisPin = 2;
const String bars[] = {
    "|---------",
    "-|--------",
    "--|-------",
    "---|------",
    "----|-----",
    "-----|----",
    "------|---",
    "-------|--",
    "--------|-",
    "---------|",
};
void setup()
{
  pinMode(zAxisPin, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop()
{
  int xValue = analogRead(xAxisPin);
  int yValue = analogRead(yAxisPin);
  int zValue = digitalRead(zAxisPin);

  int xDisplay = map(xValue, 0, 1023, 0, 9);
  int yDisplay = map(yValue, 0, 1023, 0, 9);
  String output = "x: ";
  output += String(xValue);
  output += " ";
  output += bars[xDisplay];
  output += "   \ty: ";
  output += String(yValue);
  output += " ";
  output += bars[yDisplay];
  if (zValue == LOW)
  {
    output += "   Button";
  }
  Serial.println(output);
  delay(100);
}