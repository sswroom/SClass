#include "Stdafx.h"

extern "C" UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal)
{
	UInt32 a = abVal & 0xffff;
	UInt32 b = abVal >> 16;
	UOSInt i = buffSize >> 3;
	while (i-- > 0)
	{
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);
		a = (a + *buff++);
		b = (b + a);

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
