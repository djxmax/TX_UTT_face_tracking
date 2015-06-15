#include <Servo.h>



// Attach a LED to Pin 13
int led1 = 8;
int led2 = 9;

char inData[6]; // Allocate some space for the string
char inChar=0; // Where to store the character read
int indexData = 0; // Index into array; where to store the character
Servo servoX;
Servo servoY;
int xangle;
int yangle;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output and turn off 

  //pinMode(led1, OUTPUT);
  //pinMode(led2, OUTPUT);
  //digitalWrite(led1, LOW);
  //digitalWrite(led2, LOW);
  servoX.attach(10);
  servoY.attach(11);
  
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
              if(inData[0]=='0'){
                //digitalWrite(led1, HIGH); // turn on light
                if(xangle>0) xangle-=5;;
                //Serial.println("led 1 on");
              }else if(inData[0]=='2'){
                //digitalWrite(led1, HIGH); // turn on light
                //Serial.println("led 1 on");
                if(xangle<180) xangle+=5;
              } else {
                 //digitalWrite(led1, LOW); // turn off light
                 //Serial.println("led 1 off");
              }
              
              if(inData[2]=='0'){
                 //digitalWrite(led2, HIGH); // turn on light
                 if(yangle>0) yangle+=5;
                 //Serial.println("led 2 on");
              } else if(inData[2]=='2'){
                 //digitalWrite(led2, HIGH); // turn on light
                 if(yangle<180) yangle-=5;
                 //Serial.println("led 2 on");
              } else {
                 //digitalWrite(led2, LOW); // turn off light
                 //Serial.println("led 2 off");
              }
              
              servoX.write(xangle);
              servoY.write(yangle);
              indexData=0;
              int i=0;
              while(i<5){
                inData[i]='à';
                i++; 
              }
            }
        
    }

}



