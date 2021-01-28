#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "Map/AssistedReverseGeocoderPL.h"
#include "Math/Math.h"
#include "Manage/HiResClock.h"
#include <wchar.h>

Map::AssistedReverseGeocoderPL::AssistedReverseGeocoderPL(DB::DBTool *db, IO::IWriter *errWriter)
{
	NEW_CLASS(this->revGeos, Data::ArrayList<Map::IReverseGeocoder*>());
	NEW_CLASS(this->strMap, Data::BTreeUTF8Map<const UTF8Char *>());
	NEW_CLASS(this->lcidMap, Data::Integer32Map<LCIDInfo*>());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;

	if (this->conn)
	{
		DB::DBReader *r;
		Manage::HiResClock clk;
		r = this->conn->ExecuteReader((const UTF8Char*)"select lcid, keyx, keyy, address from addrdb"); // order by lcid, keyx, keyy
		Double t1 = clk.GetTimeDiff();
		if (r)
		{
			Int32 lcid;
			Int32 keyx;
			Int32 keyy;
			Text::StringBuilderUTF8 sb;
			const UTF8Char *addr;
			LCIDInfo *lcidInfo;
			IndexInfo *indexInfo;
			Data::Integer32Map<const UTF8Char *> *addrMap1;
			Int32 index;
			while (r->ReadNext())
			{
				lcid = r->GetInt32(0);
				keyx = r->GetInt32(1);
				keyy = r->GetInt32(2);
				sb.ClearStr();
				r->GetStr(3, &sb);
				addr = this->strMap->Get(sb.ToString());
				if (addr == 0)
				{
					addr = Text::StrCopyNew(sb.ToString());
					this->strMap->Put(sb.ToString(), addr);
				}
				lcidInfo = this->lcidMap->Get(lcid);
				if (lcidInfo == 0)
				{
					lcidInfo = MemAlloc(LCIDInfo, 1);
					lcidInfo->lcid = lcid;
					NEW_CLASS(lcidInfo->indexMap, Data::Integer32Map<IndexInfo*>());
					this->lcidMap->Put(lcid, lcidInfo);
				}
				index = ((keyx & 255) << 8) | (keyy & 255);
				indexInfo = lcidInfo->indexMap->Get(index);
				if (indexInfo == 0)
				{
					indexInfo = MemAlloc(IndexInfo, 1);
					NEW_CLASS(indexInfo->addrMap, Data::Integer32Map<Data::Integer32Map<const UTF8Char *>*>());
					lcidInfo->indexMap->Put(index, indexInfo);
				}
				addrMap1 = indexInfo->addrMap->Get(keyx);
				if (addrMap1 == 0)
				{
					NEW_CLASS(addrMap1, Data::Integer32Map<const UTF8Char *>());
					indexInfo->addrMap->Put(keyx, addrMap1);
				}
				addrMap1->Put(keyy, addr);
			}
			this->conn->CloseReader(r);
		}
		wprintf(L"Time used = %lf, t1 = %lf\r\n", clk.GetTimeDiff(), t1);
	}
}

Map::AssistedReverseGeocoderPL::~AssistedReverseGeocoderPL()
{
	UOSInt i = revGeos->GetCount();
	UOSInt j;
	UOSInt k;
	LCIDInfo *lcid;
	Data::ArrayList<LCIDInfo *> *lcidList;
	IndexInfo *index;
	Data::ArrayList<IndexInfo *> *indexList;
	Data::ArrayList<Data::Integer32Map<const UTF8Char *>*> *addrList1;
	Data::Integer32Map<const UTF8Char *> *addrMap1;
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
	const UTF8Char **sarr = this->strMap->ToArray(&j);
	i = 0;
	while (i < j)
	{
		Text::StrDelNew(sarr[i]);
		i++;
	}
	MemFree(sarr);
	lcidList = lcidMap->GetValues();
	i = lcidList->GetCount();
	while (i-- > 0)
	{
		lcid = lcidList->GetItem(i);
		indexList = lcid->indexMap->GetValues();
		j = indexList->GetCount();
		while (j-- > 0)
		{
			index = indexList->GetItem(j);
			addrList1 = index->addrMap->GetValues();
			k = addrList1->GetCount();
			while (k-- > 0)
			{
				addrMap1 = addrList1->GetItem(k);
				DEL_CLASS(addrMap1);
			}
			DEL_CLASS(index->addrMap);
			MemFree(index);
		}
		DEL_CLASS(lcid->indexMap);
		MemFree(lcid);
	}
	DEL_CLASS(this->lcidMap);
	DEL_CLASS(this->strMap);

	DEL_CLASS(mut);
}

UTF8Char *Map::AssistedReverseGeocoderPL::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	UTF8Char *sptr = 0;
	if (this->conn == 0)
		return 0;

	Int32 keyx = Math::Double2Int32(lon * 5000);
	Int32 keyy = Math::Double2Int32(lat * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	const UTF8Char *addr;
	LCIDInfo *lcidInfo;
	IndexInfo *indexInfo;
	Data::Integer32Map<const UTF8Char *> *addrMap1;
	Int32 index;

	this->mut->Lock();
	lcidInfo = this->lcidMap->Get(lcid);
	if (lcidInfo == 0)
	{
		lcidInfo = MemAlloc(LCIDInfo, 1);
		lcidInfo->lcid = lcid;
		NEW_CLASS(lcidInfo->indexMap, Data::Integer32Map<IndexInfo*>());
		this->lcidMap->Put(lcid, lcidInfo);
	}
	index = ((keyx & 255) << 8) | (keyy & 255);
	indexInfo = lcidInfo->indexMap->Get(index);
	if (indexInfo)
	{
		addrMap1 = indexInfo->addrMap->Get(keyx);
		if (addrMap1)
		{
			addr = addrMap1->Get(keyy);
			if (addr)
			{
				mut->Unlock();
				return Text::StrConcatS(buff, addr, buffSize);
			}
		}
	}

	OSInt i = this->revGeos->GetCount();
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
		NEW_CLASS(sql, DB::SQLBuilder(this->conn->GetSvrType()));
		sql->AppendCmd((const UTF8Char*)"insert into addrdb (lcid, keyx, keyy, address, addrTime) values (");
		sql->AppendInt32(lcid);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendInt32(keyx);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendInt32(keyy);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendStrUTF8(buff);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendDate(&dt);
		sql->AppendCmd((const UTF8Char*)")");
		if (this->conn->ExecuteNonQuery(sql->ToString()) <= 0)
		{
			this->conn->ExecuteNonQuery(sql->ToString());
		}
		DEL_CLASS(sql);

		addr = this->strMap->Get(buff);
		if (addr == 0)
		{
			addr = Text::StrCopyNew(buff);
			this->strMap->Put(buff, addr);
		}
		indexInfo = lcidInfo->indexMap->Get(index);
		if (indexInfo == 0)
		{
			indexInfo = MemAlloc(IndexInfo, 1);
			NEW_CLASS(indexInfo->addrMap, Data::Integer32Map<Data::Integer32Map<const UTF8Char *>*>());
			lcidInfo->indexMap->Put(index, indexInfo);
		}
		addrMap1 = indexInfo->addrMap->Get(keyx);
		if (addrMap1 == 0)
		{
			NEW_CLASS(addrMap1, Data::Integer32Map<const UTF8Char *>());
			indexInfo->addrMap->Put(keyx, addrMap1);
		}
		addrMap1->Put(keyy, addr);

		mut->Unlock();
		return sptr;
	}
	else
	{
		mut->Unlock();
		return 0;
	}
}

UTF8Char *Map::AssistedReverseGeocoderPL::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	UTF8Char *sptr = 0;
	if (this->conn == 0)
		return 0;

	Int32 keyx = Math::Double2Int32(lon * 5000);
	Int32 keyy = Math::Double2Int32(lat * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	const UTF8Char *addr;
	LCIDInfo *lcidInfo;
	IndexInfo *indexInfo;
	Data::Integer32Map<const UTF8Char *> *addrMap1;
	Int32 index;

	mut->Lock();
	lcidInfo = this->lcidMap->Get(lcid);
	if (lcidInfo == 0)
	{
		lcidInfo = MemAlloc(LCIDInfo, 1);
		lcidInfo->lcid = lcid;
		NEW_CLASS(lcidInfo->indexMap, Data::Integer32Map<IndexInfo*>());
		this->lcidMap->Put(lcid, lcidInfo);
	}
	index = ((keyx & 255) << 8) | (keyy & 255);
	indexInfo = lcidInfo->indexMap->Get(index);
	if (indexInfo)
	{
		addrMap1 = indexInfo->addrMap->Get(keyx);
		if (addrMap1)
		{
			addr = addrMap1->Get(keyy);
			if (addr)
			{
				mut->Unlock();
				return Text::StrConcatS(buff, addr, buffSize);
			}
		}
	}

	OSInt i = this->revGeos->GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos->GetItem(this->nextCoder)->CacheName(buff, buffSize, lat, lon, lcid);
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
		NEW_CLASS(sql, DB::SQLBuilder(this->conn->GetSvrType()));
		sql->AppendCmd((const UTF8Char*)"insert into addrdb (lcid, keyx, keyy, address, addrTime) values (");
		sql->AppendInt32(lcid);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendInt32(keyx);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendInt32(keyy);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendStrUTF8(buff);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendDate(&dt);
		sql->AppendCmd((const UTF8Char*)")");
		if (this->conn->ExecuteNonQuery(sql->ToString()) <= 0)
		{
			this->conn->ExecuteNonQuery(sql->ToString());
		}
		DEL_CLASS(sql);

		addr = this->strMap->Get(buff);
		if (addr == 0)
		{
			addr = Text::StrCopyNew(buff);
			this->strMap->Put(buff, addr);
		}
		indexInfo = lcidInfo->indexMap->Get(index);
		if (indexInfo == 0)
		{
			indexInfo = MemAlloc(IndexInfo, 1);
			NEW_CLASS(indexInfo->addrMap, Data::Integer32Map<Data::Integer32Map<const UTF8Char *>*>());
			lcidInfo->indexMap->Put(index, indexInfo);
		}
		addrMap1 = indexInfo->addrMap->Get(keyx);
		if (addrMap1 == 0)
		{
			NEW_CLASS(addrMap1, Data::Integer32Map<const UTF8Char *>());
			indexInfo->addrMap->Put(keyx, addrMap1);
		}
		addrMap1->Put(keyy, addr);

		mut->Unlock();
		return sptr;
	}
	else
	{
		mut->Unlock();
		return 0;
	}
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
