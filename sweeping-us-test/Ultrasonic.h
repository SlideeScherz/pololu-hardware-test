#ifndef ULTRASONIC_H
#define ULTRASONIC_H

class Ultrasonic
{

    // class members====

    //Ultrasonic timing
    unsigned long uT1; //current timestamp
    unsigned long uT2; //previous timestamp
    const unsigned long uPeriod = 50; //Period to wait before sending US ping
    const unsigned long PING_TIMEOUT = 38000;

  public:

    bool debug;
    int TRIG_PIN;
    int ECHO_PIN;

    // constructor for new US
    Ultrasonic(int triggerPinNum, int echoPinNum, bool debugUltrasonic)
    {

      // initalize pins and read IO config
      TRIG_PIN = triggerPinNum;
      ECHO_PIN = echoPinNum;
      debug = debugUltrasonic;
      pinMode(TRIG_PIN, OUTPUT);
      pinMode(ECHO_PIN, INPUT);
    }

    // get distance using sensor state to determine ping
    float sendPing()
    {

      // get current time
      uT1 = millis();

      // send ping if enough time has passed
      if (uT1 > uT2 + uPeriod)
      {

        //if(debug)logUltrasonic();
        
        // Clear Trigger pin
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);

        // Activate Trigger pin for 10 microseconds
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW); // Clear Trigger pin

        // Read echo pin and read second wave travel time
        // Leave here for now
        timeDuration = pulseIn(ECHO_PIN, HIGH, PING_TIMEOUT);

        // calculate d1 TOF = return time * speed of sound / 2
        distance = timeDuration * 0.034 / 2;

        uT2 = uT1; // store last timestamp

        return distance;
      }
    }

    //show readings to monitor
    void logUltrasonic()
    {
      Serial.print(",");
      Serial.print("Distance:");
      Serial.println(distance);
    }

    // helper methods called above
  private:
    // private members
    float timeDuration, distance;

    // set trig to low
    void trigStateLOW()
    {
      digitalWrite(TRIG_PIN, LOW);
    }

    // set trig to high
    void trigStateHIGH()
    {
      digitalWrite(TRIG_PIN, HIGH);
    }
};
#endif
