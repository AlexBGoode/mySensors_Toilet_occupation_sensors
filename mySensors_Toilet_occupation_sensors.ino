// Inspired by
// https://www.mysensors.org/build/distance
// http://playground.arduino.cc/Code/NewPing
// http://www.micropik.com/PDF/HCSR04.pdf

// Enable debug prints
// #define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#define CHILD_ID_SEAT     1
#define CHILD_ID_OBJECT   2

#define SLEEP_TIME      500 // Sleep time between reports (in milliseconds)

#include <NewPing.h>
#define TRIGGER_PIN       4   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN          3   // Arduino pin tied to echo pin on the ultrasonic sensor.#define CHILD_ID 1   // Id of the sensor child
#define DISTANCE_MAX      100 // Max distance we want to start detecting (in cm)
#define DISTANCE_OCCUPAED 50 // Distance when inverted "occupied signal" should be sent to controller (in cm)

int oldOccupiedStatus = -1;

// NewPing setup of pins and maximum distance.
NewPing sonar( TRIGGER_PIN, ECHO_PIN, DISTANCE_MAX );

// Initialize occupation message
MyMessage msgSeat( CHILD_ID_SEAT, V_TRIPPED );
// Initialize distance message
MyMessage msgObject( CHILD_ID_OBJECT, V_DISTANCE );

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo( "Toilet occupation sensor", "1.0" );

  // Register all sensors to gw (they will be created as child devices)
  present( CHILD_ID_SEAT, S_DOOR, "Toilet is vacant" );
  present( CHILD_ID_OBJECT, S_DISTANCE, "Distance to object" );
}

void loop()
{
  static int lastDist = -1;
  int distance = ( sonar.ping_median() / US_ROUNDTRIP_CM );
//  int distance = sonar.ping_cm();
  if ( distance < 11)
    return;

  if (( distance != lastDist ) && ( abs( lastDist - distance ) > 5 ))
  {
    // Report a distance if changed more than by 10
    send( msgObject.set( distance ), true );
    Serial.print( "Distance changed: " );
    Serial.println( distance );
    lastDist = distance;
    Serial.print( "Distance: " );
    Serial.println( distance );
  }

  // Get the toilet occupation status
  int occupied = ( distance > 10 ) && ( distance < DISTANCE_OCCUPAED );

//  Serial.println( distance != NO_ECHO );
//  Serial.println( distance < DISTANCE_OCCUPAED );



  // First time always false and cause reporting the actual status
  if ( occupied != oldOccupiedStatus )
  {
    if ( occupied )
      Serial.println( "Toilet occupied" );
    else
      Serial.println( "Toilet vacant" );

    // Report an inverted value for a door lock (as a vacant status)
    send( msgSeat.set( !occupied ), true );
    oldOccupiedStatus = occupied;
    // Double sleep time on change to avoid bouncing
    sleep( 5000 );
  }

  sleep( SLEEP_TIME );
}


