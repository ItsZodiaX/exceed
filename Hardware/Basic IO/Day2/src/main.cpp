// If you want to run in WOKWi
// change pin and wifi
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include "traffic.h"

#define red 26
#define yellow 25
#define green 33
#define ldr 34
#define button 27

#define light 100

bool flag = false;
int state = 1;
int count = 0;
Bounce debouncer = Bounce();

void Connect_Wifi();

void setup()
{
  Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(ldr, INPUT);
  debouncer.attach(button, INPUT_PULLUP);
  debouncer.interval(25);
  Connect_Wifi();

  delay(200);
  // start LED with GREEN and POST to database
  // digitalWrite(green, HIGH);
  // POST_traffic("green");
}

void loop()
{
  // *** write your code here ***
  // Your can change everything that you want
  int i = map(analogRead(ldr), 2100, 3700, 0, 255);
  if (state == 3 && flag && i < light)
  {
    flag = false;
    state = 1;
  }
  if (state == 1)
  {
    // while led GREEN
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    POST_traffic("green");
    GET_traffic();
    while (1)
    {
      debouncer.update();
      if (debouncer.fell())
      {
        state = 2;
        break;
      }
    }
  }
  else if (state == 2)
  {
    // while led YELLOW
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    POST_traffic("yellow");
    delay(8000);
    state = 3;
  }
  else if (state == 3 && !flag)
  {
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
    POST_traffic("red");
    GET_traffic();
    delay(5000);
    flag = true;
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