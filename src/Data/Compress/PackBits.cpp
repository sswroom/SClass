#include "Stdafx.h"
#include "Data/Compress/PackBits.h"

Bool Data::Compress::PackBits::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize)
{
	UInt8 b;
	UInt8 b2;
	UOSInt writeCnt = 0;
	while (srcBuffSize-- > 0)
	{
		b = *srcBuff++;
		if (b < 128)
		{
			b++;
			if (srcBuffSize > b)
			{
				srcBuffSize -= b;
				writeCnt += b;
				while (b-- > 0)
				{
					*destBuff++ = *srcBuff++;
				}
			}
			else
			{
				writeCnt += srcBuffSize;
				while (srcBuffSize-- > 0)
				{
					*destBuff++ = *srcBuff++;
				}
				break;
			}
		}
		else if (b == 128)
		{
		}
		else
		{
			b = (UInt8)(~b + 2);
			if (srcBuffSize-- > 0)
			{
				b2 = *srcBuff++;
				writeCnt += b;
				while (b-- > 0)
				{
					*destBuff++ = b2;
				}
			}
			else
			{
				break;
			}
		}
	}
	*destBuffSize = writeCnt;
	return true;
}
