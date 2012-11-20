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


// Analog input pin that the soil moisture sensor is attached to
const int probesInPin = A1;
const int lightInPin = A2;  

// value read from the soil moisture sensor
int probesValue = 0;
int lightValue = 0;

 // if the readings from the soil sensor drop below this number, then turn on the pump
int dryValue = 700;
int darkValue = 500;

void setup() {
  
  pinMode(12, OUTPUT);
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

void loop() {
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
    delay(10000);
    digitalWrite(12, LOW);
  }
    
  
  //slow your roll - I mean... slow down the code a little
  delay(5000);                     
}
