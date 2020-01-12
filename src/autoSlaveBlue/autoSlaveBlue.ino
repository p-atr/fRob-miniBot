#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <NewPing.h>
#include <FastLED.h>

NewPing sonar(A2, A3, 30);

#define NUM_LEDS 7
#define DATA_PIN 4
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];
//define colors
int rainbow[7][3] = {{255, 0, 0}, {170, 85, 0}, {85, 170, 0}, {0, 255, 0}, {0, 170, 85}, {0, 85, 170}, {0, 0, 255}};
int black[7][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int red[7][3] = {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}};
int green[7][3] = {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}};
int blue[7][3] = {{0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}, {0, 0, 255}};

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 03;      // Address of our node in Octal format
const uint16_t master = 00;         // Address of the other node in Octal format

struct payload_t {                  // Structure of our payload
  int id;
  int left_speed;
  int right_speed;
  bool drive_direction;
  unsigned long node;
};

int normSpeed = 255;
int minDist = 15;
bool driving = true;
int left_speed = 255, right_speed = 255;
bool anim = false;

void setup(void) {
  Serial.begin(115200);
  Serial.println("SLAVE");

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

  FastLED.addLeds<WS2812B, 4, GRB>(leds, 7);
  setColor(blue);

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
    } else if (payload.id == 1) {
      left_speed = payload.left_speed;
      right_speed = payload.right_speed;
      driving = true;
    }
    else if (payload.id == 2) {
      driving = false;
      setColor(black);
    }
    else if (payload.id == 3) {
      driving = false;
      setColor(green);
    }
    else if (payload.id == 5) {
      driving = true;
      anim = true;
    }
    else {
      Serial.print("UNKNOWN PACKET FROM NODE #");
      Serial.println(payload.node);
    }
  }
}

int dist;
long t = 0;
void updateSensors() {
  //sonar
  if (millis() >= t + 29) {
    dist = sonar.ping_cm();
    t = millis();
  }
  //Serial.println(dist);
}

bool isColliding() {
  if (dist < minDist && dist > 0) {
    return true;
  }
  return false;
}

void drive_forward(int left_speed, int right_speed) {
  digitalWrite(5, LOW);
  analogWrite(6, left_speed);
  digitalWrite(9, LOW);
  analogWrite(10, right_speed);
}

void drive_backwards(int left_speed, int right_speed) {
  analogWrite(5, left_speed);
  digitalWrite(6, LOW);
  analogWrite(9, right_speed);
  digitalWrite(10, LOW);
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

void setColor(int colors[7][3]) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(colors[i][0], colors[i][1], colors[i][2]);
  }
  FastLED.show();
}

int j = 0;
int s = 0;
void animateColor(int colors[7][3]) {
  if (anim) {
    if (millis() >= s + 100) {
      j++;
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(colors[(i + j) % NUM_LEDS][0], colors[(i + j) % NUM_LEDS][1], colors[(i + j) % NUM_LEDS][2]);
      }
      FastLED.show();
      s = millis();
    }
  }
}

void loop() {
  network.update();                       // Check the network regularly
  network_receive();
  updateSensors();

  animateColor(rainbow);
  if (isColliding()) {
    drive_forward(0, 0);
    drive_backwards(0, right_speed);
    delay(80);
  } else {
    if (driving) {
      drive_forward(left_speed, right_speed);
    } else {
      drive_forward(0, 0);
    }
  }
}
