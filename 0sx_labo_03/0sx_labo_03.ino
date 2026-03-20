#include <LCD_I2C.h> // Roynel Fonseca
#include <string.h>

int THERM_PIN = A0;
int AXE_X_PIN = A1;
int AXE_Y_PIN = A2;
int BUTTON_PIN = 2;
int PIN_LED = 11;

LCD_I2C lcd(0x27, 16, 2);

uint8_t specialChar[8] = 
{
  0b10100,
  0b10100,
  0b11100,
  0b00111,
  0b00101,
  0b00111,
  0b00101,
  0b00111,
};

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);

  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, specialChar);
}

float getTemp(){
  int Vo; // Voltage à la sortie
  float R1 = 10000; // Résistance
  float logR2, R2, T, Tc, Tf;

  // Les coefficients A, B et C.
  float c1 = 1.129148e-03, c2 = 2.34125e-04, c3 = 8.76741e-08;

  Vo = analogRead(THERM_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  Tf = (Tc * 9.0)/ 5.0 + 32.0; 

  return Tc;
}

void displayLines(String line1, String line2){
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

int dispplayTherm(){
    // int temperature = (int)getTemp() + 0.5;
  float temperature = getTemp();
  static int coolingState = LOW;
  static String  coolVal = "";
  String line1 = "";
  String line2 = "";


  if(temperature > 20){
    coolingState = HIGH;
  }else if (temperature < 18){
    coolingState = LOW;
  }

  if(coolingState){
    line2 = "COOL: ON";
  }else{
    line2 = "COOL: OFF";
  }

  digitalWrite(PIN_LED, coolingState);

  line1 = String(temperature) + " C";
  

  displayLines(line1, line2);


  return coolingState;
}

void setSpeedAndDirection(int& xVal, int& yVal, unsigned long currentTime){

  xVal = analogRead(AXE_X_PIN);
  yVal = analogRead(AXE_Y_PIN);
  const int speedAdvancement = 1000;
  static unsigned long lastTime = 0;
  String direction;
  String line1 = "";
  String line2 = "";

  int xDeg = map(xVal, 0, 1023, -90, 95);
  // int ySpeed = map(yVal, 0, 1023, 0, 200);
  static int highVal = 0;

  if(yVal > 550){                                         // les valeurs 550 et 470 son estimées en fonction des « points morts » du joystick
    if(currentTime - lastTime >= speedAdvancement){
      lastTime = currentTime;
      if(highVal < 200) highVal++;
    }

  }else if(yVal < 470){
    if(currentTime - lastTime >= speedAdvancement){
      lastTime = currentTime;
      if(highVal > 0) highVal--;
    }
  }

  if(xDeg < 0){
    direction = "D";
  }else{
    direction = "G";
  }
  
  if(xDeg == 0){

    line1 = "ALT : " + String(highVal) + "m";
    line2 = "DIR : 0";
  }else{

    line1 = "ALT : " + String(highVal) + "m";
    line2 = "DIR : ";

    if(xDeg >= 90){             // l'affichage des degrées est inverse pour qu'il ait pus de sens avec le mouvement du joystick :)
      line2 += "-90";
    }else{
      line2 += String(xDeg * -1);
    }

    line2 += "(" + String(direction) + ")";
  }

  displayLines(line1, line2);
}

void displaySpeedAndDirection(unsigned long currentTime){
  int xVal, yVal;
  int sysVal;
  int currentButtonState = digitalRead(BUTTON_PIN);
  static int previousButtonState = 0;  
  static int displayCase = 0;
  static int previousTime = 0;

  if(currentButtonState == 0){
    previousButtonState = 1;
  }

  if(previousButtonState == 1 && currentButtonState == 1){
    if(displayCase){
      displayCase = 0;
    }else{
      displayCase = 1;
    }
    previousButtonState = 0;
  } 

  switch(displayCase){
    case 0:
      sysVal = dispplayTherm();
      break;
    case 1:
      setSpeedAndDirection(xVal, yVal, currentTime);
      break;
  }

  // Transmission sur le port série 

  const int displayTime = 100;

  if(currentTime - previousTime >= displayTime){
    previousTime = currentTime;
    Serial.print("etd:2510948,x:");
    Serial.print(xVal);
    Serial.print(",y:");
    Serial.print(yVal);
    Serial.print(",sys:");
    Serial.println(sysVal);
  }
}

void loop() {
  unsigned long currentTime = millis();
  String nettoyer = " ";
  if(currentTime < 3000){
    lcd.setCursor(0,0);
    lcd.print("Fonseca Cabarcas");
    lcd.setCursor(0,1);
    lcd.write(0);
    lcd.rightToLeft();
    lcd.setCursor(16,1);
    lcd.print("84*****");
  }else{   
    displaySpeedAndDirection(currentTime);
  }
}