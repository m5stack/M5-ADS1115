/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Core sample source code
*                          配套  M5Core 示例源代码
* Visit for more information: https://docs.m5stack.com/en/unit/vmeter
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/vmeter
*
* Describe: Vmeter_ADS1115.
* Date: 2022/8/10
*******************************************************************************
  Please connect to Port A,Measure voltage and display in the screen.
  请连接端口A,测量电压并显示到屏幕上
  Pay attention: EEPROM (0x53) has built-in calibration parameters when leaving
the factory. Please do not write to the EEPROM, otherwise the calibration data
will be overwritten and the measurement results will be inaccurate. 注意: EEPROM
(0x53)在出厂时具有内置的校准参数。请不要写入EEPROM，否则校准数据会被覆盖，测量结果会不准确。
*/
#include "M5Stack.h"
#include <Wire.h>
#include "M5_ADS1115.h"
#include "title.h"
#include "shut.h"

ADS1115 voltmeter;

float pgae512_volt  = 5000.0F;
float pgae4096_volt = 60000.0F;

int16_t volt_raw_list[10];
uint8_t raw_now_ptr = 0;
int16_t adc_raw     = 0;

int16_t hope           = 0.0;
ADS1115Gain_t now_gain = PGA_512;

int x     = 0;
int xt    = 0;
int value = 0;

int bright[4] = {30, 60, 100, 200};
int b         = 1;
bool d        = 0;

void setup(void) {
    M5.begin();
    M5.Power.begin();
    M5.Lcd.setSwapBytes(true);
    M5.Lcd.setBrightness(bright[b]);
    Wire.begin();

    voltmeter.setMode(SINGLESHOT);
    voltmeter.setRate(RATE_128);
    voltmeter.setGain(PGA_512);
    hope = pgae512_volt / voltmeter.resolution;

    // | PGA      | Max Input Voltage(V) |
    // | PGA_6144 |        128           |
    // | PGA_4096 |        64            |
    // | PGA_2048 |        32            |
    // | PGA_512  |        16            |
    // | PGA_256  |        8             |

    M5.Lcd.fillScreen(BLACK);

    M5.Lcd.pushImage(4, 20, 220, 48, title);
    M5.Lcd.pushImage(152, 216, 16, 16, shut);

    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.drawString("Warning!", 234, 40, 2);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.drawString("This device can", 214, 60, 1);
    M5.Lcd.drawString("measure voltage", 214, 72, 1);
    M5.Lcd.drawString("up to 32 Volts", 214, 84, 1);
    M5.Lcd.drawString("BRIGHTNESS: " + String(b), 16, 4, 1);
    M5.Lcd.drawString("BATERRY: " + String(M5.Power.getBatteryLevel()) + "%",
                      226, 4, 1);

    M5.Lcd.setTextFont(2);

    M5.Lcd.setCursor(36, 216);
    M5.Lcd.printf("D00/000");

    M5.Lcd.setCursor(220, 216);
    M5.Lcd.printf("Brightness");

    M5.Lcd.drawLine(16, 180, 304, 180, WHITE);
    M5.Lcd.drawLine(16, 16, 304, 16, WHITE);

    M5.Lcd.setTextColor(WHITE, RED);
    M5.Lcd.drawString("     mVolts     ", 210, 100, 2);
    M5.Lcd.setTextColor(WHITE, BLACK);

    int start = 16;
    M5.Lcd.setTextFont(2);
    for (int i = 0; i < 17; i++) {
        M5.Lcd.drawLine((i * 18) + start, 170, (i * 18) + start, 184, WHITE);
        if (i < 16)
            M5.Lcd.drawLine((i * 18) + start + 8, 178, (i * 18) + start + 8,
                            184, WHITE);
        if (i % 4 == 0) {
            M5.Lcd.setCursor((i * 18) + start - 4, 186);
            M5.Lcd.print(i * 2);
        }
    }
}

void loop(void) {
    M5.update();

    voltmeter.getValue();

    volt_raw_list[raw_now_ptr] = voltmeter.adc_raw;
    raw_now_ptr                = (raw_now_ptr == 9) ? 0 : (raw_now_ptr + 1);

    int count = 0;
    int total = 0;

    for (uint8_t i = 0; i < 10; i++) {
        if (volt_raw_list[i] == 0) {
            continue;
        }
        total += volt_raw_list[i];
        count += 1;
    }

    if (count == 0) {
        adc_raw = 0;
    } else {
        adc_raw = total / count;
    }

    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.setCursor(10, 80);
    M5.Lcd.setTextFont(7);

    value = adc_raw * voltmeter.resolution * voltmeter.calibration_factor;

    if (d == 0)
        M5.Lcd.printf("%.2f   \r\n", (

                                         adc_raw * voltmeter.resolution *
                                         voltmeter.calibration_factor) /
                                         1000);
    if (d == 1)
        M5.Lcd.printf("%.3f  \r\n", (adc_raw * voltmeter.resolution *
                                     voltmeter.calibration_factor) /
                                        1000);

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.drawString(
        String(adc_raw * voltmeter.resolution * voltmeter.calibration_factor) +
            "          ",
        210, 120, 2);
    M5.Lcd.setTextColor(WHITE, BLUE);
    M5.Lcd.drawString("ADC:" + String(adc_raw) + " ", 210, 140, 1);
    M5.Lcd.setTextColor(WHITE, BLACK);

    if (value < 0) value = value * -1;

    x = map(value, 0, 32000, 16, 304);

    if (x != xt) {
        M5.Lcd.fillTriangle(xt - 5, 154, xt + 5, 154, xt, 166, BLACK);
        M5.Lcd.fillTriangle(x - 5, 154, x + 5, 154, x, 166, RED);
        xt = x;
    }

    if (M5.BtnC.wasPressed()) {
        b++;
        if (b > 3) b = 0;
        M5.Lcd.setBrightness(bright[b]);
        M5.Lcd.drawString("BRIGHTNESS: " + String(b), 16, 4, 1);
    }

    if (M5.BtnA.wasPressed()) d = !d;
}
