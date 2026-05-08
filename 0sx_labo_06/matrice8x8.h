#pragma once
#include <U8g2lib.h>

class Matrice8x8 {
private:
  U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2;

public:
  Matrice8x8(uint8_t clk, uint8_t din, uint8_t cs);

  void begin();
  void displayBitMap(const uint8_t bitMap[], const int delay);
  void blinkAllLeds();
  void dessiner_message_a_position(const char *texte, uint16_t position_bits, const int LARGEUR_POLICE);
  void clear();
  void send();
};