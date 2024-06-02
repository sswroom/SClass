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

OSInt Map::AssistedReverseGeocoderPL::AddressComparator::Compare(NN<AddressEntry> a, NN<AddressEntry> b) const
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

Map::AssistedReverseGeocoderPL::AssistedReverseGeocoderPL(NN<DB::DBTool> db, NN<IO::Writer> errWriter)
{
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;

	NN<DB::DBReader> r;
	Manage::HiResClock clk;
	Double t1 = clk.GetTimeDiff();
	if (this->conn->ExecuteReader(CSTR("select lcid, keyx, keyy, address from addrdb")).SetTo(r))// order by lcid, keyx, keyy
	{
		UInt32 lcid;
		Text::String *addr;
		NN<LCIDInfo> lcidInfo;
		NN<AddressEntry> entry;
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			lcid = (UInt32)r->GetInt32(0);
			entry = MemAllocNN(AddressEntry);
			entry->keyx = r->GetInt32(1);
			entry->keyy = r->GetInt32(2);
			sb.ClearStr();
			r->GetStr(3, sb);
			addr = this->strMap.Get(sb.ToCString());
			if (addr == 0)
			{
				NN<Text::String> s = Text::String::New(sb.ToCString());
				addr = s.Ptr();
				this->strMap.Put(s, addr);
			}
			entry->address = addr;
			
			if (!this->lcidMap.Get(lcid).SetTo(lcidInfo))
			{
				NEW_CLASSNN(lcidInfo, LCIDInfo());
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
			Data::Sort::ArtificialQuickSort::Sort<NN<AddressEntry>>(&this->lcidMap.GetItemNoCheck(i)->mainList, comparator);
		}
	}
	printf("Time used = %lf, t1 = %lf\r\n", clk.GetTimeDiff(), t1);
}

Map::AssistedReverseGeocoderPL::~AssistedReverseGeocoderPL()
{
	UOSInt i;
	UOSInt j;
	NN<LCIDInfo> lcid;
	this->revGeos.DeleteAll();
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
		lcid = this->lcidMap.GetItemNoCheck(i);
		lcid->mainList.MemFreeAll();
		lcid.Delete();
	}
}

UnsafeArrayOpt<UTF8Char> Map::AssistedReverseGeocoderPL::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	UnsafeArray<UTF8Char> nnsptr;
	Int32 keyx = Double2Int32(pos.GetLon() * 5000);
	Int32 keyy = Double2Int32(pos.GetLat() * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	Text::String *addr;
	NN<LCIDInfo> lcidInfo;
	NN<AddressEntry> entry;
	OSInt index;

	Sync::MutexUsage mutUsage(this->mut);
	if (!this->lcidMap.Get(lcid).SetTo(lcidInfo))
	{
		NEW_CLASSNN(lcidInfo, LCIDInfo());
		lcidInfo->lcid = lcid;
		this->lcidMap.Put(lcid, lcidInfo);
	}
	index = AddressIndexOf(lcidInfo->mainList, keyx, keyy);
	if (index >= 0)
	{
		entry = lcidInfo->mainList.GetItemNoCheck((UOSInt)index);
		mutUsage.EndUse();
		return entry->address->ConcatToS(buff, buffSize);
	}

	UOSInt i = this->revGeos.GetCount();
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
	if (sptr.SetTo(nnsptr) && buff[0])
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
		sql.AppendStrUTF8(UnsafeArray<const UTF8Char>(buff));
		sql.AppendCmdC(CSTR(", "));
		sql.AppendTS(ts);
		sql.AppendCmdC(CSTR(")"));
		if (this->conn->ExecuteNonQuery(sql.ToCString()) <= 0)
		{
			this->conn->ExecuteNonQuery(sql.ToCString());
		}

		addr = this->strMap.Get(CSTRP(buff, nnsptr));
		if (addr == 0)
		{
			NN<Text::String> s = Text::String::New(buff, (UOSInt)(nnsptr - buff));
			addr = s.Ptr();
			this->strMap.Put(s, addr);
		}
		entry = MemAllocNN(AddressEntry);
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

UnsafeArrayOpt<UTF8Char> Map::AssistedReverseGeocoderPL::CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	return this->SearchName(buff, buffSize, pos, lcid);
}

void Map::AssistedReverseGeocoderPL::AddReverseGeocoder(NN<Map::IReverseGeocoder> revGeo)
{
	this->revGeos.Add(revGeo);
}

OSInt Map::AssistedReverseGeocoderPL::AddressIndexOf(NN<Data::ArrayListNN<AddressEntry>> list, Int32 keyx, Int32 keyy)
{
	OSInt i = 0;
	OSInt j = (OSInt)list->GetCount() - 1;
	OSInt k;
	NN<AddressEntry> entry;
	while (i <= j)
	{
		k = (i + j) >> 1;
		entry = list->GetItemNoCheck((UOSInt)k);
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
