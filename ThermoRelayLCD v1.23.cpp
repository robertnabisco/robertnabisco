//Thermometer Relay LCD Output Arduino - Brent Dean

///////Successful Test 3.28.22\\\\\\\

/// - LCD & Serial output Temp, LimT, Humidity and Fan Status
/// - Relay1 - activated at Potentiometer1 set Limit Temp
/// - Relay2 - activated at Potentiometer2 set Limit Moisture
/// - Serial print all sensors
/// v1.23 - Add 2x LEDs - System On (Red), System Idle (Green)
/// Add Soil Sensor in v1.24

////DHT
#include <Arduino.h>
#include "DHT.h"                  					//// Include DHT Library  
#define DHTPIN 4                  					//// Define DHT11 Pin - 4
#define DHTTYPE DHT11               				//// Define Sensor Type - DHT11
DHT dht(DHTPIN, DHTTYPE);             				//// Initialize the DHT11 Sensor

////Liquid Crystal I2C
#include <LiquidCrystal_I2C.h>            			//// Include LiquidCrystal_I2C Library
LiquidCrystal_I2C lcd(0x27, 16, 2);                 //// 0x27 is LCD I2C Address

////Starting Variable Definitions --- What should these be set to?
int Moisture = 0;                					//// Stores Moisture Values
int LimT = 75;                    					//// Stores Temp Limit Value After Edit
int SoilTarget = 75;                  				//// Stores Soil Moisture Value (Target) After Edit
int SoilWet = 50;                  					//// Stores Soil Moisture Value (Wet) After Edit
int SoilDry = 75;                  					//// Stores Soil Moisture Value (Dry) After Edit
int relay1_state;                      				//// Stores State of relay1
int relay2_state;                      				//// Stores State of relay2
//int startTime = 6; 								//// Integer hour in 24hr format to start lights
//int endTime = 22; 								//// Integer hour in 24hr format to cut the lights

////Pin Definitions || LCD CHEAT SHEET (SCL = A5, SDA =A4)
const int relay1_pin = 9;                           //// Light Relay GPIO
const int relay2_pin = 8;             				//// Water Relay GPIO
const int LED1 = 7;									//// LED1-Red (System Active) GPIO
const int LED2 = 6;									//// LED2-Green (System Idle) GPIO
const int pot1 = A1;                				//// Potentiometer to Change Temp Threshold GPIO
const int pot2 = A2;                				//// Potentiometer to Change Soil Moisture Threshold GPIO
const int soilPower = 2;              				//// Soil Sensor Power GPIO
const int soilSensor = A0;              			//// Soil Sensor Data GPIO
//const int piezoPin = 13;							//// Piezo Buzzer GPIO
//const int pwm_pin = XX;							//// PWM GPIO (ASSIGN #XX)


////Start Setup - Initialize and Set Pin Modes
void setup() {
 Serial.begin(9600);                                //// Initialize Serial Monitor at baud 9600
 
 dht.begin();                                       //// Initialize the DHT Temp Sensor
 
 lcd.backlight();                                   //// Turn on lcd backlight
 lcd.init();                                        //// Initialize lcd
 
 pinMode(pot1, INPUT);                				    //// Set Potentiometer1 as Input
 pinMode(pot2, INPUT);                			       //// Set Potentiometer2 as Input
 
 pinMode(LED1, OUTPUT);								       //// Set LED1 as Output
 pinMode(LED2, OUTPUT); 							       //// Set LED2 as Output
 
 pinMode(relay1_pin, OUTPUT);                       //// Set relay_pin1 as Output
 digitalWrite(relay1_pin, LOW);                     //// Set relay_pin1 initial state LOW
 
 pinMode(relay2_pin, OUTPUT);                       //// Set relay_pin2 as Output
 digitalWrite(relay2_pin, LOW);                     //// Set relay_pin2 initial state LOW
}

////Start Loop
void loop() {


////Temp/Humdity Reading
float Temp = dht.readTemperature(true);             //// Read the temperature and store as float variable "Temp" -- (true) means F 
float Humidity = dht.readHumidity();        		//// Read the humidity and store as float variable "Humidity"
//float Temp_Upper = 75;                            //// Hard Code Upper temperature threshold
//float Temp_Lower = 74;                            //// Hard Code Lower temperature threshold

int LimT = (analogRead(pot1) - 23) / 10;      		 //// Read Temp Limit from Potentiometer1 -- (1023-23)/10=100%.  Store as int "LimT"
int SoilTarget = (analogRead(pot2) - 23) / 10;      //// Read Soil Moisture Limit from Potentiometer2. Store as int "SoilTarget" 


////Soil Moisture Reading
int readSoil(); 
  digitalWrite(soilPower, HIGH);          			 //// Turn On - Soil Sensor
  delay(10);                    					       //// MAKE LESS FREQUESNT? Wait 10 milliseconds for reading
  Moisture = (analogRead(soilSensor) - 23) / 10;    //// Read the soilSensor analog signal from A0 -- (1023-23)/10=100%
  digitalWrite(soilPower, LOW);           			 //// Turn Off - Soil Sensor

////LCD Printouts
   lcd.clear();
   lcd.setCursor(0,0);                              //// Set the cursor on the first row and first column
   lcd.print("T=");
   lcd.print(Temp, 0);                              //// LCD Print - Temp Value - No Decimal
   lcd.print(" ");
   lcd.print("LT=");
   lcd.print(LimT);                             	 //// LCD Print - LimT
   lcd.print(" ");    
   lcd.print("H=");
   lcd.print(Humidity, 0);                          //// LCD Print - Humidity Value - No Decimal
   lcd.print("%"); 

////Generic Serial Print     ----- Serial.println <==this will print on a new line
   Serial.print("Temp: ");                          
   Serial.print(Temp);                            	//// Serial Print - Temp Value
   Serial.print("\t");							      	//// Tab
   Serial.print("Humidity: ");                      
   Serial.print(Humidity);                			//// Serial Print - Humidity Value
   Serial.print("\t");
   Serial.print("Soil: ");                          
   Serial.print(Moisture);                 			//// Serial Print - Soil Moisture Value
   Serial.print("\t");
   Serial.print("LimT: ");                          
   Serial.print(LimT);                      	   	//// Serial Print - Limit Temp
   Serial.print("\t");
   Serial.print("SoilTarget: ");                    
   Serial.print(SoilTarget);                    	//// Serial Print - Limit Soil Moisture 

/*
////Generic Relay State Output with Serial 1 or 0

   relay1_state = digitalRead(relay1_pin);         //// State of the Relay Pin 
   Serial.print("Fan= ");      	      				//// Serial Print - Fan Pin Status  
   Serial.println(relay1_state);				      	//// Outputs 1 or 0
   
   relay2_state = digitalRead(relay2_pin);         //// State of the Relay Pin 
   Serial.print("Pump= ");		           			   //// Serial Print - Fan Pin Status  
   Serial.println(relay2_state); 				   	//// Outputs 1 or 0 
*/ 

//// Temp with LimT ***************************************************************************
    if (isnan(Temp)) {                             //// Test for Temp Signal 
    lcd.print("N/A");                              //// LCD Print "N/A"
    Serial.println("DHT11 N/A");                   //// Serial Print "N/A"
    delay(1000);                                   //// Wait 1 Second
    return;
    }
//// else if will run if the 1st if statement=false. 
   else if (Temp > LimT) {                 			//// Changed to LimT instead of Temp_Upper
      Serial.println("Fan: ON");                  	//// Serial Print Fan Status
      digitalWrite(relay1_pin, HIGH);              //// Turn On - Normal Open (Relay) is closed when below threshold
      lcd.setCursor(0,1);                          //// Set the cursor on the LCD second row and first column
      lcd.print("Fan:ON ");                      	//// Print Fan status to LCD (ON)
	  digitalWrite(LED1, HIGH);					   	//// Turn On - LED1
	  digitalWrite(LED2, LOW);						      //// Turn OFF - LED2
      //tone(piezoPin, 1000, 500);				   	//// Play Tone
      delay(10);
	}
//// else will run if the both if and else if statements are false. 
   else { 					          			      	//// Changed to LimT instead of Temp_Lower
      Serial.println("Fan: OFF");                 	//// Serial Print Fan Status
      digitalWrite(relay1_pin, LOW);               //// Turn Off - Normal Open (Relay) is open when below threshold
	  lcd.setCursor(0,1);                           //// Set the cursor on the LCD second row and first column
      lcd.print("Fan:OFF ");                     	//// Print Fan status to LCD (OFF)
	  digitalWrite(LED2, HIGH);						   //// Turn On - LED2 -- HOW TO COMBINE ON/OFF LED INTO ONE COMMAND
	  digitalWrite(LED1, LOW);						      //// Turn OFF - LED1
	}

/*
//// Soil Moisture -- SET SoilWet***************************************************************************
   if (Moisture >= SoilWet && Moisture < SoilDry) {
      Serial.println("Soil Perfect  - Pump: OFF");  //// Serial Print Pump Status
	  Serial.print(Moisture);						//// Serial Print Soil Moisture Reading
      digitalWrite(relay2_pin, LOW);            	//// Turn Off - Normal Open (Relay) is open when above threshold
      lcd.setCursor(8,1);                       	//// Set the cursor on the LCD second row and eighth column
      lcd.print("Pump:OFF ");                   	//// Print Pump status to LCD (OFF)
	}
   else if (Moisture < SoilWet) {
      Serial.println("Soil Too Wet - Pump: OFF");   //// Serial Print Pump Status
	  Serial.print(Moisture);						//// Serial Print Soil Moisture Reading
      digitalWrite(relay2_pin, LOW);            	//// Turn Off - Normal Open (Relay) is open when above threshold
      lcd.setCursor(8,1);                       	//// Set the cursor on the LCD second row and eighth column
      lcd.print("Pump:OFF ");                   	//// Print Pump status to LCD (OFF)
	}
   else {
      Serial.println("Soil Too Dry - Pump: ON");    //// Serial Print Pump Status
	  Serial.print(Moisture);						//// Serial Print Soil Moisture Reading
      digitalWrite(relay2_pin, HIGH);       		//// Turn On - Normal Open (Relay) is closed when below threshold
      lcd.setCursor(8,1);                       	//// Set the cursor on the LCD second row and eighth column
      lcd.print("Pump:ON ");           		    	//// Print Pump status to LCD (ON)
    } 
*/

/*
//// PWM Fan ***************************************************************************
    if (isnan(Temp)) {                              //// No Temp Signal 
    lcd.print("N/A");                               //// LCD Print "N/A"
    Serial.println("DHT11 N/A");                    //// Serial Print "N/A"
    delay(1000);                                    //// Wait 1 Second
    return;
    }

   else if (Temp > 72) {                 			//// PWM Threshold
      Serial.println("Fan: ON 20%");                //// Serial Print Fan Status
      analogWrite(pwm_pin,50);               		//// analogWrite PWM 50 (20%)
      lcd.setCursor(0,1);                           //// Set the cursor on the LCD second row and first column
      lcd.print("Fan:20%");                      	//// Print Fan status to LCD (ON 20%)
      delay(10);
	}
	
   else if (Temp > 75) {                 			//// PWM Threshold
      Serial.println("Fan: ON 80%");                //// Serial Print Fan Status
      analogWrite(pwm_pin,200);               		//// analogWrite PWM 200 (80%)
      lcd.setCursor(0,1);                           //// Set the cursor on the LCD second row and first column
      lcd.print("Fan:80%");                      	//// Print Fan status to LCD (ON 80%)
      delay(10);
	}
	  
   else { 					          				//// Changed to LimT instead of Temp_Lower
      Serial.println("Fan: OFF");                 	//// Serial Print Fan Status
      digitalWrite(relay1_pin, LOW);                //// Turn Off - Normal Open (Relay) is open when below threshold
      lcd.setCursor(0,1);                           //// Set the cursor on the LCD second row and first column
      lcd.print("Fan:OFF ");                     	//// Print Fan status to LCD (OFF)
	}
*/


 delay(2000);
   lcd.clear();
}