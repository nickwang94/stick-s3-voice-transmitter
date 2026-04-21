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

// ==================== Screen Constants ====================
#define SCREEN_WIDTH 130
#define SCREEN_HEIGHT 240
#define LOGO_SIZE 110

// ==================== Global Variables ====================

static WebSocketsClient webSocket;
static int16_t audioBuffer[AUDIO_BUFFER_SIZE];
static bool wifiConnected = false;
static bool websocketConnected = false;
static bool isStreaming = false;

// Claude Code Logo 24x24 - Octopus design
const uint16_t claudeLogo[24][24] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A},
  {0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A},
  {0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0xDA6A, 0, 0, 0},
  {0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0},
  {0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0},
  {0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0, 0, 0, 0xDA6A, 0xDA6A, 0, 0xDA6A, 0xDA6A, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Status display
enum Status {
  STATUS_IDLE = 0,
  STATUS_CONNECTING,
  STATUS_LISTENING,
  STATUS_STREAMING
};

Status currentStatus = STATUS_IDLE;

const char* statusTexts[] = {
  "Idle",
  "Connecting...",
  "Listening",
  "Streaming"
};

const uint16_t statusColors[] = {
  0x07E0,  // Green - Idle
  0xFFFF,  // White - Connecting
  0x001F,  // Blue - Listening
  0xF800   // Red - Streaming
};

// ==================== Function Declarations ====================

void initWiFi();
void initWebSocket();
void drawUI();
void drawStatusText();
void drawWiFiStatus();
void drawBattery();
void audioStreamTask(void* pvParameters);

// ==================== WebSocket Callback ====================

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      websocketConnected = false;
      drawWebSocketStatus();
      break;

    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected");
      websocketConnected = true;
      drawWebSocketStatus();
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
  currentStatus = STATUS_CONNECTING;
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
    currentStatus = STATUS_LISTENING;
    drawStatusText();
    drawWiFiStatus();
  } else {
    Serial.println("\nWiFi connection failed!");
    currentStatus = STATUS_IDLE;
    drawStatusText();
    drawWiFiStatus();
    return;
  }
}

void initWebSocket() {
  currentStatus = STATUS_CONNECTING;
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
    currentStatus = STATUS_LISTENING;
    drawStatusText();
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
  drawLogo((SCREEN_WIDTH - LOGO_SIZE) / 2, 50, LOGO_SIZE);
  drawStatusText();
  drawWiFiStatus();
  drawWebSocketStatus();
  drawBattery();
}

void drawLogo(int x, int y, int size) {
  // Claude Code Octopus Logo 24x24
  for (int py = 0; py < 24; py++) {
    for (int px = 0; px < 24; px++) {
      uint16_t color = claudeLogo[py][px];
      if (color != 0) {
        int x0 = x + (px * size) / 24;
        int y0 = y + (py * size) / 24;
        int x1 = x + ((px + 1) * size) / 24;
        int y1 = y + ((py + 1) * size) / 24;
        int w = x1 - x0;
        int h = y1 - y0;
        if (w > 0 && h > 0) {
          M5.Display.fillRect(x0, y0, w, h, color);
        }
      }
    }
  }
}

void drawStatusText() {
  M5.Display.fillRect(0, 160, SCREEN_WIDTH, 50, 0x0000);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(statusColors[currentStatus]);
  const char* text = statusTexts[currentStatus];
  int textWidth = M5.Display.textWidth(text);
  int x = (SCREEN_WIDTH - textWidth) / 2;
  if (x < 0) x = 0;
  M5.Display.setCursor(x, 170);
  M5.Display.print(text);
}

void drawWiFiStatus() {
  // WiFi status indicator - left side, first row
  // Green dot = connected, Red dot = not connected
  uint16_t color = wifiConnected ? 0x07E0 : 0xF800;
  M5.Display.fillCircle(12, 12, 6, color);

  // WiFi label
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(color);
  M5.Display.setCursor(24, 10);
  M5.Display.print("WiFi");
}

void drawWebSocketStatus() {
  // WebSocket status indicator - left side, second row (aligned with WiFi)
  // Green = connected, Red = disconnected, Yellow = connecting
  uint16_t color;
  if (websocketConnected) {
    color = 0x07E0;  // Green - connected
  } else if (currentStatus == STATUS_CONNECTING) {
    color = 0xFFE0;  // Yellow - connecting
  } else {
    color = 0xF800;  // Red - disconnected
  }

  M5.Display.fillCircle(12, 32, 6, color);

  // WS label
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(color);
  M5.Display.setCursor(24, 30);
  M5.Display.print("WS");
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

  // Battery icon - top right (moved left for 100% display)
  int x = SCREEN_WIDTH - 50;
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

  // Button A - Push to Talk
  if (M5.BtnA.isPressed()) {
    if (!isStreaming) {
      // Start recording
      isStreaming = true;
      currentStatus = STATUS_STREAMING;
      drawStatusText();

      // Send START_RECORDING command to server
      webSocket.sendTXT("START_RECORDING");

      Serial.println("[Button A] Started streaming");
    }
    // Continue streaming while held (handled in audioStreamTask)
    delay(50);
  } else {
    if (isStreaming) {
      // Stop recording
      isStreaming = false;
      currentStatus = STATUS_LISTENING;
      drawStatusText();

      // Send STOP_RECORDING command to server
      webSocket.sendTXT("STOP_RECORDING");

      Serial.println("[Button A] Stopped streaming");
    }
  }

  // Button B - Reconnect WebSocket
  if (M5.BtnB.isPressed()) {
    if (!websocketConnected || !wifiConnected) {
      Serial.println("[Button B] Reconnecting...");

      // Reconnect WiFi if needed
      if (!wifiConnected || WiFi.status() != WL_CONNECTED) {
        Serial.println("[Button B] Reconnecting WiFi...");
        initWiFi();
      }

      // Reconnect WebSocket if WiFi is connected
      if (wifiConnected) {
        Serial.println("[Button B] Reconnecting WebSocket...");
        webSocket.disconnect();
        delay(100);
        initWebSocket();
      }
      drawWebSocketStatus();
    }
    delay(200);  // Debounce
  }

  // Check WiFi reconnection status
  if (wifiConnected && WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected");
    wifiConnected = false;
    websocketConnected = false;
    currentStatus = STATUS_CONNECTING;
    drawStatusText();
    drawWiFiStatus();
  }

  // Small delay to avoid consuming too much CPU
  delay(10);
}
