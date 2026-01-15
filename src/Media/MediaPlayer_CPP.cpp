#include "Stdafx.h"

extern "C"
{
void MediaPlayer_VideoCropImageY(UInt8 *yptr, UIntOS w, UIntOS h, UIntOS ySplit, UIntOS *crops)
{
	UInt8 *ptr;
	UInt8 *ptr2;
	UIntOS i;
	UIntOS j;
	UIntOS v;
	Bool found;
	if (crops[1] != 0)
	{
		j = crops[1];
		v = 0;
		ptr = yptr;
		found = false;
		while (v < j)
		{
			i = w;
			while (i-- > 0)
			{
				if (ptr[0] >= 24)
				{
					found = true;
					break;
				}
				ptr += ySplit;
			}
			if (found)
			{
				break;
			}
			v++;
		}
		crops[1] = v;
	}

	if (crops[3] != 0)
	{
		ptr = yptr + w * ySplit * h;
		j = crops[3];
		v = 0;
		found = false;
		while (v < j)
		{
			i = w;
			ptr -= w * ySplit;
			while (i-- > 0)
			{
				if (ptr[0] >= 24)
				{
					found = true;
					break;
				}
				ptr += ySplit;
			}
			if (found)
			{
				break;
			}
			ptr -= w * ySplit;
			v++;
		}
		crops[3] = v;
	}

	if (crops[0] != 0)
	{
		ptr = yptr;
		j = crops[0];
		v = 0;
		found = false;
		while (v < j)
		{
			i = h;
			ptr2 = ptr;
			while (i-- > 0)
			{
				if (ptr2[0] >= 24)
				{
					found = true;
					break;
				}
				ptr2 += w * ySplit;
			}
			if (found)
			{
				break;
			}
			ptr += ySplit;
			v++;
		}
		crops[0] = v;
	}

	if (crops[2] != 0)
	{
		ptr = yptr + w * ySplit - ySplit;
		j = crops[2];
		v = 0;
		found = false;
		while (v < j)
		{
			i = h;
			ptr2 = ptr;
			while (i-- > 0)
			{
				if (ptr2[0] >= 24)
				{
					found = true;
					break;
				}
				ptr2 += w * ySplit;
			}
			if (found)
			{
				break;
			}
			ptr -= ySplit;
			v++;
		}
		crops[2] = v;
	}
}
}
