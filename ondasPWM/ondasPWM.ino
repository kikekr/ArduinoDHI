/* DISPOSITIVOS HARDWARE E INTERFACES
 * Práctica 6: Generación PWM y adquisición de señales *
 * Autores: Enrique Goberna García / Diego Noceda Davila
 * Fecha: 23/04/2015
 * 
 *
 *
 */

#include <MsTimer2.h>

const float pi= 3.1416;
const unsigned int Ts= 5; // Intervalo entre puntos generados ms
const unsigned int Tg= 5; // Intervalo entre puntos leidos
const unsigned int Vm= 50; // Voltaje máximo*10 en V

volatile unsigned int n= 0; // Número del punto (0 a N-1)
volatile unsigned int ch[6]; // Valores calculados  

//Variables de las señales
unsigned const int dc = 512;
unsigned int N= 200; // Número de puntos por ciclo
unsigned int Ns[4] = {200,200,200,200};
unsigned int Am1= 0;
unsigned int Am2= 0;

//Potenciómetros
const int potA1 = A2;
const int potA2 = A3;
const int potF = A4;
//Switches
const int sw1 = 3;
const int sw2 = 2;
//Seleccion de señales
int sel1=0;
int sel2=0;
//PWM
const int pg1 = 9;
const int pg2 = 10;

const int pr1 = A0;
const int pr2 = A1;
//Señales
unsigned int sg1, sr1, sg2, sr2;
float xmed1 = 0, xmed2 = 0, xef1 = 0, xef2 = 0;
float acumMed1 = 0, acumMed2 = 0, acumEf1 = 0, acumEf2 = 0;

void setup() {
  Serial.begin(1000000);  
  
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  
  MsTimer2::set(Ts, actualizar); // Int. Timer2 cada Ts ms
  MsTimer2::start();
}

void loop() {
  Am1 = map(analogRead(potA1), 0, 1023, 0, 511);
  Am2 = map(analogRead(potA2), 0, 1023, 0, 511);
  
  if (digitalRead(sw1) == LOW){
    while(digitalRead(sw1) != HIGH);
    sel1 = sel1+1;
    if (sel1 == 4)
      sel1 = 0;
  }
  
  if (digitalRead(sw2) == LOW){
    while(digitalRead(sw2) != HIGH);
    sel2 = sel2+1;
    if (sel2 == 4)
      sel2 = 0;
  }
  
  Ns[0] = analogRead(potF);
  Ns[1] = analogRead(potF);
  Ns[2] = analogRead(potF);
  Ns[3] = analogRead(potF); 
  N = map(Ns[0] + Ns[1] + Ns[2] + Ns[3] /4, 0, 1023, 50, 4000);
}

void actualizar() {
  if(sel1 == 0)
    sg1 = dc + Am1*sin(n*2*pi/N); 
  if(sel1 == 1){
    if (n < N/4) sg1 = (float)dc - Am1*(0 - (float)4*n/N); 
    else if (n < 3*N/4) sg1 = (float)dc + Am1*(2 - (float)4*n/N); 
    else if (n < N) sg1 = (float)dc - Am1*(4 - (float)4*n/N);  
  }
  if(sel1 == 2){
    if (n<N/2) sg1 = dc - Am1;
    else sg1 = dc + Am1;
  }
  if(sel1 == 3)
     sg1 = Am1*2; 

  if(sel2 == 0)
    sg2 = dc + Am2*sin(n*2*pi/N); 

  if(sel2 == 1){
    if (n < N/4) sg2 = (float)dc - Am2*(0 - (float)4*n/N); 
    else if (n < 3*N/4) sg2 = (float)dc + Am2*(2 - (float)4*n/N); 
    else if (n < N) sg2 = (float)dc - Am2*(4 - (float)4*n/N);
  }
  if(sel2 == 2){
    //sg2 = dc + Am2;
    if (n<N/2) sg2 = dc - Am2;
    else sg2 = dc + Am2;
  }
  if(sel2 == 3)
     sg2 = Am2*2;      

  sg1 = map(sg1, 0, 1023, 0, 255);
  analogWrite(pg1, sg1);
  ch[0] = analogRead(pr1);
  
  sg2 = map(sg2, 0, 1023, 0, 255);
  analogWrite(pg2, sg2);
  ch[1] = analogRead(pr2);

  //Valores medio y eficaz
  acumMed1 = (acumMed1 + ch[0]);
  acumEf1 = (acumEf1 + ch[0]*ch[0]);
  
  acumMed2 = (acumMed2 + ch[1]);
  acumEf2 = (acumEf2 + ch[1]*ch[1]);
  
  if(n++==N){  
    xmed1 = acumMed1/N;
    xmed2 = acumMed2/N;
    acumMed1 = 0;
    acumMed2 = 0;
    xef1 = (sqrt(acumEf1/N));
    xef2 = (sqrt(acumEf2/N));
    acumEf1 = 0;
    acumEf2 = 0;
    n = 0; 
  }
  
  ch[2] = xmed1;
  ch[3] = xmed2;
  ch[4] = xef1;
  ch[5] = xef2;
    
    //if (n++== N) { // Incrementa número índice del punto
   //   n= 0;
   // }  
    // Envía mensaje de datos cada Ts ms a Visorduino
    Serial.print("A6"); // Cabecera de mensaje
    Serial.write(Ts); // Envía intervalo tiempo Ts  
    Serial.write(Vm); // Envía voltaje máximo   
    for (int i=0; i<6; i++) {
      Serial.write(ch[i] & 0xFF); //  Envía Byte bajo de Chi
      Serial.write((ch[i] >> 8) & 0x03); // y byte alto  
    }
 
}
