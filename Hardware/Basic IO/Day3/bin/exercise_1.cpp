#include <Arduino.h>
#include <Bounce2.h>

#define GREEN 33
#define BUTTON 27

int count = 0;

Bounce debouncer = Bounce();

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;

void UpdateLED(void *param);
void UpdateButton(void *param);

void setup()
{
  Serial.begin(115200);
  debouncer.attach(BUTTON, INPUT_PULLUP);
  debouncer.interval(25);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(GREEN, 0);

  xTaskCreatePinnedToCore(UpdateLED, "Update_LED", 1000, NULL, 1, &TaskA, 0);
  xTaskCreatePinnedToCore(UpdateButton, "Update_Button", 1000, NULL, 1, &TaskB, 1);
}

void loop()
{
}

void UpdateLED(void *param)
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

void UpdateButton(void *param)
{
  while (1)
  {
    debouncer.update();
    if (debouncer.fell())
    {
      Serial.println(++count);
    }
  }
}