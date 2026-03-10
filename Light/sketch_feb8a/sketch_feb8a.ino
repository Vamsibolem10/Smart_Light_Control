#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

#define RELAY_PIN 23

bool lightState = false;
bool timerActive = false;

// renamed variable to avoid conflict
unsigned long timerStartMillis = 0;
unsigned long timerDurationMillis = 0;

void turnON()
{
  digitalWrite(RELAY_PIN, LOW);
  lightState = true;
  Serial.println("Light ON");
}

void turnOFF()
{
  digitalWrite(RELAY_PIN, HIGH);
  lightState = false;
  timerActive = false;
  Serial.println("Light OFF");
}

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleJS()
{
  File file = SPIFFS.open("/script.js", "r");
  server.streamFile(file, "application/javascript");
  file.close();
}

void handleCSS()
{
  File file = SPIFFS.open("/style.css", "r");
  server.streamFile(file, "text/css");
  file.close();
}

void handleON()
{
  turnON();
  server.send(200, "application/json", "{\"status\":\"on\"}");
}

void handleOFF()
{
  turnOFF();
  server.send(200, "application/json", "{\"status\":\"off\"}");
}

void handleSetDuration()
{
  String body = server.arg("plain");

  StaticJsonDocument<200> doc;
  deserializeJson(doc, body);

  int h = doc["hours"];
  int m = doc["minutes"];
  int s = doc["seconds"];

  timerDurationMillis =
    ((h * 3600UL) +
     (m * 60UL) +
     s) * 1000UL;

  if(timerDurationMillis > 0)
  {
    turnON();
    timerStartMillis = millis();
    timerActive = true;

    Serial.print("Timer started for ");
    Serial.print(h);
    Serial.print("h ");
    Serial.print(m);
    Serial.print("m ");
    Serial.print(s);
    Serial.println("s");
  }

  server.send(200, "application/json", "{\"status\":\"timer_started\"}");
}

void handleStatus()
{
  StaticJsonDocument<200> doc;

  doc["light"] = lightState;
  doc["timerActive"] = timerActive;

  if(timerActive)
  {
    unsigned long elapsed = millis() - timerStartMillis;

    if(elapsed < timerDurationMillis)
      doc["remaining"] = (timerDurationMillis - elapsed) / 1000;
    else
      doc["remaining"] = 0;
  }
  else
  {
    doc["remaining"] = 0;
  }

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  turnOFF();

  if(!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS failed");
    return;
  }

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/script.js", handleJS);
  server.on("/style.css", handleCSS);

  server.on("/on", HTTP_POST, handleON);
  server.on("/off", HTTP_POST, handleOFF);

  server.on("/setDuration", HTTP_POST, handleSetDuration);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();

  Serial.println("Server started");
}

void loop()
{
  server.handleClient();

  if(timerActive)
  {
    unsigned long elapsed = millis() - timerStartMillis;

    if(elapsed >= timerDurationMillis)
    {
      turnOFF();
      Serial.println("Timer Finished");
    }
  }
}
