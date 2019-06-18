#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

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

void setup(void) {
  Serial.begin(115200);
  Serial.println("SLAVE");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

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
      digitalWrite(5, LOW);
      analogWrite(6, payload.left_speed);
      analogWrite(9, LOW);
      digitalWrite(10, payload.right_speed);
    }
    else if (payload.id == 1 && payload.drive_direction == false) {
      digitalWrite(5, payload.left_speed);
      analogWrite(6, LOW);
      analogWrite(9, payload.right_speed);
      digitalWrite(10, LOW);
    }
    else if (payload.id == 2) {
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
    }
    else {
      Serial.print("UNKNOWN PACKET FROM NODE #");
      Serial.println(payload.node);
    }
  }
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
}
