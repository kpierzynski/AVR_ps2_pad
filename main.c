#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

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

void send_packet(uint8_t *packet, uint8_t len, uint8_t *result)
{
	PORT_SS &= ~(1 << SS);

	while (len--)
		*result++ = send(*packet++);

	PORT_SS |= (1 << SS);
}

uint8_t startup[] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t config[] = {0x01, 0x43, 0x00, 0x01, 0x00};
uint8_t end_config[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
uint8_t type[] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
uint8_t read[] = {0x01, 0x42, 0x00, 0x00, 0x00};
uint8_t mode[] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
uint8_t large[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
uint8_t answer[32];

void read_pad(ps2_pad_t *pad)
{
	uint8_t *packet = read;
	uint8_t len = 5;

	PORT_SS &= ~(1 << SS);

	while (len--)
		send(*packet++);

	uint8_t *raw_pad = (uint8_t *)pad;
	for (uint8_t i = 0; i < sizeof(ps2_pad_t); i++)
	{
		*raw_pad++ = send(0x00);
	}
	PORT_SS |= (1 << SS);
}

void print_pad(ps2_pad_t *pad)
{
	uart_putbuf((uint8_t *)pad, sizeof(ps2_pad_t), "PAD");
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

	send_packet(startup, 9, answer);
	uart_putbuf(answer, 9, "STARTUP_ANSWER");

	send_packet(config, 5, answer);
	uart_putbuf(answer, 5, "CONFIG_ANSWER");

	send_packet(type, 9, answer);
	uart_putbuf(answer, 9, "TYPE_ANSWER");

	send_packet(mode, 9, answer);
	uart_putbuf(answer, 9, "MODE_ANSWER");

	send_packet(large, 9, answer);
	uart_putbuf(answer, 9, "LARGE_ANSWER");

	send_packet(end_config, 9, answer);
	uart_putbuf(answer, 9, "END_CONFIG_ANSWER");

	while (1)
	{
		_delay_ms(1000);
		memset(answer, 0, 32);
		memset(&pad, 0, sizeof(ps2_pad_t));
		read_pad(&pad);
		print_pad(&pad);

		uart_puth(pad.rightX);
		uart_putc(' ');

		uart_puth(pad.rightY);
		uart_putc(' ');

		uart_puth(pad.leftX);
		uart_putc(' ');

		uart_puth(pad.leftY);
		uart_putc(' ');

		uart_puth(pad.r1Button);
		uart_putc(' ');

		uart_puth(pad.l1Button);
		uart_putc(' ');
	}

	return -1;
}