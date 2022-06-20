#include "Search.h"

#include "Distance.h"
#include "MessageToArduino.h"

Distance Ultrasonic;
MessageToArduino Robot;

Search::Search() {
}

void Search::Start() {
  Ultrasonic.start();  // attaches the servo on pin 9 to the servo object
  Serial.println("Ultrasonic Start");
  Serial2.begin(9600, SERIAL_8N1, 16, 0);
  Serial2.println("Serial2 (motor driver) Start");
}

void Search::setupMatrix(vector<vector<char>>& grid) {
  for (char i = 0; i < 50; i++) {
    for (char j = 0; j < 50; j++) {
      grid[i][j] = 0;
    }
    Serial.println("Set all to 0");
  }
  for (char i = 0; i < 50; i++) {
    grid[i][0] = 2;
    grid[i][24] = 2;
    grid[0][i] = 2;
    grid[49][i] = 2;
  }
  Serial.println("Set Borders to 2");
}

int Search::nextLocation(vector<vector<char>>& grid, int x, int y, stack<char>& previousX, stack<char>& previousY) {
  if (grid[x + 1][y] == 0) {
    Serial.println("Stack Go Right");
    grid[x + 1][y] = 1;
    previousX.push(x + 1);
    previousY.push(y);
    return 0;
  } else if (grid[x][y + 1] == 0) {
    Serial.println("Stack Go Up");
    grid[x][y + 1] = 1;
    previousX.push(x);
    previousY.push(y + 1);
    return 1;
  } else if (grid[x - 1][y] == 0) {
    Serial.println("Stack Go Left");
    grid[x - 1][y] = 1;
    previousX.push(x - 1);
    previousY.push(y);
    return 2;
  } else if (grid[x][y - 1] == 0) {
    Serial.println("Stack Go Down");
    grid[x][y - 1] = 1;
    previousX.push(x);
    previousY.push(y - 1);
    return 3;
  } else {
    if (x - previousX.top() == 1) {
      Serial.println("Stack traceback Right");
      previousX.pop();
      previousY.pop();
      return 6;
    } else if (x - previousX.top() == -1) {
      Serial.println("Stack traceback Left");
      previousX.pop();
      previousY.pop();
      return 7;
    } else if (y - previousY.top() == 1) {
      Serial.println("Stack traceback Up");
      previousX.pop();
      previousY.pop();
      return 3;
    } else if (y - previousY.top() == -1) {
      Serial.println("Stack traceback Down");
      previousX.pop();
      previousY.pop();
      return 5;
    } else {
      return -1;
    }
  }
}

void Search::BlockCloseObsticals(vector<vector<char>>& grid, char& x_value, char& y_value, char& dir) {
  if (Ultrasonic.ScanLeft() < 22) {
    if (dir == 0) {
      grid[x_value - 1][y_value] = 2;
    } else if (dir == 1) {
      grid[x_value][y_value - 1] = 2;
    } else if (dir == 2) {
      grid[x_value + 1][y_value] = 2;
    } else if (dir == 3) {
      grid[x_value][y_value - 1] = 2;
    }
  }
  if (Ultrasonic.ScanFoward() < 22) {
    if (dir == 0) {
      grid[x_value][y_value + 1] = 2;
    } else if (dir == 1) {
      grid[x_value + 1][y_value] = 2;
    } else if (dir == 2) {
      grid[x_value][y_value - 1] = 2;
    } else if (dir == 3) {
      grid[x_value - 1][y_value] = 2;
    }
  }
  if (Ultrasonic.ScanRight() < 22) {
    if (dir == 0) {
      grid[x_value + 1][y_value] = 2;
    } else if (dir == 1) {
      grid[x_value][y_value - 1] = 2;
    } else if (dir == 2) {
      grid[x_value - 1][y_value] = 2;
    } else if (dir == 3) {
      grid[x_value][y_value + 1] = 2;
    }
  }
}

void Search::GoToNext(char& x, char& y, char& nextX, char& nextY, char dir) {
  if (nextX - x == 1) {
    Robot.MoveEast(dir);
  } else if (nextX - x == -1) {
    Robot.MoveWest(dir);
  } else if (nextY - y == 1) {
    Robot.MoveNorth(dir);
  } else if (nextY - y == -1) {
    Robot.MoveSouth(dir);
  }
}

bool Search::MoveControls(char& x, char& y, int dir) {
  switch (dir) {
    case 0:
      Robot.MoveNorth(dir);
      Serial.println("Go North");
      y++;
      return 0;
      break;
    case 1:
      Serial.println("Go East");
      Robot.MoveEast(dir);
      x++;
      return 0;
      break;
    case 2:
      Serial.println("Go South");
      Robot.MoveSouth(dir);
      y--;
      return 0;
      break;
    case 3:
      Serial.println("Go West");
      Robot.MoveWest(dir);
      x--;
      return 0;
      break;
    case 4:
      Serial.println("Go Back North");
      Robot.MoveNorth(dir);
      y++;
      return 1;
      break;
    case 5:
      Serial.println("Go Back East");
      Robot.MoveEast(dir);
      x++;
      return 1;
      break;
    case 6:
      Serial.println("Go Back South");
      Robot.MoveSouth(dir);
      y--;
      return 1;
      break;
    case 7:
      Serial.println("Go Back West");
      Robot.MoveWest(dir);
      x--;
      return 1;
      break;
    default:
      break;
  }
  return -1;
}

bool Search::waterSearch(vector<vector<char>>& grid, char& x, char& y, stack<char>& previousX, stack<char>& previousY, char& dir, char& backsteps) {
  BlockCloseObsticals(grid, x, y, dir);
  if (MoveControls(x, y, Search::nextLocation(grid, x, y, previousX, previousY))) {
    backsteps++;
  } else {
    backsteps = 0;  // if after backstepping empty space is found reset backstep case
  }
  if (backsteps >= 5) {  // if backstep 5 times finish searching
    return false;
  }
  if (Ultrasonic.WaterSensor() > 20) {
    return false;
  }
  return true;
}

const int goalx = 24;
const int goaly = 1;
const int matrix_x = 50;
const int matrix_y = 50;
int locx2;
int locy2;
int indexZ;
int costarray[4];
int g = 1;
int minV = 0;
int abs_x = 0;
int abs_y = 0;
int hueristic[matrix_x][matrix_y];

void Search::heur(vector<vector<char>>& grid) {
  if ((goaly < matrix_y && goalx < matrix_x) || (goalx > -1 && goaly > -1)) {
    for (int i = 0; i < matrix_y; i++) {
      for (int j = 0; j < matrix_x; j++)

        if (grid[j][i] == 1) {
          hueristic[j][i] = (abs(j - goalx) + abs(i - goaly));
        } else {
          hueristic[j][i] = (abs(j - goalx) + abs(i - goaly)) + 500;
        }
    }
  }
  for (int i = 0; i < matrix_x; i++) {
    for (int j = 0; j < matrix_y; j++) {
      Serial.print(hueristic[i][j]);
      Serial.print("  ");
    }
    Serial.println();
  }
}

void Search::BFSReturn(vector<vector<char>>& grid, int locx, int locy, char& nextX, char& nextY) {
  for (char i = 0; i < 50; i++) {
    for (char j = 0; j < 50; j++) {
      if (grid[i][j] == 1) {
        grid[i][j] = 0;
      }
      if (grid[i][j] == 2) {
        grid[i][j] = 1;
      }
    }
    while (locx != goalx || locy != goaly) {
      if (locx <= matrix_x && locy <= matrix_y) {
        if (locx - 1 >= 0 && locy >= 0) {
          if (grid[locx - 1][locy] == 1) {
            costarray[0] = g + hueristic[locx - 1][locy];
          } else {
            costarray[0] = 400;
          }
        } else {
          costarray[0] = 400;
        }
        if (locx >= 0 && locy + 1 >= 0) {
          if (grid[locx][locy + 1] == 1) {
            costarray[1] = g + hueristic[locx][locy + 1];
          } else {
            costarray[1] = 400;
          }
        } else {
          costarray[1] = 400;
        }
        if (locx + 1 >= 0 && locy >= 0) {
          if (grid[locx + 1][locy] == 1) {
            costarray[2] = g + hueristic[locx + 1][locy];
          } else {
            costarray[2] = 400;
          }
        } else {
          costarray[2] = 400;
        }
        if (locx >= 0 && locy - 1 >= 0) {
          if (grid[locx][locy - 1] == 1) {
            costarray[3] = g + hueristic[locx][locy - 1];
          } else {
            costarray[3] = 400;
          }
        } else {
          costarray[3] = 400;
        }
        minV = costarray[0];
        indexZ = 0;
        for (int i = 0; i < 4; i++) {
          if (costarray[i] <= minV) {
            indexZ = i;
            minV = costarray[i];
          }
        }
        if (indexZ == 0) {
          locx = locx - 1;

        } else if (indexZ == 1) {
          locy = locy + 1;
        } else if (indexZ == 2) {
          locx = locx + 1;
        } else if (indexZ == 3) {
          locy = locy - 1;
        }

        Serial.print("(");
        Serial.print(locx);
        Serial.print(",");
        Serial.print(locy);
        Serial.print(")");
        Serial.println();

        // Serial.println(grid[2][0]);
        abs_x = locx - locx2;
        abs_y = locy - locy2;
        locy2 = locy;
        locx2 = locx;
        if (abs_x == 1) {
          nextX = locx;
          nextY = locy - 1;
          Serial.println("    go back 1 step");
        } else if (abs_x == -1) {
          nextX = locx;
          nextY = locy + 1;
          Serial.println("    go forward 1 step");
        } else if (abs_y == 1) {
          nextX = locx - 1;
          nextY = locy;
          Serial.println("    go right 1 step");
        } else if (abs_y == -1) {
          nextX = locx + 1;
          nextY = locy;
          Serial.println("    go left 1 step");
        }

        if (locx == goalx && locy == goaly) {
          Serial.println("target found  ");
        }
      }
    }
  }
}
