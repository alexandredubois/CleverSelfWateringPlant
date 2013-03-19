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
#include <JeeLib.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup for low power waiting

// Analog input pin that the soil moisture sensor is attached to
const int probesInPin = A1;
const int lightInPin = A2;
const int pumpOutPin = 12;
const int debug = 1;

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
  if(debug == 1){
    Serial.println("Initialising...");
    Serial.begin(9600);
    Serial.println("Initialisation complete.");
  }
}

//Check if the plant needs water !
void water()
{
    
      // read the analog in value:
      probesValue = analogRead(probesInPin);
      lightValue = analogRead(lightInPin);
  
      // print the probes value to the serial monitor:
      if(debug == 1)
      {
        Serial.print("probes value = " );                       
        Serial.println(probesValue);
  
        Serial.print("light value = " );                       
        Serial.println(lightValue);   
      }
      //Turns on the water pump if the soil is too dry
      //Increasing the delay will increase the amount of water pumped
      if(probesValue < dryValue && lightValue < darkValue){
        if(debug == 1)
        {
          Serial.println("Pump is ON !");
        }
        pinMode(pumpOutPin, OUTPUT);
        digitalWrite(pumpOutPin, HIGH);
        Sleepy::loseSomeTime(10000);
        if(debug == 1)
        {
          Serial.println("Pump is OFF !");
        }
        digitalWrite(pumpOutPin, LOW);
        pinMode(pumpOutPin, INPUT);
      }
}

void loop () 
{
  if(timeCount >= loopCount){
    if(debug == 1)
    {
      Serial.println("Check plant");
    }
    water();
    //Reset time counter
    timeCount = 0;
  }
  //Wait more...
  else{
    if(debug == 1)
    {
      Serial.println("Wait !");
      Sleepy::loseSomeTime(300);
    }
    ++timeCount;
  }

  Sleepy::loseSomeTime(10000);      // Instead of delay(1000); 
  
  } // end of loop
