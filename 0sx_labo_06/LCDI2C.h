#pragma once
#include <LCD_I2C.h>

class LcdScreen {

private:

  LCD_I2C Lcd;
  uint8_t address; 
  uint8_t columns; 
  uint8_t rows;

public:

  LcdScreen(const uint8_t& address, const uint8_t& columns, const uint8_t& rows);

  void begin();
  void printLcd(String line1 = "", String line2 = "");
};