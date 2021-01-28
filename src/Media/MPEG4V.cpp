#include "stdafx.h"
#include "Media/MPEG4V.h"

Bool Media::MPEG4V::GetPAR(UInt8 *frame, Int32 frameSize, Int32 *arh, Int32 *arv)
{
	Int32 i = frameSize - 3;
	if (i > 64)
	{
		i = 64;
	}
	while (i-- > 0)
	{
		if (*(Int32*)frame == 0x20010000)
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
				*arh = pw;
				*arv = ph;
				return true;
			}
			else if (ar == 1)
			{
				*arh = 1;
				*arv = 1;
				return true;
			}
			else if (ar == 2)
			{
				*arh = 12;
				*arv = 11;
				return true;
			}
			else if (ar == 3)
			{
				*arh = 10;
				*arv = 11;
				return true;
			}
			else if (ar == 4)
			{
				*arh = 16;
				*arv = 11;
				return true;
			}
			else if (ar == 5)
			{
				*arh = 40;
				*arv = 33;
				return true;
			}
			return false;
		}
		frame++;
	}
	return false;
}
