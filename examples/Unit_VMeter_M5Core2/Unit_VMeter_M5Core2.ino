/**
 * @file Unit_VMeter_M5Core2.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5UnitVmeter Example
 * @version 0.1
 * @date 2024-01-30
 *
 *
 * @Hardwares: M5Core2 + Unit Vmeter
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5_ADS1115: https://github.com/m5stack/M5-ADS1115
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 */

#include "M5Unified.h"
#include "M5GFX.h"
#include "M5_ADS1115.h"

#define M5_UNIT_VMETER_I2C_ADDR             0x49
#define M5_UNIT_VMETER_EEPROM_I2C_ADDR      0x53
#define M5_UNIT_VMETER_PRESSURE_COEFFICIENT 0.015918958F

ADS1115 Vmeter;

float resolution         = 0.0;
float calibration_factor = 0.0;

void setup() {
    M5.begin();
    while (!Vmeter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 32, 33, 400000U)) {
        Serial.println("Unit Vmeter Init Fail");
        delay(1000);
    }
    Vmeter.setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
    Vmeter.setMode(ADS1115_MODE_SINGLESHOT);
    Vmeter.setRate(ADS1115_RATE_8);
    Vmeter.setGain(ADS1115_PGA_512);
    // | PGA      | Max Input Voltage(V) |
    // | PGA_6144 |        128           |
    // | PGA_4096 |        64            |
    // | PGA_2048 |        32            |
    // | PGA_512  |        16            |
    // | PGA_256  |        8             |

    resolution = Vmeter.getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
    calibration_factor = Vmeter.getFactoryCalibration();
}

void loop() {
    int16_t adc_raw = Vmeter.getSingleConversion();
    float voltage   = adc_raw * resolution * calibration_factor;
    Serial.printf("Cal ADC:%.0f\n", adc_raw * calibration_factor);
    Serial.printf("Cal Voltage:%.2f mV\n", voltage);
    Serial.printf("Raw ADC:%d\n\n", adc_raw);

    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("Cal ADC:%.0f\n", adc_raw * calibration_factor);
    M5.Display.printf("Cal Voltage:%.2f mV\n", voltage);
    M5.Display.printf("Raw ADC:%d\n\n", adc_raw);
    delay(1000);
}
