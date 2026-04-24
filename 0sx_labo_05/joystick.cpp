#pragma once
#include "conveyor.h"

Joystick::Joystick(const int &xPin, const int &yPin, const int &buttonPin){
  this->xPin = xPin;
  this->yPin = yPin;
  this->buttonPin = buttonPin;
  
  pinMode(this->buttonPin, INPUT_PULLUP);
}

int Joystick::readXValue(){
  return analogRead(this->xPin);
}

int Joystick::readYValue(){
  return analogRead(this->yPin);
}

int Joystick::getButtonPin(){
  return this->buttonPin;
}