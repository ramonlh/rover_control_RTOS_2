# Rover Control RTOS 2 – User Manual

## 1. Introduction

This document explains how to **use** the rover in normal operation.
It is intended for operators, testers, and anyone who wants to drive the rover without reading the source code.

The rover is based on an **ESP32** platform and includes:

- 4-motor drive
- servo control
- Wi-Fi connectivity
- web-based control and monitoring
- real-time communication via WebSocket
- OTA support
- FTP access
- radio-control support
- onboard sensors such as distance sensing, collision detection, gyroscope, human presence radar, and environmental sensing

The firmware uses a multitasking architecture based on **FreeRTOS** and supports different operating modes, including networked operation and radio-control-only operation.

---

## 1.1 Project Documentation

The main project documentation, source code, and updates are available on GitHub:

**Repository documentation:**  
https://github.com/ramonlh/rover_control_RTOS_2/tree/main

If you need build instructions, firmware architecture notes, or development details, start there together with the main `README.md` file in the repository.

---

## 2. Main Operating Modes

The rover supports three general operating modes:

### Mode 0 — STA + AP + RC

In this mode the rover uses:

- Wi-Fi connection
- file system
- web server
- FTP server
- WebSocket
- OTA
- radio control

### Mode 1 — AP + RC

In this mode the rover creates or uses its own access point and runs:

- file system
- web server
- FTP server
- WebSocket
- OTA
- radio control

### Mode 2 — RC

This mode is focused on **radio control only**, without the main network services.

Which mode is active depends on the firmware configuration.

---

## 3. Before Powering On

Before switching the rover on, check the following:

- the battery is charged
- motor power and logic power are correct
- no wheel is blocked
- the rover is placed on a stable surface
- no object is dangerously close to the front of the rover
- the control device you plan to use is ready:
  - PC, tablet, or phone for web control
  - RF transmitter for radio control

For first tests after firmware changes, it is a good idea to lift the rover so the wheels can spin freely.

---

## 4. Power-On and Startup

When the rover starts, the firmware initializes the main services and subsystems, including:

1. serial output
2. I2C mutex
3. enabled tasks
4. connectivity and network services depending on the configured mode
5. LED pins and collision input
6. non-I2C subsystems
7. I2C bus, MCP23017, and PCA9685
8. motor, servo, radar, and gyroscope tasks

During normal operation, the main loop services:

- flash service
- web server requests
- OTA
- FTP
- obstacle handling

Allow a few seconds after power-on before trying to drive the rover.

---

## 5. How to Connect to the Rover

The exact connection method depends on the configured operating mode.

### Option A — Web control over Wi-Fi

Use this method when the rover is operating with its web services enabled.

General procedure:

1. Power on the rover.
2. Connect your PC, tablet, or phone to the rover network or to the same network used by the rover.
3. Open the rover control page in a web browser.
4. Wait for the page to finish loading and for live communication to start.
5. Verify that the controls respond before moving the rover in a confined area.

### Option B — Radio control

Use this method when the rover is configured for radio control, or when you want a direct control method without depending on the web interface.

General procedure:

1. Power on the rover.
2. Power on the RF transmitter.
3. Check that the rover reacts to basic commands.
4. Test at low speed first.

---

## 6. Driving the Rover

The rover supports several movement types.

### 6.1 Straight movement

- **Forward**
- **Reverse**

When heading hold is enabled and the speed is high enough, the firmware can automatically correct left/right motor speed to help the rover maintain a straighter heading using gyroscope yaw feedback.

### 6.2 Curved movement

- **Forward left**
- **Forward right**
- **Reverse left**
- **Reverse right**

These are **real curves**, not pivot turns.

The current firmware reduces the speed of the inner side of the rover instead of stopping one side. This produces much more natural steering.

The curve strength also changes with speed:

- at **low speed**, the curve is tighter
- at **high speed**, the curve becomes smoother and more stable

This behavior comes from the dynamic curve factor used by the motor control logic.

### 6.3 Rotate in place

- **Rotate left**
- **Rotate right**

These commands are different from curves.
They are intended for turning the rover on the spot.

### 6.4 Lateral movement

The firmware also supports dedicated lateral movement commands:

- **Lateral left**
- **Lateral right**

These are useful on platforms designed for that style of movement.

### 6.5 Individual wheel test commands

The motor module also supports individual wheel forward/backward commands for testing and diagnosis.
These are useful during setup, troubleshooting, and maintenance.

---

## 7. Steering Behavior

One of the recent improvements in the rover is the steering model.

Previously, some turning commands could behave too abruptly. The current behavior is more refined:

- **curve commands** are used for natural turning while moving
- **rotate commands** are used only when you want to spin in place
- curve intensity is adapted automatically to the current speed

A reference curve factor of **0.45** is used in the motor control configuration, while the dynamic curve logic tightens or softens the turn depending on speed.

This makes the rover easier to control and reduces the “almost pivot” feeling during normal driving.

---

## 8. Heading Hold

The rover includes an optional heading-hold feature based on gyroscope yaw feedback.

When enabled, and when moving straight forward or straight backward above the minimum control speed, the firmware:

- stores a target heading
- compares current yaw with that heading
- applies a bounded left/right correction

This helps the rover keep a straighter path when driving forward or backward.

Heading hold is automatically disabled for movements that are not straight, such as:

- curves
- rotations
- lateral movements
- single-wheel test commands

If the rover is moving very slowly, heading correction is also disabled to avoid overcorrection.

---

## 9. Obstacle and Collision Behavior

The rover includes protection logic linked to obstacle and collision sensing.

### Distance-based stop

If the measured distance is greater than 0 and less than or equal to **150 mm**, the rover issues a stop command through the motor task protection logic.

### Collision-related flash hold

When the collision input is active, the flash manager is also held accordingly.

These protections are there to reduce the risk of impacts during operation.

Important: always supervise the rover physically. Sensor-based stop logic is a safety aid, not a substitute for operator attention.

---

## 10. Flash, Lighting, and Indicators

The project includes flash and lighting management.

Depending on the current build and web interface configuration, the operator may be able to:

- switch lights on or off
- use flash-related functions
- observe indicators associated with obstacle or collision states

Because some details may vary between firmware revisions, verify the exact button names and behavior on the active control page.

---

## 11. Camera and Monitoring

The rover project is designed to work with web-based monitoring and real-time communication.

Depending on the installed hardware and current firmware build, the operator may have access to:

- the main control page
- real-time telemetry
- camera viewing or streaming functions

If the camera or stream does not appear, verify:

- the active operating mode
- network connectivity
- camera module power
- web page loading status

---

## 12. Radio Control Use

When radio control is enabled, movement commands can come from the RF control path.

Typical good practice:

1. Start with the rover stopped.
2. Test forward, reverse, left, and right responses at low speed.
3. Verify that stop works immediately.
4. Only then move to tighter turns, lateral motion, or more aggressive maneuvers.

If radio control and web control are both available in your build, make sure you understand which control source currently has authority.

---

## 13. Using the Web Interface

The exact look of the web UI may evolve over time, but the usual workflow is:

1. connect to the rover network
2. open the control page
3. wait for the interface to load
4. verify communication
5. use on-screen buttons or controls to drive the rover
6. monitor sensor and status information in real time if available

During testing:

- begin at low speed
- avoid narrow spaces
- test stop behavior first
- confirm the steering behavior before using higher speed

---

## 14. Practical Driving Advice

To get the best results:

- use **curve** commands for normal navigation
- use **rotate** commands only when a tight in-place turn is required
- use lower speeds indoors
- use heading hold for straight forward/reverse motion when available
- avoid forcing the rover against obstacles
- keep enough free space when testing lateral movement

After the recent steering improvements, the rover should feel more natural during turns than before. Still, every chassis behaves a little differently depending on wheel traction, floor type, and weight distribution.

---

## 15. Troubleshooting

### The rover powers on but does not move

Check:

- battery voltage
- motor power stage
- motor wiring
- whether the current operating mode allows the selected control method
- whether the rover is stopped by obstacle logic

### The rover stops immediately when trying to move

Possible causes:

- obstacle detected within 150 mm
- collision input active
- control source conflict
- motor task disabled in the current build

### Steering feels wrong or asymmetric

Check:

- wheel direction mapping
- mechanical friction
- traction differences between front and rear wheels
- floor type
- recent tuning values for curve behavior and heading hold

### The rover does not keep a straight line well

Check:

- whether heading hold is enabled
- gyroscope yaw behavior
- floor grip and wheel alignment
- whether speed is high enough for heading correction to activate

### The web page opens but control is unstable

Check:

- Wi-Fi signal quality
- whether the selected mode includes web services
- browser compatibility
- system load if multiple services are active

### The rover responds to radio control but not to web control

Check:

- active operating mode
- web server availability
- WebSocket communication
- whether radio control currently has priority in your build

---

## 16. Safety Notes

- Do not operate the rover near stairs, balconies, or sharp drop-offs.
- Keep hands away from wheels and moving parts.
- Do not test at high speed in confined indoor spaces.
- Always verify stop behavior after firmware changes.
- If you are testing motor logic, lift the rover from the floor whenever practical.
- Treat sensor-based obstacle handling as assistance, not as guaranteed protection.

---

## 17. Maintenance and Testing

Recommended checks before regular use:

- battery charge
- wheel freedom of movement
- steering response
- stop response
- sensor behavior
- web or radio connectivity

Recommended checks after firmware updates:

- forward/reverse motion
- left/right curves
- rotate in place
- lateral motion
- obstacle stop behavior
- heading-hold behavior

---

## 18. Technical Notes for Operators

The rover uses a real motor mapping internally:

- motor 0 -> rear left
- motor 1 -> rear right
- motor 2 -> front left
- motor 3 -> front right

The motor task applies synchronized motion commands under I2C mutex protection to avoid conflicts with other I2C-based subsystems.

This detail is normally transparent to the operator, but it helps explain why firmware updates may affect steering feel.

---

## 19. Authors

- **Ramón Lorenzo**
- **Diego Lorenzo**

---

## 20. Final Notes

This rover project is an evolving experimental platform.

The exact user experience can vary depending on:

- the current firmware revision
- enabled modules
- hardware mounted on the rover
- selected operating mode
- calibration and tuning values

For build instructions, firmware architecture, and development details, see the GitHub repository and the main `README.md`: https://github.com/ramonlh/rover_control_RTOS_2/tree/main
