#include <Servo.h>
// Bluetooth Commands
// 2 Left , 3 right , 4 forward


#include <SoftwareSerial.h>
// Bluetooth
SoftwareSerial BT(10, 11);
// Servomotores
Servo sCabeza;
Servo sRaycast;

// Nombramos los pines que utilizaremos
const int PIN_LEFT = 2;
const int PIN_RIGHT = 3;
//4 Servo cabeza
const int PIN_REVERSE_RIGHT = 5;
const int PIN_SPEED = 6;
const int PIN_REVERSE_LEFT = 7;
const int PIN_SOUND=8;
//Pines para el sensor ultrasonico
const int PIN_TRIG=12;
const int PIN_ECHO=13;

void setup() {
  BT.begin(9600);
  Serial.begin(9600);
  // Decimos que el servomotor utilizara el pin 4
  sCabeza.attach(4);
  sRaycast.attach(9);
  // Inicializamos los pines en OUTPUT
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  pinMode(PIN_REVERSE_RIGHT, OUTPUT);
  pinMode(PIN_REVERSE_LEFT, OUTPUT);
  pinMode(PIN_SPEED, OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  sCabeza.write(90);
}

// Los grados que va a tener el servo motor
int grades = 0;
int newData = -1;
bool izqRayCast=false;
void loop() {
  // Indicamos que todos los pines empienzan en apagados
  int ACTIVE_LEFT = LOW;
  int ACTIVE_RIGHT = LOW;
  int ACTIVE_REVERSE_LEFT = LOW;
  int ACTIVE_REVERSE_RIGHT=LOW;
  
  if(izqRayCast){
    sRaycast.write(45);
  }else{
    sRaycast.write(135);
  }
  izqRayCast=!izqRayCast;

  // Obtenemos la data del bluetooth
  if (BT.available() > 0) {
    newData = BT.read();
    Serial.write(newData);
  }
  
  if(newData == '7'){
    if(detectWall()<=10){
      ACTIVE_LEFT = HIGH;
      ACTIVE_REVERSE_RIGHT=HIGH;
    }
    else{
      ACTIVE_LEFT = HIGH;
      ACTIVE_RIGHT=HIGH;
    }
  }
  // Para ir a la izquierda
  if ( newData == '3' || newData == '1') {
    ACTIVE_LEFT = HIGH;
    if ( newData == '1' ){
      ACTIVE_REVERSE_RIGHT=HIGH;
    }
  }
  
  // Para ir a la derecha
  if (newData == '2' || newData == '3') {
    ACTIVE_RIGHT = HIGH;
    if(newData == '2'){
      ACTIVE_REVERSE_LEFT=HIGH;
    }
  }
  
  if (newData=='3' && detectWall()<=10){
      ACTIVE_RIGHT=LOW;
      ACTIVE_LEFT=LOW;
  }
  // Codigo para controlar la cabeza
   moveHead();

  // Validamos que no vaya a la izquierda o derecha  , para retroceder
  if (newData == '6' && ACTIVE_LEFT == LOW && ACTIVE_RIGHT == LOW) {
    ACTIVE_REVERSE_LEFT=HIGH;
    ACTIVE_REVERSE_RIGHT=HIGH;
  }

  //Escribimos nuestros pines 
  digitalWrite(PIN_LEFT, ACTIVE_LEFT);
  digitalWrite(PIN_RIGHT, ACTIVE_RIGHT);
  digitalWrite(PIN_REVERSE_LEFT, ACTIVE_REVERSE_LEFT);
  digitalWrite(PIN_REVERSE_RIGHT, ACTIVE_REVERSE_RIGHT);
  analogWrite(PIN_SPEED, 150);
  delay(10);
}

//Controlar cabezas
void moveHead(){
  if (newData == '4' || newData == '5') {
    int izqC = newData == '4';
    int derC = newData == '5';
    int vel = 5;
    grades += (izqC - derC) * vel;
    if(grades!=0 && grades!=359){
    grades = limitRange(grades, 1, 359);
    sCabeza.write(grades);
    }
  }
}




// Funcion auxiliar para limitar el rango
int limitRange(int num, int min, int max) {
  return min(max(num, min), max);
}

//Detectar pared
float detectWall(){
  float duraction=0;
  digitalWrite(PIN_TRIG,HIGH);
  delayMicroseconds(10);
  duraction= pulseIn(PIN_ECHO,HIGH);
  digitalWrite(PIN_TRIG,LOW);
  return ((duraction*10)/292)/2;
}