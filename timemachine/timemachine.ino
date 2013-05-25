

// Time Machine Prop
//       Author: Jeff Tillotson <jeffx@jeffx.com>
//  Description: Calling it 1.0, but still needs some work.
//      Version: 1.0
//         Date: 24 May 2013
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
int jumpCount = 0;
boolean goodJump = false;
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
boolean validCode = false;

boolean jumpPressed = false;
char keypadEntry = 'z';
static byte kpadState;

void setup() {
  pinMode( greenLedPin, OUTPUT );
  pinMode( yellowLedPin, OUTPUT );
  pinMode( redLedPin, OUTPUT );
  Serial.begin( 9600 );
  digitalWrite( greenLedPin, HIGH );  
  digitalWrite( yellowLedPin, HIGH );  
  digitalWrite( redLedPin, HIGH );  
  
  attachInterrupt( panicButton, jumpInterrupt, RISING );
  keypad.begin( makeKeymap( keys ) );
  keypad.addEventListener( keypadEvent );
  keypad.setDebounceTime( 250 );
  keypad.setHoldTime( 1000 );
  matrix.begin( 0x70 );
  resetDisplay();  
  delay(5000);
  digitalWrite( greenLedPin, LOW );  
  delay(5000);
  digitalWrite( yellowLedPin, LOW );  
  delay(5000);
  digitalWrite( redLedPin, LOW );  
  Serial.println( "Setup Complete" );
}

void loop()
{
  if( goodJump ) {
    digitalWrite( redLedPin, HIGH );
    digitalWrite( yellowLedPin, HIGH );
    digitalWrite( greenLedPin, HIGH );
    matrix.writeDigitNum( 0, 8 );
    matrix.writeDigitNum( 1, 8 );
    matrix.writeDigitNum( 3, 8 );
    matrix.writeDigitNum( 4, 8 );
    matrix.blinkRate( 3 );
    matrix.writeDisplay();
    delay( 4294967295 );
  }
//  Serial.println( "Hello" );
  if( jumpPressed ){
    jump();
  }
  if( locked &&  !validCode ){
    Serial.println( "locked -time to check" );
    checkCode();
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

void checkCode()
{
  Serial.println( "CHECKCODE" );
  boolean check = true;
  for( int pos = 0; pos < 8; pos++ ){
    if( pcLocked[pos] != deactiveCode[pos] ){
      Serial.println( "MISMATCH" );
      Serial.println( pos );
      check = false;
    }
  }
  if( check ) {
    Serial.println( "validcode" );
    digitalWrite( greenLedPin, HIGH );
    digitalWrite( redLedPin, LOW );
    validCode = true;
  } else {
    digitalWrite( greenLedPin, LOW );
    digitalWrite( redLedPin, HIGH );
    validCode = false;
    locked = false;
  }
}

void jump()
{
  Serial.println( "Begin Jump" );
  jumpCount++;
  if( locked ){
    if( validCode ) {
      Serial.println( "GOOD CODE - JUMP HOME" );
      goodJump = true;
    }  
  } else {
    Serial.println( "Jumped" );
  }
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
  if( jumpCount == 0 ){
    digit0 = 4;
    digit1 = 4;
    digit3 = 5;
    digit4 = 10;
    matrix.writeDigitNum( 0, 4 );
    matrix.writeDigitNum( 1, 5 );
    matrix.writeDigitNum( 3, 0 );
    matrix.writeDigitNum( 4, 0 );

  } else{
    digit0 = 2;
    digit1 = 9;
    digit3 = 5;
    digit4 = 10;
    matrix.writeDigitNum( 0, 2 );
    matrix.writeDigitNum( 1, 0 );
    matrix.writeDigitNum( 3, 0 );
    matrix.writeDigitNum( 4, 0 );
  }
  
  matrix.writeDisplay();
}

void swOnState( char key ) {
  switch( kpadState ) {
    case PRESSED:
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
        locked = false;
        validCode = false;
        digitalWrite( yellowLedPin, LOW );
        digitalWrite( greenLedPin, LOW );
        digitalWrite( redLedPin, LOW );
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
        digitalWrite( yellowLedPin, HIGH );
      }
      lastPCPressed = curRead;
      break;
  }  
}
      
