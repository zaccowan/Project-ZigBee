//Zigbee Sensor Code

// Includes //
#include "MQ7.h" 
#include "DHT.h"  

// Defines //
#define BAUDRATE 9600     // Serial buadrate
#define A_PIN 0           // Set pin A2
#define VOLTAGE 5         // Input Voltage 5v
#define DHTPIN 2          // Set pin 2
#define DHTTYPE DHT22     // Set Sensor Type

// Initalize Sensors //
DHT dht(DHTPIN, DHTTYPE); // Init DHT22 Sensor

//----------------------------------------------

// Setup Loop //
void setup() {
  
	Serial.begin(BAUDRATE);
	while (!Serial) {
		;	// wait for serial connection
	}
 
	Serial.println(""); 	  // blank new line
  dht.begin();            // begin dht sensor
}

// Main Loop //
void loop() {
  
	Serial.print("Light: "); Serial.print(analogRead(A_PIN));
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
  Serial.print(F(", Humidity: "));
  Serial.print(h);
  Serial.print(F(", Temperature: "));
  Serial.print(t);
  Serial.print(F(""));
	Serial.println("\n");
	delay(5000);
}
