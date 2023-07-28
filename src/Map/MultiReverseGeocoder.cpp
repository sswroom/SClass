#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MultiReverseGeocoder.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"

Map::MultiReverseGeocoder::MultiReverseGeocoder(IO::Writer *errWriter)
{
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::MultiReverseGeocoder::~MultiReverseGeocoder()
{
	UOSInt i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = revGeos.RemoveAt(i);
		DEL_CLASS(revGeo);
	}
}

UTF8Char *Map::MultiReverseGeocoder::SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UTF8Char *sptr = 0;
	Sync::MutexUsage mutUsage(this->mut);
	OSInt i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItem(this->nextCoder)->SearchName(buff, buffSize, pos, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos.GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr && buff[0])
	{
		return sptr;
	}
	else
	{
		return 0;
	}
}

UTF8Char *Map::MultiReverseGeocoder::CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UTF8Char *sptr = 0;
	Sync::MutexUsage mutUsage(this->mut);
	OSInt i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItem(this->nextCoder)->CacheName(buff, buffSize, pos, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos.GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr && buff[0])
	{
		return sptr;
	}
	else
	{
		return 0;
	}
}

void Map::MultiReverseGeocoder::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
{
	this->revGeos.Add(revGeo);
}
