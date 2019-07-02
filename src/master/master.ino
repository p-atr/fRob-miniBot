#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define xAxis A7
#define yAxis A6

int left_motor;
int right_motor;
long tTime = 0;

String command;

RF24 radio(7, 8);                  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);        // Network uses that radio
const uint16_t this_node = 00;     // Address of our node in Octal format ( 04,031, etc)

struct payload_t {                 // Structure of our payload
  int id;
  int left_speed;
  int right_speed;
  bool drive_direction;
  unsigned long node;
};

void setup(void) {
  Serial.begin(115200);
  Serial.println("MASTER");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
}

void network_send(uint16_t node, payload_t localpayload) {
  //Serial.print("Sending...");
  //payload_t payload = payload;
  RF24NetworkHeader header(/*to node*/ node);
  bool ok = network.write(header, &localpayload, sizeof(localpayload));
  if (ok) {
    //Serial.println("ok.");
  }
  else {
    //Serial.println("failed.");
  }
}

void network_receive() {
  while (network.available() ) {          // Is there anything ready for us?
    RF24NetworkHeader header;             // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    if (payload.id == 0) {
      Serial.print("Received packet from node #");
      Serial.println(payload.node);
    }
    if (payload.id == 4) {
      Serial.print("Received color-packet from node #");
      Serial.print(payload.node);
      Serial.print(" including color: ");
      Serial.println(payload.left_speed);
      if (payload.left_speed == 0) {
        network_send(2, payload_t {3, 0, 0, 0, this_node});
      } else if (payload.left_speed == 2) {
        network_send(3, payload_t {3, 0, 0, 0, this_node});
      }
      
    }
    else {
      Serial.print("UNKNOWN PACKET FROM NODE #");
      Serial.print(payload.node);
      Serial.print(" WITH ID #");
      Serial.println(payload.id);

    }
  }
}
void command_handler() {
  if (Serial.available()) {
    Serial.setTimeout(2);
    command = Serial.readString();
    if (command == "ping\n") {
      Serial.println("ping -> 01");
      network_send(01, payload_t {0, 0, 0, 0, this_node});
      network_send(02, payload_t {0, 0, 0, 0, this_node});
      network_send(03, payload_t {0, 0, 0, 0, this_node});
    }
    else if (command == "start\n") {
      Serial.println("start -> 01");
      network_send(01, payload_t {1, 255, 255, 1, this_node});
      network_send(02, payload_t {1, 255, 255, 1, this_node});
      network_send(03, payload_t {1, 255, 255, 1, this_node});
    }
    else if (command == "stop\n") {
      Serial.println("stop -> 01");
      network_send(01, payload_t {2, 0, 0, 0, this_node});
      network_send(02, payload_t {2, 0, 0, 0, this_node});
      network_send(03, payload_t {2, 0, 0, 0, this_node});
    }
    else if (command == "party\n") {
      Serial.println("party!!!");
      network_send(01, payload_t {5, 0, 0, 0, this_node});
      network_send(02, payload_t {5, 0, 0, 0, this_node});
      network_send(03, payload_t {5, 0, 0, 0, this_node});
    }
    else {
      Serial.print("UNKNOWN COMMAND: ");
      Serial.println(command);
    }
  }
}


void joystick() {
  if (millis() >= tTime + 20) {
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
    /*Serial.print("direction: ");
    Serial.print(driveDirection);
    Serial.print("  left_motor: ");
    Serial.print(left_motor);
    Serial.print("  right_motor: ");
    Serial.println(right_motor);*/
    network_send(01, payload_t {1, left_motor, right_motor, driveDirection, this_node});

    tTime = millis();
  }
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



void loop(void) {
  network.update();
  network_receive();
  command_handler();
  joystick();
}
