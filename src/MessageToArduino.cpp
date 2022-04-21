#include "MessageToArduino.h"

#include <Arduino.h>
#include <SoftwareSerial.h>
String Message;
MessageToArduino::MessageToArduino() {}

void MessageToArduino::start() {
  Serial2.begin(9600, SERIAL_8N1, 16, 12);
}

void MessageToArduino::Forward() {
  Serial2.println("F");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::Backwards() {
  Serial2.println("B");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::Left() {
  Serial2.println("L");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::Right() {
  Serial2.println("R");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::TurnLeft() {
  Serial2.println("X");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::TurnRight() {
  Serial2.println("Y");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}

void MessageToArduino::TurnBack() {
  Serial2.println("Z");
  do {
    Message = Serial.readString();
  } while (Message != "Done");
}