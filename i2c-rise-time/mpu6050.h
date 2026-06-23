#ifndef __MPU6050_H__
#define __MPU6050_H__

typedef struct
{
	double Acc[3];
	double Gyro[3];
	double Temperature;
} MPU6050_Result;

int mpu6050_init(void);
int mpu6050_read(MPU6050_Result *r);

#endif
