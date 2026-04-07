#include <M5Unified.h>

enum Status {
  STATUS_IDLE = 0,
  STATUS_LISTENING = 1,
  STATUS_PROCESSING = 2
};

Status currentStatus = STATUS_IDLE;

const char* statusTexts[] = {
  "Waiting",
  "Listening",
  "Running"
};

const uint16_t statusColors[] = {
  0x07E0, 0x001F, 0xF800
};

unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 500;
bool dotVisible = true;

// Claude Code Logo 24x24 - 完全对称版本
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

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(0);
  M5.Display.fillScreen(0x0000);
  M5.Display.setTextColor(0xFFFF);
  drawUI();
  Serial.begin(115200);
}

void loop() {
  M5.update();
  if (M5.BtnA.wasClicked()) {
    currentStatus = (Status)((currentStatus + 1) % 3);
    drawUI();
  }
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    switch(cmd) {
      case '0': currentStatus = STATUS_IDLE; break;
      case '1': currentStatus = STATUS_LISTENING; break;
      case '2': currentStatus = STATUS_PROCESSING; break;
    }
    drawUI();
  }
  if (currentStatus == STATUS_LISTENING && dotVisible) {
    if (millis() - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = millis();
      dotVisible = !dotVisible;
      drawStatusText();
    }
  }
  delay(10);
}

void drawUI() {
  M5.Display.fillScreen(0x0000);
  drawLogo(15, 20, 110);
  drawStatusText();
  M5.Display.setTextSize(0);
  M5.Display.setTextColor(0x5555);
  M5.Display.setCursor(15, 305);
  M5.Display.println("Press [A] to change");
}

void drawLogo(int x, int y, int size) {
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
  M5.Display.fillRect(0, 160, 170, 50, 0x0000);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(statusColors[currentStatus]);
  const char* text = statusTexts[currentStatus];
  int textWidth = M5.Display.textWidth(text);
  int x = (170 - textWidth) / 2 - 25;
  if (x < 0) x = 0;
  M5.Display.setCursor(x, 170);
  M5.Display.print(text);
  if ((currentStatus == STATUS_LISTENING) && dotVisible) {
    M5.Display.print(".");
  }
}
