#include "spi.h"

void spi_init()
{
	DDR_SCK |= (1 << SCK);
	DDR_MOSI |= (1 << MOSI);
	DDR_MISO &= ~(1 << MISO);

	PORT_SS |= (1 << SS);
	DDR_SS |= (1 << SS);

	PORT_SCK |= (1 << SCK);
	PORT_MOSI |= (1 << MOSI);

	SPCR = (0 << SPIE) | (1 << SPE) | (1 << DORD) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);
	SPSR = (0 << SPI2X);
}

uint8_t spi(uint8_t byte)
{
	SPDR = byte;
	while (!(SPSR & (1 << SPIF)))
		;
	return SPDR;
}
