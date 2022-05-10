#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

Buzzer buzzer;
Motors motors;

//Initialize Ultrasonic
const int TRIG_PIN = 22; //Red wire
const int ECHO_PIN = 21; //blue wire
const int MAX_DISTANCE = 200; //200cm (2m)

//Ultrasonic timing
unsigned long t1; //current timestamp
unsigned long t2; //previous timestamp
const unsigned long P = 100; //Period to wait before sending US ping

//current US reading
int distance = 0;

void setup()
{

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  delay(1000); //dont you run away from me...
  buzzer.play("c32");
}

void loop() {

  usReadCm();

}

void usReadCm() {

  //Get current timestamp
  t1 = millis();

  //Wait for ping to be called
  if (t1 > t2 + P) {

    //Clear Trigger pin
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    //Activate Trigger pin for 10 microseconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    //Read echo pin and read second wave travel time
    //3800 microseconds supports max distance
    long duration = pulseIn(ECHO_PIN, HIGH, 38000);

    //calculate distance TOF = return time * speed of sound / 2
    distance = duration * 0.034 / 2;

    //apply limits
    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
    if (distance == 0) distance = MAX_DISTANCE;

    Serial.print(" Distance: ");
    Serial.println(distance);

    //Update previous time
    t2 = t1;
  }
}
