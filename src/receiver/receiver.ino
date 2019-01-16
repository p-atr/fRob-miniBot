//NETWORK
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;    // Address of the other node in Octal format

struct payload_t {
  int mes;
};
int manualSteerDirection = 0;

//COLORSENSOR
#include "Wire.h"
#include "veml6040.h"
VEML6040 RGBWSensor;

//WHEELS
const float leftCirc = 23.5; //23.5
const float rightCirc = 22.8; //22.8
float steerStrength = 0.5;
const int wheelStp = 155;
bool directionForward = true;
int autoSteerStrength = 2;
bool manualSteering = false;

bool leftLast;
bool rightLast;

int leftCounter;
int rightCounter;

int rightCorrection;

long r = 0;
long g = 0;
long b = 0;

void setup(void)
{
  //rotation sensor
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //left
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //right
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), incrementLeftCounter, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), incrementRightCounter, CHANGE);
  leftLast = digitalRead(2);
  rightLast = digitalRead(3);
  leftCounter = 0;
  rightCounter = 0;

  Serial.begin(115200);
  Serial.println("RF24Network/examples/helloworld_rx/");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  
  veml_setup();
}

void veml_setup() {
  Wire.begin();
  if (!RGBWSensor.begin()) {
    Serial.println("ERROR: couldn't detect VEML6040");
    while (1) {}
  }
  RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
}

void loop(void) {

  network.update();                  // Check the network regularly


  while ( network.available() ) {     // Is there anything ready for us?

    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println(payload.mes);
    manualSteerDirection = payload.mes;
  }
  if (manualSteering == true) {
    drive((int)manualSteerDirection - 512);
  } else {
    drive(autoSteering());
  }

  r = RGBWSensor.getRed() / 256;
  g = RGBWSensor.getGreen() / 256;
  b = RGBWSensor.getBlue() / 256;

}

int autoSteering () {

  //Serial.println(String(r) + ", " + String(g) + ", " + String(b) + ", " + String(w));

  if (r > g) {
    return -r*autoSteerStrength;
    Serial.println("Left");
  } else if (b > g) {
    return b*autoSteerStrength;
    Serial.println("Right");
  } else {  
    return 0;
    Serial.println("Stay");
  }
}

void incrementLeftCounter() {
  leftCounter += 1;
}

void incrementRightCounter() {
  rightCounter += 1;
}

void drive(int steer) {

  rightCorrection = (leftCounter * leftCirc - rightCounter * rightCirc);

  int rightStp = 0;
  int leftStp = 0;
  //Serial.println(steer);

  float steerMultiplierL = 1;
  float steerMultiplierR = 1;

  if (steer >= -20 and steer <= 20) {
    if (rightCorrection > 0) {
      rightStp = wheelStp;
    }
    else if (rightCorrection < 0) {
      leftStp = wheelStp;
    } else if (rightCorrection == 0) {
      leftCounter = 0;
      rightCounter = 0;
    }
  } else {
    leftStp = 0;
    rightStp = 0;
    if (steer < -20) {
      steerMultiplierR = 1 - ((-steer / 512));
      steerMultiplierL = 1;
    } else {
      steerMultiplierL = 1 - ((steer / 512));
      steerMultiplierR = 1;
    }
  }



  if (directionForward == true) {
    analogWrite(5, 0);
    analogWrite(6, (255 - leftStp) * steerMultiplierR);
    analogWrite(9, 0);
    analogWrite(10, (255 - rightStp) * steerMultiplierL);
  } else {
    analogWrite(5, 255 - leftStp);
    analogWrite(6, 0);
    analogWrite(9, 255 - rightStp);
    analogWrite(10, 0);
  }
}
