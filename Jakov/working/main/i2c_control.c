#include "driver/i2c.h"

#define I2C_MASTER_SDA_IO 19
#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_FREQ_HZ 100000

const int i2cMasterNum = I2C_NUM_0;

void i2c_master_init()
{
    i2c_config_t conf =
    {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config( i2cMasterNum, &conf );
    i2c_driver_install( i2cMasterNum, conf.mode, 0, 0, 0 );
}