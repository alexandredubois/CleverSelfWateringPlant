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

ISR(WDT_vect) { 
  Sleepy::watchdogEvent(); 
} // Setup for low power waiting

/************************
*** CONST & VARIABLES ***
************************/

const int probesInPin = A1;//Pin attached to the probes
const int lightInPin = A2;//Pin attached to the LDR
const int potInPin = A5;//Pin attached to the potentiometer

const int pumpOutPin = 12;//Pin attached to the water pump
const int greenOutPin = 7;
const int redOutPin = 8;

const int debug = 0;

int probesTreshold = 0;
int probesValue = 0;
int lightValue = 0;

const int loopCount = 2;
int timeCount = 1;
const int hysteresis = 100;
const int darkValue = 300;

/****************
*** FUNCTIONS ***
****************/

//Check if the plant needs water !
void monitorPlantStatus()
{
  readProbesValue();
  readLightValue();
  if(probesValue < probesTreshold && lightValue < darkValue)
  {
    water();
  }
} 

//Turn on the pump during a few seconds
//Increasing the delay will increase the amount of water pumped
void water()
{
    //Turns on the water pump if the soil is too dry
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

/*Read potentiometer value*/
void readPotValue()
{
  probesTreshold = analogRead(potInPin);
  if(debug == 1)
  {
    Serial.print("soil treshold = " );                       
    Serial.println(probesTreshold);
    delay(300);
  }
}

/*Read moisture soil value from the probes*/
void readProbesValue()
{
  probesValue = analogRead(probesInPin);
  if(debug == 1)
  {
    Serial.print("probes value = " );                       
    Serial.println(probesValue);
  }
}

/*Read light walue received by the LDR*/
void readLightValue()
{
  lightValue = analogRead(lightInPin);
  if(debug == 1)
  {
    Serial.print("light value = " );                       
    Serial.println(lightValue); 
  }
}

/*Let the user define its ideal moisture level slowly blinking led*/
void tresholdSetup()
{
  for (int i=0;i<=100;i++)
  {
    readPotValue();
    readProbesValue();
    if(probesTreshold < (probesValue - hysteresis))
    {
      digitalWrite(redOutPin, HIGH);
      digitalWrite(greenOutPin, LOW);
    }
    else if(probesTreshold > (probesValue + hysteresis))
    {
      digitalWrite(redOutPin, LOW);
      digitalWrite(greenOutPin, HIGH);
    }
    else {
      digitalWrite(redOutPin, HIGH);
      digitalWrite(greenOutPin, HIGH);
    }
    Sleepy::loseSomeTime(300);
  }
}

/*Blink leds to warn the user before launching the main program*/
void launchWarning()
{
  for (int i=0;i<=6;i++)
  {
    digitalWrite(redOutPin, HIGH);
    digitalWrite(greenOutPin, HIGH);
    Sleepy::loseSomeTime(300);
    digitalWrite(redOutPin, LOW);
    digitalWrite(greenOutPin, LOW);
    Sleepy::loseSomeTime(300);
  }
}

/*Flash the green led quickly to show the unit is working*/
void blinkStatusLed()
{
  pinMode(greenOutPin, OUTPUT);
  digitalWrite(greenOutPin, HIGH);
  delay(100);
  digitalWrite(greenOutPin, LOW);
  pinMode(greenOutPin, INPUT);
}

/***************************
*** SETUP & MAIN PROGRAM ***
***************************/

/*Setup the device*/
void setup()
{
  pinMode(greenOutPin, OUTPUT);
  pinMode(redOutPin, OUTPUT);
  tresholdSetup(); //Setup the moisture level
  launchWarning(); //Warn the user
  pinMode(greenOutPin, INPUT);
  pinMode(redOutPin, INPUT);

  if(debug == 1){
    Serial.println("Initialising...");
    Serial.begin(9600);
    Serial.println("Initialisation complete.");
  }
}

/*Main program : wait and check the plant every X minutes*/
void loop () 
{
  if(timeCount >= loopCount){
    if(debug == 1)
    {
      Serial.println("Check plant");
    }
    monitorPlantStatus();
    timeCount = 0; //Reset time counter
  }
  //Wait more...
  else{
    if(debug == 1)
    {
      Serial.println("Wait !");
      delay(300);
    }
    ++timeCount;
  }
  Sleepy::loseSomeTime(60000);// Wait a minute
} // end of loop