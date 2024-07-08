#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Data/RandomBytesGenerator.h"

Data::RandomBytesGenerator::RandomBytesGenerator()
{
	this->random = 0;
}

Data::RandomBytesGenerator::~RandomBytesGenerator()
{
	this->random.Delete();
}

void Data::RandomBytesGenerator::NextBytes(UnsafeArray<UInt8> buff, UOSInt len)
{
	NN<Data::RandomMT19937> random;
	if (!this->random.SetTo(random))
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		NEW_CLASSNN(random, Data::RandomMT19937((UInt32)(dt.ToTicks() & 0xffffffff)));
		this->random = random;
	}
	UInt8 tmpBuff[4];
	while (len >= 4)
	{
		WriteNInt32(&buff[0], random->NextInt32());
		len -= 4;
		buff += 4;
	}
	if (len > 0)
	{
		WriteNInt32(tmpBuff, random->NextInt32());
		while (len-- > 0)
		{
			buff[len] = tmpBuff[len];
		}
	}
}
