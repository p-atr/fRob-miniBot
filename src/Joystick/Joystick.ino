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
  button = digitalRead(3);
  Serial.println(analogRead(A7));
  Serial.println("---");
  Serial.println(analogRead(A6));
  /*Serial.print(analogRead(A2));
  Serial.print(", ");
  Serial.print(analogRead(A3));
  Serial.print(", ");
  Serial.println(button);*/
}
