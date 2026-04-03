# Practice 2C - Frequency Measurement with Web Interface (ESP32)

## 📌 Overview

This project implements a **real-time frequency measurement system on the ESP32** using GPIO interrupts and a circular buffer. The measured data is processed and displayed through a **web interface**, allowing remote monitoring of signal characteristics.

---

## ⚙️ Features

- Frequency measurement using **GPIO interrupts**  
- Period calculation with microsecond precision (`micros()`)  
- Circular buffer (queue) for storing recent samples  
- Calculation of:
  - Maximum frequency (Fmax)  
  - Minimum frequency (Fmin)  
  - Average frequency (Fmed)  
- Embedded **WiFi connection (STA mode)**  
- Real-time **web server visualization**  
- Automatic page refresh every 2 seconds  

---

## 🧠 How It Works

1. An external signal is connected to a GPIO pin (pin 18).
2. Each pulse triggers an interrupt (RISING edge).
3. The time between consecutive pulses is measured (period).
4. Period values are stored in a **circular queue of fixed size (10 samples)**.
5. The system computes:
   - Minimum period → Maximum frequency  
   - Maximum period → Minimum frequency  
   - Average period → Average frequency  
6. The ESP32 connects to a WiFi network and hosts a web server.
7. Frequency values are displayed in real time on a web page.

---

## 🌐 Web Interface

Once connected, open the ESP32 IP address in a browser:

```text
http://<ESP32_IP>
