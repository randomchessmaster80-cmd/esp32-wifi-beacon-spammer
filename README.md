# 📡 ESP32 Wi-Fi Airwave Spammer V2 (Max Power Boost)

An open-source, web-controlled 802.11 beacon frame broadcaster for ESP32. Broadcasts dozens of custom Wi-Fi network names (SSIDs) simultaneously into the physical airwaves with an interactive mobile web interface and maximum RF transmit power.

---

## ⚡ Quick 1-Click Install for Beginners (No Coding Required!)

**Don't know how to code or don't have Arduino IDE installed? No problem!**

1. Go to the **[Releases Page](https://github.com/randomchessmaster80-cmd/esp32-wifi-beacon-spammer/releases/latest)**.
2. Download **`ESP32-WiFi-Spammer-v2.0-Standalone.zip`** and extract it.
3. Plug your ESP32 into your computer via USB.
4. Run the 1-click flasher:
   - **Windows**: Double-click `flash_windows.bat`
   - **Linux / macOS**: Run `./flash_linux_mac.sh`
5. **Done!** Connect your phone to `ESP32-Spammer-Control` and open `http://192.168.4.1`!

---

## ⚡ Features

- **🌐 Web-Controlled Dashboard**: Connect to `ESP32-Spammer-Control` and open `http://192.168.4.1` on any phone or laptop.
- **📱 Automatic Captive Portal**: Web dashboard automatically opens when connecting from mobile devices.
- **⚡ +20 dBm RF Power Boost**: Configured to transmit at maximum possible hardware output (`WIFI_POWER_19_5dBm`) for deep penetration through walls and extended range.
- **💾 Standalone Operation (NVS Memory)**: Save your Wi-Fi lists and auto-broadcast settings to the ESP32's flash memory. Unplug from your computer and run it from **any 5V USB phone charger or power bank**!
- **🤣 Built-In Presets & Custom Lists**:
  - **Funny**: *FBI Surveillance Van, TellMyWiFiLoveHer, Virus_Infected_AP, Loading...*
  - **Matrix**: *[ACCESS DENIED], SYSTEM BREACH DETECTED, ANONYMOUS_GATEWAY...*
  - **Rickroll**: *Never Gonna Give You Up, Never Gonna Let You Down...*
  - **Custom Input**: Type any names you want (one per line).

---

## 🛠️ Hardware Requirements

- **ESP32 Development Board** (NodeMCU-32S, ESP32-WROOM-32, D1 Mini ESP32, etc.)
- Micro-USB or USB-C data cable
- Any 5V USB power adapter or power bank for portable use

---

## 🚀 Advanced: Flashing from Source Code

If you prefer building from source code:

### Option A: Using Arduino IDE
1. Install the **ESP32 board package** in Arduino IDE (*Tools → Board Manager → esp32*).
2. Open `esp32_wifi_spammer_v2.ino`.
3. Select your ESP32 board and COM port.
4. Click **Upload**.

### Option B: Using `arduino-cli`
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .
```

---

## 📖 How to Use

1. Power on the ESP32.
2. Connect your phone or laptop to the Wi-Fi network:  
   **`ESP32-Spammer-Control`** *(No password required)*.
3. Open your browser to:  
   `http://192.168.4.1`
4. Select a preset or type your custom network names.
5. Tap **Start Broadcasting**!
6. Check **"Automatically broadcast when plugged into a charger"** and hit **Save to Chip** for autonomous operation.

---

## ⚖️ Disclaimer
*This project is created for educational and experimental purposes to demonstrate 802.11 management frames and microcontroller web architecture. Please follow local RF and communication regulations.*
