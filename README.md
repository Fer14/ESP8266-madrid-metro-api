# ESP8266 Bus Arrival Display (EMT Madrid API)

This project uses an ESP8266, an I2C LCD display, and the EMT Madrid public transport API to display the estimated arrival time of bus line 132 at stop 1649. The device connects to Wi-Fi, logs in to the EMT API, retrieves the next bus arrival time, and displays it on the LCD when you press a button.

## Video Demo


https://github.com/user-attachments/assets/51d3bbff-2524-49c9-bc1f-bdbab5dfcdc5


## Features

- Wi-Fi Connectivity – Connects to your home Wi-Fi to access the EMT Madrid API.
- Secure API Requests – Uses HTTPS with WiFiClientSecure (certificate check disabled for simplicity).
- Button-Triggered Updates – Only fetches data when the button is pressed to save power.
- LCD Display – Shows connection status, login status, and bus arrival time.


## Hardware Requirements

- ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- I2C 16x2 LCD display
- Push button
- Pull-up resistor 
- Breadboard & jumper wires

## 🔌 Wiring
| Component | ESP8266 Pin   |
|-----------|--------------|
| LCD SDA   | D7 (GPIO13)  |
| LCD SCL   | D5 (GPIO14)  |
| Button    | D1 (GPIO5)   |
| LCD VCC   | 3.3V / 5V    |
| LCD GND   | GND          |
