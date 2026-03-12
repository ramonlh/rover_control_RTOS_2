
#include "pantalla.h"

TFT_eSPI tft = TFT_eSPI();

void init_pantalla()
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.println("Hola");
  tft.setTextSize(2);
  tft.println("Pantalla iniciada OK");
}