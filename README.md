# PacketStorm
An ESP8266-based WiFi deauthentication tool with OLED display and button controls.

# Key Features
- Minimal Setup.

# Hardware Requirements
- NodeMCU ESP8266
- One SSD1306 0.96" OLED Display (I2C)
- `3` Tacticle Push Buttons
- `11` Male to Male Jumper Wires
- Breadboard

# Pinout Tables
## NodeMCU ESP8266 with SSD1306 0.96" OLED Display
| SSD1306 0.96" OLED Display | NodeMCU ESP8266 |
|----------------------------|-----------------|
| GND                        | GND             |
| VCC                        | 3.3V            |
| SCL                        | D1              |
| SDA                        | D2              |
## NodeMCU ESP8266 with Tacticle Push Buttons
| Push Button | NodeMCU ESP8266 |
|-------------|-----------------|
| UP          | D5              |
| DOWN        | D6              |
| SELECT      | D7              |

# Note
- `SCL` is sometimes also refered as `SCK`.
- Connect `GND` of SSD1306 0.96" OLED Display and `3` Tacticle Push Buttons to one `GND` of NodeMCU ESP8266.

# Setup
1. Download Arduino IDE from [here](https://www.arduino.cc/en/software) according to your Operating System.
2. Install it.
3. Go to `File` → `Preferences` → `Additional Boards Manager URLs`.
4. Paste the following link :
   
   ```
   https://raw.githubusercontent.com/SpacehuhnTech/arduino/main/package_spacehuhn_index.json
   ```
6. Click on `OK`.
7. Go to `Tools` → `Board` → `Board Manager`.
8. Wait for sometimes and search `deauther` by `Spacehuhn Technologies`.
9. Simply install it.
10. Wait for sometime and after that it is installed.
11. Restart the Arduino IDE.
12. Done!

# Install Necessary Libraries
1. Open Arduino IDE.
2. Go to `Sketch` → `Include Library` → `Manage Libraries...`.
3. Wait for sometimes and search `Adafruit SSD1306` by `Adafruit.`.
4. Simply install it.
5. Wait for sometimes and search `Adafruit GFX Library` by `Adafruit.`.
6. Simply install it.
7. Done!

# Install
## Via Arduino IDE
1. Download or Clone the Repository.
2. Open the folder and just double click on `PacketStorm.ino` file.
3. It opens in Arduino IDE.
4. Compile the code.
5. Select the correct board from the `Tools` → `Board` → `Deauther ESP8266 Boards`.
   - It is `NodeMCU`.
6. Select the correct port number of that board.
7. Upload the code.
8. Done!
## Via ESP8266 Flasher
1. Download the NodeMCU ESP8266 Flasher from [here](https://github.com/nodemcu/nodemcu-flasher) according to your operating system.
2. Download the `.bin` file from [here](https://github.com/wirebits/PacketStorm/releases/download/v1.0.0/PacketStorm.bin).
3. Open NodeMCU ESP8266 Flasher.
4. Select the correct port number of that board.
5. Click on `Advanced` Tab.
6. Click on `Restore Default` button.
7. Click on `Config` Tab.
   - It show `INTERNAL://NODEMCU`.
8. Click on ![image](https://github.com/user-attachments/assets/1540d7e8-514a-4e60-a29d-3019699868df) in front of `INTERNAL://NODEMCU`.
9. Select the `CLI-Deauther.bin` file.
10. Click on `Operation` Tab.
11. Click on `Flash(F)` button.
12. Wait for sometimes and when completed, press `RST` button.
