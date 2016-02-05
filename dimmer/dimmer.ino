#include <LiquidCrystal.h>
#include <MsTimer2.h>

//Autores:  Enrique Goberna García
//          Diego Noceda Davila

//Se implementa el control de encendido de 4 LEDs a través de interrupciones y una variable de estado
//de cada una que se modifica con el pulsador

/*** Declaración de variables (globales por comodidad para la práctica) ***/
const int pulsador = 7;
const int L0 = A0;
const int L1 = A1;
const int L2 = A2;
const int L3 = A3;

//byte brillo[4] = {0,0,0,0}; //control del brillo de los leds
byte brilloL0 = 0;
byte brilloL1 = 0;
byte brilloL2 = 0;
byte brilloL3 = 0;

byte counter = 0; // 8 bits

//Variables para la gestión de tiempos del pulsador
unsigned long timeIni;
unsigned long timeFin;

// Inicializacion del LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/*****/

void actualizaLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L1:");
  //lcd.setCursor(3,0);
  lcd.print(brilloL0);
  
  lcd.setCursor(8,0);
  lcd.print("L2:");
  //lcd.setCursor(11,0);
  lcd.print(brilloL1);
  
  lcd.setCursor(0, 1);
  lcd.print("L3:");
  //lcd.setCursor(3,1);
  lcd.print(brilloL2);
  
  lcd.setCursor(8,1);
  lcd.print("L4:");
  //lcd.setCursor(11,1);
  lcd.print(brilloL3);
}

void setup() { 
  //Setup de la LCD
  lcd.begin(16, 2);
  analogWrite(6,60);
  actualizaLCD();
  
  //Serial Setup
  Serial.begin(9600);
  
  //Setup de los pines
  pinMode(pulsador, INPUT);
  pinMode(L0, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  
  //Setup del timer
  MsTimer2::set(1, controlPWM); // T2= 1 ms inicial
  MsTimer2::start();
  TCCR2B &= 0xF8; // Pone cero en bits preescaler T2
  TCCR2B |= 0x02; // Pone prescaler T2 :8  
}
 
void loop() {
  if (Serial.available() > 0) {  
    brilloL0 = Serial.parseInt();
    brilloL1 = Serial.parseInt();
    brilloL2 = Serial.parseInt();
    brilloL3 = Serial.parseInt();
    actualizaLCD();
    Serial.flush(); 
  }  
  
  if (digitalRead(pulsador) == HIGH){
    delay(80);
    timeIni = millis();
    while(digitalRead(pulsador) == HIGH){
    timeFin = millis();
    
      if (timeFin - timeIni >= 1000) {
        while(digitalRead(pulsador) == HIGH){
          brilloL0++;
          brilloL1++;
          brilloL2++;
          brilloL3++;
          delay(10); 
          actualizaLCD(); 
        }
      }
    }
    brilloL0++;
    brilloL1++;
    brilloL2++;
    brilloL3++;  
    actualizaLCD();
  }
}

// Interrupción en Timer 2 cada 25 microsegundos
void controlPWM() {
  TCNT2 += 256-50; // Recarga T2 para int. cada 25 us   
  if ((++counter == 0)){
    if (brilloL0) digitalWrite(L0, LOW);
    if (brilloL1) digitalWrite(L1, LOW);
    if (brilloL2) digitalWrite(L2, LOW);
    if (brilloL3) digitalWrite(L3, LOW);   
  }
  if (counter == brilloL0) digitalWrite(L0, HIGH);
  if (counter == brilloL1) digitalWrite(L1, HIGH);
  if (counter == brilloL2) digitalWrite(L2, HIGH);
  if (counter == brilloL3) digitalWrite(L3, HIGH);
}   
