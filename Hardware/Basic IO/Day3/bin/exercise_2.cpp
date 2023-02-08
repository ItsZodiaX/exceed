#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define GREEN 33
int ledDelay = -1;

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;

void Connect_Wifi();
void UpdateLED(void *param);
void GET_Delay(void *param);

void setup()
{
  Serial.begin(115200);

  pinMode(GREEN, OUTPUT);

  Connect_Wifi();

  xTaskCreatePinnedToCore(GET_Delay, "GET_Delay", 10000, NULL, 1, &TaskA, 0);
  xTaskCreatePinnedToCore(UpdateLED, "Update_LED", 1000, NULL, 1, &TaskB, 1);
}

void loop()
{
}

void UpdateLED(void *param)
{
  while (1)
  {
    Serial.println(ledDelay);
    if (ledDelay != -1)
    {
      digitalWrite(GREEN, 1);
      vTaskDelay(ledDelay / portTICK_PERIOD_MS);
      digitalWrite(GREEN, 0);
      vTaskDelay(ledDelay / portTICK_PERIOD_MS);
    }
  }
}

void GET_Delay(void *param)
{
  while (1)
  {
    DynamicJsonDocument doc(2048);
    HTTPClient http;
    http.begin("https://exceed-hardware-stamp465.koyeb.app/leddelay");

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200)
    {
      String payload = http.getString();
      deserializeJson(doc, payload);
      ledDelay = doc["value"].as<int>();
      Serial.print("Delay: ");
      Serial.println(ledDelay);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  }
}

void Connect_Wifi()
{
  const char *ssid = "ZodiaX";
  const char *password = "poom1234";
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("OK! IP=");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");
}