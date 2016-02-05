//Autores: Enrique Goberna García - Diego Noceda Davila
//El programa hace una medición de nuestros refejos usando un hardware basado en Arduino
//y el código siguiente.
//El código se estructura en 3 estados y una función de control de interrupción:

//    - En el estado 0:
//      Se enciende el led verde indicando que podemos iniciar y se espera el accionamiento
//      del pulsador. En ese caso se apaga el led verde y se cambia de estado.

//    - En el estado 1:
//      Se espera un tiempo aleatorio para encender el led rojo y también se detectan
//      pulsaciones anticipadas con la función de manejo de interrución
//      Cuando termina el tiempo aleatorio, si no hay pulsaciones anticipadas se cambia 
//      de estado

//    - En el estado 2:
//      Se enciende el led rojo y se inicia el contador de tiempo de nuestros reflejos
//      Se imprimen en el Monitor Serial los resultados obtenidos
        
        


const int pulsador = 3;
const int ledPinVerde = 5;
const int ledPinRojo = 10;
int estado = 0;
int reset = 0;
unsigned long timeIni;
unsigned long timeFin;
unsigned long timeRes;

void setup() {
 Serial.begin(9600);
 pinMode(ledPinVerde, OUTPUT);
 pinMode(ledPinRojo, OUTPUT);
 pinMode(pulsador, INPUT);
}

void loop() {
  if (estado == 0) {
    digitalWrite(ledPinVerde,HIGH); //Encendemos el led verde
    if (digitalRead(pulsador) == HIGH) { //Si detectamos el pulsador
       delay(200);
       if (digitalRead(pulsador) == LOW) { //Cuando se suelte el pulsador
         estado = 1;                   
         digitalWrite(ledPinVerde,LOW); //Apagamos el led verde y vamos a estado1
       }
    }
    
  }
  
  if (estado == 1) { 
//    delay(200);
    attachInterrupt(1, maneja, HIGH);  //Esperamos un tiempo aleatorio
    delay(random(500,1500)); //Detectamos pulsaciones anticipadas
    detachInterrupt(1);      //con interrupciones externas
    if (!reset){
      estado = 2;   //Si no hubo pulsaciones anticipadas pasamos
    }               //al estado2
    reset = 0;  
  }

  if (estado == 2) {     
      digitalWrite(ledPinRojo, HIGH); //Encendemos led rojo
      timeIni = millis();             //Empezamos a contar
      while (digitalRead(pulsador) == LOW); //Detectamos una pulsacion
      
        while (digitalRead(pulsador) == HIGH); //Cuando se levanta el pulsador
                                               //dejamos de contar
        timeFin = millis();
        timeRes = timeFin - timeIni;
        
        digitalWrite(ledPinRojo, LOW);         //y apagamos el led rojo
        estado = 0;
        
        Serial.print(timeRes);        //Comprobamos nuestros reflejos
        Serial.print("ms  ");
        if (timeRes < 100)
          Serial.println("ENHORABUENA! Excelentes reflejos");
        else if (timeRes < 200)
                Serial.println("BIEN! Buenos reflejos");
             else 
                 Serial.println("Intente de nuevo");
  
        delay(1000);  
  }
}
 
void maneja() {
  if(estado == 1)
    if (digitalRead(pulsador) == HIGH) {
     Serial.println("NO SE PRECIPITE. Intente de nuevo");
     reset = 1;
     estado = 0;
  }
}


