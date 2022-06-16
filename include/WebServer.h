#pragma once
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

class WebServer {
 private:
  static esp_err_t index_handler(httpd_req_t *req);
  static esp_err_t stream_handler(httpd_req_t *req);
  static esp_err_t cmd_handler(httpd_req_t *req);
  void startCameraServer();

 public:
  WebServer();

  void Websetup();
};