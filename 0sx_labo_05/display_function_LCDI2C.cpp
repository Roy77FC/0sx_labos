
#pragma once
#include "conveyor.h"
#include <LCD_I2C.h>

void printLcd(LCD_I2C lcd, String line1 = "", String line2 = ""){
  const int screenCols = 16;

  while(line1.length() < screenCols){
    line1 += ' ';
  }
  while(line2.length() < screenCols){
    line2 += ' ';
  }
 
  for(int i = 0; i < screenCols; i++){
    lcd.setCursor(i,0);
    
    if(i < line1.length()){
      lcd.print(line1[i]);
    }
  }

  for(int i = 0; i < screenCols; i++){
    lcd.setCursor(i,1);

    if(i < line1.length()){
      lcd.print(line2[i]);
    }else{
      lcd.print(' ');
    }
  }
}