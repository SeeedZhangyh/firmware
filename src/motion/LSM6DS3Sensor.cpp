#include "LSM6DS3Sensor.h"
#include "NodeDB.h"

#if !defined(ARCH_STM32WL) && !MESHTASTIC_EXCLUDE_I2C && __has_include(<Adafruit_LSM6DS3TRC.h>)

// ---------------------------------------------------------------------------
// Adafruit_LSM6DS3TRC_Compat::_init()
//
// Same initialisation sequence as Adafruit_LSM6DS3TRC::_init(), but accepts
// both the standard WHO_AM_I (0x6A) and the alternate variant (0x6C).
// Called by begin_I2C() via virtual dispatch — no library code modified.
// ---------------------------------------------------------------------------
bool Adafruit_LSM6DS3TRC_Compat::_init(int32_t sensor_id)
{
    uint8_t id = chipID();

    if (id != LSM6DS3TRC_CHIP_ID && id != LSM6DS3TRC_CHIP_ID_ALT) {
        LOG_WARN("LSM6DS3TRC_Compat: unexpected WHO_AM_I=0x%02X", id);
        return false;
    }
    LOG_DEBUG("LSM6DS3TRC_Compat: WHO_AM_I=0x%02X accepted", id);

    _sensorid_accel = sensor_id;
    _sensorid_gyro  = sensor_id + 1;
    _sensorid_temp  = sensor_id + 2;

    reset();

    // Call base class _init() — sets default data-rates, ranges & allocates
    // the Adafruit_Sensor helper objects.
    Adafruit_LSM6DS::_init(sensor_id);

    // Block Data Update: prevent MSB/LSB registers from updating until both
    // are read (same as original Adafruit_LSM6DS3TRC::_init).
    Adafruit_BusIO_Register ctrl3 = Adafruit_BusIO_Register(
        i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LSM6DS_CTRL3_C);
    Adafruit_BusIO_RegisterBits bdu = Adafruit_BusIO_RegisterBits(&ctrl3, 1, 6);
    bdu.write(1);

    return true;
}

// ---------------------------------------------------------------------------

LSM6DS3Sensor::LSM6DS3Sensor(ScanI2C::FoundDevice foundDevice) : MotionSensor::MotionSensor(foundDevice) {}

bool LSM6DS3Sensor::init()
{
    if (sensor.begin_I2C(deviceAddress())) {

        // Default threshold of 2G, less sensitive options are 4, 8 or 16G
        sensor.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);

        // Duration is number of occurrences needed to trigger, higher threshold is less sensitive
        sensor.enableWakeup(config.display.wake_on_tap_or_motion, 1, LSM6DS3_WAKE_THRESH);

        LOG_DEBUG("LSM6DS3 init ok");
        return true;
    }
    LOG_DEBUG("LSM6DS3 init failed");
    return false;
}

int32_t LSM6DS3Sensor::runOnce()
{
    if (sensor.shake()) {
        wakeScreen();
        return 500;
    }
    // // Show data test
    // if (1) {
    //     float x, y, z;
    //     bool available = sensor.accelerationAvailable();
    //     LOG_DEBUG("LSM6DS3 accelerationAvailable: %d\n", available);
    //     if (!available) {
    //         x = y = z = NAN;
    //         return 1000;
    //     }else{
    //         sensor.readAcceleration(x, y, z);
    //         LOG_DEBUG("LSM6DS3 readAcceleration: x=%.2f y=%.2f z=%.2f\n", x, y, z);
    //     }   

    //     available = sensor.gyroscopeAvailable();
    //     LOG_DEBUG("LSM6DS3 gyroscopeAvailable: %d\n", available);
    //     if (!available) {
    //         x = y = z = NAN;
    //         return 1000;
    //     }else{
    //         sensor.readGyroscope(x, y, z);
    //         LOG_DEBUG("LSM6DS3 readGyroscope:    x=%.2f y=%.2f z=%.2f\n", x, y, z);           
    //     }
    //     return 1000;
    // }
    return MOTION_SENSOR_CHECK_INTERVAL_MS;
}

#endif