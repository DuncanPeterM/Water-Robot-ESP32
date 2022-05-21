#pragma once
#include "SearchAlgo.h"

class SearchAlgo {
 private:
  int checkNextPosition(int, int);
  void getDistance();
  void moveDir(char);
  void rightNextCell();
  void leftNextCell();
  void AddBlockToMatrix();
  void findpath();
  void heur();

 public:
  SearchAlgo();
  void creatematrix();
  void matrixprint();
  void SearchAlgoSetup();
  int servocontrol();
};