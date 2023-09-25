//   -A-
//  F   B
//   -G-
//  E   C
//   -D-  .DP

const byte number[] = {
    // dot .GFEDCBA
    // 0,1,2,3,4,5,6,7,8,9
    B00111111,
    B00000110,
    B01011011,
    B01001111,
    B01100110,
    B01101101,
    B01111101,
    B00000111,
    B01111111,
    B01100111,
};

const byte alphabet[] = {
    // A       B          C          D          E          F          G          H          I          J          K          L          M
    B01110111, B01111100, B00111001, B01011110, B01111001, B01110001, B00111101, B01110110, B00000110, B00001110, B01110110, B00111000, B01010101,
    // N       O          P          Q          R          S          T          U          V          W          X          Y          Z
    B01010110, B00111111, B01110011, B01100111, B01010010, B01101101, B01111000, B00111110, B00111110, B00101010, B01001001, B01100110, B01011011};

void setup()
{
  for (int i = 2; i <= 9; i++)
  {
    pinMode(i, OUTPUT);
  };
  digitalWrite(9, LOW);
}

void loop()
{
  for (int i = 0; i <= 9; i++)
  {
    fndDisplay(i);
    delay(1000);
  }
}

void fndDisplay(int displayValue)
{
  boolean bitValue;
  for (int i = 2; i <= 9; i++)
  {
    digitalWrite(i, LOW);
  }
  for (int i = 0; i < 7; i++)
  {
    bitValue = bitRead(number[displayValue], i);
    digitalWrite(i + 2, bitValue);
  }
}