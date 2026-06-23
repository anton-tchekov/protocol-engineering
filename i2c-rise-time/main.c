#include <stdio.h>
#include "pico/stdlib.h"
#include "serial.h"
//#include "blink.h"
#include "measure.h"
#include "i2c-config.h"
#include "i2c-scan.h"
#include "mpu6050.h"

int main(void)
{
	serial_init();

	sleep_ms(5000);
	/*printf("Rise time measurement\n");

	measurement_init();
	for(;;)
	{
		int loops = measurement_run();
		measurement_print(loops);
		sleep_ms(1000);
	}*/

	printf("%s I2C Init\n", BITBANG_I2C ? "Bit-bang" : "Hardware");
	i2c_setup();
	i2c_scan();
	printf("MPU6050 Init. Return Code = %d\n", mpu6050_init());

	for(;;)
	{
		MPU6050_Result r;

		mpu6050_read(&r);

		printf("\n\n"
			"----\n"
			"Acc:  %10.2f | %10.2f | %10.2f\n"
			"Gyro: %10.2f | %10.2f | %10.2f\n"
			"Temp: %10.2f deg. C\n"
			"\n",
			r.Acc[0], r.Acc[1], r.Acc[2],
			r.Gyro[0], r.Gyro[1], r.Gyro[2],
			r.Temperature);

		sleep_ms(1000);
	}
}
