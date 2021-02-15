  #include "protothreads.h"
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

/*******FUNCTIONS*******/
void MotorL(int pwm); // left motor / motor esquerdo / motor izquierdo
void MotorR(int pwm); // right motor / motor direito / motor derecho
int readDIP(); // read DIP switch / ler chave DIP / leer el interruptor DIP

bool sen1,sen2;
const int choque=230;
const int t=200;
int ant1=666,ant2;
int var = 3000;
bool vborde=0;
bool bandera=0;
bool bandera2=0;

/*******FUNCTIONS - END*******/





/**LEFT MOTOR CONTROL / CONTROLE DO MOTOR ESQUERDO / CONTROL DEL MOTOR IZQUIERDO**/
// pwm = 0 -> stopped / parado / parado
// 0<pwm<=255 -> forward / para frente / seguir adelante
// -255<=pwm<0 -> backward / para tras / seguir espalda
void MotorL(int pwm){
  // leftMotor1=0 and leftMotor2=0 -> stopped / parado / parado 
  // leftMotor1=0 and leftMotor2=1 -> moves forward / avanca / avanzar
  // leftMotor1=1 and leftMotor2=0 -> moves back / recua / retrocede
  // leftMotor1=1 and leftMotor2=1 -> stopped (braked) / parado (travado) / parado (frenado)

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


/**RIGHT MOTOR CONTROL / CONTROLE DO MOTOR DIREITO / CONTROL DEL MOTOR DERECHO**/
// pwm = 0 -> stopped / parado / parado
// 0<pwm<=255 -> forward / frente / adelante
// -255<=pwm<0 -> backward / tras / espalda
void MotorR(int pwm){
  // rightMotor1=0 and rightMotor2=0 -> stopped / parado / parado 
  // rightMotor1=0 and rightMotor2=1 -> moves forward / avanca / avanzar
  // rightMotor1=1 and rightMotor2=0 -> moves back / recua / retrocede
  // rightMotor1=1 and rightMotor2=1 -> stopped (braked) / parado (travado) / parado (frenado)

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

/** read DIP switch / ler chave DIP / leer el interruptor DIP **/
// returns a value between 0 and 15
// retorna um valor entre 0 e 15
// devuelve un valor entre 0 y 15
int readDIP(){
  int n=0;
  if(digitalRead(DIP4)==HIGH)
    n=1;
  if(digitalRead(DIP3)==HIGH)
    n|= (1<<1);
  if(digitalRead(DIP2)==HIGH)
    n|= (1<<2);
  if(digitalRead(DIP1)==HIGH)
    n|= (1<<3);
}






/*******HILOS - STAR*******/
pt sensores;
int sensoresThread(struct pt* pt){
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


pt buscar;
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



pt tiempo;
int tiempoThread(struct pt* pt){
  PT_BEGIN(pt);
    bandera=0;
    PT_SLEEP(pt, var);
    bandera=1;
       
  
  PT_END(pt);
}



pt atacar;
int atacarThread(struct pt* pt){
  PT_BEGIN(pt);
  for(;;){
    if(sen1==1&&sen2==1){
      MotorL(choque);
      MotorR(choque);
    }
      else if(sen1==0&&sen2==1){
        MotorL(200);
        MotorR(100);        
      }
       else if(sen2==0&&sen1==1){
        MotorL(100);
        MotorR(200);
      }
   
    
  PT_YIELD(pt);
  }
  PT_END(pt);
}


pt tiempo2;
int tiempo2Thread(struct pt* pt){
  PT_BEGIN(pt);
  bandera2=0;
  PT_SLEEP(pt,4000);
  bandera2=1;
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



/*******HILOS - END*******/
void setup() {
  //Serial.begin(3200);
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
  PT_INIT(&sensores);
  PT_INIT(&buscar);
  PT_INIT(&tiempo);
  PT_INIT(&atacar);
  PT_INIT(&tiempo2);
  PT_INIT(&evadir);
  PT_INIT(&borde);
  
}

void loop() {


  //////////
  PT_SCHEDULE(sensoresThread(&sensores));
  if(bandera==0){
    PT_SCHEDULE(buscarThread(&buscar));
    PT_SCHEDULE(tiempoThread(&tiempo));
    }
  
  
  else{
    if(bandera2=0){
    PT_SCHEDULE(atacarThread(&atacar));
    PT_SCHEDULE(tiempo2Thread(&tiempo2));
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