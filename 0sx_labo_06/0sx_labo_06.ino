#include <Arduino.h>
#include <IRremote.h>
#include <OneButton.h>


//functions cpp

#include "matrice8x8.h"
#include "LCDI2C.h"

// Matrice led pins
#define CLK_PIN 30
#define DIN_PIN 34
#define CS_PIN  32  

// IR reciver pin
#define IR_PIN  8

// button pin
#define PIN_BUTTON 2
//---------------------------

enum ScreenState {DISCOUNT, NORMAL, ERROR, CLOSE, EMERGENCY};
ScreenState screenState = CLOSE;

//---------------------------

LcdScreen lcd(0x27, 16, 2);
OneButton emergencyButton(PIN_BUTTON, true);
Matrice8x8 u8g2(CLK_PIN, DIN_PIN, CS_PIN);

int irVal = -1;
int SerialVal = -1;
int lastIrVal = irVal;
int lastSerialVal = SerialVal;

const int DISCOUNT_IR_VAL = 12;
const int NORMAL_IR_VAL = 24;
const int ERROR_IR_VAL = 94;
const int CLOSE_IR_VAL = 8;

const int DISCOUNT_SERIAL_VAL = 49;
const int NORMAL_SERIAL_VAL = 50;
const int ERROR_SERIAL_VAL = 51;
const int CLOSE_SERIAL_VAL = 52;

const int printDelay = 100;



bool emergencyButtonClick = false;

void emergencyClick(){
  emergencyButtonClick = true;
}

const uint8_t bitMapSmiley[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

const uint8_t bitMapEmerncy[8] = {
  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B10000001
};

#define NOMBRE_MATRICES 1  
#define LARGEUR_POLICE 4 

// Variables pour contrôler le défilement du texte
uint16_t position_bits = 0;  // Position actuelle du texte en bits
uint16_t longueur_bits;      // Longueur totale du texte en bits

// Texte à faire défiler sur l'afficheur
char *message = " SPECIAL";

// Largeur visible en pixels = nombre de matrices * 8
const uint8_t largeur_affichage = NOMBRE_MATRICES * 8;


int getIrVal(){

  int value = -1;

  if(IrReceiver.decode()){
    value = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
  }

  return value;
}

void getSerialVal(int& value){

  if(Serial.available() > 0){
    int command = Serial.read();

    value = command != 10 ? command : value;
  }
}

void stateManagement(const int& value, const int& case1, const int& case2, const int& case3, const int& case4){
  if(value == case1){
    screenState = DISCOUNT;
  }else if(value == case2){
    screenState = NORMAL;
  }else if(value == case3){
    screenState = ERROR;
  }else if(value == case4){
    screenState = CLOSE;
  }
}

void swichtToEmergencyState(){
  if(emergencyButtonClick){

    if(screenState != EMERGENCY){
      screenState = EMERGENCY;
    }else{
      screenState = CLOSE;
    }

    emergencyButtonClick = false;
  }
}

void mainFunction(){

  String strToDisplay = "";

  switch(screenState){
    case DISCOUNT:

      static unsigned long previousTime = 0;

      if(millis() - previousTime >= printDelay){

        previousTime = millis();

        u8g2.clear();                               
        u8g2.dessiner_message_a_position(message, position_bits, LARGEUR_POLICE);   
        u8g2.send();

        position_bits++;                                       

        if (position_bits >= longueur_bits) {
          position_bits = 0;                                   
        }
      }

      strToDisplay = "Rabais";
      break;

    case NORMAL:
      u8g2.displayBitMap(bitMapSmiley, printDelay);
      swichtToEmergencyState();

      strToDisplay = "Normal";
      break;
    
    case ERROR:
      u8g2.blinkAllLeds();
      strToDisplay = "Erreur";
      break;
    
    case CLOSE:
      u8g2.clear();    
      u8g2.send();
      strToDisplay = "Fermer";
      break;

    case EMERGENCY:
      u8g2.displayBitMap(bitMapEmerncy, printDelay);
      strToDisplay = "Urgence";
      break;
  }

  lcd.printLcd(strToDisplay);
}

void setup() {
  
  Serial.begin(9600);
  u8g2.begin();
  IrReceiver.begin(IR_PIN);
  lcd.begin();

  longueur_bits = strlen(message) * LARGEUR_POLICE + largeur_affichage;

  emergencyButton.attachClick(emergencyClick);

}

void loop() {

  emergencyButton.tick();

  if(screenState != EMERGENCY){
    irVal = getIrVal();
    getSerialVal(SerialVal);
    
    if(irVal != lastIrVal){

      lastIrVal = irVal;
      stateManagement(irVal, DISCOUNT_IR_VAL, NORMAL_IR_VAL, ERROR_IR_VAL, CLOSE_IR_VAL);

    }else if(SerialVal != lastSerialVal){
      lastSerialVal = SerialVal;
      stateManagement(SerialVal, DISCOUNT_SERIAL_VAL, NORMAL_SERIAL_VAL, ERROR_SERIAL_VAL, CLOSE_SERIAL_VAL);
    }
  }

  
  swichtToEmergencyState();
  mainFunction();
  
}