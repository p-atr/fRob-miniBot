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
//---

void setup() {
  //steering
  //pinMode(A4, INPUT);
  //rotation sensor
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  //left
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //right
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);

  leftLast = digitalRead(3);
  rightLast = digitalRead(4);
  leftCounter = 0;
  rightCounter = 0;
}

void loop() {

  if (digitalRead(3) != leftLast) {
    leftCounter += 1;
    leftLast = !leftLast;
  }
  if (digitalRead(4) != rightLast) {
    rightCounter += 1;
    rightLast = !rightLast;
  }

  rightCorrection = (leftCounter * leftCirc - rightCounter * rightCirc);

  //Serial.println((analogRead(A4) - 511));
  //rightCorrection = *steerStrength;


  int rightStp = 0;
  int leftStp = 0;
  float potent = analogRead(A4) - 512;

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
    if(potent < -20){
      steerMultiplierR = 1-((-potent/512));
      steerMultiplierL = 1;
    } else {
      steerMultiplierL = 1-((potent/512));
      steerMultiplierR = 1;
    }
  }


  //Serial.println(rightCounter);
  //Serial.println(rightCorrection);
  //Serial.println(potent);
  Serial.println(1-abs(potent/512));

  if (directionForward == true) {
    analogWrite(5, 0);
    analogWrite(6, (255 - leftStp) * steerMultiplierR); //(((float)analogRead(A4)-512)/512);
    analogWrite(9, 0);
    analogWrite(10, (255 - rightStp) * steerMultiplierL);
  } else {
    analogWrite(5, 255 - leftStp);
    analogWrite(6, 0);
    analogWrite(9, 255 - rightStp);
    analogWrite(10, 0);
  }

}

