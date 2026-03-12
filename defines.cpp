
#include <Arduino.h>

// definidos en fichero user_setup.h de la libreira TFT_eSPI
// para PLACA EXPERIMENTAL V1
//#define TFT_MISO   19
//#define TFT_MOSI   23
//#define TFT_SCLK   18
//#define TFT_CS     15
//#define TFT_DC      2
//#define TFT_RST     4    // Set TFT_RST to -1 if the display RESET is connected to RST or 3.3V
//#define TOUCH_CS    5

//const IPAddress WIFI_IP(192,168,11,91);
//const IPAddress WIFI_GW(192,168,11,1);
//const IPAddress WIFI_MASK(255,255,255,0);


int enabled_task_DHT = 0;
int enabled_task_radarhumano = 0; 
int enabled_task_ultrasonidos= 0;
int enabled_task_radiocontrol = 0;
int enabled_task_giroscopio = 0; 
int enabled_task_websockets = 0; 
int enabled_task_servomotores = 0;
int enabled_task_motores = 0; 
int enabled_task_radar = 0;

int modo_conex = 0;   // 0=STA+AP+RC,  1= AP+RC,  2,  RC solamente

