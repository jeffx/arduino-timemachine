

// Time Machine Prop
//       Author: Jeff Tillotson <jeffx@jeffx.com>
//  Description:
//      Version:
//         Date: 

#include <Keypad.h>
//20 minutes is 1200000 using 300000 (5 mins for testing)
#define twentyMins 300000
const byte rows = 4;
const byte cols = 3;
char keys[rows][cols] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
char deactiveCode[ 8 ] = { '1', '2', '3', '4', '5', '6', '7', '8' };
char pcLocked[ 8 ] = { '0', '0', '0', '0', '0', '0', '0', '0' };
char pcHold[ 8 ] = { '0', '0', '0', '0', '0', '0', '0', '0' };
byte rowPins[rows] = { 9, 8, 7, 6 };
byte colPins[cols] = { 5, 4, 3 };
Keypad keypad = Keypad( makeKeymap( keys ), rowPins, colPins, rows, cols );
const int panicButton = 0;
const int ledPin = 13;
unsigned long timeSinceLast = 0;
unsigned long curRead = 0;
unsigned long lastRead = 0;
int pcPos = 0;
unsigned long lastPCPressed = 4294967295;
boolean ledOn = false;
boolean locked = false;
boolean cleanJump = false;
boolean jumpPressed = false;
char keypadEntry = 'z';
static byte kpadState;

void setup() {
  pinMode( ledPin, OUTPUT );
  Serial.begin( 9600 );
  digitalWrite( ledPin, LOW );  
  attachInterrupt( panicButton, jumpInterrupt, RISING );
  keypad.begin( makeKeymap( keys ) );
  keypad.addEventListener( keypadEvent );
  keypad.setDebounceTime( 250 );
  keypad.setHoldTime( 1000 );
  Serial.println( "Setup Complete" );
}

void loop()
{
//  delay( 1000 );
  if( jumpPressed ){
    jump();
  }
  curRead = millis();
  timeSinceLast += curRead - lastRead;
  lastRead = curRead;
  if( timeSinceLast > twentyMins ) {
    Serial.println( "TIMES UP!" );
    //times up
    timeSinceLast = 0;
    jump();
  } else {
    //Serial.println( timeSinceLast );
  } 
  keypadEntry = keypad.getKey();  
  if( ( curRead - 15000 ) > lastPCPressed ) {
    Serial.println( curRead );
    Serial.println( lastPCPressed );
    for( int pos = 0; pos < 8; pos++ ){
      pcHold[pos] = '0';
    }
    lastPCPressed = 4294967295;
  }    
}

void jump()
{
  Serial.println( "Begin Jump" );
  if( locked ){
    //check.
    for( int pos = 0; pos < 8; pos++ ){
      if( pcLocked[pos] != deactiveCode[pos] ){
        Serial.print( "BAD CODE - Random Jump" );
        cleanJump = false;
        break;
      } else{
        cleanJump = true;
      }
      if( cleanJump ) {
        Serial.println( "GOOD CODE - JUMP HOME" );
      }  
    }
  } else {
    Serial.println( "Jumped" );
  }
  if( ledOn ) {
    digitalWrite( ledPin, LOW );
    ledOn = false;
  }else {
    digitalWrite( ledPin, HIGH );
    ledOn = true;
  }
  timeSinceLast = 0;
  jumpPressed = false;
}

void jumpInterrupt()
{
  jumpPressed = true;
}

void keypadEvent( KeypadEvent key ){
  kpadState = keypad.getState();
  swOnState( key );
}

void swOnState( char key ) {
  switch( kpadState ) {
    case PRESSED:
      //Serial.println( key );
      break;
    case RELEASED:
      Serial.println( key );
      if( key == '*' ) {
        Serial.println( "RESET" );
        for( int pos = 0; pos < 8; pos++ ){
          pcHold[pos] = '0';
          pcLocked[pos] = '0';
        }
        pcPos = 0;  
      }else if( key == '#' ){
        Serial.println( "LOCK" );
        locked = true;
        for( int pos = 0; pos < 8; pos++ ){
          pcLocked[pos] = pcHold[pos];
          pcHold[pos] = '0';
          Serial.println( pos );
        }
        Serial.println( "LOCKED VALUE" );
        for( int pos = 0; pos < 8; pos++ ){
          Serial.print( pcLocked[pos] );
        }
        Serial.println( "" );
        
      }else{
        pcHold[pcPos] = key;
        if( pcPos == 7 ){
          pcPos = 0;
        } else{
          pcPos++;
        }
      }
      lastPCPressed = curRead;
      break;
  }  
}
      
