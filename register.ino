#include <math.h>

//Индикатор загрузки CPU
#define SHOW_CPU     1 //включить индикатор CPU
#define LATCHPIN_CPU 4 //latch pin
#define CLOCKPIN_CPU 6 //clock pin
#define DATAPIN_CPU  2 //data pin
#define NLEDS_CPU    8 //кол-во led'ов (от 1 до 8)

//Индикатор загрузки MEMORY
#define SHOW_MEM     0 //включить индикатор MEMORY
#define LATCHPIN_MEM 0 //latch pin
#define CLOCKPIN_MEM 0 //clock pin
#define DATAPIN_MEM  0 //data pin
#define NLEDS_MEM    8 //кол-во led'ов (от 1 до 8)

byte nCPU  = 0;
int  nlCPU = 0;
byte nMEM  = 0;
int  nlMEM = 0;

byte bCPU  = 0;
byte bMEM  = 0;

void setup() {
  Serial.begin(9600);
  if(SHOW_CPU){
    pinMode(LATCHPIN_CPU, OUTPUT);
    pinMode(CLOCKPIN_CPU, OUTPUT);
    pinMode(DATAPIN_CPU, OUTPUT);
  }
  if(SHOW_MEM){
    pinMode(LATCHPIN_MEM, OUTPUT);
    pinMode(CLOCKPIN_MEM, OUTPUT);
    pinMode(DATAPIN_MEM, OUTPUT);
  }
}

void loop() {
  while(Serial.available() > 0){
    nCPU = Serial.read();
    nMEM = Serial.read();
    if(SHOW_CPU){
      nlCPU = ceil((float) nCPU / 100.0 * NLEDS_CPU);
      for(int i = 0; i < nlCPU; i++){
        bitSet(bCPU, i);
      }
      digitalWrite(LATCHPIN_CPU, LOW);
      shiftOut(DATAPIN_CPU, CLOCKPIN_CPU, LSBFIRST, bCPU);
      digitalWrite(LATCHPIN_CPU, HIGH);
      bCPU = 0;
    }
    if(SHOW_MEM){
      nlMEM = ceil((float) nMEM / 100.0 * NLEDS_MEM);
      for(int i = 0; i < nlMEM; i++){
        bitSet(bMEM, i);
      }
      digitalWrite(LATCHPIN_MEM, LOW);
      shiftOut(DATAPIN_MEM, CLOCKPIN_MEM, LSBFIRST, bMEM);
      digitalWrite(LATCHPIN_MEM, HIGH);
      bMEM = 0;
    }
  }
  delay(1000);
}
