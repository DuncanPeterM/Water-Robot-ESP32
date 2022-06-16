<<<<<<< HEAD
#include <Arduino.h>
#include <math.h>

#include <stack>

#include "Search.h"
Search Find;

vector<vector<char>> grid(50, vector<char>(50));
char dir = 0;
char x_value = 1, y_value = 24;
char backsteps = 0;
stack<char> previousX;
stack<char> previousY;
int state = 0;

void setup() {
  Find.Start();
  Find.setupMatrix(grid);
  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
}

void loop() {
  state = 1;
  switch (state) {
    case 1:
      if (Find.waterSearch(grid, x_value, y_value, previousX, previousY, dir, backsteps)) {
        Serial.println(F("water search "));
        Serial.print(x_value);
        Serial.print(" ");
        Serial.print(y_value);
      } else {
        Serial.println(F("Finished Search "));
        Serial.print(x_value);
        Serial.print(" ");
        Serial.print(y_value);
        state = 2;
      }
      break;
    case 2:
      // water found finished manual control
      break;
    case 3:
      // Search finished automatic control home
      break;
    case 4:
      // Search finished automatic control water
      break;
    default:
      break;
=======
#include "SearchAlgo.h"

SearchAlgo Search;

int searchEnd = 0;

void setup() {
  Search.SearchAlgoSetup();
}

void loop() {
  while (searchEnd == 0) {
    searchEnd = Search.servocontrol();
>>>>>>> d39d0817381ad3d378ce533194079354c867da72
  }
}