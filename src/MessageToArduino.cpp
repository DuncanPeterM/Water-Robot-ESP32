#include "MessageToArduino.h"

#include <Arduino.h>

MessageToArduino::MessageToArduino() {}

void MessageToArduino::start() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

void MessageToArduino::Forward() {
  Serial2.println("F");
}

void MessageToArduino::Backwards() {
  Serial2.println("B");
}

void MessageToArduino::Left() {
  Serial2.println("L");
}

void MessageToArduino::Right() {
  Serial2.println("R");
}

void MessageToArduino::TurnLeft() {
  Serial2.println("X");
}

void MessageToArduino::TurnRight() {
  Serial2.println("Y");
}

void MessageToArduino::TurnBack() {
  Serial2.println("Z");
}