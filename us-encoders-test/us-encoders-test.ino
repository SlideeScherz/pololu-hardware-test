#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

//hardware
Buzzer buzzer;
Motors motors;

//Initialize Ultrasonic
const int ECHO_PIN = 21; //white wire
const int TRIG_PIN = 22; //brown wire
const float MAX_DISTANCE = 100.0F; //100cm

//Derived from max distance but dont want to mix type
//5882 = (max distance * 2) / speedOfSound
//const unsigned long PING_TIMEOUT = 5882;
const unsigned long PING_TIMEOUT = 38000; 

//normalization factor based on max
const float DISTANCE_FACTOR = MAX_DISTANCE / 100.0F;
const float STOP_DISTANCE = 10.0F;

//motor constants
const float MOTOR_BASE_SPEED = 300.0F;
const int MOTOR_MIN_SPEED = 35; //Was 30, Tuned

//motor compensation to keep straight
const float L_MOTOR_FACTOR = 1.0F;
const float R_MOTOR_FACTOR = 1.0F;
const float L_MOTOR_FACTOR_THRESHOLD = 80.0F;
const float R_MOTOR_FACTOR_THRESHOLD = 80.0F;

//normalization factor factor for speed
const float MOTOR_FACTOR = MOTOR_BASE_SPEED / 100.0F;

//motor timing
unsigned long mT1; //current timestamp
unsigned long mT2; //previous timestamp
const unsigned long mPeriod = 20; //Period to wait before setting motors

//Ultrasonic timing
unsigned long uT1; //current timestamp
unsigned long uT2; //previous timestamp
const unsigned long uPeriod = 50; //Period to wait before sending US ping

//current US readings
//Optimization, find algo to smooth this data
float distance = 0.0F;

//current motor readings, init to default
float leftSpeed = MOTOR_BASE_SPEED;
float rightSpeed = MOTOR_BASE_SPEED;

void setup() {

  //set serial monitor to 9600 baud
  Serial.begin(9600);

  //ultrasonic pin assignment
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  //we drive this robot backwards
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);

  //delay 1s before entering loop
  delay(1000);
  buzzer.play("c32");
}

void loop() {

  //get distance
  readUltraSonic();

  //move from distance derived in readUS
  setMotors();

  //debug environment
  debugMotors();
}

//return distance from ping
void readUltraSonic() {

  //get current time
  uT1 = millis();
  if (uT1 > uT2 + uPeriod) {

    //Clear Trigger pin
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    //Activate Trigger pin for 10 microseconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW); //Clear Trigger pin

    //Read echo pin and read second wave travel time
    //Leave here for now
    long duration = pulseIn(ECHO_PIN, HIGH, PING_TIMEOUT);
  
    //calculate d1 TOF = return time * speed of sound / 2
    distance = duration * 0.034 / 2;

    //apply limits
    if(distance > MAX_DISTANCE) distance = MAX_DISTANCE;
    if(distance == 0) distance = MAX_DISTANCE;
      
    uT2 = uT1; //store last timestamp
  }
}


//Distance should be read globally here
//Read US data and chase wall
void setMotors() {

  //get current time
  mT1 = millis();
  if (mT1 > mT2 + mPeriod) {

    //start at default speed, then adjust
    leftSpeed = MOTOR_BASE_SPEED;
    rightSpeed = MOTOR_BASE_SPEED;

    //check current d1, skip all if beyond
    if (distance <= MAX_DISTANCE) {

      //determine magnitude, in a linear manner
      //Explore logorithmic solutions
      float magnitude = (float)(MAX_DISTANCE - distance) / DISTANCE_FACTOR;

      //mulitply magnitude by MOTOR_FACTOR to map range (0-100) to motors
      leftSpeed = MOTOR_BASE_SPEED - (magnitude * MOTOR_FACTOR);
      rightSpeed = MOTOR_BASE_SPEED - (magnitude * MOTOR_FACTOR);
    }

    //check lower limit if too slow already
    if (leftSpeed < MOTOR_MIN_SPEED) leftSpeed = MOTOR_MIN_SPEED;
    if (rightSpeed < MOTOR_MIN_SPEED) rightSpeed = MOTOR_MIN_SPEED;

    //add motor compensation (Smoother between L and R wheels)
    if(leftSpeed <= L_MOTOR_FACTOR_THRESHOLD){
      leftSpeed *= L_MOTOR_FACTOR;
    }
    if(rightSpeed <= R_MOTOR_FACTOR_THRESHOLD){
      rightSpeed *= R_MOTOR_FACTOR;
    }

    //check for stop
    if (distance <= STOP_DISTANCE) {
      leftSpeed = 0;
      rightSpeed = 0;
    }

    //set speeds after calculations
    motors.setSpeeds(leftSpeed, rightSpeed);

    //store timestamp
    mT2 = mT1;
  }
}

//Debugging methods
//For Serial monitor, set bounds by including max and min to make chart smoother

//Output US Data
void debugUltraSonic() {
  Serial.print("Max Dist:");
  Serial.print(MAX_DISTANCE);
  Serial.print(",");
  Serial.print("Stop Dist:");
  Serial.print(STOP_DISTANCE);
  Serial.print(",");
  Serial.print("Distance:");
  Serial.println(distance);
}

//Output motor data
void debugMotors() {
  Serial.print("Min Speed:");
  Serial.print(MOTOR_MIN_SPEED);
  Serial.print(",");
  Serial.print("Base Speed:");
  Serial.print(MOTOR_BASE_SPEED);
  Serial.print(",");
  Serial.print("Left Speed:");
  Serial.print(leftSpeed);
  Serial.print(",");
  Serial.print("Right Speed:");
  Serial.println(rightSpeed);
}
