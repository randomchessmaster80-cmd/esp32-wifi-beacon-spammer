#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "esp_wifi.h"

// Management Access Point
const char* AP_SSID = "ESP32-Spammer-Control";
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_NETMASK(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;
Preferences prefs;

// Global state
bool isSpamming = false;
bool autoStart = false;
String ssidList = "1. FBI Surveillance Van #4\n2. TellMyWiFiLoveHer\n3. Loading...\n4. Virus_Infected_AP\n5. Martin Router King\n6. DropItLikeItsHotspot";
std::vector<String> activeSSIDs;

// Base beacon frame template
uint8_t beaconPacket[128] = {
  0x80, 0x00, 0x00, 0x00,                // Type: Beacon
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    // Destination: Broadcast
  0x02, 0x00, 0x00, 0x00, 0x00, 0x01,    // Source (Overwritten per SSID)
  0x02, 0x00, 0x00, 0x00, 0x00, 0x01,    // BSSID (Overwritten per SSID)
  0xc0, 0x6c,                            // Sequence control
  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  0x64, 0x00,                            // Beacon interval: 100 TU
  0x01, 0x04,                            // Capability info
  0x00                                   // Tag: SSID parameter set
};

void parseSSIDs() {
  activeSSIDs.clear();
  int start = 0;
  while (start < ssidList.length()) {
    int end = ssidList.indexOf('\n', start);
    if (end == -1) end = ssidList.length();
    String s = ssidList.substring(start, end);
    s.trim();
    if (s.length() > 0 && s.length() <= 32) {
      activeSSIDs.push_back(s);
    }
    start = end + 1;
  }
}

// Background task running on Core 0 to spam beacon frames at Max TX Power
void spammerTask(void *pvParameters) {
  uint8_t packet[128];
  memcpy(packet, beaconPacket, sizeof(beaconPacket));

  while (true) {
    if (isSpamming && !activeSSIDs.empty()) {
      for (size_t i = 0; i < activeSSIDs.size(); i++) {
        if (!isSpamming) break;

        String ssid = activeSSIDs[i];
        uint8_t ssidLen = ssid.length();

        // Unique MAC address for each SSID
        uint8_t mac[6] = {0x02, 0x13, 0x37, (uint8_t)(i >> 8), (uint8_t)(i & 0xFF), 0xAA};
        memcpy(&packet[10], mac, 6); // Source
        memcpy(&packet[16], mac, 6); // BSSID

        // Insert SSID
        packet[36] = 0x00;           // Tag number (SSID)
        packet[37] = ssidLen;        // Tag length
        memcpy(&packet[38], ssid.c_str(), ssidLen);

        int pos = 38 + ssidLen;

        // Supported Rates tag
        packet[pos++] = 0x01; // Tag
        packet[pos++] = 0x08; // Length
        packet[pos++] = 0x82;
        packet[pos++] = 0x84;
        packet[pos++] = 0x8b;
        packet[pos++] = 0x96;
        packet[pos++] = 0x24;
        packet[pos++] = 0x30;
        packet[pos++] = 0x48;
        packet[pos++] = 0x6c;

        // DS Parameter (Channel 1)
        packet[pos++] = 0x03; // Tag
        packet[pos++] = 0x01; // Length
        packet[pos++] = 0x01; // Channel 1

        esp_wifi_80211_tx(WIFI_IF_AP, packet, pos, false);
        vTaskDelay(pdMS_TO_TICKS(15));
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

// Original Website UI that you loved
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Wi-Fi Spammer</title>
<style>
  :root { --bg: #0d1117; --card: #161b22; --accent: #58a6ff; --green: #238636; --red: #da3633; --text: #c9d1d9; }
  body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: var(--bg); color: var(--text); padding: 15px; margin: 0; }
  .container { max-width: 500px; margin: 0 auto; }
  .card { background: var(--card); border: 1px solid #30363d; border-radius: 12px; padding: 20px; margin-bottom: 15px; }
  h1 { font-size: 22px; margin-top: 0; color: #fff; display: flex; align-items: center; justify-content: space-between; }
  .badge { font-size: 12px; padding: 4px 10px; border-radius: 20px; text-transform: uppercase; font-weight: bold; }
  .badge-on { background: #1f6feb33; color: #58a6ff; border: 1px solid #58a6ff; }
  .badge-off { background: #da363333; color: #f85149; border: 1px solid #f85149; }
  textarea { width: 100%; height: 160px; background: #0d1117; color: #58a6ff; border: 1px solid #30363d; border-radius: 8px; padding: 10px; font-family: monospace; font-size: 14px; box-sizing: border-box; resize: vertical; }
  button { width: 100%; padding: 12px; font-size: 15px; font-weight: bold; border-radius: 8px; border: none; cursor: pointer; margin-top: 8px; transition: 0.2s; }
  .btn-green { background: var(--green); color: white; }
  .btn-red { background: var(--red); color: white; }
  .btn-blue { background: #1f6feb; color: white; }
  .btn-preset { background: #21262d; color: var(--text); border: 1px solid #30363d; width: 31%; display: inline-block; font-size: 12px; padding: 8px 4px; margin: 2px 0; }
  .presets { display: flex; justify-content: space-between; margin-bottom: 10px; }
  label { font-size: 13px; color: #8b949e; display: flex; align-items: center; gap: 8px; margin-top: 10px; cursor: pointer; }
</style>
</head>
<body>
<div class="container">
  <div class="card">
    <h1>📡 Airwave Spammer <span id="statusBadge" class="badge">Checking...</span></h1>
    <p style="font-size: 13px; color: #8b949e; margin-top: -5px;">Broadcasts custom Wi-Fi networks in the air.</p>
    
    <div class="presets">
      <button class="btn-preset" onclick="setPreset('funny')">🤣 Funny</button>
      <button class="btn-preset" onclick="setPreset('cyber')">💀 Matrix</button>
      <button class="btn-preset" onclick="setPreset('rick')">🎵 Rickroll</button>
    </div>

    <textarea id="ssids" placeholder="Enter one Wi-Fi name per line (max 32 chars each)..."></textarea>

    <label>
      <input type="checkbox" id="autostart"> Automatically broadcast when plugged into a charger
    </label>

    <button id="toggleBtn" class="btn-green" onclick="toggleSpam()">Start Broadcasting</button>
    <button class="btn-blue" onclick="saveSettings()">💾 Save to Chip</button>
  </div>
</div>

<script>
const PRESETS = {
  funny: "1. FBI Surveillance Van #4\n2. TellMyWiFiLoveHer\n3. Loading...\n4. Virus_Infected_AP\n5. Martin Router King\n6. DropItLikeItsHotspot",
  cyber: "[ACCESS DENIED]\nSYSTEM BREACH DETECTED\nANONYMOUS_GATEWAY\n404_NETWORK_NOT_FOUND\n01001000 01001001\nDARK_NET_RELAY\nOVERRIDE_ENABLED",
  rick: "1. Never Gonna Give You Up\n2. Never Gonna Let You Down\n3. Never Gonna Run Around\n4. And Desert You\n5. Never Gonna Make You Cry\n6. Never Gonna Say Goodbye"
};

function setPreset(key) {
  document.getElementById('ssids').value = PRESETS[key];
}

function updateUI(data) {
  document.getElementById('ssids').value = data.ssids;
  document.getElementById('autostart').checked = data.autostart;
  const badge = document.getElementById('statusBadge');
  const btn = document.getElementById('toggleBtn');
  
  if (data.spamming) {
    badge.innerText = "ACTIVE";
    badge.className = "badge badge-on";
    btn.innerText = "⏹ Stop Broadcasting";
    btn.className = "btn-red";
  } else {
    badge.innerText = "IDLE";
    badge.className = "badge badge-off";
    btn.innerText = "▶ Start Broadcasting";
    btn.className = "btn-green";
  }
}

fetch('/api/status').then(r => r.json()).then(updateUI);

function toggleSpam() {
  fetch('/api/toggle', {method: 'POST'})
    .then(r => r.json())
    .then(updateUI);
}

function saveSettings() {
  const ssids = document.getElementById('ssids').value;
  const autostart = document.getElementById('autostart').checked;
  fetch('/api/save', {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: 'ssids=' + encodeURIComponent(ssids) + '&autostart=' + (autostart ? '1' : '0')
  }).then(r => r.json()).then(data => {
    alert('Settings saved to ESP32 Flash memory!');
    updateUI(data);
  });
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

void handleStatus() {
  String json = "{";
  json += "\"spamming\":" + String(isSpamming ? "true" : "false") + ",";
  json += "\"autostart\":" + String(autoStart ? "true" : "false") + ",";
  
  String escaped = ssidList;
  escaped.replace("\\", "\\\\");
  escaped.replace("\"", "\\\"");
  escaped.replace("\n", "\\n");
  escaped.replace("\r", "");
  json += "\"ssids\":\"" + escaped + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleToggle() {
  isSpamming = !isSpamming;
  handleStatus();
}

void handleSave() {
  if (server.hasArg("ssids")) {
    ssidList = server.arg("ssids");
    prefs.putString("ssids", ssidList);
    parseSSIDs();
  }
  if (server.hasArg("autostart")) {
    autoStart = (server.arg("autostart") == "1");
    prefs.putBool("autostart", autoStart);
  }
  handleStatus();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize NVS storage
  prefs.begin("spammer", false);
  ssidList = prefs.getString("ssids", ssidList);
  autoStart = prefs.getBool("autostart", false);
  isSpamming = autoStart;
  parseSSIDs();

  // Start Access Point
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
  WiFi.softAP(AP_SSID);

  // V2 TWEAK: Maximum RF Transmit Power Boost (+20 dBm / 19.5 dBm hardware ceiling)
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  esp_wifi_set_max_tx_power(78); // 78 * 0.25 dBm = 19.5 dBm

  // Captive Portal DNS
  dnsServer.start(53, "*", AP_IP);

  // Web routes
  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/toggle", HTTP_POST, handleToggle);
  server.on("/api/save", HTTP_POST, handleSave);
  server.onNotFound(handleRoot); // Captive portal redirect
  server.begin();

  // Create background task for raw packet injection on Core 0
  xTaskCreatePinnedToCore(spammerTask, "SpammerTask", 4096, NULL, 1, NULL, 0);

  Serial.println("ESP32 Airwave Spammer V2 (Original UI + Max Power Boost) Ready!");
  Serial.println("AP IP: 192.168.4.1");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  delay(2);
}
