#pragma once
#include "conveyor.h"

DC_Motor::DC_Motor(const int &input1Pin, const int &input2Pin){
  this->pinInput1 = input1Pin;
  this->pinInput2 = input2Pin;

  pinMode(this->pinInput1, OUTPUT);
  pinMode(this->pinInput2, OUTPUT);
}

void DC_Motor::activateMotor(const int &valForInput1, const int &valForInput2){
  analogWrite(this->pinInput1, valForInput1);
  analogWrite(this->pinInput2, valForInput2);
}

int DC_Motor::getSeuilMin(){
  return this->SEUIL_MIN;
}