#include "Stdafx.h"
#include "Data/ByteTool.h"

extern "C" void CRC16R_InitTable(UInt16 *tab, UInt16 polynomial)
{
	UInt16 i = 256;
	UInt32 j;
	UInt16 v;
	while (i-- > 0)
	{
		v = i;
		j = 8;
		while (j-- > 0)
		{
			if (v & 1)
			{
				v = (v >> 1) ^ polynomial;
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

extern "C" UInt16 CRC16R_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal)
{
	while (buffSize >= 16)
	{
		UInt16 currVal1 = ReadUInt16(buff) ^ currVal;
		currVal  = tab[0    + buff[15]];
		currVal ^= tab[256  + buff[14]];
		currVal ^= tab[512  + buff[13]];
		currVal ^= tab[768  + buff[12]];
		currVal ^= tab[1024 + buff[11]];
		currVal ^= tab[1280 + buff[10]];
		currVal ^= tab[1536 + buff[9]];
		currVal ^= tab[1792 + buff[8]];
		currVal ^= tab[2048 + buff[7]];
		currVal ^= tab[2304 + buff[6]];
		currVal ^= tab[2560 + buff[5]];
		currVal ^= tab[2816 + buff[4]];
		currVal ^= tab[3072 + buff[3]];
		currVal ^= tab[3328 + buff[2]];
		currVal ^= tab[3584 + ((currVal1 >> 8) & 0xff)];
		currVal ^= tab[3840 +  (currVal1  & 0xff)];
		buffSize -= 16;
		buff += 16;
	}
	while (buffSize >= 2)
	{
		currVal ^= ReadUInt16(buff);
		buff += 2;
		currVal = tab[256 + (currVal & 0xff)] ^ tab[0 + (currVal >> 8)];
		buffSize -= 2;
	}
	while (buffSize--)
	{
		currVal = tab[(currVal & 0xff) ^ (*buff)] ^ (currVal >> 8);
		buff++;
	}

	return currVal;
}
