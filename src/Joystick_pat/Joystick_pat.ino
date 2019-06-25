#define xAxis A7
#define yAxis A6

int button;
int left_motor;
int right_motor;

void setup() {
  Serial.begin(115200);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(3, INPUT_PULLUP);
}

void loop() { // put your main code here, to run repeatedly:
joystick();
}

void joystick() {
  int yRaw = analogRead(yAxis);
  int xRaw = analogRead(xAxis);
  bool driveDirection = forward(yRaw);
  int geschwindigkeit = gschwindigkeitjoystick(yRaw);

  bool xDirection = forward(xRaw);
  int xSpeed = gschwindigkeitjoystick(xRaw);

  if (xDirection == true) {
    left_motor = geschwindigkeit - xSpeed;
    right_motor = geschwindigkeit;
  }
  else {
    left_motor = geschwindigkeit;
    right_motor = geschwindigkeit - xSpeed;
  }
  if (left_motor < 0) {
    left_motor = 0;
  }
  if (right_motor < 0) {
    right_motor = 0;
  }
  Serial.print("direction: ");
  Serial.print(driveDirection);
  Serial.print("  left_motor: ");
  Serial.print(left_motor);
  Serial.print("  right_motor: ");
  Serial.println(right_motor);
}

bool forward(int k) { // true wenn forwärts, false wenn rückwärts
  if (k >= 512) {
    return false;
  }
  else {
    return true;
  }
}

int gschwindigkeitjoystick(int p) { // geschwindigkeit zwischen 0 und 255
  if (p >= 550) {
    return (p - 512) / 2;
  }
  else if (p <= 474) {
    return 255 - p / 2;
  }
  else {
    return 0;
  }
}
