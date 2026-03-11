

#define DEBUG

// librerías genéricas
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// includes del proyecto
#include "defines.h"
#include <wifi_connect.h>  
#include "ota.h"

#include <sistema_ficheros.h>  
#include "esp_task_wdt.h"
SemaphoreHandle_t i2cMutex;

#include "giroscopio.h"
#include "servomotores.h"
#include "radar_vl53l0x.h"
#include "4motores.h"
#include "fecha_hora.h"
#include "sensor_ultrasonidos.h"
#include "dht11.h" 
#include "servidor_web.h"
#include "servidor_websocket.h"
#include "mqtt.h"

#include "servidor_ftp.h"
#include "radio_control.h"
#include "radar_humano.h"
#include "pantalla.h"

#define OFF LOW
#define ON HIGH

void test_I2C()
{
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning for I2C devices ...");
  for (address = 0x01; address < 0x7f; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
      } 
    else if (error != 2) 
      {
      Serial.printf("Error %d at address 0x%02X\n", error, address);
      }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
    }
  else
    {
    Serial.print(nDevices); Serial.println(" devices found");
    }
}

void setup() {
  Serial.begin(115200);
  // init conexiones y servidores
  delay(1000);
  ini_sistema_ficheros();

  i2cMutex = xSemaphoreCreateMutex();

  if (modo_conex == 0)
    {
    Serial.println("Modo: 0 STA+AP+RC");
    init_wifi();
    init_webserver();
    init_ftp_server();  
    xTaskCreate(task_websockets, "Task WS", tamano_task_websockets, NULL, prioridad_task_websockets, NULL); // comprobado 2048
    }
  else if (modo_conex == 1)
    {
    Serial.println("Modo: 1 AP+RC");
    init_wifi();
    init_webserver();
    init_ftp_server();    
    xTaskCreate(task_websockets, "Task WS", tamano_task_websockets, NULL, prioridad_task_websockets, NULL); // comprobado 2048
    }
  else
    {
    Serial.println("Modo: 2 RC");
    }
  pinMode(pin_led_derecha, OUTPUT);
  pinMode(pin_led_izquierda, OUTPUT);
  pinMode(pin_led_7colores, OUTPUT);
  pinMode(pin_choque, INPUT_PULLUP);

  digitalWrite(pin_led_7colores, HIGH);
  delay(2000);
  digitalWrite(pin_led_7colores, LOW);
  //if (modo_conex==0) init_mqtt();
  //init_pantalla();

  // tareas que NO usan I2C
  pinMode(13, OUTPUT);
  xTaskCreate(task_dht11, "Task DHT11", tamano_task_DHT, NULL, prioridad_task_DHT, NULL); // comprobado 1024
  xTaskCreate(task_radarhumano, "Task Radar Humano", tamano_task_radarhumano, NULL, prioridad_task_radar, NULL); // comprobado 1024
  xTaskCreate(task_radiocontrol, "Task RC", tamano_task_radiocontrol, NULL, prioridad_task_radiocontrol, NULL); // comprobado 2048
  //xTaskCreate(task_ultrasonidos, "Task UltraS", tamano_task_ultrasonidos, NULL, prioridad_task_ultrasonidos, NULL); // comprobado 1024

  // tareas que usan I2C
  // init MCP23017 y PCA9685
  Wire.begin(pin_SDA, pin_SCL);
  //Wire.setClock(100000);
  //test_I2C();
  //delay(1000);
  Serial.print("MCP23017 "); Serial.println(init_MCP23017()==0?"OK":"ERROR");
  Serial.print("PCA9685 "); Serial.println(init_PCA9685(1600)!=0?"OK":"ERROR");
  xTaskCreate(task_servomotores, "Task Servos", tamano_task_servomotores, NULL, prioridad_task_servomotores, NULL);
  xTaskCreate(task_motores, "Task motores", tamano_task_motores, NULL, prioridad_task_motores, NULL); // 
  xTaskCreate(task_radar, "Task Radar", tamano_task_radar, NULL, prioridad_task_radar, NULL); 
  xTaskCreate(task_giroscopio, "Task Giros", tamano_task_giroscopio, NULL, prioridad_task_giroscopio, NULL);
  
  enabled_task_websockets = 1; 

  enabled_task_DHT = 1;
  enabled_task_radarhumano = 1; 
  enabled_task_radiocontrol = 1;
  enabled_task_ultrasonidos= 0;
  
  enabled_task_servomotores = 1;
  enabled_task_motores = 1; 
  enabled_task_giroscopio = 1; 
  enabled_task_radar = 1;

}

void loop() {
  // Manejo de conexiones y servidores  
  if (modo_conex == 0)
    {
    webSocket.loop();
    webserver.handleClient();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();
    }

  else if (modo_conex == 1)
    {
    webSocket.loop();
    webserver.handleClient();
    ArduinoOTA.handle();
    ftpSrv.handleFTP();
    }
  else if (modo_conex == 2)
    {

    }
  //if (modo_conex==0)    loop_mqtt();
  //Manejo de sensores y actuadores
  handle_obstaculos();   // mira la distancia y controla si hay que parar el coche
  //if ((rumbo_adelante==1) || (rumbo_adelante==2))
    //{
    //conservar_rumbo();
    //}
delay(1);
}