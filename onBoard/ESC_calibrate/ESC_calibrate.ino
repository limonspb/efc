#include <Servo.h>

#define ESC_MAX 1900
#define ESC_MIN 1100

Servo s[4];
void setup(){
  Serial.begin(115200);
  
  s[0].attach(A0);
  s[1].attach(A1);
  s[2].attach(A2);
  s[3].attach(A3);


  for (int i = 0; i < 4; i++)  {
    s[i].writeMicroseconds(1000);
  }
  
  Serial.println("Turn off ESC power...");
  
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); 
    
  for (int i = 0; i < 4; i++)  {
    s[i].writeMicroseconds(ESC_MAX);
  }
  
  Serial.println("Turn ON ESC power...");
  
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); 

  for (int i = 0; i < 4; i++)  {
    s[i].writeMicroseconds(ESC_MIN);
  }

  Serial.println("Finished!");
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); 
}

int p = ESC_MIN;
int dp = 10;

void loop(){
  p += dp;
  if (p > ESC_MAX){
    p = ESC_MAX;
    dp = -dp;
  }
  
  if (p < ESC_MIN){
    p = ESC_MIN;
    dp = -dp;    
  }
  for (int i = 0; i < 4; i++)  {
    s[i].writeMicroseconds(p);
  }
  delay(50);
}
