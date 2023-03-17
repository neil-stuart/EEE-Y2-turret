#include <Servo.h>

// Size of the data received.
// Has to be synchronised with the sending script packet length.
const byte DATA_SIZE = 28;

// An array to hold the received data.
char csv_in[DATA_SIZE];

// All the constants.
const float SPEED = 0.05;
const int fireExtendedPosition = 50;
const int fireRetractedPosition = 100;
const int fireTime = 350; // Total time taken to fire a bullet in milliseconds.
const int tiltMinMax[2] = {60, 100};
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
boolean leftTrig = false;
boolean rightTrig = false;
float rightJoyX = 0.0f;
float rightJoyY = 0.0f; 

// Values needed for the loop to keep track of current state.
long fireStartTime;

boolean stopped = true;

boolean firing = false;
boolean movingForwards = false;
boolean movingBackwards = false;
boolean flyWheelSpinning = false;
boolean lastLeftBumper = false;

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
  const float nextTiltPosition = tiltPosition + (rightJoyY * SPEED); 
  const float nextPanPosition = panPosition + (rightJoyX * SPEED);

  if (withinBounds(nextTiltPosition, tiltMinMax))
  {
    tiltServo.write(tiltPosition = nextTiltPosition);
  }

  // Horizontal Aim
  if (withinBounds(nextPanPosition, panMinMax))
  {
    panServo.write(panPosition = nextPanPosition);
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

  if(!stopped && (rightTrig==leftTrig)){
    stopped = true;
    movingBackwards = false;
    movingForwards = false;
    stopMoving();
  }

  if(!movingForwards && rightTrig && !leftTrig){
    movingForwards = true;
    movingBackwards = false;
    stopped = false;
    moveBackwards();
  }

  if(!movingBackwards && leftTrig && !rightTrig){
    movingBackwards = true;
    movingForwards = false;
    stopped = false;
    moveForwards();
  }

}

void moveForwards(){
    digitalWrite(FR1, LOW);
    digitalWrite(FR2, HIGH);
    digitalWrite(BR1, LOW);
    digitalWrite(BR2, HIGH);
    digitalWrite(FL1, HIGH);
    digitalWrite(FL2, LOW);
    digitalWrite(BL1, HIGH);
    digitalWrite(BL2, LOW);
}

void moveBackwards(){
    digitalWrite(FR2, LOW);
    digitalWrite(FR1, HIGH);
    digitalWrite(BR2, LOW);
    digitalWrite(BR1, HIGH);
    digitalWrite(FL2, HIGH);
    digitalWrite(FL1, LOW);
    digitalWrite(BL2, HIGH);
    digitalWrite(BL1, LOW);
}

void stopMoving(){
    digitalWrite(FR1, HIGH);
    digitalWrite(FR2, HIGH);
    digitalWrite(BR1, HIGH);
    digitalWrite(BR2, HIGH);
    digitalWrite(FL1, HIGH);
    digitalWrite(FL2, HIGH);
    digitalWrite(BL1, HIGH);
    digitalWrite(BL2, HIGH);
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

static bool inProgress = false;
static byte index = 0;

// This method receives avaiable data and stores it in csv_in.
void serialEvent()
{

  // Indicator characters to begin/end a block of controller data.
  char startChar = '[';
  char stopChar = ']';

  char lastReadChar;
  
  while (Serial.available() > 0)
  {
    lastReadChar = (char) Serial.read();

    // If the start character is encountered, change to "in progress" mode.
    if (lastReadChar == startChar)
    {
      inProgress = true;
      lastReadChar = (char) Serial.read();
    }

    if (inProgress == true)
    {
      // Terminating condition.
      if (lastReadChar == stopChar)
      {
        csv_in[index] = '\0'; // Terminate the string.
        inProgress = false;
        index = 0;
        // Parse the data into its numerical values.
        parseData();
        break;
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
  Serial.write(csv_in);

  token = strtok(csv_in, ",");
  leftTrig = atoi(token);
  
  token = strtok(NULL, ",");
  rightTrig = atoi(token);
  // Subsequent tokens are retrieved using strtok() with NULL as the first argument.
  // This means that the function continues parsing the string from where it left off in the previous call.

  // The second token is converted to a float value and stored in a global variable called rightJoyX.
  token = strtok(NULL, ",");
  rightJoyX = -atof(token);

  // The third token is converted to a float value and stored in a global variable called rightJoyY.
  token = strtok(NULL, ",");
  rightJoyY = -atof(token);

  // The fourth token is converted to an integer value and stored in a global variable called leftBumper.
  token = strtok(NULL, ",");
  leftBumper = atoi(token);

  // The fifth token is converted to an integer value and stored in a global variable called rightBumper.
  token = strtok(NULL, ",");
  rightBumper = atoi(token);

  // Code to send back the values parsed, for debugging.
  // Convert the floats to ints (multiply by *100 first)
  // because floats arent supported by sprintf in the arduino compiler.

  Serial.print(csv_in);
}
