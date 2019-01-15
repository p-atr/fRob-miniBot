// NewPing library gohlt?

#include <NewPing.h>
int Trigpin = 9;
int Echopin = 10;
int maxdistanz = 200;
int distanz1;
int distanz2;
int speeed;
int standartspeed; // muss noch bestimmt werden
int maxentfernig;  // muss noch bestimmt werden
NewPing sonar(Trigpin, Echopin, maxdistanz);
void setup() {
  // put your setup code here, to run once:
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
  Serial.println("gschwindigkeit: "+  speeed);
  delay(50); 
  if (distanz1<10 or distanz2<10) {    //nöd znöch a t wand
  Serial.println("LOOOL");
  }
  if (standartspeed*1.1<speeed or standartspeed*0.9>speeed and sonar.ping_cm()<maxentfernig) { // entdeckig vom auto
  Serial.println("Oha! Da ist mir was im weg -> auto entdeckt");
  }
  
}

