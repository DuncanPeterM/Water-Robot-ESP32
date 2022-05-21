#include "SearchAlgo.h"

SearchAlgo Search;

int searchEnd = 0;

void setup() {
  Search.SearchAlgoSetup();
}

void loop() {
  while (searchEnd == 0) {
    searchEnd = Search.servocontrol();
  }
}