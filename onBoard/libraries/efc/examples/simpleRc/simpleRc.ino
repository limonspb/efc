#include <EfRc.h>

void setup(){
  byte pins[] = {A0, A1};
  initEfRc(pins, 2);
  
  Serial.begin(9600);
}

void loop(){
  delay(200);
  Serial.println(getPPMDelay(0));
  delay(200);
  Serial.println(getPPMDelay(1));
}
