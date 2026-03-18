
#ifndef DEFINES_H
#define DEFINES_H

// definidos en fichero user_setup.h de la libreira TFT_eSPI
// para PLACA EXPERIMENTAL V1
//#define TFT_MISO   19
//#define TFT_MOSI   23
//#define TFT_SCLK   18
//#define TFT_CS     15
//#define TFT_DC      2
//#define TFT_RST     4    // Set TFT_RST to -1 if the display RESET is connected to RST or 3.3V
//#define TOUCH_CS    5


const int WIFI_TRIGGER_PIN = 36;    
const int pin_radio_control = 32;   // conector 9
const int pin_US1_receptor = 34;
const int pin_US1_emisor = 33; 
const int pin_led_7colores = 27;
const int pin_radar = 16;
const int pin_SDA = 21;
const int pin_SCL = 22;
const int pin_DHT11 = 13;  
const int pin_led_izquierda = 25;  
const int pin_led_derecha = 26;  
const int pin_choque = 17;  

//const IPAddress WIFI_IP(192,168,11,91);
//const IPAddress WIFI_GW(192,168,11,1);
//const IPAddress WIFI_MASK(255,255,255,0);


const int PORT_WEBSERVER = 80;
const int PORT_WEBSOCKET = 81;
constexpr char password_OTA[] = "1892";  // Mejor con array de caracteres

const int i2caddress_MCP23017 = 0x20;
const int i2caddress_VL53L0 = 0x29;
const int i2caddress_PCA9685 = 0x40;
const int i2caddress_giroscopio = 0x50;

// parámetros tareas
const int periodo_task_DHT11 = 10000;
const int periodo_task_radarhumano = 1000;
const int periodo_task_ultrasonidos = 500;
const int periodo_task_radiocontrol = 10;
const int periodo_task_giroscopio = 500;
const int periodo_task_websockets = 10;
const int periodo_task_servomotores = 20;
const int periodo_task_motores = 100;
const int periodo_task_radar = 100; //10;

const int prioridad_task_DHT = 1;           // a mayor número, más prioridad de la tarea
const int prioridad_task_radarhumano = 1;
const int prioridad_task_ultrasonidos= 2;
const int prioridad_task_radiocontrol = 2;
const int prioridad_task_giroscopio = 2; //3;
const int prioridad_task_websockets = 3;
const int prioridad_task_servomotores = 2;
const int prioridad_task_motores = 3;
const int prioridad_task_radar = 2;

const int tamano_task_DHT = 2048;
const int tamano_task_radarhumano = 2048; //1024;
const int tamano_task_ultrasonidos= 1024; //2048;
const int tamano_task_radiocontrol = 2048; //2048;
const int tamano_task_giroscopio = 4096; //2048;
const int tamano_task_websockets = 4096; //3000;
const int tamano_task_servomotores = 4096; //2048;
const int tamano_task_motores = 4096; //3000;
const int tamano_task_radar = 4096; //2048;

extern int enabled_task_DHT;
extern int enabled_task_radarhumano;
extern int enabled_task_ultrasonidos;
extern int enabled_task_radiocontrol;
extern int enabled_task_giroscopio; 
extern int enabled_task_websockets; 
extern int enabled_task_servomotores;
extern int enabled_task_motores; 
extern int enabled_task_radar;

extern int modo_conex;   // 0=STA+AP+RC,  1= AP+RC,  2,  RC solamente

#endif