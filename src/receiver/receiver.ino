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
float maxSpeedMultiplier = 0.7;
long whiteMin = 60000;

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
    driveCurve((int)(manualSteerDirection - 512) / 2);
  } else {
    autoSteering();
  }
}

void autoSteering () {

  r = RGBWSensor.getRed();
  g = RGBWSensor.getGreen();
  b = RGBWSensor.getBlue();

  //Serial.println(String(r) + ", " + String(g) + ", " + String(b) + ", " + String(w));

  if (RGBWSensor.getWhite() > whiteMin) {
    Serial.println("White: " + String(RGBWSensor.getWhite()));
    int steer;
    if (r > g) {
      steer = -r;
      Serial.println("Left");
    } else if (b > g) {
      steer = b;
      Serial.println("Right");
    } else {
      steer = 0;
      Serial.println("Stay");
    }
    driveCurve(steer/265);
  } else {
    driveSraight();
  }

}

void incrementLeftCounter() {
  leftCounter += 1;
}

void incrementRightCounter() {
  rightCounter += 1;
}

void driveStraight() {
  //STRAIGHT
  if (leftCounter > rightCounter) {
    steerLeft = 255;
    steerRight = 0;
  } else if (leftCounter < rightCounter) {
    steerLeft = 0;
    steerRight = 255;
  } else {
    leftCounter = 0;
    rightCounter = 0;
    steerLeft = 0;
    steerRight = 0;
  }
  steerLeft *= steerMultiplier;
  steerRight *= steerMultiplier;

  motor(255 - steerLeft, 255 - steerRight);
}
void driveCurve(int steer) {
  //CURVE
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
}

