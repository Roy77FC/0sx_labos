const int DELS[4] = { 7, 8, 9, 10 };                      // Roynel Fonseca  
const int DELS_SIZE = sizeof(DELS) / sizeof(DELS[0]);     // Nro étudiant : 2510948 
const int BOTON_PIN = 2;
const int POT_PIN = A1;
unsigned long potValue = 0;
static int botonState = HIGH;
String percent; 


void setup() {
  Serial.begin(9600);

  pinMode(BOTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < DELS_SIZE; i++) {
    pinMode(DELS[i], OUTPUT);
  }
}

void lightUpDels(unsigned long potValue){
  static int STATES_DELS[4];

  if(potValue <= 5){
    STATES_DELS[0] = HIGH;
    STATES_DELS[1] = LOW;
    STATES_DELS[2] = LOW;
    STATES_DELS[3] = LOW;
  }
  else if(potValue <= 10){
    STATES_DELS[0] = HIGH;
    STATES_DELS[1] = HIGH;
    STATES_DELS[2] = LOW;
    STATES_DELS[3] = LOW;
  }
  else if(potValue <= 15){
    STATES_DELS[0] = HIGH;
    STATES_DELS[1] = HIGH;
    STATES_DELS[2] = HIGH;
    STATES_DELS[3] = LOW;
  }
  else{
    STATES_DELS[0] = HIGH;
    STATES_DELS[1] = HIGH;
    STATES_DELS[2] = HIGH;
    STATES_DELS[3] = HIGH;
  }

  digitalWrite(DELS[0], STATES_DELS[0]);
  digitalWrite(DELS[1], STATES_DELS[1]);
  digitalWrite(DELS[2], STATES_DELS[2]);
  digitalWrite(DELS[3], STATES_DELS[3]);
}

void displayPercent(unsigned long potValue, int botonState){

  static int previousState = HIGH;
  
  if(potValue <= 5){
    percent = "[!!!!!...............]   25%";
  }
  else if(potValue <= 10){
    percent = "[!!!!!!!!!!..........]   50%";
  }
  else if(potValue <= 15){
    percent = "[!!!!!!!!!!!!!!!.....]   75%";
  }
  else{
    percent = "[!!!!!!!!!!!!!!!!!!!!]   100%";
  }
  
  if(previousState != botonState){
    Serial.println(percent);
  }
  
  previousState = botonState;
  
}

void loop() {
  // 1. Lit la valeur du potentiomètre et la convertit en une échelle de 0 à 20

  potValue = analogRead(POT_PIN);

  potValue = map(potValue, 0, 1023, 0, 20);

  // Lire la valeur du bouton

  botonState = digitalRead(BOTON_PIN);


  lightUpDels(potValue); // Allumer les leds

  displayPercent(potValue, botonState); // afficher la progression de l'état des leds

}
