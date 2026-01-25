#include <Arduino.h>

// current data point number
int currDataNum = 0;
// total ticks in data point
unsigned long totalTicks = 0;
// current tick number being output from total ticks in data point
unsigned long currTickNum = 0;
// time in milliseconds per tick
double tickTime = 0;

unsigned long startTime;

// state trigger: when true need to output high voltage, otherwise no voltage
bool highVoltTick = true;
// indicated beginning of high voltage or no voltage of tick
bool startTickSegment = true;

// output pin; can be anything
const int outputPin = 8;
// number of data points
const int numData = 3;
// need to know number of gear teeth in gear to convert rpm to number of ticks
const int numGearTeeth = 30;
// input data: rpm, amount of time in seconds at rpm
const double rpmTime[numData][2] = {
  {60, 2},
  {120, 2},
  {30, 1}
};

unsigned long getNumTicks(double rpm, double time);
void setHighVoltageOutput();
void setLowVoltageOutput();

// put function declarations here:

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(outputPin, OUTPUT);
}

void loop() {

  if (highVoltTick) {
    if (currDataNum < numData) {
      if (currTickNum == totalTicks) {
        // calculate total ticks, reset current tick counter, calculate tick time
        totalTicks = getNumTicks(rpmTime[currDataNum][0], rpmTime[currDataNum][1]);
        currTickNum = 0;
        tickTime = 1000.0 / (rpmTime[currDataNum][0] / 60.0 * numGearTeeth);
      }

      if (startTickSegment) {
        startTime = millis();
        setHighVoltageOutput();
        startTickSegment = false;
      }

      // tickTime / 2 because half the tick is higher voltage and other half is no voltage
      if (millis() - startTime >= tickTime / 2) {
        highVoltTick = false;
        startTickSegment = true;
      }
    }
    // when done with all data output no voltage
    if (currDataNum >= numData) {
      setLowVoltageOutput();
      return;
    }
  }

  else if (!highVoltTick) {
    // similar to previous
    if (startTickSegment) {
      startTime = millis();
      setLowVoltageOutput();
      startTickSegment = false;
    }
    if (millis() - startTime >= tickTime / 2) {
      highVoltTick = true;
      startTickSegment = true;
      currTickNum++;
      if (currTickNum == totalTicks)
        currDataNum++;
    }
  }
}

// gets total number of ticks for each data point
unsigned long getNumTicks(double rpm, double time) {
  return rpm / 60 * time * numGearTeeth;
}

// sets output to 5 volts
void setHighVoltageOutput() {
  digitalWrite(outputPin, HIGH);
}

// sets output to 0 volts
void setLowVoltageOutput() {
  digitalWrite(outputPin, LOW);
}