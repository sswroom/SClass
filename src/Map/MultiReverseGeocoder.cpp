#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MultiReverseGeocoder.h"
#include "Math/Math.h"

Map::MultiReverseGeocoder::MultiReverseGeocoder(IO::IWriter *errWriter)
{
	NEW_CLASS(revGeos, Data::ArrayList<Map::IReverseGeocoder*>());
	NEW_CLASS(mut, Sync::Mutex());
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::MultiReverseGeocoder::~MultiReverseGeocoder()
{
	OSInt i = revGeos->GetCount();
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = revGeos->RemoveAt(i);
		DEL_CLASS(revGeo);
	}
	DEL_CLASS(revGeos);
	DEL_CLASS(mut);
}

UTF8Char *Map::MultiReverseGeocoder::SearchName(UTF8Char *buff, Double lat, Double lon, Int32 lcid)
{
	UTF8Char *sptr = 0;
	mut->Lock();
	OSInt i = this->revGeos->GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos->GetItem(this->nextCoder)->SearchName(buff, lat, lon, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos->GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr && buff[0])
	{
		mut->Unlock();
		return sptr;
	}
	else
	{
		mut->Unlock();
		return 0;
	}
}

UTF8Char *Map::MultiReverseGeocoder::CacheName(UTF8Char *buff, Double lat, Double lon, Int32 lcid)
{
	UTF8Char *sptr = 0;
	mut->Lock();
	OSInt i = this->revGeos->GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos->GetItem(this->nextCoder)->CacheName(buff, lat, lon, lcid);
		if (sptr == 0 || buff[0] == 0)
		{
			this->nextCoder = (this->nextCoder + 1) % this->revGeos->GetCount();
		}
		else 
		{
			break;
		}
	}
	if (sptr && buff[0])
	{
		mut->Unlock();
		return sptr;
	}
	else
	{
		mut->Unlock();
		return 0;
	}
}

void Map::MultiReverseGeocoder::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
{
	this->revGeos->Add(revGeo);
}
