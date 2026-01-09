#include <Arduino.h>

int currDataNum = 0;
int currTickNum = 0;
int totalTicks = 0;
double tickTime = 0;

unsigned long startTime;

bool highVoltTick = true;
bool startTick = true;

// output pin; can be anything
const int outputPin = 8;
// number of data points
const int numData = 3;
// need to know number of gear teeth in gear to convert rpm to number of ticks
const int numGearTeeth = 30;
// input data: rpm, amount of time in seconds at rpm
const double rpmTime[numData][2] PROGMEM = {
  {1, 2},
  {1, 1},
  {1, 1}
};

// put function declarations here:

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(outputPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (!highVoltTick) {
    if (startTick) {
      startTime = millis();
      setLowVoltageOutput();
      startTick = false;
    }
    if (millis() - startTime >= tickTime / 2) {
      highVoltTick = true;
      startTick = true;
      currTickNum++;
      if (currTickNum == totalTicks)
        currDataNum++;
    }
  }

  if (highVoltTick) {
    if (currDataNum < numData) {
      if (currTickNum == totalTicks) {
        totalTicks = getNumTicks(rpmTime[currDataNum][0], rpmTime[currDataNum][1]);
        currTickNum = 0;
        tickTime = 1000.0 / (rpmTime[currDataNum][0] / 60.0 * numGearTeeth);
      }

      if (startTick) {
        startTime = millis();
        setHighVoltageOutput();
        startTick = false;
      }

      if (millis() - startTime >= tickTime / 2) {
        highVoltTick = false;
        startTick = true;
      }
    }
    else
      setLowVoltageOutput();
  }

}

// gets total number of ticks for each data point
double getNumTicks(double rpm, double time) {
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