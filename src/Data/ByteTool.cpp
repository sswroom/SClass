#include "Stdafx.h"
#include "Data/ByteTool.h"

Int32 Data::ByteTool::GetBCD8(UInt8 bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0xf);
}

Int32 Data::ByteTool::GetBCD32(const UInt8 *bcd)
{
	return (bcd[0] >> 4) * 10000000 + (bcd[0] & 0xf) * 1000000 + (bcd[1] >> 4) * 100000 + (bcd[1] & 0xf) * 10000 + (bcd[2] >> 4) * 1000 + (bcd[2] & 0xf) * 100 + (bcd[3] >> 4) * 10 + (bcd[3] & 0xf);
}

UInt8 Data::ByteTool::Int2BCDB(Int32 val)
{
	Int32 tmp = val / 10;
	return ((tmp % 10) << 4) + (val % 10);
}
