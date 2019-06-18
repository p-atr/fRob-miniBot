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
  x = analogRead(7);
  y = analogRead(6);
  bool u = forward(y);
  int gschwindigkeit = gschwindigkeitjoystick(y);
  int richtung = richtungbestimmung(x);
  Serial.print(u);
  Serial.print(":  ");
  Serial.print(y);
  Serial.print("  ");
  Serial.print(gschwindigkeit);
  Serial.print("  ");
  Serial.println(richtung);
}
// true wenn forwärts  false wenn rückwärts
bool forward(int k) {
  bool richtung;
  if (k >= 512) {
    richtung = false;
  }
  else {
    richtung = true;
  }
  return richtung;
}

// geschwindigkeit zwischen 0 und 255
int gschwindigkeitjoystick(int p) {
  int q;
  if (p >= 550) {
    q = (p - 512) / 2;
  }
  else if (p <= 474) {
    q = 255 - p / 2;
  }
  else {
    q = 0;
  }
  return (q);
}

int richtungbestimmung (int l) {
  l = l / 4;
  return l;
}
