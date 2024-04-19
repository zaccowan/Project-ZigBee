#include <SoftwareSerial.h>

SoftwareSerial sserial(0, 1);  // RX and TX


void setup() {
  Serial.begin(9600);
  sserial.begin(9600);

  sserial.println("Setup complete\n");
  Serial.println("Setup complete\n");
}

void loop() {
  sserial.println("Hello World!");
  Serial.println("Hello World!");
  delay(5000);
}
