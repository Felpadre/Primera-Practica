#include <WiFiManager.h>
#include <WebServer.h>
#include <DHT.h>
#include <HTTPClient.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define LED_PIN 5

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// URL del WebApp de Google Sheets (la vas a crear en el paso siguiente)
const char* googleScriptUrl = "https://script.google.com/macros/s/AKfycby8jVJdeQU5g_hmHtxpMVBh_DiI6QKjQ-PoTa7EmG_m6_qeMdFjbDhxesF2vCAV1oxY/exec";

String htmlPage = R"=====( 
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Panel IoT DHT22</title>
<style>
  body {
    font-family: 'Segoe UI', sans-serif;
    text-align: center;
    background-color: var(--bg);
    color: var(--text);
    transition: background 0.3s, color 0.3s;
  }
  :root {
    --bg: #f5f5f5;
    --text: #222;
    --card: #fff;
  }
  body.dark {
    --bg: #121212;
    --text: #f5f5f5;
    --card: #1e1e1e;
  }
  .card {
    background: var(--card);
    display: inline-block;
    padding: 20px;
    border-radius: 16px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    margin-top: 20px;
  }
  .toggle {
    position: relative;
    width: 60px;
    height: 30px;
    background: #ccc;
    border-radius: 30px;
    cursor: pointer;
    margin: 10px auto;
    transition: background 0.3s;
  }
  .toggle::after {
    content: '';
    position: absolute;
    width: 24px;
    height: 24px;
    top: 3px;
    left: 3px;
    background: white;
    border-radius: 50%;
    transition: transform 0.3s;
  }
  .toggle.active {
    background: #2196F3;
  }
  .toggle.active::after {
    transform: translateX(30px);
  }
  canvas {
    margin-top: 20px;
    width: 90%;
    max-width: 500px;
    height: 300px;
  }
</style>
</head>
<body>
  <h2>🌡️ Panel IoT DHT22</h2>
  <div class="card">
    <h3>Temperatura: <span id="temp">--</span> °C</h3>
    <h3>Humedad: <span id="hum">--</span> %</h3>
  </div>
  
  <div class="card">
    <h3>Modo Oscuro</h3>
    <div id="darkToggle" class="toggle"></div>
  </div>

  <div class="card">
    <h3>Encender LED</h3>
    <div id="ledToggle" class="toggle"></div>
  </div>

  <div class="card">
    <h3>Historial últimas 24h</h3>
    <canvas id="chart"></canvas>
  </div>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
const darkToggle = document.getElementById('darkToggle');
const ledToggle = document.getElementById('ledToggle');
const temp = document.getElementById('temp');
const hum = document.getElementById('hum');
const ctx = document.getElementById('chart').getContext('2d');

let chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: [],
    datasets: [{
      label: 'Temperatura (°C)',
      borderColor: '#FF6384',
      data: []
    },{
      label: 'Humedad (%)',
      borderColor: '#36A2EB',
      data: []
    }]
  },
  options: { responsive: true }
});

darkToggle.addEventListener('click', () => {
  document.body.classList.toggle('dark');
  darkToggle.classList.toggle('active');
});

ledToggle.addEventListener('click', () => {
  ledToggle.classList.toggle('active');
  fetch('/led?state=' + (ledToggle.classList.contains('active') ? 'on' : 'off'));
});

async function fetchData() {
  const res = await fetch('/data');
  const json = await res.json();
  temp.textContent = json.temp;
  hum.textContent = json.hum;

  // Actualiza gráfica
  if (chart.data.labels.length > 24) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
    chart.data.datasets[1].data.shift();
  }
  const now = new Date().toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'});
  chart.data.labels.push(now);
  chart.data.datasets[0].data.push(json.temp);
  chart.data.datasets[1].data.push(json.hum);
  chart.update();
}
setInterval(fetchData, 5000);
</script>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    server.send(500, "application/json", "{\"error\":\"sensor\"}");
    return;
  }

  // Envío a Google Sheets
  HTTPClient http;
  String url = String(googleScriptUrl) + "?temp=" + t + "&hum=" + h;
  http.begin(url);
  http.GET();
  http.end();

  String json = "{\"temp\":" + String(t,1) + ",\"hum\":" + String(h,1) + "}";
  server.send(200, "application/json", json);
}

void handleLed() {
  String state = server.arg("state");
  digitalWrite(LED_PIN, state == "on" ? HIGH : LOW);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  dht.begin();

  WiFiManager wm;
  bool res = wm.autoConnect("ESP32-DHT22");
  if(!res) {
    Serial.println("Error al conectar.");
    ESP.restart();
  }

  Serial.println("WiFi conectado.");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/led", handleLed);
  server.begin();

  Serial.println("Servidor iniciado correctamente.");
}

void loop() {
  server.handleClient();
}
