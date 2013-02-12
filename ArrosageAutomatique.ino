/*
  (Clever)Self-Watering Plant
  by Alexandre Dubois
  inspired by Randy Sarafan
  
  Reads a soil moisture sensor and a LDR and turns on a relay that controls a water pump.
  
  The soil moisture sensor involves a 10K resistor between pins A1 and ground
  and a probe connected to pin A1 and another connected to +5V. These probes 
  are embedded and inch apart in the plant's soil.
  
  The light sensor involves a 2,2K resistor between pins A2 and ground. The LDR is connected
  both to the A2 pin and the +5V.
  
  For more information, check out:
  http://www.instructables.com/id/Self-Watering-Plant/
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

// Analog input pin that the soil moisture sensor is attached to
const int probesInPin = A1;
const int lightInPin = A2;
const int pumpOutPin = 12;

// value read from the soil moisture sensor
int probesValue = 0;
int lightValue = 0;

const int loopCount = 2;
int timeCount = 0;

 // if the readings from the soil sensor drop below this number, then turn on the pump
const int dryValue = 700;
const int darkValue = 300;

/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the serial comms and the
 *                Watch dog timeout. 
 *
 ***************************************************/
void setup()
{
  Serial.println("Initialising...");
  Serial.begin(9600);
  
  Serial.println("Initialisation complete.");
}

//Check if the plant needs water !
void water()
{
    
      // read the analog in value:
      probesValue = analogRead(probesInPin);
      lightValue = analogRead(lightInPin);
  
      // print the probes value to the serial monitor:
      Serial.print("probes value = " );                       
      Serial.println(probesValue);
  
      Serial.print("light value = " );                       
      Serial.println(lightValue);   
  
      //Turns on the water pump if the soil is too dry
      //Increasing the delay will increase the amount of water pumped
      if(probesValue < dryValue && lightValue < darkValue){
        pinMode(pumpOutPin, OUTPUT);
        digitalWrite(pumpOutPin, HIGH);
        delay(10000);
        digitalWrite(pumpOutPin, LOW);
        pinMode(pumpOutPin, INPUT);
      }
}

void loop () 
{
  if(timeCount >= loopCount){
    Serial.println("Check plant status !");
    water();
    //Reset time counter
    timeCount = 0;
  }
  //Wait more...
  else{
    Serial.println("Wait !");
    ++timeCount;
  }

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = _BV (WDCE) | _BV (WDE);
  // set interrupt mode and an interval 
  WDTCSR = _BV (WDIE) | _BV (WDP3) | _BV (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = _BV (BODS) | _BV (BODSE);
  MCUCR = _BV (BODS); 
  sleep_cpu ();  
  
  // cancel sleep as a precaution
  sleep_disable();
  
  } // end of loop
