/*
  Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 2 as published by the Free Software Foundation.

  Update 2014 - TMRh20
*/

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 01;      // Address of our node in Octal format
const uint16_t master = 00;         // Address of the other node in Octal format

struct payload_t {                  // Structure of our payload
  int id;
  unsigned long node;
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("SLAVE");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void network_receive() {
  while (network.available() ) {          // Is there anything ready for us?
    RF24NetworkHeader header;             // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    if (payload.id == 0) {
      Serial.print("Received packet from node #");
      Serial.println(payload.node);
      network_send(0, master);
    }
    else {
      Serial.print("UNKNOWN PACKET FROM NODE #");
      Serial.println(payload.node);
    }
  }
}

void network_send(int id, uint16_t node) {
  Serial.print("Sending...");
  payload_t payload = {id, this_node};
  RF24NetworkHeader header(/*to node*/ node);
  bool ok = network.write(header, &payload, sizeof(payload));
  if (ok) {
    Serial.println("ok.");
  }
  else {
    Serial.println("failed.");
    network_send(id, node);
  }
}

void loop() {
  network.update();                       // Check the network regularly
  network_receive();
}
