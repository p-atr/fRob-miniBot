//NETWORK
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <NewPing.h>
#include <FastLED.h>

//Erkennungs LED
#define NUM_LEDS 7
#define DATA_PIN 4

CRGB leds[NUM_LEDS];
int led_colors[7][3] = {{255,0,0},{170,85,0},{85,170,0},{0,255,0},{0,170,85},{0,85,170},{0,0,255}};


RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;    // Address of the other node in Octal format

struct payload_t {
  int mes;
};

int timee2; // konstant Zeit lesen
int timee1;
int timee; // random zeit bis Kurven fahren beendet

//farbsensor<->Wand Erkennung
int redwall=5400;                    //zwischen 7200 und 3600
int greenwall=5300;                  //zwischen 7000 und 3600
int bluewall=1950;                   //zwischen 2600 und 1300
int standartabwichigwallred = 1800;
int standartabwichigwallgreen = 1700;
int standartabwichigwallblue = 650;

bool manualSteering = false;
int manualSteerDirection = 0;

//COLORSENSOR
#include "Wire.h"
#include "veml6040.h"
VEML6040 RGBWSensor;

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
int distanz1;
int distanz2;
int speeed;
int standartspeed= 0.35;
int maxentfernig = 5; 
int timespeed1;
int timespeed2; 
int abwichigstandartspeed = 0.12; //abweichung absolut in metern

//Steuerungvariabel
bool Wechsel = false;

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
  distanz1= sonar.ping_cm();
  
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

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

  veml_setup();
}

// LED um Auto 
void led_setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  for(int i = 0; i<NUM_LEDS; i++){
    leds[i] = CRGB(led_colors[i][0],led_colors[i][1],led_colors[i][2]);
  }
  FastLED.show();
}


void veml_setup() {
  Wire.begin();
  if (!RGBWSensor.begin()) {
    //Serial.println("ERROR: couldn't detect VEML6040");
    while (1) {}
  }
  RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
}


void driveStraight() {
  //STRAIGHT
  int steerLeft, steerRight;
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
  //motor(255 - 128, 255 - 128);
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
  if (directionForward == true) {
    analogWrite(5, 0); //leftWheel
    analogWrite(6, leftWheelVoltage * leftCirc);
    analogWrite(9, 0); //rightWheel
    analogWrite(10, rightWheelVoltage * rightCirc);
  } else {
    analogWrite(5, leftWheelVoltage * leftCirc);//leftWheel
    analogWrite(6, 0);
    analogWrite(9, leftWheelVoltage * rightCirc); //rightWheel
    analogWrite(10, 0);
  }
}

void loop(void) {
  Serial.println("speeed: " + String(speeed));
  //Serial.println("loop");
  network.update();                  // Check the network regularly

  if (( timee2) > timee + timee1) { //wieder gerade fahren nach drehen
      driveStraight();
      Wechsel= false;
      }
  if (millis() > (timespeed1 + 1000)) {//Geschwindigkeitsmessung
    timespeed2= millis();
    distanz2 = sonar.ping_cm();
    speeed =  10*(distanz1 - distanz2) / (timespeed2 - timespeed1); //Geschwindigkeitsmessung
    Serial.println("speeed: " + String(speeed) + " distanz1: " + String(distanz1) + " distanz2: " + String(distanz2) + " timespeed1: " + String(timespeed1) + " timespeed2: " + String(timespeed2));
    distanz1 = sonar.ping_cm();
    timespeed1 = millis();
  }

  //Serial.println(String(r) + ", " + String(g) + ", " + String(b));

  timee2= millis();
  led_setup();
  while ( network.available() ) {     // Is there anything ready for us?

    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    //Serial.println(payload.mes);
    manualSteerDirection = payload.mes;
  }
  if (manualSteering == true) {
    driveCurve((int)(manualSteerDirection - 512) / 2);
  } else {
    //Serial.println("loopElse");
    autoSteering();
  }
}

void autoSteering () {          //falls Wand drehe zufällig, sonst prüfe ob Slave und folgen, sonst gerade aus

  r = RGBWSensor.getRed();
  g = RGBWSensor.getGreen();
  b = RGBWSensor.getBlue();

  
  //Serial.println(String(r) + ", " + String(g) + ", " + String(b) + ", " + String(RGBWSensor.getWhite()));

  // looks if car is to close to wall
  if ((sonar.ping_cm() > maxentfernig ) and ((standartspeed-abwichigstandartspeed)<speeed<(standartspeed+abwichigstandartspeed)) and(abs(r - redwall) < standartabwichigwallred) and (abs(g - greenwall) < standartabwichigwallgreen) and (abs(b - bluewall) < standartabwichigwallblue)){
    //Serial.print("Change direction");
    if (Wechsel== false) {
      driveCurve(511);
      Wechsel= true;
      timee1 = millis();
      timee = random(1000, 7000);
    }
  
  }
  /*else if {  //prüfen ob er ein "Slave" ist falls ja programm unten
    if (RGBWSensor.getWhite() > whiteMin) {
      //Serial.println("White: " + String(RGBWSensor.getWhite()));
      int steer;
      if (r > g and r > b) {
        steer = -r;
        //Serial.println("Left");
      } else if (b > g and b > r) {
        steer = b;
        //Serial.println("Right");
      } else {
        steer = 0;
        //Serial.println("Stay");
      }
      driveCurve(steer / 265); //input between -511 and 511 maximum turns 90 degree left/right
    } 
  }*/
  else {
      //Serial.println("autoSteering");
      driveStraight();
    }
  
}
