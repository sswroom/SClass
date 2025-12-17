#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Media/MPEG4V.h"

Bool Media::MPEG4V::GetPAR(UnsafeArray<UInt8> frame, Int32 frameSize, OutParam<Int32> arh, OutParam<Int32> arv)
{
	Int32 i = frameSize - 3;
	if (i > 64)
	{
		i = 64;
	}
	while (i-- > 0)
	{
		if (ReadInt32(&frame[0]) == 0x20010000)
		{
			Int32 pw;
			Int32 ph;
			Int32 ar;

			if (frame[5] & 0x40)
			{
				ar = (frame[6] & 0x78) >> 3;
				pw = ((frame[6] & 0x7) << 5) | ((frame[7] & 0xf8) >> 3);
				ph = ((frame[7] & 0x7) << 5) | ((frame[8] & 0xf8) >> 3);
			}
			else
			{
				ar = (frame[5] & 0x3c) >> 2;
				pw = ((frame[5] & 0x3) << 5) | ((frame[6] & 0xfc) >> 3);
				ph = ((frame[6] & 0x3) << 5) | ((frame[7] & 0xfc) >> 3);
			}

			if (ar == 0xf)
			{
				arh.Set(pw);
				arv.Set(ph);
				return true;
			}
			else if (ar == 1)
			{
				arh.Set(1);
				arv.Set(1);
				return true;
			}
			else if (ar == 2)
			{
				arh.Set(12);
				arv.Set(11);
				return true;
			}
			else if (ar == 3)
			{
				arh.Set(10);
				arv.Set(11);
				return true;
			}
			else if (ar == 4)
			{
				arh.Set(16);
				arv.Set(11);
				return true;
			}
			else if (ar == 5)
			{
				arh.Set(40);
				arv.Set(33);
				return true;
			}
			return false;
		}
		frame++;
	}
	return false;
}
