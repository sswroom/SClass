#include "Stdafx.h"
#include "Map/MapsforgeUtil.h"

UOSInt Map::MapsforgeUtil::ReadVBEU(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<UInt64> v)
{
	UInt64 tmpV = 0;
	UOSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = buff[ofst];
		ofst++;
		tmpV = tmpV | ((UInt64)(b & 0x7f) << sh);
		if ((b & 0x80) == 0)
			break;
		sh += 7;
	}
	v.Set(tmpV);
	return ofst;
}

UOSInt Map::MapsforgeUtil::ReadVBES(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<Int64> v)
{
	Int64 tmpV = 0;
	UOSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = buff[ofst];
		ofst++;
		if ((b & 0x80) == 0)
		{
			tmpV = tmpV | ((Int64)(b & 0x3f) << sh);
			if (b & 0x40)
				tmpV = -tmpV;
			break;
		}
		else
		{
			tmpV = tmpV | ((Int64)(b & 0x7f) << sh);
		}
		sh += 7;
	}
	v.Set(tmpV);
	return ofst;
}
