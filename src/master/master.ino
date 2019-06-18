#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

String command;

RF24 radio(7, 8);                  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);        // Network uses that radio
const uint16_t this_node = 00;     // Address of our node in Octal format ( 04,031, etc)

struct payload_t {                  // Structure of our payload
  int id;
  int left_speed;
  int right_speed;
  bool drive_direction;
  unsigned long node;
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("MASTER");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
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

void network_receive() {
  while (network.available() ) {          // Is there anything ready for us?
    RF24NetworkHeader header;             // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    if (payload.id == 0) {
      Serial.print("Received packet from node #");
      Serial.println(payload.node);
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
      }
      else if (command == "start\n") {
        Serial.println("start -> 01");
        network_send(01, payload_t {1, 255, 255, 1, this_node});
      }
      else if (command == "stop\n") {
        Serial.println("stop -> 01");
        network_send(01, payload_t {2, 0, 0, 0, this_node});
      }
      else {
        Serial.print("UNKNOWN COMMAND: ");
        Serial.println(command);
      }
    }
  }
  void loop(void) {
    network.update();
    network_receive();
    command_handler();
  }
