/* Práctica 5: Generación de ondas */

  #include <MsTimer2.h>
  
  // Valores constantes
  const float pi= 3.1416;
  const unsigned int N= 200; // Número de puntos por ciclo
  const unsigned int Ts= 5; // Intervalo entre puntos en ms
  const unsigned int Vm= 50; // Voltaje máximo*10 en V

  // Variables globales
  volatile unsigned int n= 0; // Número del punto (0 a N-1)
  volatile unsigned int ch[6]; // Valores calculados  
  
  void setup()  
  {  
    Serial.begin(1000000);  
    MsTimer2::set(Ts, actualizar); // Int. Timer2 cada Ts ms
    MsTimer2::start();
  }  
      
  void loop() {   
  }

  void actualizar() {
    ch[0] = 512 + 256*sin(n*2*pi/N); // CH1: Seno con amplitud 1,25V y 1 Hz
    ch[1] = 512 + 256*cos(n*20*pi/N); // CH2: Coseno com apmplitud 1,25 y 10 Hz
    ch[2] = 512 + (205 + 102*sin(n*2*pi/N))*cos(n*20*pi/N); // CH3: Onda AM con portadora CH2 y moduladora CH1
    
    //CH4: Onda triangular
    if (n < N/4) ch[3] = (float)512 - 256*(0 - (float)4*n/N); 
    else if (n < 3*N/4) ch[3] = (float)512 + 256*(2 - (float)4*n/N); 
    else if (n < N) ch[3] = (float)512 - 256*(4 - (float)4*n/N);
    
    //CH5 Onda PWM
    if (n < N/5) ch[4] = 768;
    else ch[4] = 256;
    
    //CH6 Onda BPSK
    if (n < N/5) ch[5] = 512 + 256*cos(20*pi*n/N);
    else ch[5] = 512 - 256*cos(20*pi*n/N);
    
    if (n++== N) { // Incrementa número índice del punto
      n= 0;
    }  
    // Envía mensaje de datos cada Ts ms a Visorduino
    Serial.print("A6"); // Cabecera de mensaje
    Serial.write(Ts); // Envía intervalo tiempo Ts  
    Serial.write(Vm); // Envía voltaje máximo   
    for (int i=0; i<6; i++) {
      Serial.write(ch[i] & 0xFF); //  Envía Byte bajo de Chi
      Serial.write((ch[i] >> 8) & 0x03); // y byte alto  
    }
  }
  
