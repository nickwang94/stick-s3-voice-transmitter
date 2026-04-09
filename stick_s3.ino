#include <M5Unified.h>

// Stick S3 可视屏幕区域（被外壳遮挡后的实际可视范围）
#define SCREEN_WIDTH 130
#define SCREEN_HEIGHT 240

enum Status {
  STATUS_IDLE = 0,
  STATUS_LISTENING = 1
};

Status currentStatus = STATUS_IDLE;

const char* statusTexts[] = {
  "Idle",
  "Listening"
};

const uint16_t statusColors[] = {
  0x07E0, 0x001F
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
  M5.Power.begin();

  drawUI();
  Serial.begin(115200);
}

void loop() {
  M5.update();

  // 按住 A 键显示聆听，松开显示待机
  if (M5.BtnA.isPressed()) {
    if (currentStatus != STATUS_LISTENING) {
      currentStatus = STATUS_LISTENING;
      drawStatusText();
    }
  } else {
    if (currentStatus != STATUS_IDLE) {
      currentStatus = STATUS_IDLE;
      drawStatusText();
    }
  }

  // 闪烁效果
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
  // Logo 居中：(130 - 110) / 2 = 10
  drawLogo(10, 20, 110);
  drawStatusText();
  drawBattery();
  M5.Display.setTextSize(0);
  M5.Display.setTextColor(0x5555);
  M5.Display.setCursor(5, 225);
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
  M5.Display.fillRect(0, 160, SCREEN_WIDTH, 50, 0x0000);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(statusColors[currentStatus]);
  const char* text = statusTexts[currentStatus];
  int textWidth = M5.Display.textWidth(text);
  int x = (SCREEN_WIDTH - textWidth) / 2;
  if (x < 0) x = 0;
  M5.Display.setCursor(x, 170);
  M5.Display.print(text);
  if ((currentStatus == STATUS_LISTENING) && dotVisible) {
    M5.Display.print(".");
  }
}

void drawBattery() {
  int batteryLevel = M5.Power.getBatteryLevel();
  uint16_t color;

  // 检测 USB 电压判断是否充电
  int vbus = M5.Power.getVBUSVoltage();
  if (vbus > 0) {
    color = 0x07E0;  // 充电：绿色
  } else {
    color = 0xFFFF;  // 未充电：白色
  }

  // 电池图标 - 右上角，横向排列
  int x = SCREEN_WIDTH - 55;  // 靠右
  int y = 5;
  int w = 20;
  int h = 8;

  // 电池正极突起
  M5.Display.fillRect(x + w, y + 2, 2, h - 4, color);
  // 电池边框
  M5.Display.fillRect(x, y, w, h, color);
  // 内部黑色（背景）
  M5.Display.fillRect(x + 1, y + 1, w - 2, h - 2, 0x0000);
  // 内部填充显示电量
  int fillWidth = (batteryLevel * (w - 4)) / 100;
  M5.Display.fillRect(x + 2, y + 2, fillWidth, h - 4, color);

  // 显示百分比数字 - 电池右边，同一排
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(color);
  M5.Display.setCursor(x + w + 4, y + 1);
  M5.Display.print(batteryLevel);
  M5.Display.print("%");
}
