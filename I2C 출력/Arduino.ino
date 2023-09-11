#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(115200);

  lcd.init();

  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Arduino LCD");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Welcome");
  delay(1000);

  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Open Serial Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Type to display");
}

void loop()
{
  if (Serial.available())
  {
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Message from PC");
    lcd.setCursor(0, 1);
    while (Serial.available() > 0)
    {
      lcd.write(Serial.read());
    }
  }
}
