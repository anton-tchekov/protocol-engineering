#include "i2c-scan.h"
#include <stdio.h>

void i2c_scan(void)
{
	printf("\n\n\nScanning...\n");
	int count = 0;
	for(int addr = 1; addr < 127; ++addr)
	{
		if(i2c_check(addr))
		{
			printf("I2C device found at address 0x%02X\n", addr);
			count++;
		}
	}

	if(count == 0)
	{
		printf("No I2C devices found\n");
	}
	else
	{
		printf("Done. %d devices found\n", count);
	}
}
