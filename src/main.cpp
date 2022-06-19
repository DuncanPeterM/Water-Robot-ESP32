#include <Arduino.h>
#include <math.h>

#include <stack>

#include "Search.h"
#include "WebServer.h"
Search Find;
WebServer Web;

vector<vector<char>> grid(50, vector<char>(50));
char dir = 0;
char x_value = 24, y_value = 1;
char backsteps = 0;
stack<char> previousX;
stack<char> previousY;
int state = 2;
vector<Node> path;

void setup() {
  Serial.begin(9600);  // Serial Communication is starting with 9600 of baudrate speed
  Find.Start();
  Serial.println("Find Setup");
  Find.setupMatrix(grid);
  Serial.println("Find Setup Matrix");
  // Web.Websetup();
  Serial.println("Web Setup");

  Serial.println("Serial Setup");
}

void loop() {
  switch (state) {
    case 1:
      Serial.println("Entering State 1");
      if (Find.waterSearch(grid, x_value, y_value, previousX, previousY, dir, backsteps)) {
        Serial.println(F("water search "));
        Serial.print(x_value);
        Serial.print(" ");
        Serial.print(y_value);
        state = 2;  // remote later
      } else {
        Serial.println(F("Finished Search (All areas searched or water found)"));
        Serial.print(x_value);
        Serial.print(" ");
        Serial.print(y_value);
        state = 2;
      }
      break;
    case 2:
      Serial.println("Entering State 2");
      Web.Websetup();
      // water found finished manual control
      state = 3;
      break;
    case 3:
      Serial.println("Entering State 3");
      // wait state till told to go from web interface
      break;
    case 4:
      Serial.println("Entering State 4");
      if (x_value == 1 && y_value == 24) {
        state = 5;
      } else {
        Find.BlockCloseObsticals(grid, x_value, y_value, dir);
        //  command to move to new location to go here
      }
      break;
    default:
      break;
  }
}