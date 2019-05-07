/*
 * TRANSMITTER NODE
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 03;        // Address of our node in Octal format
const uint16_t other_node = 01;       // Address of the other node in Octal format

const unsigned long interval = 200; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

struct payload_t {                  // Structure of our payload
  int id;
  unsigned long l;
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("p-atr/fRob-miniBot");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop() {

  network.update();    
  // Check the network regularly
  while ( network.available() ) {     // Is there anything ready for us?
  //Serial.println("Available");
  RF24NetworkHeader header;        // If so, grab it and print it out
  payload_t payload;
  network.read(header, &payload, sizeof(payload));
  Serial.println("Recieved:" + (String)payload.id);
    if(payload.id == 0){
      sendMsg(payload.id,payload.l);
    }
  }

}


void sendMsg(int payloadMsg, unsigned long l){
      payload_t payloadB = {payloadMsg,l};
      RF24NetworkHeader header(/*to node*/ 00);
      bool ok = network.write(header,&payloadB,sizeof(payloadB));
      Serial.println("Sent: " + (String)payloadMsg);
      if (ok) {
        Serial.print("ok. - ");
      }
      else {
        Serial.println("failed.");
      }
}


