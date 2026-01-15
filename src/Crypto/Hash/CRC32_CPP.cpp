#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void CRC32_InitTable(UInt32 *tab, UInt32 polynomial)
{
	UInt32 i = 256;
	UInt32 j;
	UInt32 v;
	while (i-- > 0)
	{
		v = i << 24;
		j = 8;
		while (j-- > 0)
		{
			if (v & 0x80000000)
			{
				v = (v << 1) ^ polynomial;
			}
			else
			{
				v = (v << 1);
			}
		}
		tab[i] = v;
	}

	i = 0;
	while (i < 256)
	{
		tab[256  + i] = (tab[0    + i] << 8) ^ tab[tab[0    + i] >> 24];
		tab[512  + i] = (tab[256  + i] << 8) ^ tab[tab[256  + i] >> 24];
		tab[768  + i] = (tab[512  + i] << 8) ^ tab[tab[512  + i] >> 24];
		tab[1024 + i] = (tab[768  + i] << 8) ^ tab[tab[768  + i] >> 24];
		tab[1280 + i] = (tab[1024 + i] << 8) ^ tab[tab[1024 + i] >> 24];
		tab[1536 + i] = (tab[1280 + i] << 8) ^ tab[tab[1280 + i] >> 24];
		tab[1792 + i] = (tab[1536 + i] << 8) ^ tab[tab[1536 + i] >> 24];
		tab[2048 + i] = (tab[1792 + i] << 8) ^ tab[tab[1792 + i] >> 24];
		tab[2304 + i] = (tab[2048 + i] << 8) ^ tab[tab[2048 + i] >> 24];
		tab[2560 + i] = (tab[2304 + i] << 8) ^ tab[tab[2304 + i] >> 24];
		tab[2816 + i] = (tab[2560 + i] << 8) ^ tab[tab[2560 + i] >> 24];
		tab[3072 + i] = (tab[2816 + i] << 8) ^ tab[tab[2816 + i] >> 24];
		tab[3328 + i] = (tab[3072 + i] << 8) ^ tab[tab[3072 + i] >> 24];
		tab[3584 + i] = (tab[3328 + i] << 8) ^ tab[tab[3328 + i] >> 24];
		tab[3840 + i] = (tab[3584 + i] << 8) ^ tab[tab[3584 + i] >> 24];
		i++;
	}
}

extern "C" UInt32 CRC32_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 *tab, UInt32 currVal)
{
	UInt32 v = currVal;

	while (buffSize >= 16)
	{
		v = tab[   0 + buff[15]] ^
			tab[ 256 + buff[14]] ^
			tab[ 512 + buff[13]] ^
			tab[ 768 + buff[12]] ^
			tab[1024 + buff[11]] ^
			tab[1280 + buff[10]] ^
			tab[1536 + buff[9]] ^
			tab[1792 + buff[8]] ^
			tab[2048 + buff[7]] ^
			tab[2304 + buff[6]] ^
			tab[2560 + buff[5]] ^
			tab[2816 + buff[4]] ^
			tab[3072 + (buff[3] ^ (v & 0xff))] ^
			tab[3328 + (buff[2] ^ ((v >> 8) & 0xff))] ^
			tab[3584 + (buff[1] ^ ((v >> 16) & 0xff))] ^
			tab[3840 + (buff[0] ^ (v >> 24))];
		buff += 16;
		buffSize -= 16;
	}
	while (buffSize--)
	{
		v = tab[(v >> 24) ^ (*buff)] ^ (v << 8);
		buff++;
	}

	return v;
}
