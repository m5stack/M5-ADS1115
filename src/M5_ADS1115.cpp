#include "M5_ADS1115.h"

bool ADS1115::begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl,
                    long freq) {
    _i2c.begin(wire, sda, scl, freq);
    _addr               = addr;
    _coefficient        = 0.0;
    _calibration_factor = 0.0;
    return _i2c.exist(_addr);
}

void ADS1115::setEEPROMAddr(uint8_t addr) {
    _epprom_addr = addr;
}

float ADS1115::getCoefficient() {
    return _coefficient;
}

float ADS1115::getFactoryCalibration() {
    return _calibration_factor;
}

void ADS1115::setGain(ads1115_gain_t gain) {
    uint16_t reg_value = 0;
    bool result        = _i2c.readU16(_addr, ADS1115_REG_CONFIG, &reg_value);

    if (result == false) {
        return;
    }

    reg_value &= ~(0b0111 << 9);
    reg_value |= gain << 9;

    result = _i2c.writeU16(_addr, ADS1115_REG_CONFIG, reg_value);

    if (result) {
        _gain          = gain;
        int16_t hope   = 1;
        int16_t actual = 1;
        if (readCalibration(gain, &hope, &actual)) {
            _calibration_factor = fabs((double)hope / actual);
        }

        switch (_gain) {
            case ADS1115_PGA_6144:
                _coefficient = ADS1115_MV_6144;
                break;
            case ADS1115_PGA_4096:
                _coefficient = ADS1115_MV_4096;
                break;
            case ADS1115_PGA_2048:
                _coefficient = ADS1115_MV_2048;
                break;
            case ADS1115_PGA_1024:
                _coefficient = ADS1115_MV_1024;
                break;
            case ADS1115_PGA_512:
                _coefficient = ADS1115_MV_512;
                break;
            case ADS1115_PGA_256:
                _coefficient = ADS1115_MV_256;
                break;
            default:
                break;
        }
    }
}

void ADS1115::setRate(ads1115_rate_t rate) {
    uint16_t reg_value = 0;
    bool result        = _i2c.readU16(_addr, ADS1115_REG_CONFIG, &reg_value);
    if (result == false) {
        return;
    }

    reg_value &= ~(0b0111 << 5);
    reg_value |= rate << 5;

    result = _i2c.writeU16(_addr, ADS1115_REG_CONFIG, reg_value);

    if (result) {
        _rate = rate;
    }

    return;
}

/*! @brief Set read continuous read/single read data */
void ADS1115::setMode(ads1115_mode_t mode) {
    uint16_t reg_value = 0;
    bool result        = _i2c.readU16(_addr, ADS1115_REG_CONFIG, &reg_value);
    if (result == false) {
        return;
    }

    reg_value &= ~(0b0001 << 8);
    reg_value |= mode << 8;

    result = _i2c.writeU16(_addr, ADS1115_REG_CONFIG, reg_value);
    if (result) {
        _mode = mode;
    }

    return;
}

/*! @brief Determine if data is being converted
    @return Data being converted returns 1, otherwise 0.. */
bool ADS1115::isInConversion() {
    uint16_t value = 0x00;
    _i2c.readU16(_addr, ADS1115_REG_CONFIG, &value);

    return (value & (1 << 15)) ? false : true;
}

/*! @brief Turn on single data conversion */
void ADS1115::startSingleConversion() {
    uint16_t reg_value = 0;
    bool result        = _i2c.readU16(_addr, ADS1115_REG_CONFIG, &reg_value);

    if (result == false) {
        return;
    }

    reg_value &= ~(0b0001 << 15);
    reg_value |= 0x01 << 15;

    _i2c.writeU16(_addr, ADS1115_REG_CONFIG, reg_value);
}

int16_t ADS1115::getAdcRaw() {
    uint16_t value = 0x00;
    _i2c.readU16(_addr, ADS1115_REG_CONVERSION, &value);
    adc_raw = value;
    return value;
}

int16_t ADS1115::getSingleConversion(uint16_t timeout) {
    if (_mode == ADS1115_MODE_SINGLESHOT) {
        startSingleConversion();
        delay(cover_time);
        uint64_t time = millis() + timeout;
        while (time > millis() && isInConversion())
            ;
    }

    return getAdcRaw() * MEASURING_DIRECTION;
}

bool ADS1115::saveCalibration(ads1115_gain_t gain, int16_t hope,
                              int16_t actual) {
    if (hope == 0 || actual == 0) {
        return false;
    }

    uint8_t buff[8];
    memset(buff, 0, 8);
    buff[0] = gain;
    buff[1] = hope >> 8;
    buff[2] = hope & 0xFF;

    buff[3] = actual >> 8;
    buff[4] = actual & 0xFF;

    for (uint8_t i = 0; i < 5; i++) {
        buff[5] ^= buff[i];
    }

    uint8_t reg = 0xd0 + gain * 8;
    return _i2c.writeBytes(_epprom_addr, reg, buff, 8);
}

bool ADS1115::readCalibration(ads1115_gain_t gain, int16_t* hope,
                              int16_t* actual) {
    uint8_t reg = 0xd0 + gain * 8;
    uint8_t buff[8];
    memset(buff, 0, 8);

    bool result = _i2c.readBytes(_epprom_addr, reg, buff, 8);

    if (result == false) {
        return false;
    }

    uint8_t xor_result = 0x00;
    for (uint8_t i = 0; i < 5; i++) {
        xor_result ^= buff[i];
    }

    if (xor_result != buff[5]) {
        return false;
    }

    *hope   = (buff[1] << 8) | buff[2];
    *actual = (buff[3] << 8) | buff[4];
    return true;
}
