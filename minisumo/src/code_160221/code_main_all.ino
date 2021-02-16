#include "protothreads.h"

#pragma region PINOUT
/*******PINOUT DEFINES*********/
// it is not recommended to make changes
// nao e recomendado que se faca alteracoes
// no se recomienda hacer cambios

// LED
#define LED 6

// left motor
#define pwmL 9
#define leftMotor1 7
#define leftMotor2 8

// right motor
#define pwmR 3
#define rightMotor1 5
#define rightMotor2 4

// DIP switch
#define DIP1 10
#define DIP2 11
#define DIP3 12
#define DIP4 13

// Robocore's line sensor
#define lineL A0
#define lineR A1

// Jsumo's distance sensor
#define distL A2
#define distR A3

// Jsumo's micro-start
#define microST 2

/*******PINOUT DEFINES - END*********/
#pragma endregion 

#pragma region Variables
//Generales
int sen1,sen2; //sensor izquierda, sensor derecha
const int v_max=230; // velocidad máxima
const int t=200;
int ant1=0,ant2=0;

//estrategia 1
//estrategia 2

//estrategia 3
//estrategia 4
bool bandera = 0;
//estrategia 5
int kp = 130,ki = 1,kd = 0;
int pr = 0,in = 0,de = 0;
int last;
int error;
int salida;

#pragma endregion

#pragma region Funciones
/*******FUNCTIONS*******/
// Base
void MotorL(int pwm); // left motor / motor esquerdo / motor izquierdo
void MotorR(int pwm); // right motor / motor direito / motor derecho
int readDIP(); // read DIP switch / ler chave DIP / leer el interruptor DIP
void sensores();

void MotorL(int pwm){
/**LEFT MOTOR CONTROL / CONTROLE DO MOTOR ESQUERDO / CONTROL DEL MOTOR IZQUIERDO**/
// pwm = 0 -> stopped / parado / parado
// 0<pwm<=255 -> forward / para frente / seguir adelante
// -255<=pwm<0 -> backward / para tras / seguir espalda
  // leftMotor1=0 and leftMotor2=0 -> stopped / parado / parado 
  // leftMotor1=0 and leftMotor2=1 -> moves forward / avanca / avanzar
  // leftMotor1=1 and leftMotor2=0 -> moves back / recua / retrocede
  // leftMotor1=1 and leftMotor2=1 -> stopped (braked) / parado (travado) / parado (frenado)
  pwm = constrain(pwm,-255,255);
  if(pwm==0){
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
  }
  else if(pwm<0)
  {
    analogWrite(pwmL, -pwm);
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
  }
  else
  {
    analogWrite(pwmL, pwm);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
  }
}
void MotorR(int pwm){
  /**RIGHT MOTOR CONTROL / CONTROLE DO MOTOR DIREITO / CONTROL DEL MOTOR DERECHO**/
  // pwm = 0 -> stopped / parado / parado
  // 0<pwm<=255 -> forward / frente / adelante
  // -255<=pwm<0 -> backward / tras / espalda
  // rightMotor1=0 and rightMotor2=0 -> stopped / parado / parado 
  // rightMotor1=0 and rightMotor2=1 -> moves forward / avanca / avanzar
  // rightMotor1=1 and rightMotor2=0 -> moves back / recua / retrocede
  // rightMotor1=1 and rightMotor2=1 -> stopped (braked) / parado (travado) / parado (frenado)
  pwm = constrain(pwm,-255,255);
  if(pwm==0){
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
  }
  else if(pwm<0)
  {
    analogWrite(pwmR, -pwm);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
  }
  else
  {
    analogWrite(pwmR, pwm);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
  }
}
int readDIP(){
/** read DIP switch / ler chave DIP / leer el interruptor DIP **/
// returns a value between 0 and 15
// retorna um valor entre 0 e 15
// devuelve un valor entre 0 y 15
  int n=0;
  if(digitalRead(DIP4)==HIGH)
    n=1;
  if(digitalRead(DIP3)==HIGH)
    n|= (1<<1);
  if(digitalRead(DIP2)==HIGH)
    n|= (1<<2);
  if(digitalRead(DIP1)==HIGH)
    n|= (1<<3);
   return n;
}
void sensores(){
    sen1=digitalRead(distL);
    sen2=digitalRead(distR);
}

// Estrategia 1
// Estrategia 5
void manejo(int salida);
void sensores2();
bool isLinea();

void manejo(int salida){
  if(salida<0){
    MotorL(-salida-v_max);
    MotorR(salida+v_max);

  }else if( salida>0){
    MotorL(salida+v_max);
    MotorR(-salida-v_max);

  }else{
    MotorL(v_max);
    MotorR(v_max);
  }
  
}
void sensores2(){
  senl=-digitalRead(distL);
  send=digitalRead(distR);

  if((send+senl)==0){
    if(last == 1){
      send = 1;
    } else if(last == -1){
      senl = -1;
    }
  }
  last = senl+send;
}
bool isLinea(){

  return (digitalRead(lineL)+digitalRead(lineR))!=0;

}

// Estrategias 3 y 4
pt sensor;
pt buscar;
pt tiempo;
pt atacar;
pt curva;

int sensorThread(struct pt* pt);
int buscarThread(struct pt* pt);
int tiempoThread(struct pt* pt);
int atacarThread(struct pt* pt);
int curvaThread(struct pt* pt);

int sensorThread(struct pt* pt){
  PT_BEGIN(pt);
  for(;;){
    sen1=digitalRead(distL);
    sen2=digitalRead(distR);
    PT_YIELD(pt);
    /*
  Serial.print(sen1);
    Serial.print("       ");
    Serial.println(sen2);*/
  }
  PT_END(pt);
}
int buscarThread(struct pt* pt){
  PT_BEGIN(pt);
  for(;;){
      if(sen1==0&&sen2==1){
        MotorL(10);
        MotorR(-150);        
      }
      else if(sen1==1&&sen2==0){
        MotorL(-150);
        MotorR(10);
      }
      else{
        MotorL(0);
        MotorR(0);
      }
   PT_YIELD(pt);
  }
  PT_END(pt);
}
int tiempoThread(struct pt* pt){
  PT_BEGIN(pt);
    bandera=0;
    if (readDIP() == 3)
    {
      PT_SLEEP(pt, 3000);
    }
    else
    {
      PT_SLEEP(pt, 1500);
    }
    bandera=1;  
  
  PT_END(pt);
}
int atacarThread(struct pt* pt){
  PT_BEGIN(pt);
  for(;;){
    if(sen1==1&&sen2==1){
      MotorL(v_max);
      MotorR(v_max);
    }
      else if(sen1==0&&sen2==1){
        MotorL(200);
        MotorR(70);        
      }
       else if(sen2==0&&sen1==1){
        MotorL(70);
        MotorR(200);
      }
   
    
  PT_YIELD(pt);
  }
  PT_END(pt);
}
int curvaThread(struct pt* pt){
  PT_BEGIN(pt);
  for(;;){
      if(sen1==1&&sen2==1){
        bandera=1;
      }
      else if(sen1==0&&sen2==1){
        MotorL(150);
        MotorR(40);        
      }
      else if(sen1==1&&sen2==0){
        MotorL(40);
        MotorR(150);
      }
      else{bandera=0;}
   
   PT_YIELD(pt);
  }
  PT_END(pt);
}


int vborde=0;
pt borde;
int bordeThread(struct pt* pt){
  PT_BEGIN(pt);
  if((digitalRead(lineL)==1||digitalRead(lineR)==1)&&(vborde=1)){
   MotorL(100);
   MotorR(100);
   PT_SLEEP(pt,400);
  }
  else if((digitalRead(lineL)==1||digitalRead(lineR)==1)&&(vborde=0)){
   MotorL(-100);
   MotorR(-100);
   PT_SLEEP(pt,400);
  }
  PT_END(pt);
}
pt evadir;
int evadirThread(struct pt* pt){
  PT_BEGIN(pt);
  vborde=1;
  MotorL(-200);
  MotorR(-80);
  PT_SLEEP(pt,500);
  PT_END(pt);
}
#pragma endregion
int bandera2=0;
int tiempo2=0;
void setup() {

  /****************PINOUT CONFIG****************/
  // OUTPUTS
  pinMode(LED, OUTPUT);         // led

  // right motor
  pinMode(pwmR, OUTPUT);        // right motor power
  pinMode(rightMotor1, OUTPUT); // right motor dir.
  pinMode(rightMotor2, OUTPUT); // right motor dir.

  // left motor
  pinMode(pwmL, OUTPUT);        // left motor power
  pinMode(leftMotor1, OUTPUT);  // left motor dir.
  pinMode(leftMotor2, OUTPUT);  // left motor dir.


  // INPUTS: DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  // DIP switch
  pinMode(DIP1, INPUT_PULLUP);  // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  pinMode(DIP2, INPUT_PULLUP);  // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  pinMode(DIP3, INPUT_PULLUP);  // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  pinMode(DIP4, INPUT_PULLUP);  // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR

  // line sensor
  pinMode(lineL, INPUT); // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  pinMode(lineR, INPUT); // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR

  // distance sensor
  pinMode(distR, INPUT); // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  pinMode(distL, INPUT); // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR

  // micro-start
  pinMode(microST, INPUT); // DO NOT CHANGE / NAO MUDAR / NO CAMBIAR
  /****************PINOUT CONFIG - END***************/

  /***************INITIAL CONDITIONS*****************/
  digitalWrite(LED, LOW); // LED off / LED desligado / LED apagado 
  MotorL(0); // left motor stopped / motor esquerdo parado / motor izquierdo parado 
  MotorR(0); // right motor stopped / motor direito parado / motor derecho parado 

    
  /*************INITIAL CONDITIONS - END*************/

/*************HILOS-CONFIG - STAR*************/
  PT_INIT(&sensor);
  PT_INIT(&buscar);
  PT_INIT(&curva);
  PT_INIT(&tiempo);
  PT_INIT(&atacar);
  PT_INIT(&borde);
}

#pragma region estrategias
// Fast 1
void estrategia1(){
   if(digitalRead(microST)==HIGH){
    while(true){
  sensores();
    // Ataque
    if (sen1 == 1 && sen2 == 1)
    {
       while(true){ 
          sensores();
          if(sen1==1&&sen2==1){//Si el choque está en frente
            MotorL(v_max);
            MotorR(v_max);
          }
          if(sen1==0&&sen2==1){ //Si el coche está a la derecha
              MotorL(200);
              MotorR(150);
              sensores();
          }
          if(sen2==0&&sen1==1){ //Si el coche está a la izquierda
              MotorL(150);
              MotorR(200);
              if(sen1==1&&sen2==0){
        MotorL(0);
        MotorR(180);
        delay(t);
        ant1=1;
    }
    if(sen1==0&&sen2==1){
        MotorL(180);
        MotorR(0);
        delay(t);
        ant1=0;
    }
    if(sen1==0&&sen2==0){
        if(ant1==1){
            MotorL(-100);
            MotorR(100);
        }
        if(ant1==555){
          MotorL(60);
          MotorR(60);
        }
        else{
             MotorL(100);
             MotorR(-100);
        }
    }
    ant1=555;///ant2=0;
    
    if(analogRead(lineL)<=626||analogRead(lineR)<=626){
      MotorL(-100);
      MotorR(-100);
      delay(400);
    }sensores();
          }
          if(sen1==0&&sen2==0){ //Si no encuentra el coche
              break;
          }
       }
    }
    // Busqueda 
    MotorL(150);
    MotorR(150);
if(digitalRead(microST)==LOW){
          MotorL(0);
          MotorR(0);
          for(;;){}
        }
  
}
   }}
int var=0;
// Fast 2
void estrategia2(){
  //////////
  PT_SCHEDULE(sensorThread(&sensor));
  if(bandera==0){
    PT_SCHEDULE(buscarThread(&buscar));
    PT_SCHEDULE(tiempoThread(&tiempo));
    }
  
  
  else{
    if(bandera2=0){
    PT_SCHEDULE(atacarThread(&atacar));
    PT_SCHEDULE(tiempoThread(&tiempo));
    PT_SCHEDULE(bordeThread(&borde));
    }
    else{
    PT_SCHEDULE(evadirThread(&evadir));
    PT_SCHEDULE(bordeThread(&borde));
    var = 500;
    bandera=0;
    }
    }
  
  ////////////
  }

// Slow 
void estrategia3(){
    if(digitalRead(microST)==HIGH){
      while(true){
        //////////
        PT_SCHEDULE(sensorThread(&sensor));
        if(bandera==0){
          PT_SCHEDULE(buscarThread(&buscar));
          PT_SCHEDULE(tiempoThread(&tiempo));
          }
        
        
        else{
          PT_SCHEDULE(atacarThread(&atacar));}
        
        ////////////
        
        if(analogRead(lineL)<=626||analogRead(lineR)<=626){
        MotorL(-150);
        MotorR(-150);
        delay(600);
        MotorL(-100);
        MotorR(-10);
        delay(400);
        }
        if(digitalRead(microST)==LOW){
          MotorL(0);
          MotorR(0);
          for(;;){}
        }
      }
    }
  }


// Slow 2
void estrategia4(){
  
    if(digitalRead(microST)==HIGH){
      while(true){
        //////////
        PT_SCHEDULE(sensorThread(&sensor));
        if(bandera==0){
          MotorL(70);
          MotorR(170);
          PT_SCHEDULE(curvaThread(&curva));
          PT_SCHEDULE(tiempoThread(&tiempo));
          }
        
            
        else{
          PT_SCHEDULE(atacarThread(&atacar));}
        
        ////////////
        

        else if(analogRead(lineL)<=626&&analogRead(lineR)>=726){
        MotorL(-100);
        MotorR(100);
        delay(400);
        }
        else if(analogRead(lineL)>=726&&analogRead(lineR)<=626){
        MotorL(100);
        MotorR(-100);
        delay(400);
        }

        
        
        if(digitalRead(microST)==LOW){
          MotorL(0);
          MotorR(0);
          for(;;){}
        }
      }
    }
  }

  void estrategia5(){
        /////para que el arbitro inicie la peelea:

    if(digitalRead(microST)==HIGH){
      while(true){
    sensores2();
    if(isLinea()){
      MotorL(-v_max);
      MotorR(-v_max+100);
      delay(500);
    }

    error = send + senl;
    pr = error;
    in = error + in;
    in = constrain(in,-100,100);
    de = error-de;

    salida = pr*kp+in*ki+de*kd;
    manejo(salida);
    de = error;

    /////para que el arbitro pare la peelea:
    if(digitalRead(microST)==LOW){
          MotorL(0);
          MotorR(0);
          for(;;){}
        }

  }
  }
  }

#pragma endregion

void loop() {
  if (readDIP()==1){
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    while(true)
    estrategia1();
  }
  else if (readDIP()==2){
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    while(true)
    estrategia2();
  }
  else if (readDIP()==3){
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    while(true)
    estrategia3();
  }else if (readDIP()==4){
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
     delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    while(true)
    estrategia4();
  }else{
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
     delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
     delay(500);
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    while(true)
    estrategia5();

  }
}
