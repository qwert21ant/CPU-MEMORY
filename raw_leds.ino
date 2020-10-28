#include <math.h>

//Индикатор загрузки CPU
#define SHOW_CPU     1 //включить индикатор CPU
#define STARTPIN_CPU 2 //pin к которому подключён 1-ый светодиод
#define NLEDS_CPU    6 //количество светодиодов, подключенные к пинам STARTPIN; STARTPIN + 1; ...

//Индикатор загрузки MEMORY
#define SHOW_MEM     0 //включить индикатор MEMORY
#define STARTPIN_MEM 0 //pin к которому подключён 1-ый светодиод
#define NLEDS_MEM    0 //количество светодиодов, подключенные к пинам STARTPIN; STARTPIN + 1; ...

byte nCPU  = 0;
int  nlCPU = 0;
byte nMEM  = 0;
int  nlMEM = 0;

void setup() {
  Serial.begin(9600);
  if(SHOW_CPU){
    for(int i = STARTPIN_CPU; i < STARTPIN_CPU + NLEDS_CPU; i++){
      pinMode(i, OUTPUT);
    }
  }
  if(SHOW_MEM){
    for(int i = STARTPIN_MEM; i < STARTPIN_MEM + NLEDS_MEM; i++){
      pinMode(i, OUTPUT);
    }
  }
}

void loop() {
  while(Serial.available() > 0){
    nCPU = Serial.read();
    nMEM = Serial.read();
    if(SHOW_CPU){
      nlCPU = ceil((float) nCPU / 100.0 * NLEDS_CPU);
      for(int i = STARTPIN_CPU; i < STARTPIN_CPU + nlCPU; i++){
        digitalWrite(i, HIGH);
      }
      for(int i = STARTPIN_CPU + nlCPU; i < STARTPIN_CPU + NLEDS_CPU; i++){
        digitalWrite(i, LOW);
      }
    }
    if(SHOW_MEM){
      nlMEM = ceil((float) nMEM / 100.0 * NLEDS_MEM);
      for(int i = STARTPIN_MEM; i < STARTPIN_MEM + nlMEM; i++){
        digitalWrite(i, HIGH);
      }
      for(int i = STARTPIN_MEM + nlMEM; i < STARTPIN_MEM + NLEDS_MEM; i++){
        digitalWrite(i, LOW);
      }
    }
  }
  delay(1000);
}
