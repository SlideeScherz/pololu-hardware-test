#include <Pololu3piPlus32U4.h>
#include <Servo.h>
#include "Ultrasonic.h" //my file
using namespace Pololu3piPlus32U4;

//Buzzer Data =====  
Buzzer buzzer;

//US Data =====
/*create an object for sensor*/
Ultrasonic ultrasonic(5, 4, true);
float distance = 0;

//Servo Data =====
Servo headServo;   

/** mobo connection to servo*/
const int HEAD_SERVO_PIN = 18;

/** show status in log*/
bool debugServo = false;

/** Direction servo is sweeping*/
bool sweepingClockwise = true;

/** Store timestamps for servo*/
unsigned long sT1, sT2;

/** frequency between sweeps */
const unsigned long SWEEP_PERIOD = 300;

/**current array index of which degree the head should be
 * init to 90 Degrees
 */
int currentHeadPosition = 3;

// head servo constants
const int NUM_HEAD_POSITIONS = 7;
const int HEAD_POSITIONS[NUM_HEAD_POSITIONS] = {135, 120, 105, 90, 75, 60, 45}; 
const int MAX_HEAD_DEGREES = HEAD_POSITIONS[NUM_HEAD_POSITIONS - 1];
const int MIN_HEAD_DEGREES = HEAD_POSITIONS[0];

void setup() {
  Serial.begin(9600);

  headServo.attach(HEAD_SERVO_PIN);

  delay(1000);

}

void loop() {

  sweepHead();
  distance = ultrasonic.sendPing();

  Serial.print("distance:");
  Serial.print(distance);
  Serial.print(",");
  Serial.print("servoAngle:");
  Serial.print(headServo.read());
  Serial.print(",");
  Serial.print("headPos:");
  Serial.println(currentHeadPosition);
}


//servo methods
//TODO Config this into header file soon. Difficulty accessing Servo.h with my own 


/**
 * @brief Simple controller to move head
 * Interval = SWEEP_PERIOD
 * Reads from HEAD_POSITIONS
 * Sets servo head to a position*/
void sweepHead()
{

  sT1 = millis();

  //TODO Implement a clock class
  if (sT1 > sT2 + SWEEP_PERIOD)
  {
    // move head to the correct angle
    headServo.write(HEAD_POSITIONS[currentHeadPosition]);

    //algo to cycle through photos i used in a react.js project
    //once we hit max, "bounce" the other way
    //start 3. 4, 5, 6, then toggle
    //after 1st pass: start 0. 1, 2, 3, 4, 5, 6 then toggle
    if(sweepingClockwise)
    {
      currentHeadPosition = (NUM_HEAD_POSITIONS + currentHeadPosition + 1) % NUM_HEAD_POSITIONS;
      if(currentHeadPosition == 6) toggleSweep();
    }
    //start 6. 5, 4, 3, 2, 1, 0 then toggle
    else
    {
      currentHeadPosition = (NUM_HEAD_POSITIONS + currentHeadPosition - 1) % NUM_HEAD_POSITIONS;
      if(currentHeadPosition == 0) toggleSweep();
    }

    // store previous time
    sT2 = sT1;
  }
}

/**Output Data to serial monitor*/
void logServo()
{
  //set bounds for chart
  Serial.print("Max:");
  Serial.print(MAX_HEAD_DEGREES);
  Serial.print(",");
  Serial.print("Min:");
  Serial.print(MIN_HEAD_DEGREES);
  Serial.print(",");
  //===================
  Serial.print("ActualRead:");
  Serial.print(headServo.read());
  Serial.print(",");
  Serial.print("headPos:");
  Serial.println(currentHeadPosition);
}

void toggleSweep()
{
  sweepingClockwise = !sweepingClockwise;
}
