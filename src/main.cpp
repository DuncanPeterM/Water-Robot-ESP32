
#include <Arduino.h>

#include "Distance.h"
#include "MessageToArduino.h"
//#include "WebServer.h"

#include <WiFi.h>

#include "Arduino.h"
#include "WebServer.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "soc/rtc_cntl_reg.h"  // disable brownout problems
#include "soc/soc.h"           // disable brownout problems

Distance Ultrasonic;
MessageToArduino Robot;
// WebServer Web;

const int squareSize = 22;
// const int SidesquareSize = 22;

byte PositionArray[50][50] = {0};

int left, right, forward;
int leftMatrixCheck, rightMatrixCheck, forwardMatrixCheck = 0;
int NextMatrix = 0;
int x_value = 1, y_value = 24;

#include <math.h>
const int matrix_x = 50;
const int matrix_y = 50;

int endloop = 0;
bool WebStarted = 0;

// Declaring the grid and its variations ( grid and Hueristic and dynamic path )

int hueristic[matrix_x][matrix_y];

// Declaring start goal and various other variables
int startx = x_value;
int starty = y_value;
int goalx = 1;
int goaly = 24;
int locx = startx;
int locy = starty;
int locx2;
int locy2;
int indexZ;
int costarray[4];
int g = 1;
int minV = 0;
int abs_x = 0;
int abs_y = 0;
// water check const
int WaterConst = 0;
//

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

void findpath() {
  turnDirection('B');
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
        turnDirection('R');
        Robot.Forward();
        turnDirection('L');
        Serial.println("    go right 1 step");
      } else if (abs_y == -1) {
        Serial.println("    go left 1 step");
        turnDirection('L');
        Robot.Forward();
        turnDirection('R');
      }

      if (locx == goalx && locy == goaly) {
        Serial.println("target found  ");
        // for(;;);
      }
    }
  }
  endloop = 1;
}

void heur() {
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

const char *ssid = "wlan-ap-2.4g";
const char *password = "alan&fionamackay2010";

#define PART_BOUNDARY "123456789000000000000987654321"

#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#else
#error "Camera model not selected"
#endif

static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>
  <head>
    <title>ESP32-CAM Robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      table { margin-left: auto; margin-right: auto; }
      td { padding: 8 px; }
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      img {  width: auto ;
        max-width: 100% ;
        height: auto ; 
      }
    </style>
  </head>
  <body>
    <h1>ESP32-CAM Robot</h1>
    <img src="" id="photo" >
    <table>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('forward');" ontouchstart="toggleCheckbox('forward');" >Forward</button></td></tr>
      <tr><td align="center"><button class="button" onmousedown="toggleCheckbox('left');" ontouchstart="toggleCheckbox('left');"  >Left</button></td>
      <td align="center"><button class="button" onmousedown="toggleCheckbox('stop');" ontouchstart="toggleCheckbox('stop');">Next State</button></td>
      <td align="center"><button class="button" onmousedown="toggleCheckbox('right');" ontouchstart="toggleCheckbox('right');"  >Right</button></td></tr>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('backward');" ontouchstart="toggleCheckbox('backward');" >Backward</button></td></tr>                 
    </table>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
     xhr.send();
   }
   window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
)rawliteral";

esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if (fb->width > 400) {
        if (fb->format != PIXFORMAT_JPEG) {
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if (!jpeg_converted) {
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK) {
      break;
    }
    // Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}

esp_err_t cmd_handler(httpd_req_t *req) {
  char *buf;
  size_t buf_len;
  char variable[32] = {
      0,
  };

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char *)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  sensor_t *s = esp_camera_sensor_get();
  int res = 0;

  if (!strcmp(variable, "forward")) {
    Serial.println("Forward");
    Robot.Forward();
  } else if (!strcmp(variable, "left")) {
    Serial.println("Left");
    Robot.TurnLeft();
  } else if (!strcmp(variable, "right")) {
    Serial.println("Right");
    Robot.TurnRight();
  } else if (!strcmp(variable, "backward")) {
    Serial.println("Backward");
    Robot.Backwards();
  } else if (!strcmp(variable, "stop")) {
    heur();
    findpath();
  } else {
    res = -1;
  }

  if (res) {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_uri = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = index_handler,
      .user_ctx = NULL};

  httpd_uri_t cmd_uri = {
      .uri = "/action",
      .method = HTTP_GET,
      .handler = cmd_handler,
      .user_ctx = NULL};
  httpd_uri_t stream_uri = {
      .uri = "/stream",
      .method = HTTP_GET,
      .handler = stream_handler,
      .user_ctx = NULL};
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

bool Websetup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  /*esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }*/
  // Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.println(WiFi.localIP());

  // Start streaming web server
  startCameraServer();
}

int servocontrol() {
  getDistance();

  Serial.println(F(" servo "));

  // compare ultrasonic sensor outputs

  if ((x_value + 1 <= 50 || y_value + 1 <= 50) && (WaterConst == 0)) {  // matrix limits, distance limits .... you can add other limits like finding the source here

    if ((right > squareSize) && (rightMatrixCheck == 1) && (WaterConst == 0)) {  // priority is always right
      Serial.println(F("right servo "));

      turnDirection('R');

      getDistance();

      while ((forward > squareSize) && (rightMatrixCheck == 1) && (WaterConst == 0)) {
        moveDir('R');
        // rightNextCell();
        WaterConst = Ultrasonic.WaterSensor();
        getDistance();

        if (right > squareSize && NextMatrix == 1 && (WaterConst == 0)) {
          turnDirection('R');
          moveDir('N');
          WaterConst = Ultrasonic.WaterSensor();
          moveDir('B');  // remove when code start and add robot.forward
          turnDirection('L');
          Serial.println(F("right next"));
        }

        getDistance();
      }
      if (x_value < 50 && y_value < 50) {
        int tempx1 = x_value;
        int tempy1 = y_value + 1;
        PositionArray[tempx1][tempy1] = {0};
      }
      turnDirection('L');  // go back facing the forward position
    }
    getDistance();

    if ((left > squareSize) && (leftMatrixCheck == 1) && (WaterConst == 0)) {
      Serial.println(F("left servo "));
      turnDirection('L');
      getDistance();

      while ((forward > squareSize) && (leftMatrixCheck == 1) && (WaterConst == 0)) {
        moveDir('L');
        // leftNextCell();
        WaterConst = Ultrasonic.WaterSensor();
        getDistance();
        if (left > squareSize && NextMatrix == 1 && (WaterConst == 0)) {
          turnDirection('L');
          moveDir('N');
          WaterConst = Ultrasonic.WaterSensor();
          moveDir('B');  // not move just needs to going back
          turnDirection('R');
          Serial.println(F("Next left"));
        }
        getDistance();
      }
      if (x_value < 50 && y_value < 50) {
        int tempx1 = x_value;
        int tempy1 = y_value - 1;
        PositionArray[tempx1][tempy1] = {0};
      }
      turnDirection('R');  // go back facing the forward position
    }

    getDistance();

    if ((forward > squareSize) && (forwardMatrixCheck == 1) && (WaterConst == 0)) {
      Serial.println(F("forward servo "));
      getDistance();
      moveDir('F');
      WaterConst = Ultrasonic.WaterSensor();
    }

    if ((((forward < squareSize) || (leftMatrixCheck == 4)) && ((forward < squareSize) || (rightMatrixCheck == 4)) && ((left < squareSize) || (leftMatrixCheck == 4))) || (WaterConst == 1)) {  // stop
      Serial.println(F("mapped everything or stuck"));

      AddBlockToMatrix();
      // AddBlockToMatrix();
      startx = x_value;
      starty = y_value;
      if (!WebStarted) {
        WebStarted = true;
        endloop = 1;
        Websetup();
      }
    }

  } else {
    Serial.println(F("reached max limits"));
    AddBlockToMatrix();
    startx = x_value;
    starty = y_value;
    if (!WebStarted) {
      WebStarted = true;
      endloop = 1;
      Websetup();
    }
  }
  return 0;
}

void creatematrix() {
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

void matrixprint() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      Serial.print(PositionArray[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void setup() {
  // put your setup code here, to run once:
  Ultrasonic.start();  // attaches the servo on pin 9 to the servo object
  // Robot.start();
  // Web.setup();
  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
  Serial2.begin(9600, SERIAL_8N1, 16, 0);
  creatematrix();
}

void loop() {
  // Robot.Forward();
  if (endloop == 0) {
    servocontrol();
  }
}