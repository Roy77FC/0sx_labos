#include <HCSR04.h>
#include <Servo.h>
#include <LCD_I2C.h>
#include <OneButton.h>


const byte SESOR_TRIGGER_PIN = 9;
const byte SESOR_ECHO_PIN = 10;
const byte SERVO_PIN = 7;
const byte OPEN_BTN_PIN = 3;
const byte ERMEGENCY_STOP_BTN_PIN = 2;
const int MAX_ANGLE = 170;
const int MIN_ANGLE = 10;
const int SERVO_DELAY = 15;
const int OPENING_DOOR_DIST = 10;
enum {AUTO_OPENING, MANUAL_OPENING, EMERGENCY_STOP} currentState;
enum {DOOR_CLOSED, OPENINIG_DOOR, WAITING, CLOSING_DOOR} doorState;
String state = "";

bool opentButtonClick = false;
bool emergencyButtonClick = false;
bool manualOpeninigFinished = false;

LCD_I2C lcd(0x27, 16, 2); 
HCSR04  hc(SESOR_TRIGGER_PIN, SESOR_ECHO_PIN);
Servo servo;

OneButton openBtn(OPEN_BTN_PIN, true);
OneButton emergencyBtn(ERMEGENCY_STOP_BTN_PIN, true);



float getDistance(){

  float distance = hc.dist();

  if(distance <= 0) return 999;

  return distance;
}

void openDoor(int &angle){

  if(angle < MAX_ANGLE){
    angle++;
    servo.write(angle);
  }
}

void closeDoor(int &angle){

  if(angle > MIN_ANGLE){
    angle--;
    servo.write(angle);
  }
}

void printLcd(String line1 = "", String line2 = ""){
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

void autoOpening(int &angle, float distance){

  const int WAIT_TIME = 10000;
  static unsigned long previousTime = 0;
  static unsigned long previousTimeWaiting = 0;


  switch(doorState){
    case DOOR_CLOSED:
      if(distance > 0 && distance <= OPENING_DOOR_DIST){
        if(angle == MIN_ANGLE){
          doorState = OPENINIG_DOOR;
        }
      }
      break;

    case OPENINIG_DOOR:

      if(millis() - previousTime >= SERVO_DELAY){
        openDoor(angle);
        previousTime = millis();
      }
      
      printLcd("Ouverture " + state, "Angle: " + String(angle));

      if(angle >= MAX_ANGLE){
        previousTimeWaiting = millis();
        doorState = WAITING;
        servo.detach();
      }
      break;

    case WAITING:

      printLcd("Temps d'attente", "10 secondes");

      if(millis() - previousTimeWaiting >= WAIT_TIME){
        doorState = CLOSING_DOOR;
        previousTimeWaiting = millis();
        servo.attach(SERVO_PIN);
      }
      break;

    case CLOSING_DOOR:

      if(millis() - previousTime >= SERVO_DELAY){
        closeDoor(angle);
        previousTime = millis();
      }

      printLcd("Fermeture " + state, "Angle: " + String(angle));

      if(distance > 0 && distance <= OPENING_DOOR_DIST){
        doorState = OPENINIG_DOOR;
      }

      if(angle <= MIN_ANGLE){
        doorState = DOOR_CLOSED;
      }
      break;
  }
}

void manualOpening(int &angle){

  const int WAIT_TIME = 10000;
  static unsigned long previousTimeServo = 0;
  static unsigned long previousTimeWait = 0;
  static bool firstEntry = false;

  if(!firstEntry && doorState == CLOSING_DOOR){
    firstEntry = true;
    doorState = OPENINIG_DOOR;
  }


  switch(doorState){
    case DOOR_CLOSED:
      doorState = OPENINIG_DOOR;
      break;

    case OPENINIG_DOOR:

      if(millis() - previousTimeServo >= SERVO_DELAY){
        openDoor(angle);
        previousTimeServo = millis();
      }

      printLcd("Ouverture " + state, "Angle: " + String(angle));

      if(angle >= MAX_ANGLE){
        doorState = WAITING;
        previousTimeWait = millis();
        servo.detach();
      }
      break;
    
    case WAITING:

      printLcd("Temps d'attente", "10 secondes");

      if(millis() - previousTimeWait >= WAIT_TIME){
        previousTimeWait = millis();
        doorState = CLOSING_DOOR;
        servo.attach(SERVO_PIN);
      }
      break;

    case CLOSING_DOOR:

      if(millis() - previousTimeServo >= SERVO_DELAY){
        closeDoor(angle);
        previousTimeServo = millis();
      }

      printLcd("Fermeture " + state, "Angle: " + String(angle));

      if(angle >= MAX_ANGLE){
        doorState = DOOR_CLOSED;
        firstEntry = false;
        manualOpeninigFinished = true;
      }
      break;
  }

}

void manualOpenClick(){

  static unsigned long previousState = 0;
  int delayTime = 50;

  if(millis() - previousState >= delayTime){
    previousState = millis();
    if(!opentButtonClick){
      opentButtonClick = true;
      emergencyButtonClick = false;
    }
  }
}

void mainFunction(int &angle, float distance){

  static bool justOnceAuto = true;
  static bool justOnceManual = true;
  const int delay = 100;
  static unsigned long previousTimeManuaBt = 0;
  static unsigned long previousTimeEmergencyBt = 0;

  Serial.println(emergencyButtonClick);

  switch(currentState){
    case AUTO_OPENING:
      state = "AUTO";
      
      if(justOnceAuto){
        printLcd("Mode: ",state);
        justOnceAuto = false;
      }else if(angle <= MIN_ANGLE){ 
        justOnceAuto = true;
      }

      autoOpening(angle, distance);
      if(opentButtonClick){
        currentState = MANUAL_OPENING;
        justOnceAuto = true;
      }else if(emergencyButtonClick){
        currentState = EMERGENCY_STOP;
        emergencyButtonClick = false;
        justOnceAuto = true;
      }
      break;
    case MANUAL_OPENING:

      state = "MANUAL";

      if(justOnceManual){
        printLcd("Mode: ",state);
        justOnceManual = false;
      }
      manualOpening(angle);
      if(emergencyButtonClick){
        currentState = EMERGENCY_STOP;
        emergencyButtonClick = false;
        justOnceManual = true;
      }else if(manualOpeninigFinished){
        opentButtonClick = false;
        currentState = AUTO_OPENING;
        justOnceManual = true;
        manualOpeninigFinished = false;
      }
      break;
    case EMERGENCY_STOP:
      state = "EMERGECY STOP";
      printLcd(state,"Position: " + String(angle));

      if(emergencyButtonClick){
        servo.attach(SERVO_PIN);
        angle = MIN_ANGLE;
        doorState = DOOR_CLOSED;
        servo.write(angle);
        currentState = AUTO_OPENING;
        justOnceManual = true;
        justOnceAuto = true;
        emergencyButtonClick = false;
      }else{
        servo.detach();
      }
        
      break;
  }
}

void setup() {
  
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  servo.attach(SERVO_PIN);
  servo.write(MIN_ANGLE);

  openBtn.attachClick(manualOpenClick);
  emergencyBtn.attachClick(emergencyClick);
}

void loop() {
  openBtn.tick();
  emergencyBtn.tick();
  unsigned long currentTime = millis();
  static int angle = MIN_ANGLE;
  float distance = getDistance();

  mainFunction(angle, distance);
    
}
