#include <Servo.h>
// Bluetooth Commands
// 2 Left , 3 right , 4 forward


#include <SoftwareSerial.h>
// Bluetooth
SoftwareSerial BT(10, 11);
// Servomotores
Servo sCabeza;
// Nombramos los pines que utilizaremos
const int PIN_LEFT = 2;
const int PIN_RIGHT = 3;
const int PIN_REVERSE = 5;
const int PIN_SPEED = 6;

void setup() {
  BT.begin(9600);
  Serial.begin(9600);
  // Decimos que el servomotor utilizara el pin 4
  sCabeza.attach(4);
  // Inicializamos los pines en OUTPUT
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  pinMode(PIN_REVERSE, OUTPUT);
  pinMode(PIN_SPEED, OUTPUT);
  sCabeza.write(90);
}

// Los grados que va a tener el servo motor
int grades = 0;
int newData = -1;

void loop() {
  // Indicamos que todos los pines empienzan en apagados
  int ACTIVE_LEFT = LOW;
  int ACTIVE_RIGHT = LOW;
  int ACTIVE_REVERSE = LOW;

  // Obtenemos la data del bluetooth
  if (BT.available() > 0) {
    newData = BT.read();
    Serial.write(newData);
  }
  // Para ir a la izquierda
  if (newData == '1' || newData == '3') {
    ACTIVE_LEFT = HIGH;
    Serial.println("Izquierda");
  }

  // Para ir a la derecha
  if (newData == '2' || newData == '3') {
    ACTIVE_RIGHT = HIGH;
    Serial.println("Derecha");
  }

  // Codigo para controlar el servomotor
  if (newData == '4' || newData == '5') {
    int izqC = newData == '4';
    int derC = newData == '5';
    int vel = 5;

    grades += (izqC - derC) * vel;
    if(grades!=0 && grades!=179){
    grades = limitRange(grades, 1, 179);
    sCabeza.write(grades);
    }

  }

  // Validamos que no vaya a la izquierda o derecha  , para retroceder
  if (newData == '6' && ACTIVE_LEFT == LOW && ACTIVE_RIGHT == LOW) {
    ACTIVE_REVERSE = HIGH;
    Serial.println("Atras");
  }

  //Escribimos nuestros pines 
  digitalWrite(PIN_LEFT, ACTIVE_LEFT);
  digitalWrite(PIN_RIGHT, ACTIVE_RIGHT);
  digitalWrite(PIN_REVERSE, ACTIVE_REVERSE);
  analogWrite(PIN_SPEED, 255);
  delay(10);
}

// Funcion auxiliar para limitar el rango
int limitRange(int num, int min, int max) {
  return min(max(num, min), max);
}