// Inspired by
// https://www.mysensors.org/build/distance
// http://playground.arduino.cc/Code/NewPing

// Enable debug prints
// #define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensors.h>
#define CHILD_ID 1


//unsigned long SLEEP_TIME = 2000; // Sleep time between reports (in milliseconds)
#define SLEEP_TIME      500 // Sleep time between reports (in milliseconds)

#include <NewPing.h>
#define TRIGGER_PIN     4   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN        3   // Arduino pin tied to echo pin on the ultrasonic sensor.#define CHILD_ID 1   // Id of the sensor child
#define MAX_DISTANCE    40  // Max distance we want to start detecting (in cm)
#define OCCUPAED_DISTANCE 20  // Distance when "parked signal" should be sent to controller (in cm)

int skipZero = 0;
int oldOccupiedStatus = -1;
//unsigned long sendInterval = 5000;  // Send park status at maximum every 5 second.
//unsigned long lastSend;

// NewPing setup of pins and maximum distance.
NewPing sonar( TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE );

// Initialize occupation message
MyMessage msg( CHILD_ID, V_TRIPPED );

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo( "Toilet Sensor", "1.0" );

  // Register all sensors to gw (they will be created as child devices)
  present( CHILD_ID, S_DOOR, "Toilet occupation status" );  
}

void loop() {
  unsigned int distance = ( sonar.ping_median() / US_ROUNDTRIP_CM );

  // Get the toilet occupation status
  int occupied = distance != 0 && distance < OCCUPAED_DISTANCE;

  if ( occupied != oldOccupiedStatus ) {
    if ( occupied )
      Serial.println("Toilet occupied");
    else
      Serial.println("Toilet vacant");
    // Report an inverted value for a door lock (as a vacant status)
    send( msg.set( !occupied ));
    oldOccupiedStatus = occupied;
  }

  sleep( SLEEP_TIME );
}


