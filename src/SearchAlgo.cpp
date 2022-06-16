#include "SearchAlgo.h"

#include <Arduino.h>
#include <math.h>

#include "Distance.h"
#include "MessageToArduino.h"

Distance Ultrasonic;
MessageToArduino Robot;

int endloop = 0;
const int squareSize = 22;
const int matrix_x = 50;
const int matrix_y = 50;
byte PositionArray[50][50] = {0};

int left, right, forward;
int leftMatrixCheck, rightMatrixCheck, forwardMatrixCheck = 0;
int NextMatrix = 0;
int x_value = 1, y_value = 24;

// Declaring the grid and its variations ( grid and Hueristic and dynamic path )

int hueristic[matrix_x][matrix_y];

// Declaring start goal and various other variables
int goalx = 1;
int goaly = 24;
int locx = x_value;
int locy = y_value;
int locx2;
int locy2;
int indexZ;
int costarray[4];

int g = 1;
int minV = 0;

int abs_x = 0;
int abs_y = 0;

SearchAlgo::SearchAlgo() {}

int SearchAlgo::checkNextPosition(int i, int j) {
  if ((PositionArray[i][j] == 0) || (PositionArray[i][j] == 1)) {
    return 0;
  } else if (PositionArray[i][j] == 4) {
    return 1;
  } else {
    return 4;
  }
}

void SearchAlgo::getDistance() {
  left = Ultrasonic.ScanRight() - 13;

  rightMatrixCheck = checkNextPosition(x_value, y_value + 1);

  forward = Ultrasonic.ScanFoward();

  forwardMatrixCheck = checkNextPosition(x_value + 1, y_value);

  right = Ultrasonic.ScanLeft() - 13;

  leftMatrixCheck = checkNextPosition(x_value, y_value - 1);
  NextMatrix = checkNextPosition(x_value - 1, y_value);
  Serial.println(("right"));
  Serial.println((right));
  Serial.println(("left"));
  Serial.println((left));
  Serial.println(("forward"));
  Serial.println((forward));
}

void SearchAlgo::moveDir(char Dir) {
  switch (Dir) {
    case 'F':
      Serial.println(F("go foward"));
      Robot.Forward();
      x_value = x_value + 1;
      break;
    case 'B':
      Serial.println(F("go backwards"));
      Robot.Backwards();
      x_value = x_value + 1;
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
    case 'N':
      Serial.println(F("go backwards"));
      Robot.Forward();
      x_value = x_value - 1;
      break;
  }
  if (x_value < 50 && y_value < 50) {
    PositionArray[x_value][y_value] = {1};
  }
  // Serial.println(PositionArray[x_value][y_value]);
  Serial.println(x_value);
  Serial.println(y_value);
}

void SearchAlgo::rightNextCell() {
  Serial.println(F("right next cell"));
  int i = 1;
  if ((right > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    if ((left > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24)
      Robot.TurnRight();  // do 1 time only
  }
  getDistance();

  while ((forward > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    Robot.TurnBack();
    i++;
    getDistance();
    Serial.println(F("right next cell while 1 "));
  }
  Robot.TurnBack();  // revese the position to go backward to the position before checking the nighbouring cell
  while (i > 1) {
    Serial.println(F("right next cell while 2 "));

    // add the code for moving 1 step forward
    i--;

    if (i == 1) {
      i = 0;
    }
  }

  if (i == 0) {
    Robot.TurnRight();  // look to the origianl direction
  }
}

void SearchAlgo::leftNextCell() {
  Serial.println(F("left next cell"));
  int i = 1;
  if ((left > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    Robot.TurnLeft();                                                                             // do 1 time only
  }
  getDistance();

  while ((forward > squareSize) && (PositionArray[x_value - i][y_value] == 4) && (x_value - i) > 24) {  // check unvisited cells next to this cell
    Robot.TurnBack();
    i++;
    getDistance();
    Serial.println(F("left next cell while 1 "));
  }
  Robot.TurnBack();  // revese the position to go backward to the position before checking the nighbouring cell
  while (i > 1) {
    Serial.println(F("left next cell while 2 "));

    // add the code for moving 1 step forward
    i--;

    if (i == 1) {
      i = 0;
    }
  }

  if (i == 0) {
    Robot.TurnLeft();  // look to the origianl direction
  }
}

void SearchAlgo::AddBlockToMatrix() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      if (PositionArray[i][j] == 4) {
        PositionArray[i][j] = {0};
      }
    }
  }
}

void SearchAlgo::findpath() {
  Robot.TurnBack();
  locx = x_value;
  locy = y_value;
  /*Serial.println("hereeeeeee");
   Serial.println(locx);
    Serial.println(locy);
     Serial.println(goalx);
      Serial.println(goaly);*/

  while (locx != goalx || locy != goaly) {
    if (locx <= matrix_x && locy <= matrix_y) {
      if (locx - 1 >= 0 && locy >= 0) {
        if (PositionArray[locx - 1][locy] == 1) {
          costarray[0] = g + hueristic[locx - 1][locy];
          // Serial.println("found 0 ");
        } else {
          costarray[0] = 400;
        }
      } else {
        costarray[0] = 400;
      }
      if (locx >= 0 && locy + 1 >= 0) {
        if (PositionArray[locx][locy + 1] == 1) {
          costarray[1] = g + hueristic[locx][locy + 1];
          // Serial.println("found 1 ");
        } else {
          costarray[1] = 400;
        }
      } else {
        costarray[1] = 400;
      }
      if (locx + 1 >= 0 && locy >= 0) {
        if (PositionArray[locx + 1][locy] == 1) {
          costarray[2] = g + hueristic[locx + 1][locy];
          // Serial.println("found 2 ");
        } else {
          costarray[2] = 400;
        }
      } else {
        costarray[2] = 400;
      }
      if (locx >= 0 && locy - 1 >= 0) {
        if (PositionArray[locx][locy - 1] == 1) {
          costarray[3] = g + hueristic[locx][locy - 1];
          // Serial.println("found 3 ");
        } else {
          costarray[3] = 400;
        }
      } else {
        costarray[3] = 400;
      }
      //----------------------------------------------------------------------------------------------------------------------
      minV = costarray[0];
      indexZ = 0;
      // Finding node with least cost
      for (int i = 0; i < 4; i++) {
        if (costarray[i] <= minV) {
          indexZ = i;
          minV = costarray[i];
        }
      }
      //----------------------------

      // PositionArray[locx][locy]=1;
      // move your position to the new location
      if (indexZ == 0) {
        locx = locx - 1;

      } else if (indexZ == 1) {
        locy = locy + 1;
      } else if (indexZ == 2) {
        locx = locx + 1;
      } else if (indexZ == 3) {
        locy = locy - 1;
      }

      /* Serial.print("(");
       Serial.print(locx);
       Serial.print(",");
       Serial.print(locy);
       Serial.print(")");
       Serial.println();*/

      // Serial.println(PositionArray[2][0]);
      abs_x = locx - locx2;
      abs_y = locy - locy2;
      locy2 = locy;
      locx2 = locx;
      Serial.print("(");
      Serial.print(abs_x);
      Serial.print(",");
      Serial.print(abs_y);
      Serial.print(")");
      Serial.println();
      if (abs_x == 1) {
        Serial.println("    go back 1 step");
      }

      if (abs_x == -1) {
        Serial.println("    go forward 1 step");
        Robot.Forward();
      } else if (abs_y == 1) {
        Robot.TurnRight();
        Robot.Forward();
        Robot.TurnLeft();
        Serial.println("    go right 1 step");
      } else if (abs_y == -1) {
        Serial.println("    go left 1 step");
        Robot.TurnLeft();
        Robot.Forward();
        Robot.TurnRight();
      }

      if (locx == goalx && locy == goaly) {
        Serial.println("target found  ");
        // for(;;);
      }
    }
  }
  endloop = 1;
}

void SearchAlgo::heur() {
  if ((goaly < matrix_y && goalx < matrix_x) || (goalx > -1 && goaly > -1)) {
    for (int i = 0; i < matrix_y; i++) {
      for (int j = 0; j < matrix_x; j++)

        if (PositionArray[j][i] == 1) {
          hueristic[j][i] = (abs(j - goalx) + abs(i - goaly));

        } else {
          hueristic[j][i] = (abs(j - goalx) + abs(i - goaly)) + 500;
        }
    }
  }
  //------------------------------------------------// Uncomment Region to view the hueristic PositionArray
  for (int i = 0; i < matrix_x; i++) {
    for (int j = 0; j < matrix_y; j++) {
      // Serial.print(hueristic[i][j]);
      // Serial.print("  ");
    }
    // Serial.println();
  }
  // for(;;);
  //------------------------------------------------
}

int SearchAlgo::servocontrol() {
  if (endloop == 1) {
    return 1;
  }
  getDistance();

  Serial.println(F(" servo "));

  // compare ultrasonic sensor outputs

  if (x_value + 1 <= 50 || y_value + 1 <= 50) {  // matrix limits, distance limits .... you can add other limits like finding the source here

    if ((right > squareSize) && (rightMatrixCheck == 1)) {  // priority is always right
      Serial.println(F("right servo "));

      Robot.TurnRight();

      getDistance();

      while ((forward > squareSize) && (rightMatrixCheck == 1)) {
        moveDir('R');
        // rightNextCell();
        getDistance();

        if (right > squareSize && NextMatrix == 1) {
          Robot.TurnRight();
          moveDir('N');
          moveDir('B');  // remove when code start and add robot.forward
          Robot.TurnLeft();
          Serial.println(F("right next"));
        }

        getDistance();
      }
      if (x_value < 50 && y_value < 50) {
        int tempx1 = x_value;
        int tempy1 = y_value + 1;
        PositionArray[tempx1][tempy1] = {0};
      }
      Robot.TurnLeft();  // go back facing the forward position
    }
    getDistance();

    if ((left > squareSize) && (leftMatrixCheck == 1)) {
      Serial.println(F("left servo "));
      Robot.TurnLeft();

      getDistance();

      while ((forward > squareSize) && (leftMatrixCheck == 1)) {
        moveDir('L');

        // leftNextCell();
        getDistance();
        if (left > squareSize && NextMatrix == 1) {
          Robot.TurnLeft();
          moveDir('N');
          moveDir('B');  // not move just needs to going back
          Robot.TurnRight();
          Serial.println(F("Next left"));
        }
        getDistance();
      }
      if (x_value < 50 && y_value < 50) {
        int tempx1 = x_value;
        int tempy1 = y_value - 1;
        PositionArray[tempx1][tempy1] = {0};
      }
      Robot.TurnRight();  // go back facing the forward position
    }

    getDistance();

    if ((forward > squareSize) && (forwardMatrixCheck == 1)) {
      Serial.println(F("forward servo "));

      getDistance();

      moveDir('F');
    }

    if (((forward < squareSize) || (leftMatrixCheck == 4)) && ((forward < squareSize) || (rightMatrixCheck == 4)) && ((left < squareSize) || (leftMatrixCheck == 4))) {  // stop
      Serial.println(F("mapped everything or stuck"));
      AddBlockToMatrix();
      // AddBlockToMatrix();
      heur();
      findpath();
    }

  } else {
    Serial.println(F("reached max limits"));
    AddBlockToMatrix();
    heur();
    findpath();
  }
  return 0;
}

void SearchAlgo::creatematrix() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      PositionArray[i][j] = 4;
      if ((i == 0) || (i == 49) || (j == 0) || (j == 49)) {
        PositionArray[i][j] = 0;
      }
    }
  }
  PositionArray[1][24] = {1};
  Serial.println(F("matrix created"));
}

void SearchAlgo::matrixprint() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      Serial.print(PositionArray[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void SearchAlgo::SearchAlgoSetup() {
  Ultrasonic.start();  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
  Serial2.begin(9600, SERIAL_8N1, 16, 0);
  creatematrix();
}