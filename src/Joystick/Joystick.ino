int x;
int y;
int button;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(3, INPUT_PULLUP);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  /*button = digitalRead(3);
  Serial.println(analogRead(A7));
  Serial.println("---");
  Serial.println(analogRead(A6));
  delay(500);
  /*Serial.print(analogRead(A2));
  Serial.print(", ");
  Serial.print(analogRead(A3));
  Serial.print(", ");
  Serial.println(button);*/
  x = analogRead(7);
  y = analogRead(6);
  bool u = forward(y);
  int gschwindigkeit = gschwindigkeitjoystick(y);
  Serial.println(gschwindigkeit);
  
  
}

bool forward(int k){
  bool richtung;
  if (k >=512){
    richtung= true;
  }
  else{
    richtung= false;
    }
   return richtung;
}

int gschwindigkeitjoystick(int p){
  int q;
  if (p>= 550){
    q = (p-512)/2;
    }
  else if (p<=474){
    q = 255 - p/2;
    }
  else {
    q = 0;
    }
  return(q);
  }
  
