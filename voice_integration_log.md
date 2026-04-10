# 语音功能集成尝试日志

## 日期
2026-04-09

## 目标
在 M5Stack Stick S3 上集成麦克风功能，通过 USB 串口将音频数据传输到电脑。

## 硬件配置
- 设备：M5Stack Stick S3 (ESP32-S3-PICO-1)
- 屏幕：130x240 像素（可视区域）
- 音频编解码器：ES8311 (I2C 地址 0x18)
- I2S 配置：16kHz 采样率，16 位采样

## 尝试过程

### 第一次尝试：完整 I2S + ES8311 初始化

**代码配置：**
```cpp
// I2S 引脚配置
i2s_pin_config_t i2s_pin_config = {
  .bck_io_num = 0,    // BCK
  .ws_io_num = 13,    // LRCK
  .data_in_num = 10   // DIN
};

// ES8311 初始化
Wire.begin(11, 9);  // SDA=11, SCL=9
writeES8311(0x00, 0x04);  // 芯片复位
writeES8311(0x08, 0x00);  // 启用 ADC
i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
i2s_set_pin(I2S_NUM_0, &i2s_pin_config);
```

**结果：失败**
- 设备绿灯常亮，但屏幕完全不亮
- 程序可能在初始化过程中卡住

**分析：**
1. ES8311 I2C 通信可能失败，导致系统 hangs
2. I2S 引脚可能与屏幕引脚冲突
3. 初始化顺序可能有问题

---

### 第二次尝试：仅 I2S 初始化（移除 ES8311）

**改动：**
```cpp
void setupAudio() {
  // 移除 Wire.begin() 和 ES8311 初始化
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &i2s_pin_config);
}
```

**结果：仍然失败**
- 屏幕依然不亮
- 说明问题不在 ES8311 I2C 通信

---

### 第三次尝试：调整初始化顺序

**改动：**
```cpp
void setup() {
  M5.begin();
  // 先初始化屏幕和显示
  drawUI();
  Serial.begin(115200);
  Serial.println("Display initialized");
  
  // 然后初始化音频
  setupAudio();
  Serial.println("Audio initialized");
}
```

**结果：失败**
- 屏幕仍然不亮

---

## 问题分析

### 可能的原因

1. **I2S 引脚冲突**
   - GPIO 13 (LRCK/WS) 可能与屏幕背光或 CS 引脚冲突
   - GPIO 0 (BCK) 是 Boot 引脚，可能影响启动
   - GPIO 10 (DIN) 可能与 PSRAM 或其他外设冲突

2. **I2S 驱动与显示驱动冲突**
   - M5Unified 可能内部使用了 I2S 驱动屏幕背光或音频
   - I2S_NUM_0 可能已被 M5Unified 占用

3. **内存问题**
   - DMA 缓冲区配置过大 (8x1024 = 8KB)
   - 但全球变量使用仅 8%，不太可能是主因

### 需要尝试的解决方案

1. **更换 I2S 端口**
   - 尝试使用 I2S_NUM_1 而不是 I2S_NUM_0

2. **查阅 Stick S3 原理图**
   - 确认麦克风实际使用的 GPIO 引脚
   - 确认哪些引脚与屏幕冲突

3. **使用 M5Unified 内置音频 API**
   - M5Unified 可能提供了内置的麦克风支持
   - 避免直接使用 I2S 驱动

4. **减小 DMA 缓冲区**
   - 尝试更小的 dma_buf_count 和 dma_buf_len

5. **延迟初始化 I2S**
   - 在 loop() 中延迟初始化，确保屏幕完全启动

## 当前状态

- 代码已回滚到只有显示和电量功能的可用版本
- Commit: `b85ba2c` - "Revert voice feature - screen was not turning on"
- 已 push 到 GitHub: nickwang94/stick-s3-voice-transmitter

## 下一步计划

1. ~~阅读 M5Unified 文档，查找是否有内置麦克风 API~~ ✅ 已完成
2. ~~查阅 Stick S3 原理图，确认正确的 GPIO 引脚~~ ✅ 使用 M5Unified 抽象层，无需关心底层引脚
3. ~~尝试使用 I2S_NUM_1 而不是 I2S_NUM_0~~ ✅ 使用 M5.Mic API 自动处理
4. ~~添加串口调试输出，确定程序卡在哪个步骤~~ ✅ 已完成

---

## 最终成功方案（2026-04-10）

### 关键发现

**使用 M5Unified 内置的 `M5.Mic` API** 而不是直接使用 `driver/i2s.h`！

之前失败的原因：
1. 直接使用 `i2s_driver_install()` 和 `i2s_set_pin()` 会导致屏幕不亮
2. 手动配置 ES8311 编解码器（I2C 通信）可能失败或 hangs 系统
3. I2S 引脚配置可能与屏幕驱动冲突

### 正确的代码结构

```cpp
#include <M5Unified.h>

void setup() {
  M5.begin();
  M5.Speaker.end();  // 必须先关闭扬声器
  M5.Mic.begin();    // 初始化麦克风
  Serial.begin(115200);
}

void loop() {
  if (M5.BtnA.isPressed()) {
    // 使用 M5.Mic.record() 读取音频数据
    if (M5.Mic.record(buffer, size)) {
      Serial.write(buffer, size);
    }
  }
}
```

### 串口接收测试

运行 Python 脚本接收音频数据：
```bash
cd /Users/nickwang/Documents/application/stick_s3_voice_receiver
source venv/bin/activate
python record.py
```

**测试结果：**
- ✅ 成功接收音频数据
- ✅ 平均速率约 10KB/s
- ✅ 可保存为 WAV 文件
- ✅ 屏幕显示正常工作

### 相关文件

- Arduino 代码：`/Users/nickwang/Documents/Arduino/stick_s3/stick_s3.ino`
- Python 接收器：`/Users/nickwang/Documents/application/stick_s3_voice_receiver/`
  - `record.py` - 录音并保存为 WAV
  - `listen_serial.py` - 持续监听串口
  - `test_mic.py` - 快速测试

### 参考资料

- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5Unified Microphone Example](https://github.com/m5stack/M5Unified/blob/master/examples/Basic/Microphone/Microphone.ino)
- [StickS3 Mic Documentation](https://docs.m5stack.com/en/arduino/m5sticks3/mic)
- [M5Unified Mic Class](https://docs.m5stack.com/en/arduino/m5unified/mic_class)

## 相关文件

- Arduino 代码：`/Users/nickwang/Documents/Arduino/stick_s3/stick_s3.ino`
- Python 接收器：`/Users/nickwang/Documents/application/stick_s3_voice_receiver/`
