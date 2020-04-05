#include <math.h>
#include "LCD_Launchpad.h"

// pin outs
const int NTCPin = A11;

// LCD
LCD_LAUNCHPAD myLCD;

// temperature variables
// temperature will be averaged over 'TEMP_SIZE' * 'DELAY' seconds
float Vo = 3.3;
const float R1 = 20000;
float logR2, R2, T;
float ADCvalue = 0.0;
float Resistance = 0.0;
const float c1 = 2.124064633e-03, c2 = 0.7716788971e-04, c3 = 4.872126180e-07;
const int SERIESRESISTOR = 20000;

const int DELAY = 500; // milliseconds
// how many temperatures to average
const int TEMP_SIZE = 1;
float tempArr[TEMP_SIZE] = {};
int tempArrCounter = 0;
float outputTemperature = 0;

boolean firstIteration = true; // if program is at first iteration

void setup() {
  Serial.begin(9600);
  myLCD.init();
  pinMode(P1_1, INPUT_PULLUP);
}

void loop() {
  
  findTemperature();
  
  // LCD message
  Serial.println(String(outputTemperature, 2));
  String message = (String(outputTemperature, 2) + "F").substring(2,8);
  message.replace(".", "");
  Serial.println(message);
  myLCD.displayText(message, 0);
  myLCD.showSymbol(LCD_SEG_DOT2, 1);
  
  myLCD.showSymbol(LCD_SEG_BAT_ENDS, 1); 
  myLCD.showSymbol(LCD_SEG_BAT0, 1); // bat 0-5
  myLCD.showSymbol(LCD_SEG_BAT1, 1); // bat 0-5
  myLCD.showSymbol(LCD_SEG_BAT2, 1); // bat 0-5
  myLCD.showSymbol(LCD_SEG_BAT3, 1); // bat 0-5
  myLCD.showSymbol(LCD_SEG_BAT4, 1); // bat 0-5
  myLCD.showSymbol(LCD_SEG_BAT5, 1); // bat 0-5

  
  delay(DELAY);
}

void findTemperature(){
  analogReadResolution(10);
  ADCvalue = analogRead(NTCPin);

  /*
  Serial.print("ADC: ");
  Serial.println(ADCvalue);
  */
  
 

  //convert value to resistance
  Resistance = (1023 / ADCvalue) - 1;
  Resistance = SERIESRESISTOR / Resistance;

  /*
  Serial.print(Resistance);
  Serial.println(" Ohm");
  */
  
  R2 = R1 * ((1023.0 / ADCvalue) - 1.0);
  logR2 = logf(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 
  
  Serial.print("Temp = ");
  Serial.print(T);   
  Serial.println(" F");

  // if program is ran first iteration, then must immediately output first temperature result
  if(firstIteration){
    outputTemperature = T;
    
    firstIteration = false;
  }
  else{
    // find temperature
    if(tempArrCounter == TEMP_SIZE){
      outputTemperature = findAverage();
        
      tempArrCounter = 0;
    }
    else{
      tempArr[tempArrCounter] = T;
      tempArrCounter++;
    }
  }
}


float findAverage(){
  float sum = 0;
  for(int i = 0; i < TEMP_SIZE; i++){
    sum += tempArr[i];
  }
  return sum/TEMP_SIZE;
}
