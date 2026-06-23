#include "i2c.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C_PORT        i2c0

#if !BITBANG_I2C

void i2c_setup(void)
{
	i2c_init(I2C_PORT, I2C_FREQ);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
}

int i2c_read_byte(uint8_t addr, uint8_t reg)
{
	uint8_t val = 0;
	if(i2c_write_blocking(I2C_PORT, addr, &reg, 1, true) != 1)
	{
		return -1;
	}

	if(i2c_read_blocking(I2C_PORT, addr, &val, 1, false) != 1)
	{
		return -1;
	}

	return val;
}

int i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t value)
{
	uint8_t buf[2] = { reg, value };
	if(i2c_write_blocking(I2C_PORT, addr, buf, 2, false) != 2)
	{
		return -1;
	}

	return 0;
}

int i2c_read_block(uint8_t addr, uint8_t reg, int bytes, uint8_t *buf)
{
	if(i2c_write_blocking(I2C_PORT, addr, &reg, 1, true) != 1)
	{
		return -1;
	}

	if(i2c_read_blocking(I2C_PORT, addr, buf, bytes, false) != bytes)
	{
		return -1;
	}

	return 0;
}

bool i2c_check(uint8_t addr)
{
	if(i2c_write_blocking(I2C_PORT, addr, NULL, 0, false) == PICO_ERROR_GENERIC)
	{
		return false;
	}

	return true;
}

#endif
