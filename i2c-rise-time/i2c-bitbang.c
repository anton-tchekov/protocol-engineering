#include "i2c-bitbang.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#if BITBANG_I2C

static bool i2c_update_flag = false;

static bool sda_read(void)
{
	return gpio_get(I2C_SDA);
}

static int scl_read(void)
{
	return gpio_get(I2C_SCL);
}

static void sda_high(void)
{
	gpio_set_dir(I2C_SDA, false);
}

static void sda_low(void)
{
	gpio_put(I2C_SDA, false);
	gpio_set_dir(I2C_SDA, true);
}

static void scl_high(void)
{
	gpio_set_dir(I2C_SCL, false);
}

static void scl_low(void)
{
	gpio_put(I2C_SCL, false);
	gpio_set_dir(I2C_SCL, true);
}

static void i2c_delay(void)
{
	sleep_us(2);
}

void i2c_setup(void)
{
	gpio_init(I2C_SCL);
	gpio_init(I2C_SDA);
	scl_high();
	sda_high();
}

static void i2c_start(void)
{
	sda_high();
	i2c_delay();
	scl_high();
	i2c_delay();
	sda_low();
	i2c_delay();
	scl_low();
	i2c_delay();
}

static void i2c_stop(void)
{
	sda_low();
	i2c_delay();
	scl_high();
	i2c_delay();
	sda_high();
	i2c_delay();
}

static bool i2c_tx(uint8_t b)
{
	for(uint8_t i = 8; i; --i)
	{
		(b & 0x80) ? sda_high() : sda_low();
		b <<= 1;
		i2c_delay();
		scl_high();
		i2c_delay();
		scl_low();
		i2c_delay();
	}

	sda_high();
	scl_high();
	i2c_delay();
	bool ack = !sda_read();
	scl_low();
	i2c_delay();
	return ack;
}

static uint8_t i2c_rx(bool ack)
{
	uint8_t b = 0;
	sda_high();
	for(int i = 0; i < 8; ++i)
	{
		b <<= 1;
		do
		{
			scl_high();
		}
		while(!scl_read());
		i2c_delay();
		if(sda_read())
		{
			b |= 1;
		}

		i2c_delay();
		scl_low();
		i2c_delay();
	}

	ack ? sda_low() : sda_high();
	scl_high();
	i2c_delay();
	scl_low();
	sda_high();
	i2c_delay();
	return b;
}

int i2c_read_byte(uint8_t addr, uint8_t reg)
{
	i2c_start();
	if(!i2c_tx((addr << 1) | 0)) { return -1; }
	if(!i2c_tx(reg)) { return -1; }
	i2c_start();
	if(!i2c_tx((addr << 1) | 1)) { return -1; }
	int ret = i2c_rx(false);
	i2c_stop();
	return ret;
}

int i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t value)
{
	i2c_start();
	if(!i2c_tx((addr << 1) | 0)) { return -1; }
	if(!i2c_tx(reg)) { return -1; }
	if(!i2c_tx(value)) { return -1; }
	i2c_stop();
	return 0;
}

int i2c_read_block(uint8_t addr, uint8_t reg, int bytes, uint8_t *buf)
{
	i2c_start();
	if(!i2c_tx((addr << 1) | 0)) { return -1; }
	if(!i2c_tx(reg)) { return -1; }
	i2c_start();
	if(!i2c_tx((addr << 1) | 1)) { return -1; }
	for(int i = 0; i < bytes; ++i)
	{
		buf[i] = i2c_rx(i < bytes - 1);
	}

	i2c_stop();
	return 0;
}

bool i2c_check_update_flag(void)
{
	bool ret = i2c_update_flag;
	if(ret)
	{
		i2c_update_flag = false;
	}

	return ret;
}

bool i2c_check(uint8_t addr)
{
	i2c_start();
	bool ack = i2c_tx(addr << 1);
	i2c_stop();
	return ack;
}

#endif
