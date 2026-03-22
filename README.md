# Rover Control RTOS 2

Control project for an **ESP32-based rover** with a modular, multitasking architecture built on **FreeRTOS**.  
The system integrates Wi‑Fi connectivity, a web server, real-time WebSocket communication, OTA updates, FTP access, motor and servo control, and multiple sensors for navigation, telemetry, and remote operation.

> This README has been drafted from the main sketch `rover_control_RTOS_2.ino`. The project also depends on several additional `.h/.cpp` modules not included here, so some details may need adjustment based on your final implementation.

---

## Main Features

- **4-motor** rover control
- **Servo motor** control
- **Multitasking** architecture using FreeRTOS
- Connection modes: **STA + AP**, **AP**, or **radio-control only**
- **Web server** for control and monitoring
- Real-time communication through **WebSocket**
- Remote firmware updates via **OTA**
- File access through **FTP**
- Sensor and peripheral support:
  - **DHT11** (temperature / humidity)
  - **VL53L0X** (distance)
  - **gyroscope**
  - **human presence radar**
  - **collision** sensor
- **I2C** expansion via:
  - **MCP23017**
  - **PCA9685**
- Flash / lighting management
- **Radio control** support

---

## Project Architecture

The main sketch initializes different subsystems and creates separate tasks to distribute the workload across the system. Based on the main code, the project relies on modules such as:

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

The main logic splits tasks into two major groups:

### Tasks that do not use I2C

- flash management
- DHT11 reading
- human presence radar
- radio control
- WebSocket server

### Tasks that use I2C

- I2C bus initialization
- **MCP23017** expander
- **PCA9685** servo controller
- servos
- motors
- distance radar
- gyroscope

This separation helps prevent concurrent access conflicts on the I2C bus through the use of a **mutex**:

```cpp
SemaphoreHandle_t i2cMutex;
```

---

## Operating Modes

The system supports several modes defined by `modo_conex`:

### Mode 0 — `STA + AP + RC`

- Wi‑Fi connection
- file system
- web server
- FTP server
- WebSocket
- OTA
- radio control

### Mode 1 — `AP + RC`

- own access point
- file system
- web server
- FTP server
- WebSocket
- OTA
- radio control

### Mode 2 — `RC`

- radio-control-focused operation without the main network services

---

## Technologies Used

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

## Hardware Requirements

According to the main code, this project is designed for a platform with:

- 1 × **ESP32**
- driver or power stage for **4 motors**
- **servos**
- **PCA9685** module
- **MCP23017** expander
- **VL53L0X** sensor
- **DHT11** sensor
- **gyroscope**
- **human presence / radar** sensor
- **collision** sensor
- LEDs / flash
- suitable power supply for both traction and logic

---

## Build Instructions

This project is intended to be compiled from the **Arduino IDE** or a compatible ESP32 development environment.

### Software Requirements

- **ESP32 Arduino core** installed
- Required libraries for the modules used in the project
- All `.h` and `.cpp` files from the repository placed in the same sketch folder or correctly organized

### Basic Steps

1. Install ESP32 board support in Arduino IDE.
2. Copy all project modules into the sketch folder.
3. Review `defines.h` and check:
   - pins
   - Wi‑Fi credentials
   - connection mode
   - task sizes and priorities
4. Select your ESP32 board.
5. Compile and upload the firmware.

---

## Runtime Behavior

During startup, the firmware:

1. Initializes serial output.
2. Creates the I2C mutex.
3. Enables or disables tasks.
4. Starts connectivity and network services depending on the configured mode.
5. Configures LED pins and the collision sensor.
6. Initializes non-I2C subsystems.
7. Initializes the I2C bus, MCP23017, and PCA9685.
8. Launches motor, servo, radar, and gyroscope tasks.

In the main `loop()`, the following are handled:

- flash service
- web server requests
- OTA
- FTP
- obstacle control

---

## Suggested Repository Structure

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

## Things to Keep in Mind

- The project uses **multitasking**, so it is important to review:
  - priorities
  - stack sizes
  - concurrent access to peripherals
- Shared I2C usage requires respecting the mutex.
- The FTP server appears to be initialized with hardcoded credentials in the main sketch:

```cpp
init_ftp_server("admin", "12341234")
```

For a real deployment, it is strongly recommended to **change credentials**, harden security, or disable FTP if it is not needed.

---

## Future Improvements

- pinout and wiring documentation
- task and priority map
- advanced telemetry integration
- more detailed logs per module
- centralized configuration from the web interface
- stronger authentication for network services
- per-module testing and hardware validation

---

## Project Status

Project under development / subsystem integration.  
Highly oriented toward experimentation, mobile robotics, and ESP32-based remote control.

---

## Screenshots / Photos

You can add rover images, web UI screenshots, or assembly photos here:

```md
![Rover](docs/img/rover.jpg)
![Web interface](docs/img/web-ui.png)
```

---

## License

Add the license you want to use here, for example:

- MIT
- GPLv3
- Apache 2.0
- Personal / non-commercial use

Example:

```text
This project is distributed under the MIT License.
```

---

## Author

**Ramón Lorenzo**

You can expand this section with your GitHub profile, hardware documentation, and screenshots of the system in operation.
