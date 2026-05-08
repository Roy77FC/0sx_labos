#pragma once
#include <LCD_I2C.h>
#include <Arduino.h>

class DC_Motor{
  public:
    DC_Motor(const int &input1Pin, const int &input2Pin);
    void activateMotor(const int &valForInput1, const int &valForInput2);
    int getSeuilMin();

  private:
    const int SEUIL_MIN = 60;
    int pinInput1;
    int pinInput2;
};

class Joystick{
  
  public:
    Joystick(const int &xPin, const int &yPin, const int &buttonPin);
    int readXValue();
    int readYValue();
    int getButtonPin();
  private:
    int xPin;
    int yPin;
    int buttonPin;
  
};

void printLcd(LCD_I2C lcd, String line1 = "", String line2 = "");