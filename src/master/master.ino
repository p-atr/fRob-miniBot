#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

String command;

RF24 radio(7,8);                   // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);        // Network uses that radio
const uint16_t this_node = 00;     // Address of our node in Octal format ( 04,031, etc)

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

void loop(void){
  if (Serial.available()) {
    Serial.setTimeout(2);
    command = Serial.readString();
    Serial.println(command);
  }

  network.update();                   // Check the network regularly
  while ( network.available() ) {     // Is there anything ready for us?
    RF24NetworkHeader header;         // If so, grab it and print it out
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.print("Received packet from node #");
    Serial.println(payload.node);
  }
}
