#ifndef _M5_ADS1115_H_
#define _M5_ADS1115_H_

#include "Arduino.h"
#include "Wire.h"
#include "I2C_Class.h"

#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG     0x01
#define ADS1115_I2C_ADDR_0     0x48
#define ADS1115_I2C_ADDR_1     0x49

#define ADS1115_MV_6144 0.187500F
#define ADS1115_MV_4096 0.125000F
#define ADS1115_MV_2048 0.062500F  // default
#define ADS1115_MV_1024 0.031250F
#define ADS1115_MV_512  0.015625F
#define ADS1115_MV_256  0.007813F

#define MEASURING_DIRECTION -1

typedef enum {
    ADS1115_PGA_6144 = 0,
    ADS1115_PGA_4096,
    ADS1115_PGA_2048,  // default
    ADS1115_PGA_1024,
    ADS1115_PGA_512,
    ADS1115_PGA_256,
} ads1115_gain_t;

typedef enum {
    ADS1115_RATE_8 = 0,
    ADS1115_RATE_16,
    ADS1115_RATE_32,
    ADS1115_RATE_64,
    ADS1115_RATE_128,  // default
    ADS1115_RATE_250,
    ADS1115_RATE_475,
    ADS1115_RATE_860,
} ads1115_rate_t;

typedef enum {
    ADS1115_MODE_CONTINUOUS = 0,
    ADS1115_MODE_SINGLESHOT,  // default
} ads1115_mode_t;

class ADS1115 {
   private:
    I2C_Class _i2c;
    uint8_t _addr;
    uint8_t _epprom_addr;
    float _coefficient;

   public:
    ads1115_gain_t _gain;
    ads1115_rate_t _rate;
    ads1115_mode_t _mode;

    uint16_t cover_time;
    int16_t adc_raw;
    float _calibration_factor;

   public:
    bool begin(TwoWire* wire = &Wire, uint8_t addr = ADS1115_I2C_ADDR_0,
               uint8_t sda = 21, uint8_t scl = 22, long freq = 400000U);

    void setGain(ads1115_gain_t gain);
    void setRate(ads1115_rate_t rate);
    void setMode(ads1115_mode_t mode);

    float getCoefficient();

    float getFactoryCalibration();

    int16_t getSingleConversion(uint16_t timeout = 125);
    int16_t getAdcRaw();

    bool isInConversion();
    void startSingleConversion();

    void setEEPROMAddr(uint8_t addr);

    void setCalibration(int8_t voltage, uint16_t actual);
    bool saveCalibration(ads1115_gain_t gain, int16_t hope, int16_t actual);
    bool readCalibration(ads1115_gain_t gain, int16_t* hope, int16_t* actual);
};

#endif
