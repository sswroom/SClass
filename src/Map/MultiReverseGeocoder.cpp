#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MultiReverseGeocoder.h"
#include "Math/Math_C.h"
#include "Sync/MutexUsage.h"

Map::MultiReverseGeocoder::MultiReverseGeocoder(Optional<IO::Writer> errWriter)
{
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::MultiReverseGeocoder::~MultiReverseGeocoder()
{
	NN<Map::ReverseGeocoder> revGeo;
	UIntOS i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		if (revGeos.RemoveAt(i).SetTo(revGeo))
		{
			revGeo.Delete();
		}
	}
}

UnsafeArrayOpt<UTF8Char> Map::MultiReverseGeocoder::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	Sync::MutexUsage mutUsage(this->mut);
	IntOS i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItemNoCheck(this->nextCoder)->SearchName(buff, buffSize, pos, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos.GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr.NotNull() && buff[0])
	{
		return sptr;
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> Map::MultiReverseGeocoder::CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	Sync::MutexUsage mutUsage(this->mut);
	IntOS i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItemNoCheck(this->nextCoder)->CacheName(buff, buffSize, pos, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos.GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr.NotNull() && buff[0])
	{
		return sptr;
	}
	else
	{
		return 0;
	}
}

void Map::MultiReverseGeocoder::AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo)
{
	this->revGeos.Add(revGeo);
}
