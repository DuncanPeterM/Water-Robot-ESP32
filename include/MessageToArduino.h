#pragma once
#include "MessageToArduino.h"

class MessageToArduino {
 private:
 public:
  MessageToArduino();

  void start();

  void Forward();

  void Backwards();

  void Left();

  void Right();

  void TurnLeft();

  void TurnRight();

  void TurnBack();
};