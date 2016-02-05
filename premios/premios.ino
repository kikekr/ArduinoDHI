


//Autores:   Diego Noceda Davila
//           Enrique Goberna García

//El programa calcula los premios con el algoritmo dado, pinta la secuencia de LEDs a través del 8574A, actualiza la LCD y por último la memoria.
//El watchdog está activado para comprobar el funcionamiento correcto.


/* Entradas que indican los LEDs en el chip
p3 p2 p1 p0

p0 : 1110 = 14
p1 : 1101 = 13
p2 : 1011 = 11
p3 : 0111 = 7

*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>

/*** Declaración de variables (globales por comodidad para la práctica) ***/
int pulsador = 7;

//valores para los leds en el 8574A
byte x[4] = {14,13,11,7};
//byte i = 0;
// Inicializacion del LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//Inicializacion de los premios
byte P1,P2,P3,P4;
byte premio;
/*****/

/*** Funciones auxiliares del programa ***/
//Visualización de los premios restantes en la LCD
void pintarPremios(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P1:");
  lcd.setCursor(3,0);
  lcd.print(P1);
  
  lcd.setCursor(8,0);
  lcd.print("P2:");
  lcd.setCursor(11,0);
  lcd.print(P2);
  
  lcd.setCursor(0, 1);
  lcd.print("P3:");
  lcd.setCursor(3,1);
  lcd.print(P3);
  
  lcd.setCursor(8,1);
  lcd.print("P4:");
  lcd.setCursor(11,1);
  lcd.print(P4);
}

//Algoritmo para el calculo del premio
void calcularPremio(){
  byte p = P1+P2+P3+P4;
  if (p == 0){
   premio = 0; 
  }
  else{
    int n = random(1,p);
    if(n <= P1) { premio=1; P1--;  }
    else 
      if(n <= P1+P2) { premio=2; P2--;  }
      else 
        if(n <= P1+P2+P3) { premio=3; P3--; }
        else { premio=4; P4--; }
  }
}

//Algoritmo de secuencia de los leds
void secuenciaLeds(){ 
  int duracion = 8000;
  int t = 0;
  int espera = 150;
  int paso = 50;
  int i;  
  
  while (t <= 7000){  
    for (i = 0; i<8; i++){
      Wire.beginTransmission(0x38);
      Wire.write(x[i%4]);
      Wire.endTransmission(0x38);
      delay(espera);
      t = t+espera;
    }
    wdt_reset();
    espera = espera+paso;
  }
  while(i%4 != premio-1){
    Wire.beginTransmission(0x38);  //Nos aseguramos de que el último LED encendido es el adecuado
    Wire.write(x[i%4]);
    Wire.endTransmission(0x38);
    delay(espera);
    i++;
  }
  wdt_reset();
}

//Funcion que escribe los premios en la EEPROM
void actualizarEEPROM(){
  EEPROM.write(0,P1);
  EEPROM.write(1,P2);
  EEPROM.write(2,P3);
  EEPROM.write(3,P4);
}

//Manejo del reinicio del sistema
void reinicio(){
  lcd.setCursor(0, 0);
  lcd.print("REINICIANDO");
  delay(500);
  //variables de premios default
  P1 = 1; P2 = 2; P3 = 5; P4 = 10;
  actualizarEEPROM();
  lcd.clear();
  lcd.print("LISTO");
  while(digitalRead(pulsador) != LOW);
}
/*****/

void setup() { 
  //Setup de la LCD
  lcd.begin(16, 2);
  analogWrite(6,60);
  
  //Reinicio del sistema
  if (digitalRead(pulsador) == HIGH){
    reinicio();
  }
  else{
    //Cargo los premios desde la EEPROM
    P1 = EEPROM.read(0);
    P2 = EEPROM.read(1);
    P3 = EEPROM.read(2);
    P4 = EEPROM.read(3);
  }
  
  //Actualiza la LCD con los premios
  pintarPremios();
  
  //Setup del 8574A
  Wire.begin(); 
  Wire.beginTransmission(0x38);
  Wire.write(15);
  Wire.endTransmission(0x38);
  
  //Setup del Watchdog Timer
  wdt_enable(WDTO_8S);
}
 
void loop() {
  if (digitalRead(pulsador) == HIGH){
   
    calcularPremio();
    secuenciaLeds();
    pintarPremios();
    actualizarEEPROM();
    wdt_reset();
    
    //El led que indica el premio se mantiene encendido hasta el nuevo sorteo
    Wire.beginTransmission(0x38);
    Wire.write(x[premio-1]);
    Wire.endTransmission(0x38);
    
//    delay(premio*1000);
    
//    Wire.beginTransmission(0x38);
//    Wire.write(15);
//    Wire.endTransmission(0x38);
  }
  wdt_reset();
}
