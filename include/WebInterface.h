#pragma once
#include <Arduino.h>

class WebInterface {
 private:
  void startCameraServer();

 public:
  WebInterface();
  void wifiSetup();
};