#pragma once
#include "Distance.h"

class Distance {
 private:
 public:
  Distance();

  void start();

  void LookAngle(int angle);

  int GetDistance();

  int ScanLeft();

  int ScanRight();

  int ScanFoward();
};