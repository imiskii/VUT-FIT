# Bluetooth metronome

Simple embeded program for ESP32 with bluetooth. The project consists of two parts:

1. **Bluetooth client:** make a connection with the ESP32 (Bluetooth Metronome) and provides ability to control the metronome
2. **Bluetooth metronom:** back end functions of the metronome. This could be devided into two parts:
    1. **Metronome functions:** functions responsible for running the metronome and adjustment of its settings and configurations
    2. **Bluetooth connection:** functions responsible for handeling connection and events between client and ESP32


This program was build in C programming language in `esp-idf` framework. Requirments:

+ ESP32 with bluetooth
+ ESP-IDF Release v5.1.2 or newer
+ Chrome browser (version 119.0 or newer)

For more see `/doc/documentation.pdf`

## Set up client

### Enable web bluetooth in a browser (chrome/chromium => does not work on Brave brawser)

1. make sure to use bluetooth version 5.41 and above. `bluetoothd --version`
2. go to: [chrome://flags/#enable-experimental-web-platform-features](chrome://flags/#enable-experimental-web-platform-features) and set `Experimental Web Platform features` to Enable, then relaunch the browser
3. go to: [chrome://flags/#enable-web-bluetooth-new-permissions-backend](chrome://flags/#enable-web-bluetooth-new-permissions-backend) and set `Use the new permissions backend for Web Bluetooth` to Enable, then relaunch the browser
4. Go to `Settings` => `Privacy and Security` => `Site settings` => `Additional permissions` => `Bluetooth` and enable sites to use bluetooth
5. Access web page with the Bluetooth Metronome client

## Set up metronome

1. build the project `idf.py build`
2. flash it on EPS32 `idf.py /dev/ttyUSB0 flash` (replace `ttyUSB0` with your port)
3. if you want to monitor the program on ESP32 use `idf.py /dev/ttyUSB0 monitor` (replace `ttyUSB0` with your port)
4. Connect client to Bluetooth Metronome
5. If the client writes `Connection successful` then the connection is set up and Metronome should work