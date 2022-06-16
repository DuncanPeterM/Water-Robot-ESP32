#include "Search.h"

#include "Distance.h"
#include "MessageToArduino.h"

Distance Ultrasonic;
MessageToArduino Robot;

Search::Search() {
}

void Search::Start() {
  Ultrasonic.start();  // attaches the servo on pin 9 to the servo object
  Serial2.begin(9600, SERIAL_8N1, 16, 0);
}

void Search::setupMatrix(vector<vector<char>>& grid) {
  for (char i = 0; i < 50; i++) {
    for (char j = 0; j < 50; j++) {
      grid[i][j] = 0;
    }
    Serial.println("Set all to 0");
  }
  for (char i = 0; i < 50; i++) {
    grid[i][0] = 1;
    grid[i][49] = 1;
    grid[0][i] = 1;
    grid[49][i] = 1;
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
    if (dir = 0) {
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
    if (dir = 0) {
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
  return true;
}

int row[] = {-1, 0, 0, 1};
int col[] = {0, -1, 1, 0};

bool Search::isValid(vector<Node> const& path, Node pt, int N) {
  return (pt.first >= 0) && (pt.first < N) &&
         (pt.second >= 0) && (pt.second < N) &&
         (find(path.begin(), path.end(), pt) == path.end());
}

int rowNum[] = {-1, 0, 0, 1};
int colNum[] = {0, -1, 1, 0};

bool Search::BFSReturn(vector<vector<char>> const& mat, vector<Node>& path, Node& curr) {
  // base case
  if (mat.size() == 0) {
    return false;
  }

  // include the current cell in the path
  path.push_back(curr);

  // `N × N` matrix
  int N = mat.size();

  // if the destination is found, return true
  if (curr.first == N - 1 && curr.second == N - 1) {
    return true;
  }

  // get the value of the current cell
  int n = mat[curr.first][curr.second];

  // check all four possible movements from the current cell
  // and recur for each valid movement
  for (int i = 0; i < 4; i++) {
    // get the next position using the value of the current cell
    int x = curr.first + row[i] * n;
    int y = curr.second + col[i] * n;

    Node next = make_pair(x, y);

    // check if it is possible to go to a position (x, y)
    // from the current position
    if (isValid(path, next, N) && BFSReturn(mat, path, next)) {
      return true;
    }
  }

  // backtrack: exclude the current cell from the path
  path.pop_back();

  return false;
}