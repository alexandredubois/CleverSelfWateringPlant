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

volatile int f_wdt=1;

/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


// Analog input pin that the soil moisture sensor is attached to
const int probesInPin = A1;
const int lightInPin = A2;  

// value read from the soil moisture sensor
int probesValue = 0;
int lightValue = 0;

 // if the readings from the soil sensor drop below this number, then turn on the pump
int dryValue = 700;
int darkValue = 500;

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
  Serial.begin(9600);
  Serial.println("Initialising...");
  
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  
  pinMode(12, OUTPUT);
  
  Serial.println("Initialisation complete.");
}

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
      digitalWrite(12, HIGH);
      delay(5000);
      digitalWrite(12, LOW);
    }
}

/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  if(f_wdt == 1)
  {
    water();
    
    /* Don't forget to clear the flag. */
    f_wdt = 0;
    
    /* Re-enter sleep mode. */
    enterSleep();
  }
  else
  {
    /* Do nothing. */
  }
}
