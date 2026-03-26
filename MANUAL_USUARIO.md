# Rover Control RTOS 2 – Manual de usuario

## 1. Introducción

Este documento explica cómo **usar** el rover en funcionamiento normal.
Está pensado para operadores, probadores y cualquier persona que quiera conducir el rover sin necesidad de leer el código fuente.

El rover está basado en una plataforma **ESP32** e incluye:

- tracción de 4 motores
- control de servos
- conectividad Wi‑Fi
- control y monitorización vía web
- comunicación en tiempo real mediante WebSocket
- soporte OTA
- acceso FTP
- soporte de radiocontrol
- sensores a bordo como medida de distancia, detección de colisión, giróscopo, radar de presencia humana y sensado ambiental

El firmware utiliza una arquitectura multitarea basada en **FreeRTOS** y soporta distintos modos de funcionamiento, incluyendo operación en red y operación solo por radiocontrol.

---

## 1.1 Documentación del proyecto

La documentación principal del proyecto, el código fuente y las actualizaciones están disponibles en GitHub:

**Documentación del repositorio:**  
https://github.com/ramonlh/rover_control_RTOS_2/tree/main

Si necesitas instrucciones de compilación, notas sobre la arquitectura del firmware o detalles de desarrollo, empieza por ahí junto con el fichero principal `README.md` del repositorio.

---

## 2. Modos principales de funcionamiento

El rover soporta tres modos generales de funcionamiento:

### Modo 0 — STA + AP + RC

En este modo el rover utiliza:

- conexión Wi‑Fi
- sistema de ficheros
- servidor web
- servidor FTP
- WebSocket
- OTA
- radiocontrol

### Modo 1 — AP + RC

En este modo el rover crea o utiliza su propio punto de acceso y ejecuta:

- sistema de ficheros
- servidor web
- servidor FTP
- WebSocket
- OTA
- radiocontrol

### Modo 2 — RC

Este modo está centrado en **radiocontrol únicamente**, sin los servicios principales de red.

El modo activo depende de la configuración del firmware.

---

## 3. Antes de encender

Antes de encender el rover, comprueba lo siguiente:

- la batería está cargada
- la alimentación de motores y de lógica es correcta
- ninguna rueda está bloqueada
- el rover está colocado sobre una superficie estable
- no hay ningún objeto peligrosamente cerca de la parte frontal del rover
- el dispositivo de control que vayas a utilizar está preparado:
  - PC, tableta o teléfono para control web
  - transmisor RF para radiocontrol

Para las primeras pruebas después de cambios en el firmware, es buena idea levantar el rover para que las ruedas puedan girar libremente.

---

## 4. Encendido y arranque

Cuando el rover arranca, el firmware inicializa los principales servicios y subsistemas, incluyendo:

1. salida serie
2. mutex I2C
3. tareas habilitadas
4. conectividad y servicios de red según el modo configurado
5. pines LED y entrada de colisión
6. subsistemas que no usan I2C
7. bus I2C, MCP23017 y PCA9685
8. tareas de motores, servos, radar y giróscopo

Durante el funcionamiento normal, el bucle principal atiende:

- servicio de flash
- peticiones del servidor web
- OTA
- FTP
- gestión de obstáculos

Deja pasar unos segundos tras el encendido antes de intentar mover el rover.

---

## 5. Cómo conectarse al rover

El método exacto de conexión depende del modo de funcionamiento configurado.

### Opción A — Control web por Wi‑Fi

Usa este método cuando el rover esté funcionando con sus servicios web habilitados.

Procedimiento general:

1. Enciende el rover.
2. Conecta tu PC, tableta o teléfono a la red del rover o a la misma red que use el rover.
3. Abre la página de control del rover en un navegador web.
4. Espera a que la página termine de cargar y a que se inicie la comunicación en tiempo real.
5. Verifica que los controles responden antes de mover el rover en una zona reducida.

### Opción B — Radiocontrol

Usa este método cuando el rover esté configurado para radiocontrol, o cuando quieras un método de control directo sin depender de la interfaz web.

Procedimiento general:

1. Enciende el rover.
2. Enciende el transmisor RF.
3. Comprueba que el rover responde a las órdenes básicas.
4. Empieza siempre probando a baja velocidad.

---

## 6. Conducción del rover

El rover soporta varios tipos de movimiento.

### 6.1 Movimiento recto

- **Adelante**
- **Atrás**

Cuando el mantenimiento de rumbo está activado y la velocidad es suficiente, el firmware puede corregir automáticamente la velocidad de los motores izquierdo y derecho para ayudar a que el rover mantenga una trayectoria más recta utilizando la realimentación de yaw del giróscopo.

### 6.2 Movimiento en curva

- **Adelante izquierda**
- **Adelante derecha**
- **Atrás izquierda**
- **Atrás derecha**

Estas maniobras son **curvas reales**, no giros por pivotaje.

El firmware actual reduce la velocidad del lado interior del rover en lugar de detener un lado completo. Eso proporciona una dirección mucho más natural.

La intensidad de la curva también cambia con la velocidad:

- a **baja velocidad**, la curva es más cerrada
- a **alta velocidad**, la curva se vuelve más suave y estable

Este comportamiento proviene del factor dinámico de curva utilizado por la lógica de control de motores.

### 6.3 Rotación sobre sí mismo

- **Rotar a la izquierda**
- **Rotar a la derecha**

Estas órdenes son distintas de las curvas.
Están pensadas para girar el rover sobre el sitio.

### 6.4 Movimiento lateral

El firmware también soporta órdenes específicas de desplazamiento lateral:

- **Lateral izquierda**
- **Lateral derecha**

Son útiles en plataformas diseñadas para ese tipo de movimiento.

### 6.5 Órdenes de prueba de rueda individual

El módulo de motores también soporta órdenes de avance y retroceso de ruedas individuales para pruebas y diagnóstico.
Son útiles durante la puesta en marcha, la resolución de problemas y el mantenimiento.

---

## 7. Comportamiento de la dirección

Una de las mejoras recientes del rover es el modelo de dirección.

Anteriormente, algunas órdenes de giro podían comportarse de manera demasiado brusca. El comportamiento actual es más refinado:

- las órdenes de **curva** se usan para girar con naturalidad mientras el rover está en movimiento
- las órdenes de **rotación** se usan únicamente cuando se quiere girar sobre el sitio
- la intensidad de la curva se adapta automáticamente a la velocidad actual

Se utiliza como referencia un factor de curva de **0,45** en la configuración del control de motores, mientras que la lógica dinámica endurece o suaviza el giro en función de la velocidad.

Esto hace que el rover sea más fácil de controlar y reduce la sensación de “casi pivotaje” durante la conducción normal.

---

## 8. Mantenimiento de rumbo

El rover incluye una función opcional de mantenimiento de rumbo basada en la realimentación de yaw del giróscopo.

Cuando está activada, y cuando el rover avanza recto o retrocede recto por encima de la velocidad mínima de control, el firmware:

- almacena un rumbo objetivo
- compara el yaw actual con ese rumbo
- aplica una corrección izquierda/derecha limitada

Esto ayuda al rover a mantener una trayectoria más recta al avanzar o retroceder.

El mantenimiento de rumbo se desactiva automáticamente para movimientos que no son rectos, como:

- curvas
- rotaciones
- desplazamientos laterales
- órdenes de prueba de rueda individual

Si el rover se mueve muy despacio, la corrección de rumbo también se desactiva para evitar sobrecorrecciones.

---

## 9. Comportamiento ante obstáculos y colisiones

El rover incluye lógica de protección vinculada a la detección de obstáculos y colisiones.

### Parada por distancia

Si la distancia medida es mayor que 0 y menor o igual a **150 mm**, el rover emite una orden de parada mediante la lógica de protección de la tarea de motores.

### Retención del flash por colisión

Cuando la entrada de colisión está activa, la gestión del flash también se mantiene en el estado correspondiente.

Estas protecciones están ahí para reducir el riesgo de impactos durante el funcionamiento.

Importante: supervisa siempre físicamente el rover. La lógica de parada basada en sensores es una ayuda de seguridad, no un sustituto de la atención del operador.

---

## 10. Flash, iluminación e indicadores

El proyecto incluye gestión de flash e iluminación.

Dependiendo de la compilación actual y de la configuración de la interfaz web, el operador puede poder:

- encender o apagar luces
- usar funciones relacionadas con el flash
- observar indicadores asociados a estados de obstáculo o colisión

Dado que algunos detalles pueden variar entre revisiones del firmware, comprueba los nombres exactos de los botones y su comportamiento en la página de control activa.

---

## 11. Cámara y monitorización

El proyecto del rover está diseñado para trabajar con monitorización vía web y comunicación en tiempo real.

Dependiendo del hardware instalado y de la versión actual del firmware, el operador puede disponer de:

- la página principal de control
- telemetría en tiempo real
- funciones de visualización o streaming de cámara

Si la cámara o el stream no aparecen, comprueba:

- el modo de funcionamiento activo
- la conectividad de red
- la alimentación del módulo de cámara
- el estado de carga de la página web

---

## 12. Uso del radiocontrol

Cuando el radiocontrol está habilitado, las órdenes de movimiento pueden llegar por la vía RF.

Buenas prácticas típicas:

1. Empieza con el rover detenido.
2. Prueba adelante, atrás, izquierda y derecha a baja velocidad.
3. Verifica que la parada funciona de forma inmediata.
4. Solo entonces pasa a giros más cerrados, movimiento lateral o maniobras más agresivas.

Si en tu compilación están disponibles tanto el radiocontrol como el control web, asegúrate de entender qué fuente de control tiene actualmente la prioridad.

---

## 13. Uso de la interfaz web

El aspecto exacto de la interfaz web puede evolucionar con el tiempo, pero el flujo habitual es:

1. conectarse a la red del rover
2. abrir la página de control
3. esperar a que la interfaz cargue
4. verificar la comunicación
5. utilizar los botones o controles en pantalla para conducir el rover
6. supervisar en tiempo real la información de sensores y estado, si está disponible

Durante las pruebas:

- empieza a baja velocidad
- evita espacios estrechos
- prueba primero la parada
- confirma el comportamiento de giro antes de usar velocidades más altas

---

## 14. Consejos prácticos de conducción

Para obtener los mejores resultados:

- usa las órdenes de **curva** para la navegación normal
- usa las órdenes de **rotación** solo cuando necesites un giro cerrado sobre el sitio
- utiliza velocidades más bajas en interiores
- usa el mantenimiento de rumbo para avance/retroceso recto cuando esté disponible
- evita forzar el rover contra obstáculos
- deja espacio suficiente al probar el movimiento lateral

Tras las mejoras recientes de la dirección, el rover debería sentirse más natural en los giros que antes. Aun así, cada chasis se comporta de forma algo distinta según la tracción de las ruedas, el tipo de suelo y la distribución de pesos.

---

## 15. Resolución de problemas

### El rover enciende pero no se mueve

Comprueba:

- tensión de batería
- etapa de potencia de motores
- cableado de motores
- si el modo de funcionamiento actual permite el método de control seleccionado
- si el rover está detenido por la lógica de obstáculos

### El rover se detiene inmediatamente al intentar moverse

Posibles causas:

- obstáculo detectado a 150 mm o menos
- entrada de colisión activa
- conflicto entre fuentes de control
- tarea de motores deshabilitada en la compilación actual

### La dirección se siente mal o asimétrica

Comprueba:

- mapeo del sentido de giro de las ruedas
- rozamientos mecánicos
- diferencias de tracción entre ruedas delanteras y traseras
- tipo de suelo
- valores recientes de ajuste de curva y mantenimiento de rumbo

### El rover no mantiene bien la línea recta

Comprueba:

- si el mantenimiento de rumbo está activado
- el comportamiento del yaw del giróscopo
- agarre del suelo y alineación de las ruedas
- si la velocidad es suficiente para que se active la corrección de rumbo

### La página web abre pero el control es inestable

Comprueba:

- la calidad de la señal Wi‑Fi
- si el modo seleccionado incluye servicios web
- compatibilidad del navegador
- carga del sistema si hay varios servicios activos

### El rover responde al radiocontrol pero no al control web

Comprueba:

- el modo de funcionamiento activo
- la disponibilidad del servidor web
- la comunicación WebSocket
- si el radiocontrol tiene actualmente prioridad en tu compilación

---

## 16. Notas de seguridad

- No manejes el rover cerca de escaleras, balcones o desniveles pronunciados.
- Mantén las manos alejadas de las ruedas y de las partes móviles.
- No hagas pruebas a alta velocidad en espacios interiores reducidos.
- Verifica siempre el comportamiento de parada después de cambios en el firmware.
- Si estás probando la lógica de motores, levanta el rover del suelo siempre que sea práctico.
- Considera la detección de obstáculos por sensores como una ayuda, no como una protección garantizada.

---

## 17. Mantenimiento y pruebas

Comprobaciones recomendadas antes del uso habitual:

- carga de batería
- libertad de giro de las ruedas
- respuesta de dirección
- respuesta de parada
- comportamiento de sensores
- conectividad web o por radio

Comprobaciones recomendadas después de actualizar el firmware:

- avance / retroceso
- curvas izquierda / derecha
- rotación sobre sí mismo
- movimiento lateral
- comportamiento de parada por obstáculo
- comportamiento del mantenimiento de rumbo

---

## 18. Notas técnicas para operadores

El rover utiliza internamente un mapeo real de motores:

- motor 0 -> trasera izquierda
- motor 1 -> trasera derecha
- motor 2 -> delantera izquierda
- motor 3 -> delantera derecha

La tarea de motores aplica órdenes de movimiento sincronizadas bajo protección del mutex I2C para evitar conflictos con otros subsistemas basados en I2C.

Este detalle normalmente es transparente para el operador, pero ayuda a entender por qué las actualizaciones de firmware pueden afectar a la sensación de dirección.

---

## 19. Autores

- **Ramón Lorenzo**
- **Diego Lorenzo**

---

## 20. Notas finales

Este proyecto de rover es una plataforma experimental en evolución.

La experiencia exacta de uso puede variar según:

- la revisión actual del firmware
- los módulos habilitados
- el hardware montado en el rover
- el modo de funcionamiento seleccionado
- los valores de calibración y ajuste

Para instrucciones de compilación, arquitectura del firmware y detalles de desarrollo, consulta el repositorio de GitHub y el fichero principal `README.md`: https://github.com/ramonlh/rover_control_RTOS_2/tree/main
