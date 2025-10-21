#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "DHT.h"

#define DHTPIN 2       // Pin del sensor
#define DHTTYPE DHT22  // Tipo de sensor
DHT dht(DHTPIN, DHTTYPE);

WebServer server(80);

// Credenciales WiFi (cámbialas por las tuyas)
const char* ssid = "RULI2019";
const char* password = "RULI1569";

// Función para manejar la raíz (sirve index.html)
void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

// Para servir CSS y JS
void handleFileRequest() {
  String path = server.uri();
  if (path.endsWith(".css")) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, "text/css");
    file.close();
  } else if (path.endsWith(".js")) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, "application/javascript");
    file.close();
  }
}

// Enviar datos del sensor en formato JSON
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

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.println(WiFi.localIP());

  // Rutas del servidor
  server.on("/", handleRoot);
  server.on("/style.css", handleFileRequest);
  server.on("/script.js", handleFileRequest);
  server.on("/data", handleData);
  server.begin();

  Serial.println("Servidor iniciado.");
}

void loop() {
  server.handleClient();
}
