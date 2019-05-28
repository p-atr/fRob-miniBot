//NETWORK
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <NewPing.h>
#include <FastLED.h>

//Erkennungs LED
#define NUM_LEDS 7
#define DATA_PIN 4

//CRGB leds[NUM_LEDS];
//int led_colors[7][3] = {{255, 0, 0}, {170, 85, 0}, {85, 170, 0}, {0, 255, 0}, {0, 170, 85}, {0, 85, 170}, {0, 0, 255}};


//RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

//RF24Network network(radio);      // Network uses that radio
//const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
//const uint16_t other_node = 01;    // Address of the other node in Octal format

//struct payload_t {
  //int mes;
//};

int timee2; // konstant Zeit lesen

// noch StandartRGBFarbwerte messen und hier einfügen um zwischen Wand und Auto zu unterscheiden
//int redwall;                    //zwischen 7200 und 3600
//int greenwall;                  //zwischen 7000 und 3600
//int bluewall;                   //zwischen 2600 und 1300
//int standartabwichigwall;

//bool manualSteering = false;
//int manualSteerDirection = 0;

//COLORSENSOR
#include "Wire.h"
#include "veml6040.h"
//VEML6040 RGBWSensor;

//WHEELS
const float leftCirc = 1; //23.5
const float rightCirc = 0.97; //22.8
float steerStrength = 0.5;
const int wheelStp = 155;
bool directionForward = true;
float maxSpeedMultiplier = 0.7;
long whiteMin = 30000;
int steerMultiplier = 1;

int leftCounter;
int rightCounter;

int rightCorrection;

long r = 0;
long g = 0;
long b = 0;


// distanzsensor varible
//HEAD
int Trigpin = A3;
int Echopin = A2;
#define Trigpin A2
#define Echopin A3
int maxdistanz = 200;
float distanz1;
float distanz2;
float speeed;
int standartspeed; // muss noch bestimmt werden
int maxentfernig = 5;
long timespeed1;
long timespeed2;
int abwichigstandartspeed = 0.1;

NewPing sonar(Trigpin, Echopin, maxdistanz);

void incrementLeftCounter() {
  leftCounter += 1;
}

void incrementRightCounter() {
  rightCounter += 1;
}

void setup(void)
{
  //Startwert für distanz1
  distanz1 = sonar.ping_cm();

  //rotation sensor
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //LED
  //pinMode(4,OUTPUT);
  //left
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //right
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);


  //attachInterrupt(digitalPinToInterrupt(2), incrementLeftCounter, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(3), incrementRightCounter, CHANGE);
  leftCounter = 0;
  rightCounter = 0;

  Serial.begin(115200);
  //Serial.println("RF24Network/examples/helloworld_rx/");

 
}






void driveStraight() {
  //STRAIGHT
  motor(255,255);
  /*int steerLeft, steerRight;
  if (leftCounter > rightCounter) {
    motor(255 * steerMultiplier, 255);
    //steerLeft = 255;
    //steerRight = 0;
  } else if (leftCounter < rightCounter) {
    motor(255, 255 * steerMultiplier);
    //steerLeft = 0;
    //steerRight = 255;
  } else {
    motor(255, 255);
    leftCounter = 0;
    rightCounter = 0;
    //steerLeft = 0;
    //steerRight = 0;
  }
  //steerLeft *= steerMultiplier;
  //steerRight *= steerMultiplier;

  //Serial.println("LC:" + String(leftCounter) + "  RC:" + String(rightCounter) + "  SL:" + String(steerLeft) + "  SR:" + String(steerRight));
  //motor(255 - 128, 255 - 128);*/
}
void driveCurve(int steer) {
  //CURVE
  int steerLeft, steerRight;
  if (steer > 0) {
    steerLeft = steer;
    steerRight = 0;
    leftCounter = 0;
    rightCounter = 0;
  } else {
    steerLeft = 0;
    steerRight = -steer;
    leftCounter = 0;
    rightCounter = 0;
  }

  steerLeft *= steerMultiplier;
  steerRight *= steerMultiplier;

  motor(255 - steerLeft, 255 - steerRight);
}

void motor(int leftWheelVoltage, int rightWheelVoltage) {
    analogWrite(5, 0); //leftWheel
    analogWrite(6, 255);//        leftWheelVoltage * leftCirc);
    analogWrite(9, 0); //rightWheel
    analogWrite(10,255);// rightWheelVoltage * rightCirc);
}

void loop(void) {
  driveStraight();
  
  //Serial.println("loop");    // Check the network regularly

  if (millis() > (timespeed1 + 1000)) {
    timespeed2= millis();
    distanz2 = sonar.ping_cm();
    speeed =  10*(distanz1 - distanz2) / (timespeed2 - timespeed1); //Geschwindigkeitsmessung
    Serial.println("speeed: " + String(speeed) + " distanz1: " + String(distanz1) + " distanz2: " + String(distanz2) + " timespeed1: " + String(timespeed1) + " timespeed2: " + String(timespeed2));
    distanz1 = sonar.ping_cm();
    timespeed1 = millis();
  }

  //Serial.println(String(r) + ", " + String(g) + ", " + String(b));

  timee2 = millis();
  
}
