/*
 * M5 StickS3 WiFi Audio Stream - Simplified from Xiaozhi Voice Assistant Project
 *
 * Features:
 * 1. Audio capture via M5.Mic (16kHz, 16-bit, mono)
 * 2. Send to PC via WiFi WebSocket
 * 3. Auto-reconnect on disconnection
 *
 * Dependencies:
 * - M5Unified (Install: Arduino Library Manager, search "M5Unified")
 * - WebSockets (Install: Arduino Library Manager, search "WebSockets" by Markus Sattler)
 *
 * Usage:
 * 1. Modify WiFi and WebSocket configuration in config.h
 * 2. Compile and upload to StickS3
 */

#include <M5Unified.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include "config.h"

// ==================== Global Variables ====================

static WebSocketsClient webSocket;
static int16_t audioBuffer[AUDIO_BUFFER_SIZE];
static bool wifiConnected = false;
static bool websocketConnected = false;
static bool isStreaming = false;

// Status display
enum Status {
  STATUS_IDLE = 0,
  STATUS_CONNECTING_WIFI,
  STATUS_CONNECTING_WS,
  STATUS_LISTENING,
  STATUS_STREAMING
};

Status currentStatus = STATUS_IDLE;

const char* statusTexts[] = {
  "Idle",
  "WiFi...",
  "WS...",
  "Listening",
  "Streaming"
};

const uint16_t statusColors[] = {
  0x07E0,  // Green - Idle
  0xFFFF,  // White - Connecting WiFi
  0xFFFF,  // White - Connecting WS
  0x001F,  // Blue - Listening
  0xF800   // Red - Streaming
};

// ==================== Function Declarations ====================

void initWiFi();
void initWebSocket();
void drawUI();
void drawStatusText();
void drawBattery();
void audioStreamTask(void* pvParameters);

// ==================== WebSocket Callback ====================

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      websocketConnected = false;
      M5.Display.setTextColor(0xFFFF);
      M5.Display.setCursor(0, 200);
      M5.Display.println("WS disconnected");
      break;

    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected");
      websocketConnected = true;
      break;

    case WStype_TEXT:
      Serial.printf("[WebSocket] Received text: %s\n", payload);
      break;

    case WStype_BIN:
      Serial.printf("[WebSocket] Received binary: %u bytes\n", length);
      break;

    case WStype_PING:
      Serial.println("[WebSocket] Ping received");
      break;

    case WStype_PONG:
      Serial.println("[WebSocket] Pong received");
      break;
  }
}

// ==================== Initialization Functions ====================

void initWiFi() {
  currentStatus = STATUS_CONNECTING_WIFI;
  drawStatusText();

  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setAutoReconnect(true);

  int timeout = 30;  // 30 seconds timeout
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    wifiConnected = true;
  } else {
    Serial.println("\nWiFi connection failed!");
    currentStatus = STATUS_IDLE;
    drawStatusText();
    return;
  }
}

void initWebSocket() {
  currentStatus = STATUS_CONNECTING_WS;
  drawStatusText();

  Serial.print("Connecting to WebSocket: ");
  Serial.print(WEBSOCKET_HOST);
  Serial.print(":");
  Serial.print(WEBSOCKET_PORT);
  Serial.println(WEBSOCKET_PATH);

  webSocket.begin(WEBSOCKET_HOST, WEBSOCKET_PORT, WEBSOCKET_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);  // 5 seconds reconnect interval

  // Wait for WebSocket connection
  int timeout = 20;  // 20 seconds timeout
  while (!websocketConnected && timeout > 0) {
    webSocket.loop();
    delay(500);
    timeout--;
  }

  if (websocketConnected) {
    Serial.println("WebSocket connected!");
  } else {
    Serial.println("WebSocket connection failed!");
    currentStatus = STATUS_IDLE;
    drawStatusText();
  }
}

// ==================== UI Drawing Functions ====================

void drawUI() {
  M5.Display.fillScreen(0x0000);
  // Logo centered
  drawLogo(10, 20, 110);
  drawStatusText();
  drawBattery();
  M5.Display.setTextSize(0);
  M5.Display.setTextColor(0x5555);
  M5.Display.setCursor(5, 225);
  M5.Display.println("Press [A] to stream");
}

void drawLogo(int x, int y, int size) {
  // Simplified logo drawing (can be replaced with your own logo)
  M5.Display.fillRect(x, y, size, size, 0x07E0);
  M5.Display.setTextColor(0xFFFF);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(x + 25, y + 45);
  M5.Display.println("AI");
}

void drawStatusText() {
  M5.Display.fillRect(0, 160, 130, 50, 0x0000);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(statusColors[currentStatus]);
  const char* text = statusTexts[currentStatus];
  int textWidth = M5.Display.textWidth(text);
  int x = (130 - textWidth) / 2;
  if (x < 0) x = 0;
  M5.Display.setCursor(x, 170);
  M5.Display.print(text);

  // Display WiFi signal strength
  if (wifiConnected) {
    int rssi = WiFi.RSSI();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(0xFFFF);
    M5.Display.setCursor(5, 210);
    M5.Display.printf("WiFi: %d dBm", rssi);
  }
}

void drawBattery() {
  int batteryLevel = M5.Power.getBatteryLevel();
  uint16_t color;

  // Detect USB voltage to determine charging status
  int vbus = M5.Power.getVBUSVoltage();
  if (vbus > 0) {
    color = 0x07E0;  // Charging: green
  } else {
    color = 0xFFFF;  // Not charging: white
  }

  // Battery icon - top right
  int x = 130 - 55;
  int y = 5;
  int w = 20;
  int h = 8;

  M5.Display.fillRect(x + w, y + 2, 2, h - 4, color);
  M5.Display.fillRect(x, y, w, h, color);
  M5.Display.fillRect(x + 1, y + 1, w - 2, h - 2, 0x0000);
  int fillWidth = (batteryLevel * (w - 4)) / 100;
  M5.Display.fillRect(x + 2, y + 2, fillWidth, h - 4, color);

  M5.Display.setTextSize(1);
  M5.Display.setTextColor(color);
  M5.Display.setCursor(x + w + 4, y + 1);
  M5.Display.print(batteryLevel);
  M5.Display.print("%");
}

// ==================== Audio Stream Task ====================

void audioStreamTask(void* pvParameters) {
  int frameSize = SAMPLE_RATE / 1000 * OPUS_FRAME_DURATION_MS;  // 960 samples @ 60ms
  int bufferSize = frameSize * sizeof(int16_t);

  Serial.printf("[Audio] Stream task started, frame size: %d samples, buffer size: %d bytes\n", frameSize, bufferSize);

  while (1) {
    // Check if in streaming state
    if (!isStreaming) {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }

    // Check WebSocket connection
    if (!websocketConnected) {
      Serial.println("[Audio] WebSocket not connected, stopping stream");
      isStreaming = false;
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    // Read audio data - Note: record() second parameter is sample count, not bytes!
    if (M5.Mic.record(audioBuffer, frameSize)) {
      // Wait for recording to complete
      while (M5.Mic.isRecording()) {
        vTaskDelay(pdMS_TO_TICKS(1));
      }

      // Send binary data via WebSocket
      webSocket.sendBIN((uint8_t *)audioBuffer, bufferSize);

      // Process WebSocket events to maintain connection
      webSocket.loop();

      // Immediately start next recording (continuous capture)
      M5.Mic.record(audioBuffer, frameSize);
    } else {
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }

  vTaskDelete(NULL);
}

// ==================== Arduino Main Functions ====================

void setup() {
  // Initialize M5
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(0);
  M5.Display.fillScreen(0x0000);
  M5.Display.setTextColor(0xFFFF);
  M5.Power.begin();

  // Initialize serial
  Serial.begin(115200);
  Serial.println("\n=== M5 StickS3 WiFi Audio Stream ===");

  // Initialize microphone - must turn off speaker first
  M5.Speaker.end();
  M5.Mic.begin();
  Serial.println("Mic initialized");

  // Initialize UI
  drawUI();

  // Initialize WiFi
  initWiFi();
  drawStatusText();

  // Initialize WebSocket
  if (wifiConnected) {
    initWebSocket();
    drawStatusText();
  }

  // Create audio stream task
  if (websocketConnected) {
    xTaskCreatePinnedToCore(
      audioStreamTask,    // Task function
      "audio_stream",     // Task name
      4096,               // Stack size
      NULL,               // Parameters
      5,                  // Priority
      NULL,               // Task handle
      0                   // Run on core 0
    );
    Serial.println("Audio stream task created");
  }

  Serial.println("Setup complete");
}

void loop() {
  // Process WebSocket events
  webSocket.loop();

  // Process button input
  M5.update();

  if (M5.BtnA.isPressed()) {
    if (!isStreaming) {
      isStreaming = true;
      currentStatus = STATUS_STREAMING;
      drawStatusText();
      Serial.println("[Button A] Started streaming");
    }
    // Continue streaming while held (handled in audioStreamTask)
    delay(50);
  } else {
    if (isStreaming) {
      isStreaming = false;
      currentStatus = STATUS_LISTENING;
      drawStatusText();
      Serial.println("[Button A] Stopped streaming");
    }
  }

  // Check WiFi reconnection status
  if (wifiConnected && WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi reconnected");
    wifiConnected = false;
    websocketConnected = false;
    currentStatus = STATUS_CONNECTING_WIFI;
    drawStatusText();
  }

  // Small delay to avoid consuming too much CPU
  delay(10);
}
