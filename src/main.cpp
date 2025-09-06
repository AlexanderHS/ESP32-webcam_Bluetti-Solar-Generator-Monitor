#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Aperture";
const char* password = "let me in!";

WebServer server(80);

// Camera pin definitions for ESP32-S3 (Freenove standard pinout)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5
#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

// Function declarations
void handleRoot();
void handleCapture();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32-S3 Camera Server...");
  
  // Check PSRAM availability
  if(psramFound()) {
    Serial.println("PSRAM found and enabled");
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
  } else {
    Serial.println("PSRAM not found - using smaller frame size");
  }
  
  // Camera config
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;  // Fixed deprecated naming
  config.pin_sccb_scl = SIOC_GPIO_NUM;  // Fixed deprecated naming
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Frame size and quality
  if(psramFound()) {
    config.frame_size = FRAMESIZE_SVGA; // 800x600 (higher than VGA)
    config.jpeg_quality = 10; // 0-63, lower = better quality
    config.fb_count = 2;
    Serial.println("Using SVGA resolution (800x600)");
  } else {
    config.frame_size = FRAMESIZE_QVGA; // 320x240
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.println("Using QVGA resolution (320x240)");
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
  Serial.println("Camera initialized successfully!");

  // Get camera sensor for advanced settings
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    // Adjust settings for reading bright LCD screens
    s->set_brightness(s, -1);     // Reduce brightness (-2 to 2)
    s->set_contrast(s, 1);        // Increase contrast (-2 to 2)
    s->set_saturation(s, 0);      // Normal saturation (-2 to 2)
    s->set_exposure_ctrl(s, 1);   // Enable auto exposure control
    s->set_aec2(s, 0);            // Disable AEC2 (can cause overexposure)
    s->set_ae_level(s, -1);       // Reduce auto exposure level (-2 to 2)
    s->set_aec_value(s, 200);     // Manual exposure value (lower = darker)
    s->set_gain_ctrl(s, 1);       // Enable auto gain control
    s->set_agc_gain(s, 0);        // Set gain level (0-30, lower = less sensitive)
    s->set_gainceiling(s, (gainceiling_t)2); // Limit max gain
    
    Serial.println("Camera settings optimized for LCD reading");
  } else {
    Serial.println("Warning: Could not get camera sensor for advanced settings");
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Camera server ready! Visit: http://");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/capture", handleCapture);
  server.on("/", handleRoot);
  
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", 
    "<h1>ESP32-S3 Camera Server</h1>"
    "<p><a href='/capture'>Capture Image</a></p>"
    "<p>Or visit /capture directly for raw JPEG data</p>");
}

void handleCapture() {
  Serial.println("Capturing image...");
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  
  Serial.printf("Image captured: %d bytes\n", fb->len);
  
  server.sendHeader("Content-Type", "image/jpeg");
  server.sendHeader("Content-Length", String(fb->len));
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  
  esp_camera_fb_return(fb);
  Serial.println("Image sent successfully");
}
