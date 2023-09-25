const int numRows = 4;
const int numCols = 4;

char keys[numRows][numCols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

int rowPins[] = {2, 3, 4, 5};
int colPins[] = {6, 7, 8, 9};

void setup()
{
  for (int i = 0; i < numRows; i++)
  {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < numCols; i++)
  {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
  Serial.begin(115200);
}

void loop()
{
  char key = keypadRead();
  if (key != 0)
  {
    Serial.print("You push ");
    Serial.print(key);
    Serial.println(" Key");
  }
}
char keypadRead()
{
  char key = 0;

  for (int i = 0; i < numCols; i++)
  {
    digitalWrite(colPins[i], LOW);
    for (int j = 0; j < numRows; j++)
    {
      if (digitalRead(rowPins[j]) == LOW)
      {
        delay(10);
        while (digitalRead(rowPins[j]) == LOW)
          ;
        key = keys[i][j];
      }
    }
    digitalWrite(colPins[i], HIGH);
  }
  return key;
}