


// include rest of receiver!!!!!!!!!


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <NewPing.h>          //noch einfügen!!!

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
//simi müll:
int Trigpin = 9;
int Echopin = 10;
int maxdistanz = 200;
int distanz1;
int distanz2;
int speeed;
int standartspeed; // muss noch bestimmt werden
int maxentfernig;  // muss noch bestimmt werden
int zit1;
int zit2;
NewPing sonar(Trigpin, Echopin, maxdistanz);
bool neuisekunde;
int i;
void setup(void)
{
  //simi Müll
  pinMode(9, INPUT);
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
void Distanzmessig(){       //Simi Müll
  // gschwindigkeit bestimme
  if(neuisekunde==true){
    distanz1 = sonar.ping_cm();
    zit1=millis();
    neuisekunde=false;
  }
  
  if (i < millis()){
    distanz2 = sonar.ping_cm();
    zit2=millis();
    i = millis()+1000;
    speeed= (distanz1-distanz2)/(zit2-zit1);
    neuisekunde=true;
    Serial.println("gschwindigkeit: "+  speeed);
  }
  
  //Serial.println(distanz1);
  //Serial.println(distanz2);
  
  if (sonar.ping_cm()<10) {    //nöd znöch a t wand
  Serial.println("LOOOL");      // da mues me no s signal sende dass das ding ahalte und richtig ändere mues
  }
  if (standartspeed*1.1<speeed or standartspeed*0.9>speeed and sonar.ping_cm()<maxentfernig) { // entdeckig vom auto
  Serial.println("Oha! Da ist mir was im weg -> auto entdeckt");
  }
}
void loop(void) {
  Distanzmessig();
  network.update();                  // Check the network regularly


  while ( network.available() ) {     // Is there anything ready for us?

    RF24NetworkHeader header;        // If so, grab it by the tits and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println(payload.mes);
    mesage = payload.mes;
  
  }
  drive((int)mesage);
  
    
  }
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
