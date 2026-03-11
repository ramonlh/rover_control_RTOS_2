
WiFiClient tcpclient;

WiFiServer tcpserver(PORT_TCPSOCKET);

void init_tcpserver()
{
  tcpserver.begin();
  Serial.println("Servidor TCP iniciado.");  
}

void handle_tcpserver()
{
  tcpclient = tcpserver.available();
  if (tcpclient.connected()) {
    Serial.println("Cliente conectado.");
    while (tcpclient.connected()) 
      {
      if (tcpclient.available()) {
        String data = tcpclient.readStringUntil('\n');
        Serial.println("Recibido tcp: " + data);
        data.remove(data.length() - 1);
        // Confirmar al cliente
        //tcpclient.println("ACK: " + data);
        if (data == "adelante")
          {
          rumbo_adelante=1;
          rover_adelante();
          }
        else if (data == "atras")
          {
          //rumbo_adelante=2;
          rover_atras();
          }
        else if (data == "giroizda")
          {
          rumbo_adelante=0;
          rover_giro_izda();
          }        
        else if (data == "girodcha")
          {
          rumbo_adelante=0;
          rover_giro_dcha();
          }
        else if (data == "stop")
          {
          rumbo_adelante=0;
          rover_stop();
          }
        else if (data == "bajavelocidad")
          {
          set_speed_rover(rover_speed-500);
          }
        else if (data == "subevelocidad")
          {
          set_speed_rover(rover_speed+500);
          }
        }
      }
    tcpclient.stop();
    Serial.println("Cliente desconectado.");
   }
}

