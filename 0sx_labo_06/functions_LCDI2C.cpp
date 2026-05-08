#include "LCDI2C.h"

LcdScreen::LcdScreen(const uint8_t& address, const uint8_t& columns, const uint8_t& rows)
  :Lcd(address, columns, rows)
  {   
    this->address = address;
    this->columns = columns;
    this->rows = rows;
  }

void LcdScreen::begin(){
  this->Lcd.begin();
  this->Lcd.backlight();
}

void LcdScreen::printLcd(String line1 = "", String line2 = ""){

  while(line1.length() < this->columns) line1 += ' ';
  while(line2.length() < this->columns) line2 += ' ';

  this->Lcd.setCursor(0, 0);
  this->Lcd.print(line1);

  this->Lcd.setCursor(0, 1);
  this->Lcd.print(line2);
}