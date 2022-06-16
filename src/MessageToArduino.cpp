#include "MessageToArduino.h"

#define RXp2 16
#define TXp2 0

#include <Arduino.h>
String Message;
MessageToArduino::MessageToArduino() {}

void MessageToArduino::start() {
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
}

void MessageToArduino::Forward() {
  Serial2.print("F\n");
  delay(4000);
  /*do {
    Message = Serial.readString();
  } while (Message != "Done");*/
}

void MessageToArduino::Backwards() {
  Serial2.print("B\n");
  delay(4000);
}

void MessageToArduino::Left() {
  Serial2.print("L\n");
  delay(8000);
}

void MessageToArduino::Right() {
  Serial2.print("R\n");
  delay(4000);
}

void MessageToArduino::TurnLeft() {
  Serial2.println("X\n");
  delay(4000);
}

void MessageToArduino::TurnRight() {
  Serial2.println("Y\n");
  delay(4000);
}

void MessageToArduino::TurnBack() {
  Serial2.println("Z\n");
<<<<<<< HEAD
}

void MessageToArduino::MoveNorth(int dir) {
  if (dir = 1) {
    TurnLeft();
  } else if (dir = 2) {
    TurnLeft();
    TurnLeft();
  } else if (dir = 3) {
    TurnRight();
  }
  Forward();
}

void MessageToArduino::MoveSouth(int dir) {
  if (dir = 0) {
    TurnRight();
    TurnRight();
  } else if (dir = 1) {
    TurnRight();
  } else if (dir = 3) {
    TurnLeft();
  }
  Forward();
}

void MessageToArduino::MoveEast(int dir) {
  if (dir = 0) {
    TurnRight();
  } else if (dir = 2) {
    TurnLeft();
  } else if (dir = 2) {
    TurnRight();
    TurnRight();
  }
  Forward();
}

void MessageToArduino::MoveWest(int dir) {
  if (dir = 0) {
    TurnLeft();
  } else if (dir = 1) {
    TurnRight();
    TurnRight();
  } else if (dir = 2) {
    TurnLeft();
    TurnRight();
  }
=======
  delay(8000);
>>>>>>> d39d0817381ad3d378ce533194079354c867da72
}