#include <stdio.h>

// Attach a LED to Pin 13
int led1 = 10;
int led2 = 11;

char inData[5]; // Allocate some space for the string
char xaxis[2];
char yaxis[2];
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output and turn off 
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  
  // initialize serial port at a baud rate of 115200 bps
  Serial.begin(115200);
  delay(100);
  Serial.println("start");
}

void loop() {
  

  
   while (Serial.available() > 0) // Don't read unless there you know there is data
   {
        if(index < 4) // One less than the size of the array
        {
            inChar = Serial.read(); // Read a character
            inData[index] = inChar; // Store it
            index++; // Increment where to write next
            inData[index] = '\0'; // Null terminate the string
        }
    }
    
    if(inData[3]==';'){
      if(inData[0]=='0'){
        digitalWrite(led1, HIGH); // turn on light
        Serial.println("led 1 on");
      }else if(inData[0]=='2'){
        digitalWrite(led1, HIGH); // turn on light
        Serial.println("led 1 on");
      } else {
         digitalWrite(led1, LOW); // turn off light
         Serial.println("led 1 on");
      }
      
      if(inData[2]=='0'){
         digitalWrite(led2, HIGH); // turn on light
         Serial.println("led 1 on");
      } else if(inData[2]=='2'){
         digitalWrite(led2, HIGH); // turn on light
         Serial.println("led 1 on");
      } else {
         digitalWrite(led2, LOW); // turn off light
         Serial.println("led 2 off");
      }
    }

  delay(10); 
}



