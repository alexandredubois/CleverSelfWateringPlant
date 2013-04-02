/*
  (Clever)Self-Watering Plant
 by Alexandre Dubois
 inspired by Randy Sarafan
 
 Sketch features
 - Monitor a plant and add water when both the soil moisture and the light are low (to prevent watering the plant at noon)
 - Let the user manually setup its ideal moisture level
 - Save power to run for months using 3 AA batteries
 - Noise free using transistors instead of a relay
 - Check the battery level
 
 For more information, check out:
 http://blog.alexandredubois.com
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

const int debug = 1;

int probesTreshold = 0;
int probesValue = 0;
int lightValue = 0;

const long voltTreshold = 3500;
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
  if(debug == 1)
  {
     Serial.println("Treshold setup.");
  }
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
  
  if(debug == 1)
  {
     Serial.println("Treshold setup done.");
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
void blinkGreenStatusLed()
{
  pinMode(greenOutPin, OUTPUT);
  digitalWrite(greenOutPin, HIGH);
  delay(50);
  digitalWrite(greenOutPin, LOW);
  pinMode(greenOutPin, INPUT);
}

/*Flash the red led quickly to show the unit is low battery*/
void blinkRedStatusLed()
{
  pinMode(redOutPin, OUTPUT);
  digitalWrite(redOutPin, HIGH);
  delay(50);
  digitalWrite(redOutPin, LOW);
  pinMode(redOutPin, INPUT);
}

//Secret voltmeter function from : http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

//Check if the battery is low
void checkVccLevel(){
  long vcc = readVcc();
  if(vcc < voltTreshold)
  {
    blinkRedStatusLed();
  }
  else
  {
    blinkGreenStatusLed();
  }
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
  Sleepy::loseSomeTime(10000);// Wait a minute
  checkVccLevel();
} // end of loop
