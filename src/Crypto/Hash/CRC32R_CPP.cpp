#include "Stdafx.h"
#include "Data/ByteTool.h"

extern "C" void CRC32R_InitTable(UInt32 *tab, UInt32 rpn)
{
	UInt32 i = 256;
	UInt32 j;
	UInt32 v;
	while (i-- > 0)
	{
		v = i;
		j = 8;
		while (j-- > 0)
		{
			if (v & 1)
			{
				v = (v >> 1) ^ rpn;
			}
			else
			{
				v = (v >> 1);
			}
		}
		tab[i] = v;
	}

	i = 256;
	while (i-- > 0)
	{
		tab[256  + i] = (tab[0    + i] >> 8) ^ tab[tab[0    + i] & 0xff];
		tab[512  + i] = (tab[256  + i] >> 8) ^ tab[tab[256  + i] & 0xff];
		tab[768  + i] = (tab[512  + i] >> 8) ^ tab[tab[512  + i] & 0xff];
		tab[1024 + i] = (tab[768  + i] >> 8) ^ tab[tab[768  + i] & 0xff];
		tab[1280 + i] = (tab[1024 + i] >> 8) ^ tab[tab[1024 + i] & 0xff];
		tab[1536 + i] = (tab[1280 + i] >> 8) ^ tab[tab[1280 + i] & 0xff];
		tab[1792 + i] = (tab[1536 + i] >> 8) ^ tab[tab[1536 + i] & 0xff];
		tab[2048 + i] = (tab[1792 + i] >> 8) ^ tab[tab[1792 + i] & 0xff];
		tab[2304 + i] = (tab[2048 + i] >> 8) ^ tab[tab[2048 + i] & 0xff];
		tab[2560 + i] = (tab[2304 + i] >> 8) ^ tab[tab[2304 + i] & 0xff];
		tab[2816 + i] = (tab[2560 + i] >> 8) ^ tab[tab[2560 + i] & 0xff];
		tab[3072 + i] = (tab[2816 + i] >> 8) ^ tab[tab[2816 + i] & 0xff];
		tab[3328 + i] = (tab[3072 + i] >> 8) ^ tab[tab[3072 + i] & 0xff];
		tab[3584 + i] = (tab[3328 + i] >> 8) ^ tab[tab[3328 + i] & 0xff];
		tab[3840 + i] = (tab[3584 + i] >> 8) ^ tab[tab[3584 + i] & 0xff];
	}
}

extern "C" UInt32 CRC32R_Reverse(UInt32 polynomial)
{
	UInt32 v;
	UInt32 v2;
	OSInt i = 32;
	v = polynomial;
	v2 = 0;
	while (i-- > 0)
	{
		v2 = (v2 >> 1) | (v & 0x80000000);
		v <<= 1;
	}
	return v2;
}

extern "C" UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal)
{
	while (buffSize >= 16)
	{
		UInt32 currVal1 = ReadUInt32(buff) ^ currVal;
		UInt32 currVal2 = ReadUInt32(buff + 4);
		UInt32 currVal3 = ReadUInt32(buff + 8);
		UInt32 currVal4 = ReadUInt32(buff + 12);
		buff += 16;
		currVal  = tab[0    +  (currVal4 >> 24)];
		currVal ^= tab[256  + ((currVal4 >> 16) & 0xff)];
		currVal ^= tab[512  + ((currVal4 >> 8) & 0xff)];
		currVal ^= tab[768  +  (currVal4 & 0xff)];
		currVal ^= tab[1024 +  (currVal3 >> 24)];
		currVal ^= tab[1280 + ((currVal3 >> 16) & 0xff)];
		currVal ^= tab[1536 + ((currVal3 >> 8) & 0xff)];
		currVal ^= tab[1792 +  (currVal3 & 0xff)];
		currVal ^= tab[2048 +  (currVal2 >> 24)];
		currVal ^= tab[2304 + ((currVal2 >> 16) & 0xff)];
		currVal ^= tab[2560 + ((currVal2 >> 8) & 0xff)];
		currVal ^= tab[2816 +  (currVal2 & 0xff)];
		currVal ^= tab[3072 +  (currVal1 >> 24)];
		currVal ^= tab[3328 + ((currVal1 >> 16) & 0xff)];
		currVal ^= tab[3584 + ((currVal1 >> 8) & 0xff)];
		currVal ^= tab[3840 +  (currVal1  & 0xff)];
		buffSize -= 16;
	}
	while (buffSize >= 4)
	{
		currVal ^= ReadUInt32(buff);
		buff += 4;
		currVal = tab[768 + (currVal & 0xff)] ^ tab[512 + ((currVal >> 8) & 0xff)] ^  tab[256 + ((currVal >> 16) & 0xff)] ^ tab[0 + (currVal >> 24)];
		buffSize -= 4;
	}
	while (buffSize-- > 0)
	{
		currVal = tab[((UInt8)(currVal & 0xff)) ^ (*buff)] ^ (currVal >> 8);
		buff++;
	}
	return currVal;
}
