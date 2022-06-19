#include "distance.h"

#include <Arduino.h>
#include <ESP32Servo.h>

Servo myservo;

#define echoPin 13
#define trigPin 15
#define servoPin 14
#define WaterSensorPin 12

Distance::Distance() {
}

void Distance::start() {
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);   // Sets the echoPin as an INPUT
  Serial.println("Setup Ultrasonic Sensor");
  myservo.attach(servoPin);
}

void Distance::LookAngle(int angle) {
  myservo.write(angle + 90);
  delay(1000);  // to be changed to match later
}

int Distance::GetDistance() {
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distance = pulseIn(echoPin, HIGH) * 0.034 / 2;
  Serial.println(distance);
  if (distance < 1.5) {
    GetDistance();
  }
  return distance;
}

int Distance::ScanLeft() {
  LookAngle(-90);
  return GetDistance();
}

int Distance::ScanRight() {
  LookAngle(90);
  return GetDistance();
}

int Distance::ScanFoward() {
  LookAngle(0);
  return GetDistance();
}

int Distance::WaterSensor() {
  return analogRead(WaterSensorPin);
}