#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/ODBCConn.h"
#include "Map/DBAssistedRReverseGeocoder.h"
#include "Math/Math_C.h"
#include "Sync/MutexUsage.h"

OSInt Map::DBAssistedRReverseGeocoder::GetLangIndex(UInt32 lcid)
{
	OSInt i = 0;
	OSInt j = this->langMaps.GetCount() - 1;
	OSInt k;
	NN<LangMap> langMap;
	while (i <= j)
	{
		k = (i + j) >> 1;
		langMap = this->langMaps.GetItemNoCheck(k);
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

Map::DBAssistedRReverseGeocoder::DBAssistedRReverseGeocoder(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Optional<IO::Writer> errWriter)
{
	this->conn = 0;
	this->conn = DB::ODBCConn::CreateDBTool(dsn, uid, pwd, nullptr, log, CSTR("MAPDB: "));
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::DBAssistedRReverseGeocoder::~DBAssistedRReverseGeocoder()
{
	this->revGeos.DeleteAll();
	NN<LangMap> langMap;
	UOSInt i = this->langMaps.GetCount();
	while (i-- > 0)
	{
		if (this->langMaps.RemoveAt(i).SetTo(langMap))
		{
			MemFreeNN(langMap);
		}
	}
	this->conn.Delete();
}

UnsafeArrayOpt<UTF8Char> Map::DBAssistedRReverseGeocoder::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	DB::SQLBuilder *sql;
	NN<DB::DBReader> r;
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	OSInt i;
	Int32 lang;
	NN<DB::DBTool> conn;
	if (!this->conn.SetTo(conn))
		return 0;

	i = GetLangIndex(lcid);
	if (i < 0)
		return 0;
	lang = this->langMaps.GetItemNoCheck(i)->lang;
	Int32 xind = Double2Int32(pos.x * 5000);
	Int32 yind = Double2Int32(pos.y * 5000);

	Sync::MutexUsage mutUsage(this->mut);
	NEW_CLASS(sql, DB::SQLBuilder(conn));
	sql->AppendCmdC(CSTR("select name from mapaddr where lang = "));
	sql->AppendInt32(lang);
	sql->AppendCmdC(CSTR(" and xind = "));
	sql->AppendInt32(xind);
	sql->AppendCmdC(CSTR(" and yind = "));
	sql->AppendInt32(yind);
	if (conn->ExecuteReader(sql->ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			conn->CloseReader(r);
			DEL_CLASS(sql);
			return sptr;
		}
		conn->CloseReader(r);
	}


	i = this->revGeos.GetCount();
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
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sql->Clear();
		sql->AppendCmdC(CSTR("insert into mapaddr (lang, xind, yind, name, createTime) values ("));
		sql->AppendInt32(lang);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(xind);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(yind);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendStrUTF8(UnsafeArray<const UTF8Char>(buff));
		sql->AppendCmdC(CSTR(", "));
		sql->AppendDateTime(&dt);
		sql->AppendCmdC(CSTR(")"));
		conn->ExecuteNonQuery(sql->ToCString());

		DEL_CLASS(sql);
		return sptr;
	}
	else
	{
		DEL_CLASS(sql);
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> Map::DBAssistedRReverseGeocoder::CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	DB::SQLBuilder *sql;
	NN<DB::DBReader> r;
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	NN<DB::DBTool> conn;
	OSInt i;
	Int32 lang;
	if (!this->conn.SetTo(conn))
		return 0;

	i = GetLangIndex(lcid);
	if (i < 0)
		return 0;
	lang = this->langMaps.GetItemNoCheck(i)->lang;
	Int32 xind = Double2Int32(pos.x * 5000);
	Int32 yind = Double2Int32(pos.y * 5000);

	Sync::MutexUsage mutUsage(this->mut);
	NEW_CLASS(sql, DB::SQLBuilder(conn));
	sql->AppendCmdC(CSTR("select name from mapaddr where lang = "));
	sql->AppendInt32(lang);
	sql->AppendCmdC(CSTR(" and xind = "));
	sql->AppendInt32(xind);
	sql->AppendCmdC(CSTR(" and yind = "));
	sql->AppendInt32(yind);
	if (conn->ExecuteReader(sql->ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			conn->CloseReader(r);
			DEL_CLASS(sql);
			return sptr;
		}
		conn->CloseReader(r);
	}

	i = this->revGeos.GetCount();
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
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sql->Clear();
		sql->AppendCmdC(CSTR("insert into mapaddr (lang, xind, yind, name, createTime) values ("));
		sql->AppendInt32(lang);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(xind);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendInt32(yind);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendStrUTF8(UnsafeArray<const UInt8>(buff));
		sql->AppendCmdC(CSTR(", "));
		sql->AppendDateTime(&dt);
		sql->AppendCmdC(CSTR(")"));
		conn->ExecuteNonQuery(sql->ToCString());

		DEL_CLASS(sql);
		return sptr;
	}
	else
	{
		DEL_CLASS(sql);
		return 0;
	}
}

void Map::DBAssistedRReverseGeocoder::AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo)
{
	if (this->conn.NotNull())
	{
		this->revGeos.Add(revGeo);
	}
	else
	{
		revGeo.Delete();
	}
}

void Map::DBAssistedRReverseGeocoder::AddLangMap(Int32 lcid, Int32 lang)
{
	OSInt index = GetLangIndex(lcid);
	if (index < 0)
	{
		NN<LangMap> langMap = MemAllocNN(LangMap);
		langMap->lcid = lcid;
		langMap->lang = lang;
		this->langMaps.Insert(~index, langMap);
	}
}

Int32 Map::DBAssistedRReverseGeocoder::ToLang(const UTF8Char *name)
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
