#include "Stdafx.h"

extern "C" UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal)
{
	UInt32 a = abVal & 0xffff;
	UInt32 b = abVal >> 16;
	UOSInt i = buffSize >> 3;
	while (i-- > 0)
	{
		a = (a + buff[0]);
		b = (b + a);
		a = (a + buff[1]);
		b = (b + a);
		a = (a + buff[2]);
		b = (b + a);
		a = (a + buff[3]);
		b = (b + a);
		a = (a + buff[4]);
		b = (b + a);
		a = (a + buff[5]);
		b = (b + a);
		a = (a + buff[6]);
		b = (b + a);
		a = (a + buff[7]);
		b = (b + a);
		buff += 8;

		while (a >= 65521)
			a -= 65521;
		while (b >= 65521)
			b -= 65521;
	}

	buffSize = buffSize & 7;
	while (buffSize-- > 0)
	{
		a = (a + *buff++);
		if (a >= 65521)
			a -= 65521;
		b = (b + a);
		if (b >= 65521)
			b -= 65521;
	}
	return (b << 16) | a;
}
