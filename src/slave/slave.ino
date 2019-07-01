#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <NewPing.h>
#include <Wire.h>
#include "veml6040.h"
VEML6040 RGBWSensor;
NewPing sonar(A2, A3, 30);

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 01;      // Address of our node in Octal format
const uint16_t master = 00;         // Address of the other node in Octal format

struct payload_t {                  // Structure of our payload
  int id;
  int left_speed;
  int right_speed;
  bool drive_direction;
  unsigned long node;
};

void veml_setup() {
  Wire.begin();
  if(!RGBWSensor.begin()) {
    Serial.println("ERROR: couldn't detect VEML6040");
    while(1){}
  }
  RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("SLAVE");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  veml_setup();

  //motor_l
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //motor_r
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

int left_speed, right_speed, driveMode;
void network_receive() {
  while (network.available() ) {          // Is there anything ready for us?
    RF24NetworkHeader header;             // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet id #");
    Serial.print(payload.id);
    Serial.print(" from node #");
    Serial.println(payload.node);

    if (payload.id == 0) {
      Serial.println("pong -> 00");
      network_send(00, payload_t {0, 0, 0, 0, this_node});
    }
    else if (payload.id == 1 && payload.drive_direction == true) {
      left_speed = payload.left_speed;
      right_speed = payload.right_speed;
      driveMode = 0;
    }
    else if (payload.id == 1 && payload.drive_direction == false) {
      left_speed = payload.left_speed;
      right_speed = payload.right_speed;
      driveMode = 1;
    }
    else if (payload.id == 2) {
      driveMode = 2;
    }
    else {
      Serial.print("UNKNOWN PACKET FROM NODE #");
      Serial.println(payload.node);
    }
  }
}

//COLLISION
int dist;
long t = 0;
void updateUltrasound() {
  //sonar
  if (millis() >= t + 29) {
    dist = sonar.ping_cm();
    t = millis();
  }
  //Serial.println(dist);
}

int colorLimit = 800;
int updateColorsensor() {
  if(RGBWSensor.getRed() > RGBWSensor.getGreen() && RGBWSensor.getRed() > RGBWSensor.getBlue() && RGBWSensor.getRed() > colorLimit){
    return 0;
  } else if (RGBWSensor.getGreen() > RGBWSensor.getRed() && RGBWSensor.getGreen() > RGBWSensor.getBlue() && RGBWSensor.getGreen() > colorLimit) {
    return 1;
  } else if (RGBWSensor.getBlue() > RGBWSensor.getRed() && RGBWSensor.getBlue() > RGBWSensor.getGreen() && RGBWSensor.getBlue() > colorLimit) {
    return 2;
  }
  /*  if(RGBWSensor.getGreen() > colorLimit){
  }
    return 1;
  } else if(RGBWSensor.getBlue() > colorLimit){
    return 2;
  }*/
  return -1; //no right color
}

bool isColliding() {
  if (dist < 15 && dist > 0) {
    return true;
  }
  return false;
}

//DRIVEMODES
void drive_forward(int left_speed, int right_speed) {
  digitalWrite(5, LOW);
  analogWrite(6, left_speed);
  digitalWrite(9, LOW);
  analogWrite(10, right_speed);
}

void drive_backwards(int left_speed, int right_speed) {
  analogWrite(5, left_speed);
  digitalWrite(6, LOW);
  analogWrite(9, right_speed);
  digitalWrite(10, LOW);
}

void network_send(uint16_t node, payload_t localpayload) {
  Serial.print("Sending...");
  //payload_t payload = payload;
  RF24NetworkHeader header(/*to node*/ node);
  bool ok = network.write(header, &localpayload, sizeof(localpayload));
  if (ok) {
    Serial.println("ok.");
  }
  else {
    Serial.println("failed.");
  }
}

void loop() {
  network.update();                       // Check the network regularly
  network_receive();
  updateUltrasound();

  if(isColliding()){
    int color = updateColorsensor();
    if(color >= 0){
        network_send(00, payload_t {4, color, 0, 0, this_node});
    }
    drive_backwards(0, right_speed);
    
  } else {
    if(driveMode == 0){
      drive_forward(left_speed, right_speed);
    } else if (driveMode == 1){
      drive_backwards(left_speed, right_speed);
    } else {
      drive_forward(0, 0);
    }
  }
}
