// Libraries
#include <Servo.h>

/////////////////
//  PROPERTIES
/////////////////

// Scanner Servo
Servo scanServo; // New Servo Object

// Motors
const int LeftMotorForward = 6; // Pin 6 
const int LeftMotorBackward = 7; // Pin 6
const int RightMotorForward = 8; // Pin 5 
const int RightMotorBackward = 9; // Pin 5 
// Onboard LED
int arduinoLed = 13; // LED on Pin 13

// Ultrasonic Sensor(HC-SR04)
const int USTrigger = 10; // Pin 10
const int USEcho = 11; // Pin 11
const int maxFrontDistance = 27; // Distance limit for obstacles in front
const int maxSideDistance = 12; // Distance limit for obstacles at sides

// Scan Variables
int leftDistance;
int rightDistance;
int centerDistance;

// Other Variables and Constants
int distance;                // Current Distance to closest Obstacle
int numcycles = 0;
int pulseDuration;           // Duration of Sonar Pulse
int check;                   // Number of correction
char moveDirection;          // Gets 'l' 'r' 'f', depending on the considered direction
const int turnTime = 900;    // Time of turning in miliseconds

//Wireless or IR Controller(Optional)


/////////////////
//    SETUP
/////////////////
void setup(){
  scanServo.attach(5);
  scanServo.write(90); // Servo at Center (Half of 10 Degrees)
  delay(100); // Wait for Servo's Setup
  
  pinMode(LeftMotorForward, OUTPUT); 
  pinMode(LeftMotorBackward, OUTPUT); 
  pinMode(RightMotorForward, OUTPUT); 
  pinMode(RightMotorBackward, OUTPUT); 
  pinMode(USTrigger,OUTPUT);
  pinMode(USEcho,INPUT);
  pinMode(arduinoLed, OUTPUT);    
  
  digitalWrite(LeftMotorForward,LOW); // Sets Left Motor Off
  digitalWrite(LeftMotorBackward,LOW); // Sets Left Motor Off
  digitalWrite(RightMotorForward,LOW); // Sets Right Motor Off
  digitalWrite(RightMotorBackward,LOW); // Sets Right Motor Off
  digitalWrite(USTrigger,LOW); // Sets Ultrasonic Trigger Off
  pinMode(arduinoLed, LOW); // Sets LED on Arduino Off
  
  Serial.begin(9600); // For Serial Monitor
  delay(2000); // Wait 2 seconds at startup
}

/////////////////
//    LOOP
/////////////////

void loop(){
  moveForward(); // Start moving forward
  distance = sendPulse(); // Send a pulse in every loop
  if(distance < maxFrontDistance){check++  ;} 
  if(distance > maxFrontDistance){check = 0;} // Set zero to Correction value
  
  if(check > 25){ // Checks the distance to correct for 25 times
    moveStop(); // Stops for Scanning since there is an obstacle ahead
    moveDirection = decideDirection(); // Scans and decides the direction that it should go
    switch (moveDirection){
      case 'l':
        turnLeft(turnTime);
        break;
      case 'r':
        turnRight(turnTime);
        break;
      case 'f':
        ; //  Does not turn if there was nothing ahead
        break;
    }
  }
}

/////////////////
//   METHODS
/////////////////

int sendPulse(){
  long howFar;
  // Send Pulse
  delayMicroseconds(2);
  digitalWrite(USTrigger,HIGH); // Sends a pulse 
  delayMicroseconds(5);         // Wait for 5 Microseconds
  digitalWrite(USTrigger,LOW);  // Stop sending
  
  // Read Echo
  pulseDuration = pulseIn(USEcho,HIGH); // Read Echo
  howFar = (pulseDuration / 2) / 29.1; // Converts Microseconds to Centimeters
  Serial.print("Pulse:  "); // For Serial Monitor
  Serial.println(howFar); // For Serial Monitor
  return round(howFar); // Rounds and Returns as an integer formed distance
}

void scan(){
 
  centerDistance = sendPulse();  // Scan Center
  if(centerDistance < maxFrontDistance){moveStop();}  // Stops if the Maximum Distance has been Exceeded
  scanServo.write(160);  // Servo turns full left
  delay(100);  
  
  leftDistance = sendPulse();  // Scan Left
  if(leftDistance < maxSideDistance){moveStop();}  // Stops if the Maximum Distance has been Exceeded
  scanServo.write(90); // Servo returns normal
  delay(100);  
 
  centerDistance = sendPulse();  // Scan Center Again
  if(centerDistance < maxFrontDistance){moveStop();}  // Stops if the Maximum Distance has been Exceeded
  scanServo.write(20); // Servo turns full right
  delay(100);  
  
  rightDistance = sendPulse();   // Scan Right
  if(rightDistance < maxSideDistance){moveStop();}  // Stops if the Maximum Distance has been Exceeded
  scanServo.write(90); // Servo looks forward after finishing
  delay(300);
}

char decideDirection(){
  char choice;
  scan(); // the method fills the distance datas for the decision
  if(leftDistance > rightDistance && leftDistance > centerDistance ){
    choice = 'l'; // Decision is Left
  }
  else if(leftDistance < rightDistance && rightDistance > centerDistance ){
    choice = 'r'; // Decision is Right
  }
  else{ // Means that Center Distance is greater than Sides
    choice = 'f'; // Decision is Front
  }
  return choice;  // Returns the final decision as an char
}


void moveForward(){
  digitalWrite(LeftMotorForward,HIGH);
  digitalWrite(LeftMotorBackward,LOW);
  digitalWrite(RightMotorForward,HIGH);
  digitalWrite(LeftMotorBackward,LOW);
}

void moveBackward(){
  digitalWrite(LeftMotorForward,LOW);
  digitalWrite(LeftMotorBackward,HIGH);
  digitalWrite(RightMotorForward,LOW);
  digitalWrite(LeftMotorBackward,HIGH);
}

void turnRight(int time)                                    
{
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, HIGH);
  delay(time);
}

void turnLeft(int time)                                      
{
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(RightMotorForward, HIGH);
  delay(time);
}

void moveStop()                                     
{
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

