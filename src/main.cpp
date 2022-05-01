#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

#define LED_TWO 21
#define LED_PIN 23
#define BTN_PIN 0

#define HTTP_PORT 80

const uint8_t DEBOUNCE_DELAY = 10;

const char *WIFI_SSID = "TG862G72";
const char *WIFI_PASS = "72367236";

String hostname = "Eitan's ESP32";

struct Led
{
  /* data */
  uint8_t pin;
  bool on;

  void update()
  {
    digitalWrite(pin, on ? HIGH : LOW);
  }
};

struct Button
{
  uint8_t pin;
  bool lastReading;
  uint32_t lastDebounceTime;
  uint16_t state;

  bool pressed() { return state == 1; }
  bool released() { return state == 0xffff; }
  bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

  void read()
  {
    bool reading = digitalRead(pin);

    if (reading != lastReading)
    {
      lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > DEBOUNCE_DELAY)
    {
      bool pressed = reading == LOW;
      if (pressed)
      {
        if (state < 0xfffe)
          state++;
        else if (state == 0xfffe)
          state = 2;
      }
      else if (state)
      {
        state = state == 0xffff ? 0 : 0xffff;
      }
    }
    lastReading = reading;
  }
};

Led onboard_led = {LED_TWO, false};
Led led = {LED_PIN, false};
Button button = {BTN_PIN, HIGH, 0, 0};

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

void notifyClients()
{
  // ws.textAll(led.on ? "on" : "off");

  const uint8_t size = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<size> json;
  json["status"] = led.on ? "on" : "off";

  char data[17];
  size_t len = serializeJson(json, data);

  ws.textAll(data, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    // data[len] = 0;
    // if (strcmp((char *)data, "toggle") == 0)
    // {
    //   led.on = !led.on;
    //   notifyClients();
    // }

    const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<size> json;
    DeserializationError err = deserializeJson(json, data);
    if (err)
    {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      return;
    }

    if (json.containsKey("action"))
    {
      const char *action = json["action"];
      Serial.println(action);
      if (strcmp(action, "toggle") == 0)
      {
        led.on = !led.on;
        notifyClients();
      }
    }
    if (json.containsKey("connection"))
    {
      const char *connection = json["connection"];
      if (strcmp(connection, "ping") == 0)
      {
        // const uint8_t size = JSON_OBJECT_SIZE(1);
        // StaticJsonDocument<size> json;
        // json["connection"] = "pong";

        // char data[17];
        // size_t len = serializeJson(json, data);

        // ws.textAll(data, len);
        Serial.println("ping request");
      }
    }
  }
}

void onEvent(AsyncWebSocket *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void *arg,
             uint8_t *data,
             size_t len)
{

  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;

  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;

  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;

  default:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String &var)
{
  return String(var == "STATE" && led.on ? "on" : "off");
}

void onRootRequest(AsyncWebServerRequest *request)
{
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer()
{
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname.c_str());
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());

  if (!MDNS.begin("holamundo"))
  {
    Serial.println("Error starting mDNS");
  }
}

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("Cannot mount SPIFFS volume...");
    while (true)
    {
      onboard_led.on = millis() % 200 < 50;
      onboard_led.update();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  // put your setup code here, to run once:
  pinMode(onboard_led.pin, OUTPUT);
  pinMode(led.pin, OUTPUT);
  pinMode(button.pin, INPUT_PULLUP);
  initSPIFFS();
  initWiFi();
  initWebSocket();
  initWebServer();
}

void loop()
{
  ws.cleanupClients();
  // put your main code here, to run repeatedly:

  // led.on = millis() % 1000 < 500;

  button.read();
  // if (button.held()) led.on = true;
  // else if (button.released()) led.on = false;

  if (button.pressed())
  {
    led.on = !led.on;
    notifyClients();
  }

  onboard_led.on = millis() % 1000 < 50;
  led.update();
  onboard_led.update();
}