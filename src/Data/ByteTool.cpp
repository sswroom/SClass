#include "Stdafx.h"
#include "Data/ByteTool.h"

UInt8 Data::ByteTool::GetBCD8(UInt8 bcd)
{
	return (UInt8)((bcd >> 4) * 10 + (bcd & 0xf));
}

UInt32 Data::ByteTool::GetBCD32(const UInt8 *bcd)
{
	return (UInt32)((bcd[0] >> 4) * 10000000 + (bcd[0] & 0xf) * 1000000 + (bcd[1] >> 4) * 100000 + (bcd[1] & 0xf) * 10000 + (bcd[2] >> 4) * 1000 + (bcd[2] & 0xf) * 100 + (bcd[3] >> 4) * 10 + (bcd[3] & 0xf));
}

UInt8 Data::ByteTool::Int2BCDB(UInt32 val)
{
	UInt32 tmp = val / 10;
	return (UInt8)(((tmp % 10) << 4) + (val % 10));
}
