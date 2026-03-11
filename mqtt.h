

#include <PubSubClient.h>

// Dirección del servidor MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;  // Puerto por defecto de MQTT

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastTime = 0;  // Variable para almacenar el tiempo de la última publicación
unsigned long timerDelay = 60000;  // Intervalo de tiempo en milisegundos (60,000 ms = 1 minuto)

void send_topic_ip()
{
  if (!client.connected()) {
    client.connect("ESP32Client");
    }
  String localip = WiFi.localIP().toString();
  if (client.publish("fa/ips/iproverdiego", "x.x.x.x")) // Enviar la IP al servidor MQTT
//  if (client.publish("fa/ips/iproverdiego", localip.c_str())) // Enviar la IP al servidor MQTT
    Serial.println("mensaje enviado: " + String(WiFi.localIP().toString().c_str()));
  else
    Serial.println("mensaje NO enviado");
}

void init_mqtt()
{
 // Configurar el servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  // Intentar conectar al broker MQTT
  if (client.connect("ESP32Client")) {
    Serial.println(F("Conectado al servidor MQTT"));
    send_topic_ip();
    } 
  else {
    Serial.println(F("No se pudo conectar al servidor MQTT"));
    }
}

void loop_mqtt() {
  client.loop();
  // Comprobar si ha pasado un minuto (60,000 ms)
  if (millis() - lastTime > timerDelay) {
    lastTime = millis();  // Actualizar el tiempo de la última publicación

    // Obtener la IP local si está conectado
    String localIP = WiFi.localIP().toString();
    send_topic_ip(); 
    }  
}