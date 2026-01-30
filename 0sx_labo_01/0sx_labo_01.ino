const int DEL = 13;
String etatDel[3] = {"Allume", "Varie", "Clignote"};;
int luminosite = 0;
int degradement = 5;
int position = 0;

void setup() {
  pinMode(DEL, OUTPUT);
  Serial.begin(9600);
}

void allumerEteindre(){
  digitalWrite(DEL, LOW);
  delay(300);                      
  digitalWrite(DEL, HIGH);   
  Serial.println("message :"+ etatDel[position] +"-2510948");
  delay(2000);                    
  digitalWrite(DEL, LOW);  
  delay(1000);  
}

void varierEtats(){
  float dureeTotale = 2048;
  const int tempPause = 30;

  for(int i = 0; i < dureeTotale/tempPause; i++){
    analogWrite(DEL, luminosite);

    luminosite = luminosite + degradement;

    if(luminosite <= 0 || luminosite >= 255){
      degradement = -degradement;
    }
    delay(tempPause);
  }
  Serial.println("message :"+ etatDel[position] +"-2510948");
}

void clignoter(){

  // NO Étudiant 2510948

  for(int i = 0; i < 2; i++){
    digitalWrite(DEL, HIGH);
    delay(350);
    digitalWrite(DEL,LOW);
    delay(350);
  }
  Serial.println("message :"+ etatDel[position] +"-2510948");
}

void loop() {

  switch(position){
    case 0: 
      // État allumé éteint
      allumerEteindre();
      position++; 
      break;
    case 1:
       //État variation
      varierEtats();
      position++;
      break;
    case 2:
      //État clignotement
      clignoter();
      position = 0;
      break;
  }
}