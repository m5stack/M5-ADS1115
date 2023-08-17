/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5StickC sample source code
*                          配套  M5StickC 示例源代码
* Visit for more information: https://docs.m5stack.com/en/unit/vmeter
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/vmeter
*
* Product:  Vmeter_ADS1115.
* Date: 2022/7/11
*******************************************************************************
  Please connect to Port A,Measure voltage and display in the screen.
  请连接端口A,测量电压并显示到屏幕上
  Pay attention: EEPROM (0x53) has built-in calibration parameters when leaving
  the factory. Please do not write to the EEPROM, otherwise the calibration data
  will be overwritten and the measurement results will be inaccurate.
  注意: EEPROM(0x53)在出厂时具有内置的校准参数。
  请不要写入EEPROM，否则校准数据会被覆盖，测量结果会不准确。
*/

#include "M5StickC.h"
#include "M5_ADS1115.h"

ADS1115 voltmeter;

void setup() {
    M5.begin();
    Wire.begin();
    voltmeter.setMode(SINGLESHOT);
    voltmeter.setRate(RATE_8);   // Lowest most stable read time.
    voltmeter.setGain(PGA_256);  // Most sensitive detection.
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextColor(WHITE, BLACK);
    // M5.Lcd.fillRect(65, 10, 5, 10, BLACK);
}

void loop(void) {
    double volts = 0;
    volts        = voltmeter.getValue() / 1000.0;
    if (volts < 0.000001) volts = 0.0;
    M5.Lcd.setCursor(30, 40);
    M5.Lcd.printf("%.3lf v   ", volts);
}