#include "Stdafx.h"

extern "C" UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal)
{
	UInt32 a = abVal & 0xffff;
	UInt32 b = abVal >> 16;
	UOSInt l = buffSize % 5552;
	UOSInt i = l >> 3;
	while (i-- > 0)
	{
		a += buff[0];
		b += a;
		a += buff[1];
		b += a;
		a += buff[2];
		b += a;
		a += buff[3];
		b += a;
		a += buff[4];
		b += a;
		a += buff[5];
		b += a;
		a += buff[6];
		b += a;
		a += buff[7];
		b += a;
		buff += 8;
	}

	i = l & 7;
	while (i-- > 0)
	{
		a = (a + *buff++);
	}
	buffSize -= l;
	a %= 65521;
	b %= 65521;
	while (buffSize > 0)
	{
		i = 5552 / 8;
		while (i-- > 0)
		{
			a += buff[0];
			b += a;
			a += buff[1];
			b += a;
			a += buff[2];
			b += a;
			a += buff[3];
			b += a;
			a += buff[4];
			b += a;
			a += buff[5];
			b += a;
			a += buff[6];
			b += a;
			a += buff[7];
			b += a;
			buff += 8;
		}
		buffSize -= 5552;
		a %= 65521;
		b %= 65521;
	}
	return (b << 16) | a;
}
