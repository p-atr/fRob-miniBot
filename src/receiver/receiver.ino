#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>


RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;   // Address of the other node in Octal format

struct payload_t {
  int mes;
};

bool leftLast;
bool rightLast;

int leftCounter;
int rightCounter;

int rightCorrection;

//INPUT
const float leftCirc = 23.5; //23.5
const float rightCirc = 22.8; //22.8

float steerStrength = 0.5;
const int wheelStp = 155;

bool directionForward = true;
int mesage = 0;

void setup(void)
{
  //rotation sensor
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  //left
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //right
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  leftLast = digitalRead(3);
  rightLast = digitalRead(4);
  leftCounter = 0;
  rightCounter = 0;

  Serial.begin(115200);
  Serial.println("RF24Network/examples/helloworld_rx/");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void) {

  network.update();                  // Check the network regularly


  while ( network.available() ) {     // Is there anything ready for us?

    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println(payload.mes);
    mesage = payload.mes;
  }
  if(manualSteering == true){
    drive((int)mesage); 
  } else {
    drive(autoSteering());
  }
}

int autoSteering (){
  
}

void drive(int steer) {
  if (digitalRead(3) != leftLast) {
    leftCounter += 1;
    leftLast = !leftLast;
  }
  if (digitalRead(4) != rightLast) {
    rightCounter += 1;
    rightLast = !rightLast;
  }

  rightCorrection = (leftCounter * leftCirc - rightCounter * rightCirc);

  int rightStp = 0;
  int leftStp = 0;
  float potent = steer - 512;
  //Serial.println(steer);
  //Serial.println(potent);

  float steerMultiplierL = 1;
  float steerMultiplierR = 1;

  if (potent >= -20 and potent <= 20) {
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
    if (potent < -20) {
      steerMultiplierR = 1 - ((-potent / 512));
      steerMultiplierL = 1;
    } else {
      steerMultiplierL = 1 - ((potent / 512));
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
