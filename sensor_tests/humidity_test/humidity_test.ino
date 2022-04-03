
/* 
 HTU21D Humidity Sensor Example Code
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 15th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 Modified By: Karthik Baggaon Rajendra (As per required functionality)
 Uses the HTU21D library to display the current humidity.
 
 Open serial monitor at 9600 baud to see readings. Errors 998 if not sensor is detected. Error 999 if CRC is bad.
  
 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 -GND = GND
 -SDA = A4 (use inline 330 ohm resistor if your board is 5V)
 -SCL = A5 (use inline 330 ohm resistor if your board is 5V)

 */

#include <Wire.h>
#include "SparkFunHTU21D.h"

//Create an instance of the object
HTU21D myHumidity;

void setup()
{
  Serial.begin(9600);
  Serial.println("HTU21D Humidity Sensor Test!");
  myHumidity.begin();
}

void loop()
{
  
  Serial.print(" Humidity:");Serial.print(myHumidity.readHumidity());
  Serial.print("%");

  Serial.println();
  delay(4000);
}
