
#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

void init_pantalla()
{
  // Pantalla TFT
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
