#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// HTML embebido (puedes reemplazar por tu versión completa)
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Panel IoT - DHT22</title>
<style>
body{font-family:sans-serif;text-align:center;background:#f2f2f2;color:#222;}
.card{margin:20px auto;padding:20px;border-radius:15px;background:white;max-width:300px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}
h1{color:#0078d4;}
</style>
</head>
<body>
<h1>Panel IoT - DHT22</h1>
<div class='card'>
<h3>Temperatura</h3><p id='temp'>-- °C</p>
<h3>Humedad</h3><p id='hum'>-- %</p>
</div>
<script>
async function getData(){
 const res = await fetch('/data');
 const json = await res.json();
 document.getElementById('temp').textContent = json.temp.toFixed(1)+' °C';
 document.getElementById('hum').textContent = json.hum.toFixed(1)+' %';
}
setInterval(getData,1000);
getData();
</script>
</body>
</html>
)rawliteral";

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    server.send(500, "application/json", "{\"error\":\"Lectura fallida\"}");
    return;
  }
  String json = "{\"temp\":" + String(t, 1) + ",\"hum\":" + String(h, 1) + "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFiManager wm;
  bool res;
  res = wm.autoConnect("ESP32-DHT22"); // AP de configuración

  if (!res) {
    Serial.println("⚠️ Falló conexión o timeout");
    ESP.restart();
  } else {
    Serial.println("✅ Conectado a WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
