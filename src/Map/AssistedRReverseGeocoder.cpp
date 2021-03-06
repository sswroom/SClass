#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/AssistedRReverseGeocoder.h"
#include "Math/Math.h"

OSInt Map::AssistedRReverseGeocoder::GetLangIndex(Int32 lcid)
{
	OSInt i = 0;
	OSInt j = this->langMaps->GetCount() - 1;
	OSInt k;
	LangMap *langMap;
	while (i <= j)
	{
		k = (i + j) >> 1;
		langMap = this->langMaps->GetItem(k);
		if (langMap->lcid > lcid)
		{
			j = k - 1;
		}
		else if (langMap->lcid < lcid)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}

Map::AssistedRReverseGeocoder::AssistedRReverseGeocoder(const WChar *dsn, const WChar *uid, const WChar *pwd, IO::LogTool *log, IO::Writer *errWriter)
{
	NEW_CLASS(revGeos, Data::ArrayList<Map::IReverseGeocoder*>());
	NEW_CLASS(mut, Sync::Mutex());
	NEW_CLASS(langMaps, Data::ArrayList<LangMap*>());
	this->conn = 0;
	this->conn = DB::DBTool::ODBCSource(dsn, uid, pwd, log, false, L"MAPDB: ");
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::AssistedRReverseGeocoder::~AssistedRReverseGeocoder()
{
	OSInt i = revGeos->GetCount();
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = revGeos->RemoveAt(i);
		DEL_CLASS(revGeo);
	}
	DEL_CLASS(revGeos);
	i = langMaps->GetCount();
	while (i-- > 0)
	{
		MemFree(langMaps->RemoveAt(i));
	}
	DEL_CLASS(langMaps);
	if (this->conn)
	{
		DEL_CLASS(this->conn);
	}
	DEL_CLASS(mut);
}

WChar *Map::AssistedRReverseGeocoder::SearchName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	DB::SQLBuilder *sql;
	DB::DBReader *r;
	WChar *sptr = 0;
	OSInt i;
	Int32 lang;
	if (this->conn == 0)
		return 0;

	i = GetLangIndex(lcid);
	if (i < 0)
		return 0;
	lang = this->langMaps->GetItem(i)->lang;
	Int32 xind = Math::Double2Int32(lon * 5000);
	Int32 yind = Math::Double2Int32(lat * 5000);

	mut->Lock();
	NEW_CLASS(sql, DB::SQLBuilder(this->conn));
	sql->AppendCmd((const UTF8Char*)"select name from mapaddr where lang = ");
	sql->AppendInt32(lang);
	sql->AppendCmd((const UTF8Char*)" and xind = ");
	sql->AppendInt32(xind);
	sql->AppendCmd((const UTF8Char*)" and yind = ");
	sql->AppendInt32(yind);
	r = this->conn->ExecuteReader(sql->ToString());
	if (r)
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff);
			this->conn->CloseReader(r);
			DEL_CLASS(sql);
			mut->Unlock();
			return sptr;
		}
		this->conn->CloseReader(r);
	}


	i = this->revGeos->GetCount();
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
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sql->Clear();
		sql->AppendCmd(L"insert into mapaddr (lang, xind, yind, name, createTime) values (");
		sql->AppendInt32(lang);
		sql->AppendCmd(L", ");
		sql->AppendInt32(xind);
		sql->AppendCmd(L", ");
		sql->AppendInt32(yind);
		sql->AppendCmd(L", ");
		sql->AppendStr(buff);
		sql->AppendCmd(L", ");
		sql->AppendDate(&dt);
		sql->AppendCmd(L")");
		this->conn->ExecuteNonQuery(sql->ToString());

		DEL_CLASS(sql);
		mut->Unlock();
		return sptr;
	}
	else
	{
		DEL_CLASS(sql);
		mut->Unlock();
		return 0;
	}
}

WChar *Map::AssistedRReverseGeocoder::CacheName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	DB::SQLBuilder *sql;
	DB::DBReader *r;
	WChar *sptr = 0;
	OSInt i;
	Int32 lang;
	if (this->conn == 0)
		return 0;

	i = GetLangIndex(lcid);
	if (i < 0)
		return 0;
	lang = this->langMaps->GetItem(i)->lang;
	Int32 xind = Math::Double2Int(lon * 5000);
	Int32 yind = Math::Double2Int(lat * 5000);

	mut->Lock();
	NEW_CLASS(sql, DB::SQLBuilder(this->conn));
	sql->AppendCmd(L"select name from mapaddr where lang = ");
	sql->AppendInt32(lang);
	sql->AppendCmd(L" and xind = ");
	sql->AppendInt32(xind);
	sql->AppendCmd(L" and yind = ");
	sql->AppendInt32(yind);
	r = this->conn->ExecuteReader(sql->ToString());
	if (r)
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff);
			this->conn->CloseReader(r);
			DEL_CLASS(sql);
			mut->Unlock();
			return sptr;
		}
		this->conn->CloseReader(r);
	}


	i = this->revGeos->GetCount();
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
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sql->Clear();
		sql->AppendCmd(L"insert into mapaddr (lang, xind, yind, name, createTime) values (");
		sql->AppendInt32(lang);
		sql->AppendCmd(L", ");
		sql->AppendInt32(xind);
		sql->AppendCmd(L", ");
		sql->AppendInt32(yind);
		sql->AppendCmd(L", ");
		sql->AppendStr(buff);
		sql->AppendCmd(L", ");
		sql->AppendDate(&dt);
		sql->AppendCmd(L")");
		this->conn->ExecuteNonQuery(sql->ToString());

		DEL_CLASS(sql);
		mut->Unlock();
		return sptr;
	}
	else
	{
		DEL_CLASS(sql);
		mut->Unlock();
		return 0;
	}
}

void Map::AssistedRReverseGeocoder::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
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

void Map::AssistedRReverseGeocoder::AddLangMap(Int32 lcid, Int32 lang)
{
	OSInt index = GetLangIndex(lcid);
	if (index < 0)
	{
		LangMap *langMap = MemAlloc(LangMap, 1);
		langMap->lcid = lcid;
		langMap->lang = lang;
		this->langMaps->Insert(~index, langMap);
	}
}

Int32 Map::AssistedRReverseGeocoder::ToLang(const WChar *name)
{
	Char sbuff[4];
	OSInt i = 4;
	while (i-- > 0)
	{
		if ((sbuff[3 - i] = (Char)*name++) == 0)
			break;
	}
	while (i-- > 0)
	{
		sbuff[3 - i] = 0;
	}
	return *(Int32*)sbuff;
}
