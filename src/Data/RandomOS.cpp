#include "Stdafx.h"
#include "Data/RandomOS.h"

#include <stdlib.h>
#ifdef _WIN32_WCE
#include <winbase.h>
#else
#include <time.h>
#endif

Data::RandomOS::RandomOS()
{
#ifdef _WIN32_WCE
	srand(GetTickCount());
#else
	srand((UInt32)time(0));
#endif
}

Data::RandomOS::~RandomOS()
{
}

Double Data::RandomOS::NextDouble()
{
	Int32 num = ((rand() << 16) | (rand() << 1)) & 0x7fffffff;
	Double ret = num / 2147483646.0;
	return ret;
}

Int32 Data::RandomOS::NextInt32()
{
	return (rand() | (rand() << 15)) ^ (rand() << 17);
}

UInt32 Data::RandomOS::NextInt15()
{
	return (UInt32)rand();
}

UInt32 Data::RandomOS::NextInt30()
{
	return (UInt32)rand() | (UInt32)(rand() << 15);
}
