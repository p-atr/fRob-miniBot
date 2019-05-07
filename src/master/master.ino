#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

String command;

RF24 radio(7, 8);                  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);        // Network uses that radio
const uint16_t this_node = 00;     // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;

struct payload_t {                 // Structure of our payload
  int id ;
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

void network_send(int id, uint16_t node) {
  Serial.print("Sending...");
  payload_t payload = {id, this_node};
  RF24NetworkHeader header(/*to node*/ other_node);
  bool ok = network.write(header, &payload, sizeof(payload));
  if (ok) {
    Serial.println("ok.");
  }
  else {
    Serial.println("failed.");
  }
}

void loop(void) {
  network.update();
  
  if (Serial.available()) {
    Serial.setTimeout(2);
    command = Serial.readString();
    if (command == "ping\n") {
      Serial.println("PING RECEIVED; SENDING TO NODE 01");
      network_send(0, 01);
    }
    else {
      Serial.print("UNKNOWN COMMAND: ");
      Serial.println(command);
    }
  }
}
