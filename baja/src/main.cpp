#include <Arduino.h>

// amount of time per tick in microseconds; time when tick starts
unsigned long tickTimeUs, startTime;

// state trigger: when true need to output high voltage, otherwise no voltage
bool highVoltTick = true;
// indicated beginning of high voltage or no voltage of tick
bool startTickSegment = true;

// potentiometer analog pin
const int potPin = A0;
// voltage output pin
const int outputPin = 2;
// need to know number of gear teeth in gear to convert rpm to number of ticks
const int numGearTeeth = 16;

const double minRPM = 0;
const double maxRPM = 6000;

// unsigned long getNumTicks(double rpm, double time);
void setHighVoltageOutput();
void setLowVoltageOutput();

// put function declarations here:

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(outputPin, OUTPUT);
}

void loop() {
  // potentiometer returns value from 1-1023
  int potVal = analogRead(potPin);
  double rpm = minRPM + (double)potVal / 1023.0 * (maxRPM - minRPM);

  if (rpm > 0) 
    tickTimeUs = (unsigned long)(60000000.0 / (rpm * numGearTeeth));
  else {
    setLowVoltageOutput();
    return;
  }

  unsigned long now = micros();
  if (highVoltTick) {
    if (startTickSegment) {
        startTime = now;
        setHighVoltageOutput();
        startTickSegment = false;
    }

    // tickTime / 2 because half the tick is higher voltage and other half is no voltage
    if (now - startTime >= tickTimeUs / 2) {
      highVoltTick = false;
      startTickSegment = true;
    }
  }

  else {
    // similar to previous
    if (startTickSegment) {
      startTime = now;
      setLowVoltageOutput();
      startTickSegment = false;
    }
    if (now - startTime >= tickTimeUs / 2) {
      highVoltTick = true;
      startTickSegment = true;
    }
  }
}

// sets output to 5 volts
void setHighVoltageOutput() {
  digitalWrite(outputPin, HIGH);
}

// sets output to 0 volts
void setLowVoltageOutput() {
  digitalWrite(outputPin, LOW);
}