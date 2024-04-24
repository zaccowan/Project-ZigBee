//Zigbee Sensor Code

// Includes //
#include "DHT.h"  
#include <XBee.h>

// Defines //
#define BAUDRATE 9600       // Serial buadrate
#define A_PIN A0            // Analog pin
#define VOLTAGE 5           // Input Voltage
#define DHTPIN 2            // DHT pin
#define DHTTYPE DHT22       // Set Sensor Type
#define SEND_INTERVAL 10000 // Send interval in millisecond

// You should change this for each station
#define STATION_NAME "STATION_NAME"

// Initalize Sensors //
DHT dht(DHTPIN, DHTTYPE); // Init DHT22 Sensor

XBee xbee = XBee();

// Coordinator Address
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x423D8CEB);

//----------------------------------------------

// Setup Loop //
void setup() {
  
	Serial.begin(BAUDRATE);
  xbee.setSerial(Serial);
	while (!Serial) {
		;	// wait for serial connection
	}
 
	Serial.println(""); 	  // blank new line
  dht.begin();        // begin dht sensor
}

// Main Loop //
void loop() {
  static long lastTime = 0;
  long currentTime = millis();
  if (currentTime - lastTime >= SEND_INTERVAL) {
    // Update timer
    lastTime = currentTime;

    // Read all the DHT data
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    // Serial Print out //
    long h_int = h * 1000;
    long f_int = f * 1000;

    char payload[100];
    int payload_size = snprintf(payload, sizeof(payload), "S:" STATION_NAME " L:%d H:%ld.%03ld T:%ld.%03ld", analogRead(A_PIN), h_int / 1000, h_int % 1000, f_int / 1000, f_int % 1000) + 1;
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, payload_size);
    xbee.send(zbTx);
  }
}