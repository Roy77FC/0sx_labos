#include "conveyor.h"
#include <LCD_I2C.h>
#include <OneButton.h>

Joystick joystick(A0, A1, 2);
DC_Motor DC(44,45);
LCD_I2C lcd(0x27, 16, 2);
OneButton emergencyBtn(joystick.getButtonPin(), true);



const int GREEN_LED_PIN = 10;
const int RED_LED_PIN = 9;
const int JOY_DEAD_ZONE = 30; 
const long JOY_MAX_VAL = 255;
const long JOY_MIN_VAL = 0;
const int MID_ZONE_UP = (JOY_MAX_VAL / 2) + JOY_DEAD_ZONE;
const int MID_ZONE_DOWN = (JOY_MAX_VAL / 2) - JOY_DEAD_ZONE;
const int MIN_SPEED = 60;

bool emergencyButtonClick = false;
enum ConveyorState {ON, OFF, EMERGENCY};
ConveyorState  conveyorState = OFF;
enum ConveyorDirection {FRONT, BACK, QUIET};
ConveyorDirection conveyorDirection;

void emergencyClick(){

  static unsigned long previousState = 0;
  int delayTime = 50;

  if(millis() - previousState >= delayTime){
    previousState = millis();
    if(!emergencyButtonClick){
      emergencyButtonClick = true;
    }
  }
}

void setJoystickValues(int &xVal, int &yVal){
  
  xVal = map(xVal, 0, 1023, JOY_MIN_VAL, JOY_MAX_VAL);
  yVal = map(yVal, 0, 1023, JOY_MAX_VAL, JOY_MIN_VAL);
}

void setSpeedAndDirection(int &xVal, int &yVal){

  if(yVal > MID_ZONE_UP){
    DC.activateMotor(xVal, LOW);
    conveyorDirection = FRONT;
    Serial.println("FRONT");
  }else if(yVal < MID_ZONE_DOWN){

    if(xVal < MIN_SPEED){
      xVal = MIN_SPEED;
    }
    DC.activateMotor(LOW, xVal);
    Serial.println("BACK");
    conveyorDirection = BACK;

  }else{
    DC.activateMotor(LOW, LOW);
    Serial.println("QUIET");
    conveyorDirection = QUIET;
  }
}

void mainFunction(int &xVal, int &yVal){

  static unsigned long previousTime = 0;
  static bool alternate = false;
  static bool joyExpresion;
  String speepString = "Vitesse: ";
  String directionString = "Sens: ";
  String stateString = "Etat: ";
  String state = "OFF";
  String direction = "AVANT";
  // Serial.print("X value : " + String(xVal));
  // Serial.println("Y value : " + String(yVal));

  if(conveyorState != 2){
    if(yVal < MID_ZONE_DOWN){
      direction = "ARRIERE";
    }else{
      direction = "AVANT";
    }

    if(conveyorState == 0){
      state = "ON";

      speepString += String(xVal);
      directionString += direction;
      stateString += state;

      if(millis() - previousTime >= 1000){
        previousTime = millis();

        if(alternate){
          printLcd(lcd, directionString, speepString);
          alternate = false;
        }else{
          printLcd(lcd, stateString, speepString);
          alternate = true;
        }
      }

    }else{
      state = "OFF";

      speepString += "0";
      directionString += "ACCUN";
      stateString += state;

      if(millis() - previousTime >= 1000){
        previousTime = millis();

        if(alternate){
          printLcd(lcd, directionString, speepString);
          alternate = false;
        }else{
          printLcd(lcd, stateString, speepString);
          alternate = true;
        }
      }
    }
  }else{
    printLcd(lcd, "URGENCE");
  }

  switch(conveyorState){
    case OFF:

      if(yVal > MID_ZONE_UP || yVal < MID_ZONE_DOWN){
        conveyorState = ON;
        digitalWrite(GREEN_LED_PIN, HIGH);
      }

      if(emergencyButtonClick) {
        conveyorState = EMERGENCY;
        emergencyButtonClick = false;
      }
      break;
    
    case ON:

      setSpeedAndDirection(xVal, yVal);

      if(!(yVal > MID_ZONE_UP || yVal < MID_ZONE_DOWN)){
        conveyorState = OFF;
        digitalWrite(GREEN_LED_PIN, LOW);
      }

      if(emergencyButtonClick) {
        conveyorState = EMERGENCY;
        emergencyButtonClick = false;
      }
      break;
    
    case EMERGENCY:

      digitalWrite(RED_LED_PIN, HIGH);

      if(emergencyButtonClick){
        emergencyButtonClick = false;
        conveyorState = OFF;

        digitalWrite(RED_LED_PIN, LOW);
      }

      break;
  }
}


void setup() {
  Serial.begin(9600);

  lcd.begin();
  lcd.backlight();

  emergencyBtn.attachClick(emergencyClick);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {

  int xVal = joystick.readXValue();
  int yVal = joystick.readYValue();
  setJoystickValues(xVal, yVal);

  emergencyBtn.tick();
  mainFunction(xVal, yVal);
}
