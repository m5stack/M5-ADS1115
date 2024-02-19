/**
 * @file Unit_AMeter_M5StickC.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5UnitAmeter Example
 * @version 0.1
 * @date 2024-01-30
 *
 *
 * @Hardwares: M5StickC + Unit Ameter
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5_ADS1115: https://github.com/m5stack/M5-ADS1115
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 */

#include "M5Unified.h"
#include "M5GFX.h"
#include "M5_ADS1115.h"

#define M5_UNIT_AMETER_I2C_ADDR             0x48
#define M5_UNIT_AMETER_EEPROM_I2C_ADDR      0x51
#define M5_UNIT_AMETER_PRESSURE_COEFFICIENT 0.05

ADS1115 Ameter;

float resolution         = 0.0;
float calibration_factor = 0.0;

void setup() {
    M5.begin();
    while (!Ameter.begin(&Wire, M5_UNIT_AMETER_I2C_ADDR, 32, 33, 400000U)) {
        Serial.println("Unit Ameter Init Fail");
        delay(1000);
    }
    Ameter.setEEPROMAddr(M5_UNIT_AMETER_EEPROM_I2C_ADDR);
    Ameter.setMode(ADS1115_MODE_SINGLESHOT);
    Ameter.setRate(ADS1115_RATE_8);
    Ameter.setGain(ADS1115_PGA_512);
    // | PGA      | Max Input Voltage(V) |
    // | PGA_6144 |        128           |
    // | PGA_4096 |        64            |
    // | PGA_2048 |        32            |
    // | PGA_512  |        16            |
    // | PGA_256  |        8             |

    resolution = Ameter.getCoefficient() / M5_UNIT_AMETER_PRESSURE_COEFFICIENT;
    calibration_factor = Ameter.getFactoryCalibration();
}

void loop() {
    int16_t adc_raw = Ameter.getSingleConversion();
    float current   = adc_raw * resolution * calibration_factor;
    Serial.printf("Cal ADC:%.0f\n", adc_raw * calibration_factor);
    Serial.printf("Cal Current:%.2f mA\n", current);
    Serial.printf("Raw ADC:%d\n\n", adc_raw);

    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("Cal ADC:%.0f\n", adc_raw * calibration_factor);
    M5.Display.printf("Cal Current:%.2f mA\n", current);
    M5.Display.printf("Raw ADC:%d\n\n", adc_raw);
    delay(1000);
}
