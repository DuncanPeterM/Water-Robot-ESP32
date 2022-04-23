#include "MessageToArduino.h"

#define RXp2 16
#define TXp2 0

#include <Arduino.h>
#include <SoftwareSerial.h>
String Message;
MessageToArduino::MessageToArduino() {}

void MessageToArduino::start() {
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
}

void MessageToArduino::Forward() {
  Serial2.print("F\n");
  /*do {
    Message = Serial.readString();
  } while (Message != "Done");*/
}

void MessageToArduino::Backwards() {
  Serial2.print("B\n");
}

void MessageToArduino::Left() {
  Serial2.print("L\n");
}

void MessageToArduino::Right() {
  Serial2.print("R\n");
}

void MessageToArduino::TurnLeft() {
  Serial2.println("X\n");
}

void MessageToArduino::TurnRight() {
  Serial2.println("Y\n");
}

void MessageToArduino::TurnBack() {
  Serial2.println("Z\n");
}