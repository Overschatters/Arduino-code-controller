#include <Arduino.h>
#include "RF24.h"			// Comm module library

typedef byte pin;			// Define pin data type as byte for better readability

// Pins
const pin speedJoy = A0;	// Pin of the joystick that controls speed
const pin steeringJoy = A1;	// Pin of the joystick that controls the steering
const pin forward = 2;		// Pin that reads if the switch is in the forward position
const pin backward = 3;		// Pin that reads if the switch is in the backward position

// Structs that holds the data of the single package that the controller sends
struct Package {
	u8 direction = 0;		// Byte that holds the value of the direction switch
	u16 speed = 0;			// Byte that holds the value of the speed joystick passthrough
	u16 steering = 0;		// Byte that holds the value of the steering joystick.
};
Package package; 			// Structs that holds the data of the single package that the controller sends

u8 pynqReading;				// Byte that holds the (RAW!!!!!) speed reading from the pynq board

// Wireless comm
RF24 radio(9, 10);  // using pin 9 for the CE pin, and pin 10 for the CSN pin
const byte addresses[][6] = {"1Node", "2Node"};	// Define addresses as a byte

void setup() {
// Open transmission pipes
	radio.openWritingPipe(addresses[1]);
	radio.openReadingPipe(1,addresses[0]);
}

void loop() {
// Read package data into the package
package.speed = analogRead(speedJoy);
package.steering = analogRead(steeringJoy);

if (digitalRead(forward)) {
	package.direction = 1;
}
else if (digitalRead(backward)) {
	package.direction = 2;
}
else {
	package.direction = 0;
}

// Send package data
radio.stopListening();
radio.write(&package, sizeof(package));

// Receive screen-data
radio.startListening();
if (radio.available()) {
	radio.read(&pynqReading, sizeof(pynqReading));
}
}