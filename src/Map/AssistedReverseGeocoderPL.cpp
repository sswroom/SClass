#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "Map/AssistedReverseGeocoderPL.h"
#include "Math/Math.h"
#include "Manage/HiResClock.h"
#include "Sync/MutexUsage.h"
#include <stdio.h>

Map::AssistedReverseGeocoderPL::AddressComparator::~AddressComparator()
{
}

OSInt Map::AssistedReverseGeocoderPL::AddressComparator::Compare(AddressEntry *a, AddressEntry *b) const
{
	if (a->keyx > b->keyx)
	{
		return 1;
	}
	else if (a->keyx < b->keyx)
	{
		return -1;
	}
	else if (a->keyy > b->keyy)
	{
		return 1;
	}
	else if (a->keyy < b->keyy)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

Map::AssistedReverseGeocoderPL::AssistedReverseGeocoderPL(NotNullPtr<DB::DBTool> db, IO::Writer *errWriter)
{
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;

	NotNullPtr<DB::DBReader> r;
	Manage::HiResClock clk;
	Double t1 = clk.GetTimeDiff();
	if (this->conn->ExecuteReader(CSTR("select lcid, keyx, keyy, address from addrdb")).SetTo(r))// order by lcid, keyx, keyy
	{
		UInt32 lcid;
		Text::String *addr;
		LCIDInfo *lcidInfo;
		AddressEntry *entry;
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			lcid = (UInt32)r->GetInt32(0);
			entry = MemAlloc(AddressEntry, 1);
			entry->keyx = r->GetInt32(1);
			entry->keyy = r->GetInt32(2);
			sb.ClearStr();
			r->GetStr(3, sb);
			addr = this->strMap.Get(sb.ToCString());
			if (addr == 0)
			{
				NotNullPtr<Text::String> s = Text::String::New(sb.ToCString());
				addr = s.Ptr();
				this->strMap.Put(s, addr);
			}
			entry->address = addr;
			
			lcidInfo = this->lcidMap.Get(lcid);
			if (lcidInfo == 0)
			{
				NEW_CLASS(lcidInfo, LCIDInfo());
				lcidInfo->lcid = lcid;
				this->lcidMap.Put(lcid, lcidInfo);
			}
			lcidInfo->mainList.Add(entry);
		}
		this->conn->CloseReader(r);

		AddressComparator comparator;
		UOSInt i = this->lcidMap.GetCount();
		while (i-- > 0)
		{
			Data::Sort::ArtificialQuickSort::Sort<AddressEntry*>(&this->lcidMap.GetItem(i)->mainList, comparator);
		}
	}
	printf("Time used = %lf, t1 = %lf\r\n", clk.GetTimeDiff(), t1);
}

Map::AssistedReverseGeocoderPL::~AssistedReverseGeocoderPL()
{
	UOSInt i = this->revGeos.GetCount();
	UOSInt j;
	LCIDInfo *lcid;
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = this->revGeos.RemoveAt(i);
		DEL_CLASS(revGeo);
	}
	this->conn.Delete();
	Text::String **strArr = this->strMap.ToArray(&j);
	i = 0;
	while (i < j)
	{
		strArr[i]->Release();
		i++;
	}
	MemFree(strArr);
	i = this->lcidMap.GetCount();
	while (i-- > 0)
	{
		lcid = this->lcidMap.GetItem(i);
		j = lcid->mainList.GetCount();
		while (j-- > 0)
		{
			MemFree(lcid->mainList.GetItem(j));
		}
		DEL_CLASS(lcid);
	}
}

UTF8Char *Map::AssistedReverseGeocoderPL::SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UTF8Char *sptr = 0;
	Int32 keyx = Double2Int32(pos.GetLon() * 5000);
	Int32 keyy = Double2Int32(pos.GetLat() * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	Text::String *addr;
	LCIDInfo *lcidInfo;
	AddressEntry *entry;
	OSInt index;

	Sync::MutexUsage mutUsage(this->mut);
	lcidInfo = this->lcidMap.Get(lcid);
	if (lcidInfo == 0)
	{
		NEW_CLASS(lcidInfo, LCIDInfo());
		lcidInfo->lcid = lcid;
		this->lcidMap.Put(lcid, lcidInfo);
	}
	index = AddressIndexOf(&lcidInfo->mainList, keyx, keyy);
	if (index >= 0)
	{
		entry = lcidInfo->mainList.GetItem((UOSInt)index);
		mutUsage.EndUse();
		return entry->address->ConcatToS(buff, buffSize);
	}

	UOSInt i = this->revGeos.GetCount();
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
		Data::Timestamp ts = Data::Timestamp::UtcNow();
		DB::SQLBuilder sql(this->conn);
		sql.AppendCmdC(CSTR("insert into addrdb (lcid, keyx, keyy, address, addrTime) values ("));
		sql.AppendInt32((Int32)lcid);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32(keyx);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32(keyy);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendStrUTF8(buff);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendTS(ts);
		sql.AppendCmdC(CSTR(")"));
		if (this->conn->ExecuteNonQuery(sql.ToCString()) <= 0)
		{
			this->conn->ExecuteNonQuery(sql.ToCString());
		}

		addr = this->strMap.Get(CSTRP(buff, sptr));
		if (addr == 0)
		{
			NotNullPtr<Text::String> s = Text::String::New(buff, (UOSInt)(sptr - buff));
			addr = s.Ptr();
			this->strMap.Put(s, addr);
		}
		entry = MemAlloc(AddressEntry, 1);
		entry->keyx = keyx;
		entry->keyy = keyy;
		entry->address = addr;
		lcidInfo->mainList.Insert((UOSInt)~index, entry);
		return sptr;
	}
	else
	{
		return 0;
	}
}

UTF8Char *Map::AssistedReverseGeocoderPL::CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	return this->SearchName(buff, buffSize, pos, lcid);
}

void Map::AssistedReverseGeocoderPL::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
{
	this->revGeos.Add(revGeo);
}

OSInt Map::AssistedReverseGeocoderPL::AddressIndexOf(Data::ArrayList<AddressEntry *> *list, Int32 keyx, Int32 keyy)
{
	OSInt i = 0;
	OSInt j = (OSInt)list->GetCount() - 1;
	OSInt k;
	AddressEntry *entry;
	while (i <= j)
	{
		k = (i + j) >> 1;
		entry = list->GetItem((UOSInt)k);
		if (entry->keyx > keyx)
		{
			j = k - 1;
		}
		else if (entry->keyx < keyx)
		{
			i = k + 1;
		}
		else if (entry->keyy > keyy)
		{
			j = k - 1;
		}
		else if (entry->keyy < keyy)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return ~i;
}
