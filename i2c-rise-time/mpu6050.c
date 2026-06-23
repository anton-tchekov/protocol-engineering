#include "mpu6050.h"
#include "i2c-config.h"
#include <stdint.h>

#define MPU6050_ADDR       0x68

#define SMPLRT_DIV         0x19
#define CONFIG             0x1A
#define GYRO_CONFIG        0x1B
#define ACCEL_CONFIG       0x1C
#define INT_PIN_CFG        0x37
#define INT_ENABLE         0x38
#define INT_STATUS         0x3A
#define ACCEL_XOUT_H       0x3B
#define PWR_MGMT_1         0x6B
#define WHO_AM_I_MPU6050   0x75

#define AFS_2G                0
#define AFS_4G                1
#define AFS_8G                2
#define AFS_16G               3

#define GFS_250DPS            0
#define GFS_500DPS            1
#define GFS_1000DPS           2
#define GFS_2000DPS           3

#define A_SCALE                AFS_2G
#define G_SCALE                GFS_250DPS

#define G                      9.81

#if A_SCALE == AFS_2G
#define A_RES                  (2.0 / 32768.0 * G)
#elif A_SCALE == AFS_4G
#define A_RES                  (4.0 / 32768.0 * G)
#elif A_SCALE == AFS_8G
#define A_RES                  (8.0 / 32768.0 * G)
#elif A_SCALE == AFS_16G
#define A_RES                  (16.0 / 32768.0 * G)
#endif

#if G_SCALE == GFS_250DPS
#define G_RES                  (250.0 / 32768.0)
#elif G_SCALE == GFS_500DPS
#define G_RES                  (500.0 / 32768.0)
#elif G_SCALE == GFS_1000DPS
#define G_RES                  (1000.0 / 32768.0)
#elif G_SCALE == GFS_2000DPS
#define G_RES                  (2000.0 / 32768.0)
#endif

int mpu6050_init(void)
{
	int v;
	if((v = i2c_read_byte(MPU6050_ADDR, WHO_AM_I_MPU6050)) < 0)
	{
		return 1;
	}

	if(v != MPU6050_ADDR)
	{
		return 2;
	}

	if((v = i2c_write_byte(MPU6050_ADDR, PWR_MGMT_1, 0x01))) { return 3; }
	if((v = i2c_write_byte(MPU6050_ADDR, CONFIG, 0x03))) { return 4; }
	if((v = i2c_write_byte(MPU6050_ADDR, SMPLRT_DIV, 0x04))) { return 5; }

	if((v = i2c_read_byte(MPU6050_ADDR, GYRO_CONFIG)) < 0)
	{
		return 6;
	}

	if((v = i2c_write_byte(MPU6050_ADDR, GYRO_CONFIG, v & ~0xE0))) { return 7; }
	if((v = i2c_write_byte(MPU6050_ADDR, GYRO_CONFIG, v & ~0x18))) { return 8; }
	if((v = i2c_write_byte(MPU6050_ADDR, GYRO_CONFIG, v | G_SCALE << 3))) { return 9; }

	if((v = i2c_read_byte(MPU6050_ADDR, ACCEL_CONFIG)) < 0)
	{
		return 10;
	}

	if((v = i2c_write_byte(MPU6050_ADDR, ACCEL_CONFIG, v & ~0xE0))) { return 11; }
	if((v = i2c_write_byte(MPU6050_ADDR, ACCEL_CONFIG, v & ~0x18))) { return 12; }
	if((v = i2c_write_byte(MPU6050_ADDR, ACCEL_CONFIG, v | A_SCALE << 3))) { return 13; }

	if((v = i2c_write_byte(MPU6050_ADDR, INT_PIN_CFG, 0x22))) { return 14; }
	if((v = i2c_write_byte(MPU6050_ADDR, INT_ENABLE, 0x01))) { return 15; }
	return 0;
}

int mpu6050_read(MPU6050_Result *r)
{
	int v;
	if((v = i2c_read_byte(MPU6050_ADDR, INT_STATUS)) < 0)
	{
		return 1;
	}

	if(!(v & 0x01))
	{
		return 1;
	}

	uint8_t raw[14];
	if((v = i2c_read_block(MPU6050_ADDR, ACCEL_XOUT_H, 14, raw)) < 0)
	{
		return 1;
	}

	r->Acc[0] = (double)((int16_t)((raw[0] << 8) | raw[1])) * A_RES;
	r->Acc[1] = (double)((int16_t)((raw[2] << 8) | raw[3])) * A_RES;
	r->Acc[2] = (double)((int16_t)((raw[4] << 8) | raw[5])) * A_RES;
	r->Temperature = (double)((int16_t)((raw[6] << 8) | raw[7])) / 340.0 + 36.53f;
	r->Gyro[0] = (double)((int16_t)((raw[8] << 8) | raw[9])) * G_RES;
	r->Gyro[1] = (double)((int16_t)((raw[10] << 8) | raw[11])) * G_RES;
	r->Gyro[2] = (double)((int16_t)((raw[12] << 8) | raw[13])) * G_RES;
	return 0;
}
