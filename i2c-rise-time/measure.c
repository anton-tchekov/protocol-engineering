#include "measure.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "risetime.pio.h"
#include "risetime2.pio.h"
#include <stdio.h>
#include <stdint.h>

#define MEASUREMENT_PIN 16
#define SM_0 0
#define SM_1 1

uint offset, offset2;
pio_sm_config c, c2;

void measurement_init(void)
{
	offset = pio_add_program(pio0, &risetime_program);
	c = risetime_program_get_default_config(offset);

	offset2 = pio_add_program(pio0, &risetime2_program);
	c2 = risetime_program_get_default_config(offset2);
}

int measurement_run(void)
{
	gpio_init(MEASUREMENT_PIN);
	sm_config_set_in_pins(&c, MEASUREMENT_PIN);
	sm_config_set_set_pins(&c, MEASUREMENT_PIN, 1);
	sm_config_set_jmp_pin(&c, MEASUREMENT_PIN);

	sm_config_set_in_pins(&c2, MEASUREMENT_PIN);
	sm_config_set_set_pins(&c2, MEASUREMENT_PIN, 1);
	sm_config_set_jmp_pin(&c2, MEASUREMENT_PIN);

	pio_gpio_init(pio0, MEASUREMENT_PIN);
	pio_sm_set_consecutive_pindirs(pio0, SM_0, MEASUREMENT_PIN, 1, true);
	pio_sm_set_consecutive_pindirs(pio0, SM_1, MEASUREMENT_PIN, 1, true);

	pio_sm_init(pio0, SM_0, offset, &c);
	pio_sm_init(pio0, SM_1, offset2, &c2);

	// Pin output low
	gpio_set_dir(MEASUREMENT_PIN, GPIO_OUT);
	gpio_put(MEASUREMENT_PIN, 0);

	// Delay
	sleep_ms(100);

	// Reset PIO
	pio_sm_clear_fifos(pio0, SM_0);
	pio_sm_restart(pio0, SM_0);
	pio_sm_clear_fifos(pio0, SM_1);
	pio_sm_restart(pio0, SM_1);

	// Do measurement
	pio_enable_sm_mask_in_sync(pio0, 3);
	uint32_t x0 = pio_sm_get_blocking(pio0, SM_0);
	uint32_t x1 = pio_sm_get_blocking(pio0, SM_1);
	pio_sm_set_enabled(pio0, SM_0, false);
	pio_sm_set_enabled(pio0, SM_1, false);
	uint32_t l0 = 0xffffffffu - x0;
	uint32_t l1 = 0xffffffffu - x1;
	printf("l0 = %d, l1 = %d\n", l0, l1);
	return l0 * 2;
}

void measurement_print(int loops)
{
	printf("Measurement = %d ns\n", loops * 8);
}
