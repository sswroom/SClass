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

OSInt Map::AssistedReverseGeocoderPL::AddressComparator::Compare(AddressEntry *a, AddressEntry *b)
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

Map::AssistedReverseGeocoderPL::AssistedReverseGeocoderPL(DB::DBTool *db, IO::Writer *errWriter)
{
	NEW_CLASS(this->revGeos, Data::ArrayList<Map::IReverseGeocoder*>());
	NEW_CLASS(this->strMap, Data::BTreeMap<Text::String *>());
	NEW_CLASS(this->lcidMap, Data::UInt32Map<LCIDInfo*>());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;

	if (this->conn)
	{
		DB::DBReader *r;
		Manage::HiResClock clk;
		r = this->conn->ExecuteReader(CSTR("select lcid, keyx, keyy, address from addrdb")); // order by lcid, keyx, keyy
		Double t1 = clk.GetTimeDiff();
		if (r)
		{
			UInt32 lcid;
			Text::String *addr;
			Text::String *s;
			LCIDInfo *lcidInfo;
			AddressEntry *entry;
			while (r->ReadNext())
			{
				lcid = (UInt32)r->GetInt32(0);
				entry = MemAlloc(AddressEntry, 1);
				entry->keyx = r->GetInt32(1);
				entry->keyy = r->GetInt32(2);
				s = r->GetNewStr(3);
				addr = this->strMap->Get(s);
				if (addr == 0)
				{
					addr = s->Clone();
					this->strMap->Put(s, addr);
				}
				entry->address = addr;
				s->Release();
				
				lcidInfo = this->lcidMap->Get(lcid);
				if (lcidInfo == 0)
				{
					lcidInfo = MemAlloc(LCIDInfo, 1);
					lcidInfo->lcid = lcid;
					NEW_CLASS(lcidInfo->mainList, Data::ArrayList<AddressEntry*>());
					this->lcidMap->Put(lcid, lcidInfo);
				}
				lcidInfo->mainList->Add(entry);
			}
			this->conn->CloseReader(r);

			AddressComparator comparator;
			Data::ArrayList<LCIDInfo*> *lcidList = this->lcidMap->GetValues();
			UOSInt i = lcidList->GetCount();
			while (i-- > 0)
			{
				Data::Sort::ArtificialQuickSort::Sort(lcidList->GetItem(i)->mainList, &comparator);
			}
		}
		printf("Time used = %lf, t1 = %lf\r\n", clk.GetTimeDiff(), t1);
	}
}

Map::AssistedReverseGeocoderPL::~AssistedReverseGeocoderPL()
{
	UOSInt i = revGeos->GetCount();
	UOSInt j;
	LCIDInfo *lcid;
	Data::ArrayList<LCIDInfo *> *lcidList;
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = this->revGeos->RemoveAt(i);
		DEL_CLASS(revGeo);
	}
	DEL_CLASS(this->revGeos);
	if (this->conn)
	{
		DEL_CLASS(this->conn);
	}
	Text::String **strArr = this->strMap->ToArray(&j);
	i = 0;
	while (i < j)
	{
		strArr[i]->Release();
		i++;
	}
	MemFree(strArr);
	lcidList = lcidMap->GetValues();
	i = lcidList->GetCount();
	while (i-- > 0)
	{
		lcid = lcidList->GetItem(i);
		j = lcid->mainList->GetCount();
		while (j-- > 0)
		{
			MemFree(lcid->mainList->GetItem(j));
		}
		DEL_CLASS(lcid->mainList);
		MemFree(lcid);
	}
	DEL_CLASS(this->lcidMap);
	DEL_CLASS(this->strMap);

	DEL_CLASS(mut);
}

UTF8Char *Map::AssistedReverseGeocoderPL::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
{
	UTF8Char *sptr = 0;
	if (this->conn == 0)
		return 0;

	Int32 keyx = Double2Int32(lon * 5000);
	Int32 keyy = Double2Int32(lat * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	Text::String *addr;
	LCIDInfo *lcidInfo;
	AddressEntry *entry;
	OSInt index;

	Sync::MutexUsage mutUsage(this->mut);
	lcidInfo = this->lcidMap->Get(lcid);
	if (lcidInfo == 0)
	{
		lcidInfo = MemAlloc(LCIDInfo, 1);
		lcidInfo->lcid = lcid;
		NEW_CLASS(lcidInfo->mainList, Data::ArrayList<AddressEntry*>());
		this->lcidMap->Put(lcid, lcidInfo);
	}
	index = AddressIndexOf(lcidInfo->mainList, keyx, keyy);
	if (index >= 0)
	{
		entry = lcidInfo->mainList->GetItem((UOSInt)index);
		mutUsage.EndUse();
		return Text::StrConcatS(buff, entry->address->v, buffSize);
	}

	UOSInt i = this->revGeos->GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos->GetItem(this->nextCoder)->SearchName(buff, buffSize, lat, lon, lcid);
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
		Data::DateTime dt;
		DB::SQLBuilder *sql;
		dt.SetCurrTimeUTC();
		NEW_CLASS(sql, DB::SQLBuilder(this->conn));
		sql->AppendCmdC(CSTR("insert into addrdb (lcid, keyx, keyy, address, addrTime) values ("));
		sql->AppendInt32((Int32)lcid);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(keyx);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(keyy);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendStrUTF8(buff);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendDate(&dt);
		sql->AppendCmdC(CSTR(")"));
		if (this->conn->ExecuteNonQuery(sql->ToCString()) <= 0)
		{
			this->conn->ExecuteNonQuery(sql->ToCString());
		}
		DEL_CLASS(sql);

		addr = this->strMap->Get(buff);
		if (addr == 0)
		{
			addr = Text::String::New(buff, (UOSInt)(sptr - buff));
			this->strMap->Put(addr, addr);
		}
		entry = MemAlloc(AddressEntry, 1);
		entry->keyx = keyx;
		entry->keyy = keyy;
		entry->address = addr;
		lcidInfo->mainList->Insert((UOSInt)~index, entry);
		mutUsage.EndUse();
		return sptr;
	}
	else
	{
		mutUsage.EndUse();
		return 0;
	}
}

UTF8Char *Map::AssistedReverseGeocoderPL::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
{
	return this->SearchName(buff, buffSize, lat, lon, lcid);
}

void Map::AssistedReverseGeocoderPL::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
{
	if (this->conn)
	{
		this->revGeos->Add(revGeo);
	}
	else
	{
		DEL_CLASS(revGeo);
	}
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
