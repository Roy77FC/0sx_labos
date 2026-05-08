#include "matrice8x8.h"

Matrice8x8::Matrice8x8(uint8_t clk, uint8_t din, uint8_t cs)
  :u8g2(U8G2_R1, clk, din, cs, U8X8_PIN_NONE, U8X8_PIN_NONE)
  {}

void Matrice8x8::begin(){
  this->u8g2.begin();
  this->u8g2.setContrast(5);
  this->u8g2.setFont(u8g2_font_4x6_tr);
  this->u8g2.setDisplayRotation(U8G2_R0);
  this->u8g2.clearBuffer(); 
  this->u8g2.sendBuffer(); 
}

void Matrice8x8::dessiner_message_a_position(const char *texte, uint16_t position_bits, const int LARGEUR_POLICE){
  int TAILLE_TAMPON = 12;
  char tampon[TAILLE_TAMPON];               // Tampon pour contenir une portion du texte
  uint16_t debut_caractere = position_bits / LARGEUR_POLICE; // Index du caractère de départ
  uint16_t i;

  // Copier une portion du texte dans le tampon
  for (i = 0; i < TAILLE_TAMPON - 1; i++) {
    tampon[i] = texte[debut_caractere + i];
    if (texte[debut_caractere + i] == '\0')
      break;
  }

  tampon[TAILLE_TAMPON - 1] = '\0'; // S'assurer que le tampon est bien terminé

  // Afficher le texte en tenant compte du décalage de bits (scroll horizontal)
  this->u8g2.drawStr(-(position_bits & (LARGEUR_POLICE - 1)), 7, tampon);
}

void Matrice8x8::displayBitMap(const uint8_t bitMap[], const int delay){
  static unsigned long previousTime = 0;
  
  if(millis() - previousTime >= delay){
    previousTime = millis();
    this->u8g2.clearBuffer();
    this->u8g2.drawBitmap(0,  0, 1, 8, bitMap);
    this->u8g2.sendBuffer();
  }
}

void Matrice8x8::blinkAllLeds(){

  static unsigned long previousTime = 0;
  static bool flag = false;
  const int waitTime = 200;

  if(millis() - previousTime >= waitTime){
    previousTime = millis();
    if(flag){
      this->u8g2.clearBuffer();
      this->u8g2.drawBox(0, 0, 8, 8);
      this->u8g2.sendBuffer();
      
    }else{
      this->u8g2.clearBuffer();
      this->u8g2.sendBuffer();
      
    }
    flag = !flag;
  }
}

void Matrice8x8::clear(){
  this->u8g2.clearBuffer();
}

void Matrice8x8::send(){
  this->u8g2.sendBuffer();
}
