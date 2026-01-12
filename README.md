# LED Tower – Real-Time 3D LED Cube with ESP32 and FreeRTOS

This project implements a real-time interactive 3D LED tower using an ESP32 microcontroller running FreeRTOS. The system controls a 6×6×6 LED grid capable of displaying multiple animations and a 3D Snake game.

The project was developed as part of the Embedded Systems course at the Faculty of Engineering of the University of Porto.

---

## Features

- 6×6×6 individually addressable LED grid  
- Real-time multitasking using FreeRTOS  
- Passive I/O expansion using MCP23017 GPIO expanders  
- LED control using 74HC164 shift registers  
- Multiple animations:
  - Rain effect  
  - Converging arrows  
  - Resizing cube  
- Interactive 3D Snake game  
- Button-based user input  
- Real-time scheduling and task prioritization  

---

## Hardware

- ESP32 microcontroller  
- 3 × MCP23017 GPIO expanders (I2C)  
- 2 × 74HC164 shift registers  
- 6×6×6 LED cube  
- Push buttons for user input


## Demo

A demonstration video of the project is available here:

👉 https://www.youtube.com/watch?v=YuEzPVUi9ik

---
