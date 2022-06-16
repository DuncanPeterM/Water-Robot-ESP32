#pragma once
#include <Arduino.h>

#include <Stack>

#include "Search.h"
using namespace std;

class Search {
 private:
 public:
  Search();

  void Start();

  void setupMatrix(vector<vector<char> >& grid);

  int nextLocation(vector<vector<char> >& grid, int x, int y, stack<char>& previousX, stack<char>& previousY);

  void BlockCloseObsticals(vector<vector<char> >& grid, char& x_value, char& y_value, char& dir);

  bool MoveControls(char& x, char& y, int dir);

  bool waterSearch(vector<vector<char> >& grid, char& x, char& y, stack<char>& previousX, stack<char>& previousY, char& dir, char& backsteps);
};