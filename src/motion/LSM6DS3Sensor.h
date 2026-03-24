#pragma once
#ifndef _LSM6DS3_SENSOR_H_
#define _LSM6DS3_SENSOR_H_

#include "MotionSensor.h"

#if !defined(ARCH_STM32WL) && !MESHTASTIC_EXCLUDE_I2C && __has_include(<Adafruit_LSM6DS3TRC.h>)

#ifndef LSM6DS3_WAKE_THRESH
#define LSM6DS3_WAKE_THRESH 20
#endif

#include <Adafruit_LSM6DS3TRC.h>

// Chip ID returned by some LSM6DS3TRC-compatible modules (e.g. LSM6DSOX silicon)
#define LSM6DS3TRC_CHIP_ID_ALT 0x6C

/**
 * @brief Drop-in replacement for Adafruit_LSM6DS3TRC that also accepts the
 *        alternate WHO_AM_I value 0x6C seen on certain hardware variants.
 *        The library's own _init() rejects 0x6C, so this subclass overrides
 *        _init() with the same initialisation sequence while widening the ID
 *        check — without modifying any library source.
 */
class Adafruit_LSM6DS3TRC_Compat : public Adafruit_LSM6DS3TRC {
    bool _init(int32_t sensor_id) override;
};

class LSM6DS3Sensor : public MotionSensor
{
  private:
    // Adafruit_LSM6DS3TRC sensor;
    Adafruit_LSM6DS3TRC_Compat sensor;

  public:
    explicit LSM6DS3Sensor(ScanI2C::FoundDevice foundDevice);
    virtual bool init() override;
    virtual int32_t runOnce() override;
};

#endif

#endif