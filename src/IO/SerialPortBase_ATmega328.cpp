#include "Stdafx.h"
#include "IO/SerialPort.h"
#include <avr/io.h>

extern "C" void SerialPort_Init(Int32 baudRate, IO::SerialPort::ParityType parity)
{
	UInt16 setting = (UInt16)(((F_CPU / 4 / baudRate) - 1) >> 1);
	if (((F_CPU == 16000000UL) && (baudRate == 57600)) || (setting > 4095))
	{
		setting = (UInt16)(((F_CPU / 8 / baudRate) - 1) >> 1);
		UCSR0A = 0;
	}
	else
	{
		UCSR0A = 1 << U2X0;
	}
	UBRR0H = (UInt8)(setting >> 8);
	UBRR0L = (UInt8)(setting & 0xff);

	if (parity == IO::SerialPort::PARITY_ODD)
	{
		UCSR0C = (1 << UPM01) | (1 << UPM00) | (1 << UCSZ01) | (1 << UCSZ00);
	}
	else if (parity == IO::SerialPort::PARITY_EVEN)
	{
		UCSR0C = (1 << UPM01) | (1 << UCSZ01) | (1 << UCSZ00);
	}
	else
	{
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	}
	UCSR0B = (UCSR0B & ~((1 << RXCIE0) | (1 << TXCIE0) | (1 << UDRIE0))) | (1 << RXEN0) | (1 << TXEN0);
}	

extern "C" void SerialPort_Write(UInt8 b)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = b;
	UCSR0A |= (1 << TXC0);
}

extern "C" void SerialPort_Flush()
{
	while ((UCSR0A & (1 << TXC0)) == 0);
}

extern "C" Bool SerialPort_Available()
{
	return (UCSR0A & (1 << RXC0)) != 0;
}

extern "C" UInt8 SerialPort_Read()
{
	while (!(UCSR0A & (1<<RXC0)));

	UInt8 status = UCSR0A;
	UInt8 data = UDR0;
	/* If error, return -1 */
	if (status & ((1<<FE0) | (1<<DOR0) | (1<<UPE0)))
		return data;//-1;
	return data;
}
