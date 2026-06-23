#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

#define BITBANG_I2C    1

#define I2C_SDA        8
#define I2C_SCL        9
#define I2C_FREQ  400000

void i2c_setup(void);
int i2c_read_byte(uint8_t addr, uint8_t reg);
int i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t value);
int i2c_read_block(uint8_t addr, uint8_t reg, int bytes, uint8_t *buf);
bool i2c_check(uint8_t addr);

#endif
