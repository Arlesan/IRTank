#include "Arduino.h"
#include "ir.h"

void onReceiveIR(unsigned int makerCode, unsigned int dataCode) {
  Serial.println(String(makerCode, 16) + ":" + String(dataCode, 16));
}

void setup() {
  Serial.begin(9600);
  Serial.println("listening to IR signals...");
  initIR(4);
}

void loop() {

}
