#include <Servo.h>

char inData[6]; // Allocate some space for the string
char inChar=0; // Where to store the character read
int indexData = 0; // Index into array; where to store the character
Servo servoX;
Servo servoY;
int xangle;
int yangle;

void setup() {
	// initialize servo 
	servoX.attach(10);
	servoY.attach(11);

	// init angle
	xangle=90;
	yangle=90;
	servoX.write(xangle);
	servoY.write(yangle);


	// initialize serial port at a baud rate of 115200 bps
	Serial.begin(115200);
	delay(100);
	Serial.println("start");
}

void loop() {
  
	while (Serial.available() > 0) // Don't read unless there you know there is data
	{
       
		inChar = Serial.read(); // Read a character
		inData[indexData] = inChar; // Store it
		indexData++; // Increment where to write next
		if(indexData>=4) {
		  indexData=0;
		}
		if(inData[3]==';'){
			// x angle
			if(inData[0]=='0'){
			// decrement angle 
			if(xangle>0) xangle-=5;;
			}else if(inData[0]=='2'){
			// increment angle
			if(xangle<180) xangle+=5;
			}

			// y angle
			if(inData[2]=='0'){
			// increment angle
			if(yangle>0) yangle+=5;
			} else if(inData[2]=='2'){
			// decrement angle
			if(yangle<180) yangle-=5;
			} 
		  
			// give angle to servo 
			servoX.write(xangle);
			servoY.write(yangle);

			// erease information
			indexData=0;
			int i=0;
			while(i<5){
				inData[i]='0';
				i++; 
			}
		}       
    }
}



