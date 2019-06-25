#include <NewPing.h>
const int Trigpin = 9;
const int Echopin = 10;
const int maxdistance = 200;
NewPing sonar(Trigpin, Echopin, maxdistance);

int distanz1;
int distanz2;
int speeed;
int standardspeed; // muss noch bestimmt werden
int maxentfernig;  // muss noch bestimmt werden

void setup() {
  pinMode(9, INPUT);
  Serial.begin(19200);
}

void loop() {
  
  // gschwindigkeit bestimme
  distanz1 = sonar.ping_cm();
  //Serial.println(distanz1);
  delay(1000);
  distanz2 = sonar.ping_cm();
  //Serial.println(distanz2);
  speeed= (distanz1-distanz2)/1;
  Serial.println("gschwindigkeit: " +  speeed);
  
  if (distanz1<10 or distanz2<10) {    //nöd znöch a t wand
  Serial.println("LOOOL");
  }
  
  if (standardspeed*1.1<speeed or standardspeed*0.9>speeed and sonar.ping_cm()<maxentfernig) { // entdeckig vom auto
  Serial.println("Oha! Da ist mir was im weg -> auto entdeckt");
  }
}
