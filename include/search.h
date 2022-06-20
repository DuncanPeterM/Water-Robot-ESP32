#pragma once
#include <Arduino.h>

#include <Stack>
#include <algorithm>
#include <vector>
using namespace std;

#include "Search.h"

typedef pair<int, int> Node;
class Search {
 private:
 public:
  Search();

  void Start();

  void setupMatrix(vector<vector<char>>& grid);

  int nextLocation(vector<vector<char>>& grid, int x, int y, stack<char>& previousX, stack<char>& previousY);

  void BlockCloseObsticals(vector<vector<char>>& grid, char& x_value, char& y_value, char& dir);

  void GoToNext(char& x, char& y, char& nextX, char& nextY, char dir);

  void ManualControl(int button);

  bool MoveControls(char& x, char& y, int dir);

  bool waterSearch(vector<vector<char>>& grid, char& x, char& y, stack<char>& previousX, stack<char>& previousY, char& dir, char& backsteps);

  bool isValid(bool vis[][50], int row, int col);

  void heur(vector<vector<char>>& grid);

  void BFSReturn(vector<vector<char>>& grid, int locx, int locy, char& nextX, char& nextY);
};