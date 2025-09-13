#include <Adafruit_NeoPixel.h>

// Salidas Motor driver TA6586-1
#define IN1A 22
#define IN1B 23

// Salidas Motor driver TA6586-2
#define IN2A 4
#define IN2B 16

// Pines TCRT
#define TCR2 35  //ANTIGUO D1
#define TCR1 32  //ANTIGUO D2
#define TCR3 33  //ANTIGUO D3

// pines AD32
#define AD1 25  //ANTIGUO D4 - SENSOR DE LA IZQUIERDA
#define AD2 26  //ANTIGUO D5 - SENSOR DE LA IZQUIERDA A 45°
#define AD3 27  //ANTIGUO D6 - SENSOR DEL CENTRO
#define AD4 14  //ANTIGUO D7 - SENSOR DE LA DERECHA A 45°
#define AD5 13  //ANTIGUO D8 - SENSOR DE LA DERECHA

// Botón que va a estar en input pull up
#define BUTTON_PIN 21

int ledneopixel = 17;
int pinsneopixel = 1;

// PWM's
int PWM1 = 250;  //pwm de la izquierda
int PWM2 = 250;  //pwm de la derecha

// Configuración de PWM para control de motores
const int frequency = 1000;
const int resolution = 8;

// Canales PWM del ESP32
const int ledChannel = 0;
const int ledChannel1 = 1;
const int ledChannel2 = 2;
const int ledChannel3 = 3;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pinsneopixel, ledneopixel, NEO_GRB + NEO_KHZ800);

enum MODOS {  //para moverse y detectar objetos
  ATACAR,
  BUSCARIZQ,
  BUSCARDERE,
  BUSCARIZQ45,
  BUSCARDERE45,
  BUSCARIZQ20,
  BUSCARDER20,
  AVANZ45,
  AVANZ245,
  VELOCISTA,
  SUMO,
  DESPEJA_AREA
};

MODOS set_mode = BUSCARDERE;

void setup() {

  pixels.begin();

  //Configuración de sensores TCRT y pines QTR8A
  pinMode(TCR2, INPUT);
  pinMode(TCR1, INPUT);
  pinMode(TCR3, INPUT);

  //Configuración de sensores AD32 y pines QTR8A
  pinMode(AD1, INPUT);
  pinMode(AD2, INPUT);
  pinMode(AD3, INPUT);
  pinMode(AD4, INPUT);
  pinMode(AD5, INPUT);

  //configuracion del boton de salidas del driver
  pinMode(IN1A, OUTPUT);
  pinMode(IN1B, OUTPUT);
  pinMode(IN2A, OUTPUT);
  pinMode(IN2B, OUTPUT);


  // Configuración de PWM en cada canal y pin

  //ledcAttach(IN1A, frequency, resolution);

  ledcSetup(ledChannel, frequency, resolution);
  ledcAttachPin(IN1A, ledChannel);

  ledcSetup(ledChannel1, frequency, resolution);
  ledcAttachPin(IN1B, ledChannel1);

  ledcSetup(ledChannel2, frequency, resolution);
  ledcAttachPin(IN2A, ledChannel2);

  ledcSetup(ledChannel3, frequency, resolution);
  ledcAttachPin(IN2B, ledChannel3);

  //configuracion de  pin del boton
  pinMode(BUTTON_PIN, INPUT_PULLUP);

 while (digitalRead(BUTTON_PIN == 0)){
 }
 espera();

}

void loop() {
  // put your main code here, to run repeatedly:
  /*if (digitalRead(BUTTON_PIN==1)){
  espera();
  }*/
  search_mode();
 
}

// ==== FUNCIONES ====

void motores(int izq, int der) {  //0 hasta 255 adelate 0 hasta -255 atras

  if (izq >= 0) {
    ledcWrite(ledChannel, izq);
    ledcWrite(ledChannel1, 0);  //analog
  } else {
    izq = izq * (-1);
    ledcWrite(ledChannel1, izq);
    ledcWrite(ledChannel, 1);
  }
  //motor derecho//
  if (der >= 0) {
    ledcWrite(ledChannel2, der);
    ledcWrite(ledChannel3, 0);
  } else {
    der = der * (-1);
    ledcWrite(ledChannel3, der);
    ledcWrite(ledChannel2, 1);
  }
}
void avanzar() {
  ledcWrite(ledChannel, PWM1);   //ledcWrite(IN1A, PWM1);
  ledcWrite(ledChannel1, 0);     // ledcWrite(IN1B, LOW);
  ledcWrite(ledChannel2, PWM2);  //ledcWrite(IN2A, PWM2);
  ledcWrite(ledChannel3, 0);     // ledcWrite(IN2B, LOW);
}
void retroceder() {
  ledcWrite(ledChannel, 0);      //ledcWrite(IN1A, LOW);
  ledcWrite(ledChannel1, PWM1);  //ledcWrite(IN1B, PWM1);
  ledcWrite(ledChannel2, 0);     //ledcWrite(IN2A, LOW);
  ledcWrite(ledChannel3, PWM2);  //ledcWrite(IN2B, PWM2);
}
void girarIzq() {
  ledcWrite(ledChannel, 0);      //ledcWrite(IN1A, LOW);
  ledcWrite(ledChannel1, PWM1);  //ledcWrite(IN1B, PWM1);
  ledcWrite(ledChannel2, PWM2);  //ledcWrite(IN2A, PWM2);
  ledcWrite(ledChannel3, 0);     //ledcWrite(IN2B, LOW);
}
void girarDer() {
  ledcWrite(ledChannel, PWM1);   //ledcWrite(IN1A, PWM1);
  ledcWrite(ledChannel1, 0);     //ledcWrite(IN1B, LOW);
  ledcWrite(ledChannel2, 0);     //ledcWrite(IN2A, LOW);
  ledcWrite(ledChannel3, PWM2);  //ledcWrite(IN2B, PWM2);
}
void detener() {
  ledcWrite(ledChannel, 0);   //ledcWrite(IN1A, LOW);
  ledcWrite(ledChannel1, 0);  //ledcWrite(IN1B, LOW);
  ledcWrite(ledChannel2, 0);  //ledcWrite(IN2A, LOW);
  ledcWrite(ledChannel3, 0);  //ledcWrite(IN2B, LOW);
}
void espera() {
  detener();
  delay(3000);
  pixels.show();
  delay(2000);
  pixels.clear();
}


// ==== DESPEJA AREA ====
void search_mode() {

  if (!digitalRead(TCR1)) {  // Evitar que se salga del tatami
    retroceder();
    delay(500);
    set_mode = BUSCARIZQ;
  } 
  else if (!digitalRead(TCR2)) {
    retroceder();
    delay(500);
    set_mode = BUSCARDERE;
  } 

  // --- Solo usamos el sensor delantero AD3 ---
switch (currentState) {
  case MOVING_FORWARD:
    if (digitalRead(AD3) == 0) {   // Objeto detectado
      Serial.println("Objeto detectado, empujando...");
      moveForward(255);
      time_init = millis();
    } 
    else if (current_time - time_init > 500) {  // No hay objeto
      Serial.println("No se detecta nada, rotando...");
      rotate(PWM);
      rotation_start_time = millis();
      currentState = ROTATING;
    }
    break;

  case ROTATING:
    if (current_time - rotation_start_time >= 500) {
      stopMotors();
      Serial.println("Rotación terminada, buscando...");
      search_start_time = millis();
      currentState = SEARCHING;
    }
    break;

  case SEARCHING:
    if (current_time - search_start_time >= RAMDOM) {
      Serial.println("Buscando, avanzando...");
      moveForward(255);
      time_init = millis();
      currentState = MOVING_FORWARD;
    }
    break;

  case STOPPED:
    stopMotors();
    break;
}

  // --- Movimiento según el modo ---
  switch (set_mode) {
    case ATACAR:
      PWM1 = 220;
      PWM2 = 220;
      avanzar();
      break;

    case BUSCARIZQ:
      PWM1 = 150;
      PWM2 = 200;
      girarIzq();
      break;

    case BUSCARDERE:
      PWM1 = 200;
      PWM2 = 150;
      girarDer();
      break;

    case BUSCARIZQ45:
      PWM1 = 180;
      PWM2 = 200;
      avanzar();
      break;

    case BUSCARDERE45:
      PWM1 = 200;
      PWM2 = 180;
      avanzar();
      break;

    case AVANZ45:
      PWM1 = 120;
      PWM2 = 160;
      avanzar();
      break;

    case AVANZ245:
      PWM1 = 160;
      PWM2 = 120;
      avanzar();
      break;
  }
}
