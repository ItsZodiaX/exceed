#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Bounce2.h>

#define BUTTON 27
#define RED 26
#define GREEN 33

bool light = false;
Bounce debouncer = Bounce();

TaskHandle_t Task1Handle;
TaskHandle_t Task1ExtraHandle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;

void Connect_Wifi();
void Task1(void *param);
void Task1Extra(void *param);
void Task2(void *param);
void Task3(void *param);

void setup()
{
  Serial.begin(115200);

  debouncer.attach(BUTTON, INPUT_PULLUP);
  debouncer.interval(25);

  pinMode(RED, OUTPUT);
  digitalWrite(RED, 0);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(GREEN, 0);

  Connect_Wifi();

  xTaskCreatePinnedToCore(Task2, "Task_2", 1000, NULL, 2, &Task2Handle, 0);
  xTaskCreatePinnedToCore(Task3, "Task_3", 10000, NULL, 1, &Task3Handle, 0);
  xTaskCreatePinnedToCore(Task1, "Task_1", 1000, NULL, 3, &Task1Handle, 1);
}

void loop()
{
}

void Task1(void *param)
{
  while (1)
  {
    debouncer.update();
    if (debouncer.fell())
    {
      light = !light;
      digitalWrite(RED, light);
      if (light)
      {
        xTaskCreatePinnedToCore(Task1Extra, "Task_1Extra", 1000, NULL, 4, &Task1ExtraHandle, 1);
      }
      else
      {
        if (Task1ExtraHandle != NULL)
        {
          vTaskDelete(Task1ExtraHandle);
        }
      }
    }
  }
}

void Task1Extra(void *param)
{
  while (1)
  {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    light = false;
    digitalWrite(RED, light);
    if (Task1ExtraHandle != NULL)
    {
      vTaskDelete(Task1ExtraHandle);
    }
  }
}

void Task2(void *param)
{
  while (1)
  {
    for (int i = 0; i < 255; i++)
    {
      ledcWrite(0, i);
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    for (int i = 255; i > 0; i--)
    {
      ledcWrite(0, i);
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
}

void Task3(void *param)
{
  while (1)
  {
    DynamicJsonDocument doc(2048);
    HTTPClient http;
    http.begin("https://exceed-hardware-stamp465.koyeb.app/question");

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200)
    {
      String payload = http.getString();
      deserializeJson(doc, payload);
      int result = 0;
      int a = doc["a"].as<int>();
      int b = doc["b"].as<int>();
      String op = doc["op"].as<String>();
      String id = doc["questionId"].as<String>();
      if (op == "+")
      {
        result = a + b;
      }
      else if (op == "-")
      {
        result = a - b;
      }
      else if (op == "*")
      {
        result = a * b;
      }
      Serial.printf("a: %d\nb: %d\nop: %s\nresult: %d\nquestionId: %s\n", a, b, op, result, id);

      DynamicJsonDocument doc(2048);
      HTTPClient http;
      String json;

      doc["questionId"] = id;
      doc["result"] = result;
      serializeJson(doc, json);

      http.begin("https://exceed-hardware-stamp465.koyeb.app/answer");
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(json);
      if (httpResponseCode >= 200 && httpResponseCode < 300)
      {
        String payload = http.getString();
        deserializeJson(doc, payload);
        Serial.print("{message: ");
        Serial.print(doc["message"].as<String>());
        Serial.println("}");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
      else
      {
        Serial.print("POST Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else
    {
      Serial.print("GET Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
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