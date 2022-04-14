#include <ESP32Servo.h>

#include "Distance.h"

Distance Ultrasonic;

#define squareSize 21
#define echoPin 2  // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 4  // attach pin D3 Arduino to pin Trig of HC-SR04
unsigned char left = 200;
unsigned char right = 200;
unsigned char forward = 200;

byte PositionArray[50][50] = {0};
int pos = 0;  // variable to store the servo position
int x_value = 24;
int y_value = 24;

bool leftMatrixCheck = 0;
bool rightMatrixCheck = 0;
bool forwardMatrixCheck = 0;

char accumaltivePosition = 0;  // right = 1 left = 2 forward = 3 backward = 4

bool checkNextPosition(int i, int j) {
  if (PositionArray[i][j] == 0 || 1) {
    return 0;
  } else if (PositionArray[i][j] == 4) {
    return 1;
  }
}

void getDistance() {
  right = Ultrasonic.ScanRight();
  rightMatrixCheck = checkNextPosition(x_value, y_value + 1);

  forward = Ultrasonic.ScanFoward();
  forwardMatrixCheck = checkNextPosition(x_value + 1, y_value);

  left = Ultrasonic.ScanLeft();
  leftMatrixCheck = checkNextPosition(x_value, y_value - 1);
}

void turnDirection(int Rotation) {
  switch (Rotation) {
    case -90:
      Serial.println(F("turn 90 to left "));
      // add the code that turns 90 degree to the right
      break;
    case 90:
      Serial.println(F("turn 90 to right "));
      // add the code that turns 90 degree to the right
      break;
    case 180:
      Serial.println(F("turn 180 "));
      // add the code that reverse the direction
      break;
  }
}

void moveDir(char Dir) {
  switch (Dir) {
    case 'F':
      Serial.println(F("go foward"));
      x_value = x_value + 1;
      y_value = y_value;
      break;
    case 'B':
      Serial.println(F("go backwards"));
      x_value = x_value - 1;
      y_value = y_value;
      break;
    case 'L':
      Serial.println(F("go left"));
      x_value = x_value;
      y_value = y_value - 1;
      break;
    case 'R':
      Serial.println(F("go right"));
      x_value = x_value;
      y_value = y_value + 1;
      break;
  }
  PositionArray[x_value][y_value] = {1};
  Serial.println(PositionArray[x_value][y_value]);
}

void NextCell(char Dir) {
  Serial.print(F("left next cell  "));
  int i = 1;
  if ((left > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    if (Dir == 'L') {
      turnDirection(-90);
    } else if (Dir == 'R') {
      turnDirection(90);
    }
  }
  getDistance();

  while ((forward > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    moveDir('B');
    i++;
    getDistance();
    Serial.print(F("left next cell while 1 "));
  }
  turnDirection(180);  // revese the position to go backward to the position before checking the nighbouring cell
  while (i > 1) {
    Serial.print(F("left next cell while 2 "));

    // add the code for moving 1 step forward
    i--;

    if (i == 1) {
      i = 0;
    }
  }

  if (i == 0) {
    if (Dir == 'L') {
      turnDirection(-90);
    } else if (Dir == 'R') {
      turnDirection(90);
    }
  }
}

void AddBlockToMatrix() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      if (PositionArray[i][j] == 4) {
        PositionArray[i][j] = {0};
      }
    }
  }
}

int servocontrol() {
  getDistance();

  Serial.println(F(" servo "));

  // compare ultrasonic sensor outputs

  if (x_value + 1 < 50 || y_value + 1 < 50) {  // matrix limits, distance limits .... you can add other limits like finding the source here

    if ((right > squareSize) && (rightMatrixCheck == 1)) {  // priority is always right
      Serial.print(F("right servo "));

      turnDirection(90);
      getDistance();

      while ((forward > squareSize) && (rightMatrixCheck == 1)) {
        moveDir('R');
        NextCell('R');
        getDistance();
      }
      turnDirection(-90);  // go back facing the forward position
    }

    if ((left > squareSize) && (leftMatrixCheck == 1)) {
      Serial.print(F("left servo "));
      turnDirection(-90);
      getDistance();
      while ((forward > squareSize) && (leftMatrixCheck == 1)) {
        moveDir('L');
        NextCell('L');
        getDistance();
      }
      turnDirection(90);  // go back facing the forward position
    }

    if ((forward > squareSize) && (forwardMatrixCheck == 1)) {
      Serial.print(F("forward servo "));
      getDistance();
      while ((forward > squareSize) && (leftMatrixCheck == 1) && (forward > squareSize) && (rightMatrixCheck == 1) && (left > squareSize) && (leftMatrixCheck == 1)) {
        moveDir('f');
        getDistance();
      }
    }
    if (((forward < squareSize) || (leftMatrixCheck == 0)) && ((forward < squareSize) || (rightMatrixCheck == 0)) && ((left < squareSize) || (leftMatrixCheck == 0))) {  // stop
      Serial.println(F("mapped everything or stuck"));
      AddBlockToMatrix();
    }

  } else {
    Serial.println(F("reached max limits"));
    AddBlockToMatrix();
  }
  return 0;
}

void creatematrix() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      PositionArray[i][j] = 4;
    }
  }
  PositionArray[24][24] = {1};
  Serial.println(F("matrix created"));
}

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an OUTPUT
  Ultrasonic.start();        // attaches the servo on pin 9 to the servo object
  pinMode(echoPin, INPUT);   // Sets the echoPin as an INPUT
  Serial.begin(9600);        // // Serial Communication is starting with 9600 of baudrate speed
  creatematrix();
}

void loop() {
  servocontrol();
}