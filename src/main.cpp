#include "Arduino.h"
#include "ir.h"

#define SCK 12
#define RCK 10
#define SI  11
#define GAUGE_MAX_BIT 8

int speed = 5;

void outputGauge(int level) {
  unsigned char gauge = bit(constrain(level, 1, 9) - 1) - 1;
  digitalWrite(RCK, LOW);
  shiftOut(SI, SCK, MSBFIRST, gauge);
  digitalWrite(RCK, HIGH);
  digitalWrite(GAUGE_MAX_BIT, level >= 10 ? HIGH : LOW);
}

void onReceiveIR(unsigned int makerCode, unsigned int dataCode, bool isRepeat) {
  if (makerCode != 0x08F7) return;
  switch (dataCode) {
    case 0x1FE0:
      speed--;
      break;
    case 0x1EE1:
      speed = 5;
      break;
    case 0x1AE5:
      speed++;
      break;
  }
  speed = constrain(speed, 1, 10);
  outputGauge(speed);
}

void setup() {
  Serial.begin(9600);
  Serial.println("listening to IR signals...");
  pinMode(SCK, OUTPUT);
  pinMode(RCK, OUTPUT);
  pinMode(SI,  OUTPUT);
  pinMode(GAUGE_MAX_BIT, OUTPUT);
  outputGauge(speed);
  initIR(4);
}

unsigned char count = 0;

void loop() {

  delay(75);
}
