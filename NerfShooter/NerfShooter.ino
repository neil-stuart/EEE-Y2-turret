#include <Servo.h>

// Size of the data received.
// Has to be synchronised with the sending script packet length.
const byte DATA_SIZE = 25;

// An array to hold the received data.
char csv_in[DATA_SIZE];

// All the constants.
const float SPEED = 0.05;
const int fireExtendedPosition = 50;
const int fireRetractedPosition = 100;
const int fireTime = 350; // Total time taken to fire a bullet in milliseconds.
const int tiltMinMax[2] = {85, 120};
const int panMinMax[2] = {30, 150};

Servo tiltServo;
float tiltPosition = 90;

Servo panServo;
float panPosition = 90;

Servo fireServo; 

// Pins for the pan and tilt servos. (PWM)
const byte panServoPin = 9;
const byte tiltServoPin = 10;
const byte fireServoPin = 11;

// Fly wheel control pin D2
const byte flyWheelPin = 2; 

// H-Bridge pins FR = front right motor, BR = back right motor etc..
const byte FR1 = A0; 
const byte FR2 = A1;
const byte FL1 = 7; 
const byte FL2 = 8;
const byte BL1 = 3; 
const byte BL2 = 4;
const byte BR1 = A2; 
const byte BR2 = A3;

// Variables to store the numerical values of the received data.
boolean rightBumper = false;
boolean leftBumper = false;
float leftJoyX = 0.0f; // Ranges from -1 to 1.
float rightJoyX = 0.0f;
float rightJoyY = 0.0f; 

// Values needed for the loop to keep track of current state.
long fireStartTime;
boolean firing = false;
boolean movingForwards, movingBackwards;
boolean stopped = true;
boolean flyWheelSpinning, lastLeftBumper = false;

void setup()
{
  // Begin the Serial transmission
  Serial.begin(115200);

  firing = false;
  
  // Attach the servos to their respective pins.
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);
  fireServo.attach(fireServoPin);

  // Retract the firing servo, write initial aim direction to pan tilt servos.
  fireServo.write(fireRetractedPosition);
  tiltServo.write(tiltPosition);
  panServo.write(panPosition);

  // Set up pins as output
  pinMode(FR1, OUTPUT);
  pinMode(FR2, OUTPUT);

  pinMode(FL1, OUTPUT);
  pinMode(FL2, OUTPUT);

  pinMode(BL1, OUTPUT);
  pinMode(BL2, OUTPUT);

  pinMode(BR1, OUTPUT);
  pinMode(BR2, OUTPUT);

  pinMode(flyWheelPin, OUTPUT);

  stopMoving();
}

void loop()
{
  // Control the aiming servos.
  // Vertical Aim - First check if within aim bounds and then write the new angle if true.
  if (withinBounds(tiltPosition + (rightJoyY * SPEED), tiltMinMax))
  {
    tiltServo.write(tiltPosition += rightJoyY * SPEED);
  }

  // Horizontal Aim
  if (withinBounds(panPosition + (rightJoyX * SPEED), panMinMax))
  {
    panServo.write(panPosition += rightJoyX * SPEED);
  }

  // Check if fire button pressed, if so, fire a bullet. 175 ms forward  & 175ms back
  if (rightBumper && !firing)
  {
    firing = true;
    fireServo.write(fireExtendedPosition);
    fireStartTime = millis();
  }

  if(firing){
    if(millis() - fireStartTime >= fireTime){
      firing = false;
    }else if(millis() - fireStartTime >= (fireTime/2)){
      fireServo.write(fireRetractedPosition);
    }
  }

  // Button to turn on/off the flywheel.
  if (leftBumper && !lastLeftBumper)
  {
    digitalWrite(flyWheelPin, flyWheelSpinning = !flyWheelSpinning);
  }

  lastLeftBumper = leftBumper;

  // Movement
  if (!movingForwards && leftJoyX>0)
  {
    movingForwards = true;
    movingBackwards, stopped = false;
    moveForwards();
  }

  if(!movingBackwards && leftJoyX<0){
    movingBackwards = true;
    movingForwards, stopped = false;
    moveBackwards();
  }

  if(!stopped && leftJoyX==0){
    stopped = true;
    movingBackwards, movingForwards = false;
    stopMoving();
  }

}

void moveForwards(){

}

void moveBackwards(){

}

void stopMoving(){

}

// Returns true if n is between the bounds in the form int[2] = [int min, int max]
boolean withinBounds(float n, const int bounds[2])
{
  if (n > bounds[0] && n < bounds[1])
  {
    return true;
  }
  return false;
}

// This method receives avaiable data and stores it in csv_in.
void serialEvent()
{
  boolean dataReceived = false;
  static bool inProgress = false;
  static byte index = 0;

  // Indicator characters to begin/end a block of controller data.
  char startChar = '[';
  char stopChar = ']';

  char lastReadChar;
  
  while (Serial.available() > 0 && dataReceived == false)
  {
    lastReadChar = (char) Serial.read();

    // If the start character is encountered, change to "in progress" mode.
    if (lastReadChar == startChar)
    {
      inProgress = true;
    }

    else if (inProgress == true)
    {
      // Terminating condition.
      if (lastReadChar == stopChar)
      {
        csv_in[index] = '\0'; // Terminate the string.
        inProgress = false;
        index = 0;

        // Parse the data into its numerical values.
        parseData();

        dataReceived = true;
      }

      csv_in[index] = lastReadChar;
      index++;

      // Stop data index exceeding the max.
      if (index >= DATA_SIZE)
      {
        index = DATA_SIZE - 1;
      }
    }
  }
}

// Takes a string of comma-separated values (csv_in)
// and parses each value, storing them in their corresponding gamepad variables.
void parseData()
{
  // A pointer to a character (char *) called token is declared to store the parsed values.
  char *token;

  // strtok() is a C library function used to parse a string (in this case, csv_in)
  // into tokens (i.e., substrings) based on a specified delimiter (",").
  // Here, it is called with csv_in as the first argument, and a comma as the delimiter.

  // The first token is then retrieved and converted to a float value using atof().
  // The value is stored in a global variable called leftJoyX.
  token = strtok(csv_in, ",");
  leftJoyX = atof(token);

  // Subsequent tokens are retrieved using strtok() with NULL as the first argument.
  // This means that the function continues parsing the string from where it left off in the previous call.

  // The second token is converted to a float value and stored in a global variable called rightJoyX.
  token = strtok(NULL, ",");
  rightJoyX = -atof(token);

  // The third token is converted to a float value and stored in a global variable called rightJoyY.
  token = strtok(NULL, ",");
  rightJoyY = atof(token);

  // The fourth token is converted to an integer value and stored in a global variable called leftBumper.
  token = strtok(NULL, ",");
  leftBumper = atoi(token);

  // The fifth token is converted to an integer value and stored in a global variable called rightBumper.
  token = strtok(NULL, ",");
  rightBumper = atoi(token);

  // Code to send back the values parsed, for debugging.
  // Convert the floats to ints (multiply by *100 first)
  // because floats arent supported by sprintf in the arduino compiler.

  // char strBuf[33];
  // sprintf(strBuf, "[%d\%%,%d\%%,%d\%%,%d,%d]", (int)(leftJoyX * 100), (int)(rightJoyX * 100), (int)(rightJoyY * 100), leftBumper, rightBumper);
  // Serial.print(strBuf);
}
