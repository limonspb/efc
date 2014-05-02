#include "EfRc.h"

#include <../PinChangeInt/PinChangeInt.h>

volatile unsigned long time = 0;
volatile char ppm_pins[10] = {A0,A1,A2,A3,A4,2,3,4,5,6};
volatile unsigned long risingT[MAX_PIN_NUMBER];
volatile unsigned long fallingT[MAX_PIN_NUMBER];
volatile unsigned int ppm_delays[MAX_PIN_NUMBER];

#include <math.h>

char ppm_pin_count = 6;

unsigned int getPPMDelay(char i){
  uint32_t now = micros();
  if ((now > fallingT[i] ? now - fallingT[i] : 0) < 500000){
    return ppm_delays[i];
    //return (now > fallingT[i] ? now - fallingT[i] : 0)/1000;//only!!! for testing
  }else{
    return 901;
  }
}

inline char pin2order(char pin){
  for (int i = 0; i<ppm_pin_count; i++){
    if (ppm_pins[i] == pin){
	  return i;
	}
  }
  return 255;
}

unsigned long dt;
char order;

void efRCRising(){
  unsigned long time = micros();
  order = pin2order(PCintPort::arduinoPin);
  risingT[order] = time;    
}

void efRCFalling(){
  unsigned long time = micros();
  order = pin2order(PCintPort::arduinoPin);
  fallingT[order] = time;
  unsigned long dt = fallingT[order] > risingT[order] ? fallingT[order] - risingT[order] : 0;
  if (dt > 500 && dt < 2500){
    ppm_delays[order] = dt;
  }
}

void initEfRc(const char * pins, char count){
  ppm_pin_count = count;
  for (int i = 0; i<ppm_pin_count; i++){
    fallingT[i] = micros();
    risingT[i] = micros();
    ppm_delays[i] = 0;
    ppm_pins[i] = pins[i];
    PCintPort::attachInterrupt(ppm_pins[i], efRCRising, RISING);
    PCintPort::attachInterrupt(ppm_pins[i], efRCFalling, FALLING);
    pinMode(ppm_pins[i], INPUT);
  }
}
