#include <Arduino.h>
#include <Bounce2.h>
#include <string.h>
#define LDR 34
#define BUTTON 27
#define RED 26
#define YELLOW 25
#define GREEN 33

int cnt = 0;
int status[8][3] = {{0,0,0}, {0,0,1}, {0,1,0}, {0,1,1}, {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}};
Bounce debouncer = Bounce();

void solve(int ldr){
  for(int i = 0; i < 3; i++){
    if(status[cnt][i] == 1){
      ledcWrite(i, ldr);
    }else{
      ledcWrite(i, 0);
    }
  }
}

void setup(){
  Serial.begin(115200);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(RED, 0);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(YELLOW, 1);
  ledcSetup(2, 5000, 8);
  ledcAttachPin(GREEN, 2);

  debouncer.attach(BUTTON, INPUT_PULLUP);
  debouncer.interval(25);
}

void loop(){
  debouncer.update();
  if (debouncer.fell()){
    if(++cnt == 8){
      cnt = 0;
    }
  }
  solve(map(analogRead(LDR), 2100, 3700, 0, 255));
}