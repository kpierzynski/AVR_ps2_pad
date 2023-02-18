#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi.h"
#include "uart.h"

typedef struct
{
	uint8_t rightX;
	uint8_t rightY;

	uint8_t leftX;
	uint8_t leftY;

	uint8_t rightButton;
	uint8_t leftButton;
	uint8_t upButton;
	uint8_t downButton;

	uint8_t triangleButton;
	uint8_t circleButton;
	uint8_t xButton;
	uint8_t squareButton;

	uint8_t l1Button;
	uint8_t r1Button;
	uint8_t l2Button;
	uint8_t r2Button;
} ps2_pad_t;

uint8_t send(uint8_t byte)
{
	register uint8_t value;

	value = spi(byte);

	return value;
}

int main()
{
	_delay_ms(1000);
	spi_init();
	uart_init();
	sei();
	uart_puts("starting...\r\n");

	_delay_ms(1000);
	ps2_pad_t pad;
	PORT_SS &= ~(1 << SS);
	uint8_t header_1 = send(0x01);
	uint8_t header_2 = send(0x42);
	uint8_t header_3 = send(0x00);

	if (!(header_1 == 0xFF && header_2 == 0x79 && header_3 == 0x5A))
	{
		// not ok
	}

	uint8_t buttons_1 = send(0x00);
	uint8_t buttons_2 = send(0x00);

	for (uint8_t i = 0; i < sizeof(ps2_pad_t); i++)
	{
		*((uint8_t *)(&pad) + i) = send(0x00);
	}
	PORT_SS |= (1 << SS);

	uart_puth(header_1);
	uart_putc(' ');
	uart_puth(header_2);
	uart_putc(' ');
	uart_puth(header_3);
	uart_putc(' ');
	uart_puth(buttons_1);
	uart_putc(' ');
	uart_puth(buttons_2);
	uart_putc(' ');

	for (uint8_t i = 0; i < sizeof(ps2_pad_t); i++)
	{
		uart_puth(*((uint8_t *)(&pad) + i));
		uart_putc(' ');
	}

	while (1)
	{
	}

	return -1;
}