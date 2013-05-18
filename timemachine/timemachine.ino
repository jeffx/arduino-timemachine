

// Time Machine Prop
//       Author: Jeff Tillotson <jeffx@jeffx.com>
//  Description:
//      Version:
//         Date: 
// 
// TODO:
//    The timeout between key presses needs to get put back in.  If 15 (I think) seconds passes between button
//        presses, reset the the deactivateCode

#include <Keypad.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();
const byte cols = 3;
const byte rows = 4;
byte colPins[cols] = { 5, 4, 3 };

char deactiveCode[ 8 ] = { '1', '2', '3', '4', '5', '6', '7', '8' };
int digit0 = 0;
int digit1 = 0;
int digit3 = 0;
int digit4 = 0;
char keys[rows][cols] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
char pcHold[ 8 ] = { '0', '0', '0', '0', '0', '0', '0', '0' };
char pcLocked[ 8 ] = { '0', '0', '0', '0', '0', '0', '0', '0' };
byte rowPins[rows] = { 9, 8, 7, 6 };

Keypad keypad = Keypad( makeKeymap( keys ), rowPins, colPins, rows, cols );
const int panicButton = 0;
const int greenLedPin = 11;
const int yellowLedPin = 12;
const int redLedPin = 13;

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
  pinMode( greenLedPin, OUTPUT );
  pinMode( yellowLedPin, OUTPUT );
  pinMode( redLedPin, OUTPUT );
  Serial.begin( 9600 );
  digitalWrite( greenLedPin, LOW );  
  digitalWrite( yellowLedPin, LOW );  
  digitalWrite( redLedPin, LOW );  
  
  attachInterrupt( panicButton, jumpInterrupt, RISING );
  keypad.begin( makeKeymap( keys ) );
  keypad.addEventListener( keypadEvent );
  keypad.setDebounceTime( 250 );
  keypad.setHoldTime( 1000 );
  matrix.begin( 0x70 );
  resetDisplay();  
  delay( 15000 );
  Serial.println( "Setup Complete" );
}

void loop()
{
  if( jumpPressed ){
    jump();
  }
  curRead = millis(); 
  timeSinceLast = curRead - lastRead;
  if( timeSinceLast > 1000 ) {
    downTick();
    lastRead = curRead;
  }
  if( digit0 < 0 ){
    Serial.println( "TIMES UP!" );
    jump();
  }  
  keypadEntry = keypad.getKey(); 

//  Serial.println( lastPCPressed );
  if( ( curRead - 15000 ) > lastPCPressed ) {
    Serial.println( "Reset" );
    for( int pos = 0; pos < 8; pos++ ){
      pcHold[pos] = '0';
    }
    digitalWrite( yellowLedPin, LOW );
    lastPCPressed = 4294967295;
  }    
}
void downTick()
{
  if( digit4 == 0 ) {
    digit3--;
    digit4 = 10;
    if( digit3 < 0 ) {
      digit1--;
      digit3 = 5;
    }
    if( digit1 < 0 ){
      digit0--;
      digit1 = 9;
    }
  }
  digit4--;
  matrix.clear();
  if( digit0 > 0 ){
    matrix.writeDigitNum( 0, digit0 );
  }
  if( digit1 > 0 || digit0 > 0 ) {
    matrix.writeDigitNum( 1, digit1 );
  }
  matrix.writeDigitNum( 3, digit3 );
  matrix.writeDigitNum( 4, digit4 );
  if( digit4 % 2) {
    matrix.drawColon( false );
  } else {
    matrix.drawColon( true );
  }
  matrix.writeDisplay();
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
//  if( ledOn ) {
//    digitalWrite( ledPin, LOW );
//    ledOn = false;
//  }else {
//    digitalWrite( ledPin, HIGH );
//    ledOn = true;
//  }
  resetDisplay();
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

void resetDisplay(){
  digit0 = 1;
  digit1 = 9;
  digit3 = 5;
  digit4 = 10;
  matrix.writeDigitNum( 0, 2 );
  matrix.writeDigitNum( 1, 0 );
  matrix.writeDigitNum( 3, 0 );
  matrix.writeDigitNum( 4, 0 );
  matrix.writeDisplay();
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
        digitalWrite( yellowLedPin, LOW );
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
      Serial.println( "here world!" );
      Serial.println( curRead );
      lastPCPressed = curRead;
      digitalWrite( yellowLedPin, HIGH );
      break;
  }  
}
      
