#include <Arduino.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WebServer server(80);

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Panel IoT - DHT22</title>
<style>
body {
  font-family: 'Segoe UI', sans-serif;
  margin: 0; padding: 0;
  background-color: #f4f4f4;
  color: #333;
  transition: background-color 0.3s, color 0.3s;
  display: flex; flex-direction: column;
  align-items: center; justify-content: center;
  height: 100vh;
}
.container { text-align: center; }
header {
  display: flex; justify-content: center; align-items: center;
  gap: 10px; margin-bottom: 20px;
}
.card {
  background: white; border-radius: 16px;
  box-shadow: 0 4px 10px rgba(0,0,0,0.1);
  padding: 20px; margin: 10px auto; width: 250px;
}
footer { margin-top: 10px; font-size: 0.8rem; color: #666; }
.switch { position: relative; display: inline-block; width: 50px; height: 26px; }
.switch input { display:none; }
.slider {
  position: absolute; cursor: pointer;
  top: 0; left: 0; right: 0; bottom: 0;
  background-color: #ccc; transition: 0.4s; border-radius: 26px;
}
.slider:before {
  position: absolute; content: "🌙";
  height: 22px; width: 22px; left: 2px; bottom: 2px; transition: 0.4s;
}
input:checked + .slider { background-color: #2196F3; }
input:checked + .slider:before { transform: translateX(24px); }
body.dark { background-color: #121212; color: white; }
body.dark .card { background-color: #1e1e1e; color: white; }
</style>
</head>
<body>
<div class="container">
<header>
  <h1>Panel IoT - DHT22</h1>
  <label class="switch">
    <input type="checkbox" id="theme-toggle">
    <span class="slider"></span>
  </label>
</header>
<main>
  <div class="card">
    <h2>Temperatura</h2>
    <p id="temp">-- °C</p>
  </div>
  <div class="card">
    <h2>Humedad</h2>
    <p id="hum">-- %</p>
  </div>
</main>
<footer>ESP32 + DHT22 | WiFiManager</footer>
</div>
<script>
async function getData() {
  try {
    const res = await fetch('/data');
    const json = await res.json();
    if (json.temp !== undefined) {
      document.getElementById('temp').textContent = json.temp.toFixed(1) + ' °C';
      document.getElementById('hum').textContent = json.hum.toFixed(1) + ' %';
    }
  } catch (err) { console.error(err); }
}
setInterval(getData, 2000);
getData();
const toggle = document.getElementById('theme-toggle');
toggle.addEventListener('change', () => {
  document.body.classList.toggle('dark', toggle.checked);
});
</script>
</body>
</html>
)rawliteral";

void handleData() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp)) {
    server.send(500, "application/json", "{\"error\":\"Sensor no disponible\"}");
    return;
  }

  String json = "{\"temp\":" + String(temp, 1) + ",\"hum\":" + String(hum, 1) + "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFiManager wifiManager;
  //wifiManager.resetSettings(); // descomenta si querés reiniciar el portal
  wifiManager.autoConnect("ESP32-DHT22");

  Serial.println("✅ Conectado a WiFi!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("Servidor iniciado correctamente.");
}

void loop() {
  server.handleClient();
}
