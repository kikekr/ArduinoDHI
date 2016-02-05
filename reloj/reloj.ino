#include <MsTimer2.h>

#include <MsTimer2.h>
#include <avr/sleep.h>

const int ledRojo = 10;
const int ledVerde = 5;
const int boton = 3;

int estadoLed  = HIGH;
int horas = 0;
int horas_prov;
int minutos = 0;
int minutos_prov;
int segundos = 0;
int segundos_prov;

char leido[9];
char * token;

void setup() {
  Serial.begin(9600);
  
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(boton, INPUT);
  
  MsTimer2::set(1000, contar);
  MsTimer2::start();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(1,interrupcion, RISING);
}

void interrupcion() {
 sleep_disable(); 
}

void loop() {
  if (Serial.available() > 0) {  //Leemos la hora enviada por serial
    Serial.readBytes(leido, 8);
    leido[8] = 0;
    
    token = strtok(leido, ":"); //Valor de las horas
    horas_prov = atoi(token);
      
    token = strtok(0,":"); //Valor de los minutos
    minutos_prov = atoi(token);
      
    token = strtok(0,":"); //Valor de los segundos
    segundos_prov = atoi(token);
      
    if (horas_prov > 23 || minutos_prov > 59 || segundos_prov > 59){
      Serial.println("modo power down!");
      delay(20);
      digitalWrite(ledRojo, LOW);
      digitalWrite(ledVerde,LOW);
      sleep_mode();
    }    
    else {
      horas = horas_prov;
      minutos = minutos_prov;
      segundos = segundos_prov;
    }
      
//    Serial.println(token);
    Serial.flush();
    leido[0] = 0;
 
  }  
}

void contar() {
  digitalWrite(ledVerde, estadoLed);
  digitalWrite(ledRojo, !estadoLed);
  estadoLed = !estadoLed;
  segundos = segundos + 1;
  if (segundos == 60) {
    segundos = 0;
    minutos = minutos + 1;
    if (minutos == 60) {
      minutos = 0;
      horas = horas + 1;
      if (horas == 24) {
        horas = 0;
      }
    }
  }
  Serial.print(horas);
  Serial.print(":"); 
  Serial.print(minutos);
  Serial.print(":");
  Serial.println(segundos);
}
