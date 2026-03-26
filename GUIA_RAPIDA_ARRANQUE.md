# Rover Control RTOS 2 - Guía rápida de arranque

## 1. Antes de encender

- Comprueba batería, alimentación de lógica y motores.
- Asegúrate de que las ruedas giran libres y de que la zona delante del rover está despejada.
- Ten preparado el método de control: navegador web o mando RF.
- Tras cambios de firmware, conviene hacer la primera prueba con las ruedas en el aire.

## 2. Conexión Wi‑Fi y URL de acceso

- **Wi‑Fi recomendado:** conecta el dispositivo de control al SSID **ROVER_DIEGO**.
- **URL principal de control:** `http://192.168.4.1`
- **Cámara directa:** `http://192.168.4.2`
- **Stream de la cámara:** `http://192.168.4.2:81/stream`

## 3. Secuencia de arranque recomendada

1. Enciende el rover.
2. Espera unos segundos a que termine la inicialización.
3. Conéctate al Wi‑Fi **ROVER_DIEGO**.
4. Abre `http://192.168.4.1` en el navegador.
5. Comprueba primero la orden **STOP**.
6. Haz una prueba corta: adelante, atrás, curva izquierda y curva derecha.

## 4. Cómo conduce el rover

- **Curvas:** reducen la velocidad del lado interior para una conducción más natural.
- **Rotación sobre sí mismo:** pensada para giros cerrados en el sitio.
- **Mantenimiento de rumbo:** ayuda en avance recto y retroceso recto si la velocidad es suficiente.
- **Parada por obstáculo:** el rover ordena parada si detecta un obstáculo a 150 mm o menos.

## 5. Funciones del mando RF

| Tecla | Función | Descripción |
|---|---|---|
| 1 | + velocidad | Aumenta la velocidad de movimiento |
| 2 | - velocidad | Reduce la velocidad de movimiento |
| 3 | Luces | Activa o desactiva la iluminación |
| 4 | Flash | Activa o desactiva el flash |
| 5 | Rumbo ON/OFF | Activa o desactiva el mantenimiento de rumbo |
| 6 | STOP | Parada inmediata |
| 7 | Giro izda | Curva a la izquierda en avance |
| 8 | Adelante | Avance recto |
| 9 | Giro dcha | Curva a la derecha en avance |
| 10 | Lateral izda | Desplazamiento lateral a la izquierda |
| 11 | Sin uso | Reservada actualmente |
| 12 | Lateral dcha | Desplazamiento lateral a la derecha |
| 13 | Rot izda | Giro sobre sí mismo a la izquierda |
| 14 | Sin uso | Reservada actualmente |
| 15 | Rot dcha | Giro sobre sí mismo a la derecha |
| 16 | Giro izda atrás | Curva a la izquierda en retroceso |
| 17 | Atrás | Retroceso recto |
| 18 | Giro dcha atrás | Curva a la derecha en retroceso |

## 6. Si algo no va bien

- La web no abre: revisa el Wi‑Fi y la URL `http://192.168.4.1`.
- La cámara no aparece: prueba `http://192.168.4.2:81/stream`.
- El rover no se mueve: revisa batería, potencia, motores y posible parada por obstáculo.
- Gira raro o asimétrico: revisa suelo, rozamientos y calibración.

## 7. Documentación

- GitHub: https://github.com/ramonlh/rover_control_RTOS_2/tree/main

## 8. Autores

- Ramón Lorenzo
- Diego Lorenzo
