# Rover Control RTOS 2

Proyecto de control para un **rover basado en ESP32** con arquitectura modular y multitarea usando **FreeRTOS**.  
El sistema integra conectividad Wi‑Fi, servidor web, WebSocket en tiempo real, OTA, FTP, control de motores y servos, y varios sensores para navegación, telemetría y operación remota.

> Este README se ha redactado a partir del sketch principal `rover_control_RTOS_2.ino`. El proyecto depende además de varios módulos `.h/.cpp` no incluidos aquí, por lo que algunos detalles pueden requerir ajuste según tu implementación final.

---

## Características principales

- Control de rover con **4 motores**.
- Control de **servomotores**.
- Arquitectura **multitarea** con FreeRTOS.
- Modos de conexión **STA + AP**, **AP**, o **solo radiocontrol**.
- **Servidor web** para control y monitorización.
- Comunicación en tiempo real mediante **WebSocket**.
- Actualización remota por **OTA**.
- Acceso a archivos mediante **FTP**.
- Gestión de sensores y periféricos:
  - **DHT11** (temperatura/humedad).
  - **VL53L0X** (distancia).
  - **giroscopio**.
  - **radar de presencia humana**.
  - sensor de **choque**.
- Expansión por **I2C** mediante:
  - **MCP23017**.
  - **PCA9685**.
- Gestión de flash / iluminación.
- Soporte de **radiocontrol**.

---

## Arquitectura del proyecto

El sketch principal inicializa distintos subsistemas y crea tareas separadas para distribuir la carga del sistema. Según el código principal, el proyecto se apoya en módulos como:

- `wifi_connect.h`
- `ota.h`
- `sistema_ficheros.h`
- `giroscopio.h`
- `servomotores.h`
- `radar_vl53l0x.h`
- `4motores.h`
- `fecha_hora.h`
- `dht11.h`
- `servidor_web.h`
- `servidor_websocket.h`
- `servidor_ftp.h`
- `radio_control.h`
- `radar_humano.h`
- `mux_mcp23017.h`
- `mux_servos_pca9685.h`
- `flash_manager.h`

La lógica principal divide las tareas en dos grandes grupos:

### Tareas que no usan I2C

- gestión de flash
- lectura DHT11
- radar de presencia humana
- radiocontrol
- servidor WebSocket

### Tareas que usan I2C

- inicialización del bus I2C
- expansor **MCP23017**
- controlador de servos **PCA9685**
- servomotores
- motores
- radar de distancia
- giroscopio

Esta separación ayuda a evitar conflictos de acceso concurrente al bus I2C mediante el uso de un **mutex**:

```cpp
SemaphoreHandle_t i2cMutex;
```

---

## Modos de funcionamiento

El sistema contempla varios modos definidos por `modo_conex`:

### Modo 0 — `STA + AP + RC`

- conexión Wi‑Fi
- sistema de ficheros
- servidor web
- servidor FTP
- WebSocket
- OTA
- radiocontrol

### Modo 1 — `AP + RC`

- punto de acceso propio
- sistema de ficheros
- servidor web
- servidor FTP
- WebSocket
- OTA
- radiocontrol

### Modo 2 — `RC`

- funcionamiento centrado en radiocontrol, sin servicios de red principales

---

## Tecnologías usadas

- **ESP32**
- **Arduino framework**
- **FreeRTOS**
- **WiFi.h**
- **ESPmDNS**
- **ArduinoOTA**
- **WebServer**
- **WebSocketsServer**
- **I2C / Wire**

---

## Requisitos de hardware

Según el código principal, este proyecto está pensado para una plataforma con:

- 1 × **ESP32**
- driver o etapa de potencia para **4 motores**
- **servos**
- módulo **PCA9685**
- expansor **MCP23017**
- sensor **VL53L0X**
- sensor **DHT11**
- **giroscopio**
- sensor de **presencia humana / radar**
- sensor de **choque**
- LEDs / flash
- sistema de alimentación adecuado para tracción y lógica

---

## Compilación

Este proyecto está pensado para compilarse desde el **IDE de Arduino** o un entorno compatible con ESP32.

### Requisitos de software

- Core de **ESP32 para Arduino** instalado.
- Librerías necesarias para los módulos usados en el proyecto.
- Todos los archivos `.h` y `.cpp` del repositorio en la misma carpeta del proyecto o estructurados correctamente.

### Pasos básicos

1. Instala el soporte de placas ESP32 en Arduino IDE.
2. Copia todos los módulos del proyecto en la carpeta del sketch.
3. Revisa `defines.h` para comprobar:
   - pines
   - credenciales Wi‑Fi
   - modo de conexión
   - tamaños y prioridades de tareas
4. Selecciona tu placa ESP32.
5. Compila y sube el firmware.

---

## Ejecución

Durante el arranque, el firmware:

1. Inicializa serie.
2. Crea el mutex de I2C.
3. Habilita o deshabilita tareas.
4. Arranca conectividad y servicios de red según el modo configurado.
5. Configura pines de LEDs y sensor de choque.
6. Inicializa subsistemas no I2C.
7. Inicializa bus I2C, MCP23017 y PCA9685.
8. Lanza tareas de motores, servos, radar y giroscopio.

En el `loop()` principal se atienden:

- servicio de flash
- peticiones del servidor web
- OTA
- FTP
- control de obstáculos

---

## Estructura orientativa del repositorio

```text
rover_control_RTOS_2/
├── rover_control_RTOS_2.ino
├── defines.h
├── wifi_connect.h / .cpp
├── ota.h / .cpp
├── sistema_ficheros.h / .cpp
├── giroscopio.h / .cpp
├── servomotores.h / .cpp
├── radar_vl53l0x.h / .cpp
├── 4motores.h / .cpp
├── fecha_hora.h / .cpp
├── dht11.h / .cpp
├── servidor_web.h / .cpp
├── servidor_websocket.h / .cpp
├── servidor_ftp.h / .cpp
├── radio_control.h / .cpp
├── radar_humano.h / .cpp
├── mux_mcp23017.h / .cpp
├── mux_servos_pca9685.h / .cpp
└── flash_manager.h / .cpp
```

---

## Puntos a tener en cuenta

- El proyecto usa **multitarea**, así que conviene revisar cuidadosamente:
  - prioridades
  - tamaños de pila
  - acceso concurrente a periféricos
- El uso de I2C compartido requiere respetar el mutex.
- El servidor FTP aparece inicializado con credenciales fijas en el sketch principal:

```cpp
init_ftp_server("admin", "12341234")
```

Para un entorno real conviene **cambiar credenciales**, endurecer seguridad o desactivar FTP si no se necesita.

---

## Mejoras futuras

- documentación de pines y esquema eléctrico
- mapa de tareas y prioridades
- integración de telemetría avanzada
- logs más detallados por módulo
- configuración centralizada desde interfaz web
- autenticación más robusta para servicios de red
- pruebas por módulo y validación de hardware

---

## Estado del proyecto

Proyecto en desarrollo / integración de subsistemas.  
Muy orientado a experimentación, robótica móvil y control remoto con ESP32.

---

## Capturas / fotos

Puedes añadir aquí imágenes del rover, de la interfaz web o del montaje:

```md
![Rover](docs/img/rover.jpg)
![Interfaz web](docs/img/web-ui.png)
```

---

## Licencia

Añade aquí la licencia que quieras usar, por ejemplo:

- MIT
- GPLv3
- Apache 2.0
- Uso personal / no comercial

Ejemplo:

```text
Este proyecto se distribuye bajo licencia MIT.
```

---

## Autor

**Ramón Lorenzo**

Si quieres, puedes completar esta sección con tu GitHub, documentación del hardware y capturas del sistema funcionando.
