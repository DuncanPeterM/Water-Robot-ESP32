#include <Arduino.h>

#include "Distance.h"
#include "MessageToArduino.h"

Distance Ultrasonic;
MessageToArduino Robot;

#define squareSize 21

byte PositionArray[50][50] = {0};

int x_value = 24, y_value = 24;
int left, right, forward;
int leftMatrixCheck, rightMatrixCheck, forwardMatrixCheck = 0;

int checkNextPosition(int i, int j) {
  if ((PositionArray[i][j] == 0) || (PositionArray[i][j] == 1)) {
    return 0;
  } else if (PositionArray[i][j] == 4) {
    return 1;
  } else {
    return 4;
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

void turnDirection(char Rotation) {
  switch (Rotation) {
    case 'L':
      Serial.println(F("turn 90 to left "));
      // add the code that turns 90 degree to the right
      Robot.TurnLeft();
      break;
    case 'R':
      Serial.println(F("turn 90 to right "));
      // add the code that turns 90 degree to the right
      Robot.TurnRight();
      break;
    case 'B':
      Serial.println(F("turn 180 "));
      // add the code that reverse the direction
      Robot.TurnBack();
      break;
  }
}

void moveDir(char Dir) {
  switch (Dir) {
    case 'F':
      Serial.println(F("go foward"));
      Robot.Forward();
      x_value = x_value + 1;
      break;
    case 'B':
      Serial.println(F("go backwards"));
      Robot.Backwards();
      x_value = x_value - 1;
      break;
    case 'L':
      Serial.println(F("go left"));
      Robot.Forward();
      y_value = y_value - 1;
      break;
    case 'R':
      Serial.println(F("go right"));
      Robot.Forward();
      y_value = y_value + 1;
      break;
  }
  if (x_value < 50 && y_value < 50) {
    PositionArray[x_value][y_value] = {1};
  }
  Serial.println(PositionArray[x_value][y_value]);
}

void rightNextCell() {
  Serial.println(F("right next cell"));
  int i = 1;
  if ((right > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    if ((left > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24)
      turnDirection('R');  // do 1 time only
  }
  getDistance();

  while ((forward > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    turnDirection('B');
    i++;
    getDistance();
    Serial.println(F("right next cell while 1 "));
  }
  turnDirection('B');  // revese the position to go backward to the position before checking the nighbouring cell
  while (i > 1) {
    Serial.println(F("right next cell while 2 "));

    // add the code for moving 1 step forward
    i--;

    if (i == 1) {
      i = 0;
    }
  }

  if (i == 0) {
    turnDirection('R');  // look to the origianl direction
  }
}

void leftNextCell() {
  Serial.println(F("left next cell"));
  int i = 1;
  if ((left > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    turnDirection('L');                                                                           // do 1 time only
  }
  getDistance();

  while ((forward > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    turnDirection('B');
    i++;
    getDistance();
    Serial.println(F("left next cell while 1 "));
  }
  turnDirection('B');  // revese the position to go backward to the position before checking the nighbouring cell
  while (i > 1) {
    Serial.println(F("left next cell while 2 "));

    // add the code for moving 1 step forward
    i--;

    if (i == 1) {
      i = 0;
    }
  }

  if (i == 0) {
    turnDirection('L');  // look to the origianl direction
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

  if (x_value + 1 <= 50 || y_value + 1 <= 50) {  // matrix limits, distance limits .... you can add other limits like finding the source here

    if ((right > squareSize) && (rightMatrixCheck == 1)) {  // priority is always right
      Serial.println(F("right servo "));

      turnDirection(90);
      getDistance();

      while ((forward > squareSize) && (rightMatrixCheck == 1)) {
        moveDir('R');
        // rightNextCell();
        getDistance();
      }
      turnDirection(-90);  // go back facing the forward position
    }

    if ((left > squareSize) && (leftMatrixCheck == 1)) {
      Serial.println(F("left servo "));
      turnDirection(-90);
      getDistance();
      while ((forward > squareSize) && (leftMatrixCheck == 1)) {
        moveDir('L');
        // leftNextCell();
        getDistance();
      }
      turnDirection('R');  // go back facing the forward position
    }

    if ((forward > squareSize) && (forwardMatrixCheck == 1)) {
      Serial.println(F("forward servo "));
      getDistance();
      moveDir('F');
    }
    if (((forward < squareSize) || (leftMatrixCheck == 4)) && ((forward < squareSize) || (rightMatrixCheck == 4)) && ((left < squareSize) || (leftMatrixCheck == 4))) {  // stop
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
  Ultrasonic.start();  // attaches the servo on pin 9 to the servo object
  Robot.start();
  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
  creatematrix();
}

void loop() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      Serial.print(PositionArray[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  servocontrol();
}