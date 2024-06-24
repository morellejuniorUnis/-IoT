#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define BUTTON_LEFT 16
#define BUTTON_RIGHT 17
#define LED_CONNECTION 15
#define LED_SECTION_ERROR 0
#define LED_SECTION_SUCCESS 2
#define LED_HANDLE_RESPONSE 5

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const String api_url = "https://faculapi.avlq.online/sections";

String token = "";
bool handle_response = false;
HTTPClient http;

void setup() {
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(LED_CONNECTION, OUTPUT);
  pinMode(LED_SECTION_ERROR, OUTPUT);
  pinMode(LED_SECTION_SUCCESS, OUTPUT);
  pinMode(LED_HANDLE_RESPONSE, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Informe o token:");

  while (token == "") { 
    if (Serial.available() > 0) {
      token = Serial.readStringUntil('\n'); 
    }
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  digitalWrite(LED_CONNECTION, HIGH);
  validateConnection();
}

void loop() {
  if (digitalRead(BUTTON_LEFT) == 0 && handle_response) {
    sendResponse("1");
  }
  if (digitalRead(BUTTON_RIGHT) == 0 && handle_response) {
    sendResponse("2");
  }
}

void validateConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = api_url + "/validateConnection/" + token;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      if (payload == "true") {
        digitalWrite(LED_SECTION_SUCCESS, HIGH);
        handle_response = true;
        digitalWrite(LED_HANDLE_RESPONSE, HIGH);
      } else {
        digitalWrite(LED_SECTION_ERROR, HIGH);
      }
    } else {
      digitalWrite(LED_SECTION_ERROR, HIGH);
    }
    http.end();
  }
}

void sendResponse(String response) {
  if (WiFi.status() == WL_CONNECTED) {
    handle_response = false;
    digitalWrite(LED_HANDLE_RESPONSE, LOW);
    HTTPClient http;
    String url = api_url + "/response";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    String jsonPayload = "{\"key\":\"" + token + "\",\"response\":\"" + response + "\"}";
    int httpCode = http.POST(jsonPayload);
    if (httpCode == 200) {
      String payload = http.getString();
    }
    http.end();
    handle_response = true;
    digitalWrite(LED_HANDLE_RESPONSE, HIGH);
  }
}
