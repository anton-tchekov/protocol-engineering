#include "blink.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "blink.pio.h"
#include <stdio.h>

void blink_init(void)
{
	PIO pio = pio0;
	uint offset = pio_add_program(pio, &blink_program);
	printf("Loaded program at %d\n", offset);
	uint freq = 3;
	uint pin = PICO_DEFAULT_LED_PIN;
	uint sm = 0;
	pio_gpio_init(pio, pin);
	pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
	pio_sm_config c = blink_program_get_default_config(offset);
	sm_config_set_set_pins(&c, pin, 1);
	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
	printf("Blinking pin %d at %d Hz\n", pin, freq);
	pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}
