#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomMT19937.h"

Data::RandomMT19937::RandomMT19937(UInt32 seed)
{
	this->mt = MemAllocArr(UInt32, 624);
	this->mt_index = 624;

    mt[0] = seed;
	UOSInt i = 1;
	while (i < 624)
	{
		this->mt[i] = (UInt32)((1812433253 * (this->mt[i - 1] ^ (this->mt[i - 1] >> 30)) + i) & 0xffffffff);
		i++;
    }
}

Data::RandomMT19937::~RandomMT19937()
{
	MemFreeArr(this->mt);
}

Double Data::RandomMT19937::NextDouble()
{
	return (this->NextInt32() & 0x7fffffff) / 2147483647.0;
}

Int32 Data::RandomMT19937::NextInt32()
{
	if (this->mt_index >= 624)
	{
		OSInt i = 0;
		while (i < 624)
		{
			UInt32 y = (this->mt[i] & 0x80000000) + (this->mt[(i + 1) % 624] & 0x7fffffff);
			this->mt[i] = this->mt[(i + 397) % 624] ^ (y >> 1);
			if (y % 2 != 0)
			{
				this->mt[i] ^= 0x9908b0df;
			}
			i++;
		}
		this->mt_index = 0;
	}
   
    UInt32 y = this->mt[this->mt_index];
    y ^= y >> 11;
    y ^= (y << 7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= y >> 18;
   
    this->mt_index++;
    return (Int32)y;
}
