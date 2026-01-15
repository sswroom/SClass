#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "Map/DBAssistedReverseGeocoder.h"
#include "Math/Math_C.h"
#include "Sync/MutexUsage.h"

Map::DBAssistedReverseGeocoder::DBAssistedReverseGeocoder(NN<DB::DBTool> db, NN<IO::Writer> errWriter)
{
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::DBAssistedReverseGeocoder::~DBAssistedReverseGeocoder()
{
	this->revGeos.DeleteAll();
	this->conn.Delete();
}

UnsafeArrayOpt<UTF8Char> Map::DBAssistedReverseGeocoder::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	NN<DB::DBReader> r;
	UnsafeArrayOpt<UTF8Char> sptr = nullptr;
	Int32 keyx = Double2Int32(pos.GetLon() * 5000);
	Int32 keyy = Double2Int32(pos.GetLat() * 5000);
	if (keyx == 0 && keyy == 0)
		return nullptr;

	Sync::MutexUsage mutUsage(this->mut);
	DB::SQLBuilder sql(this->conn);
	sql.AppendCmdC(CSTR("select address from addrdb where lcid = "));
	sql.AppendInt32((Int32)lcid);
	sql.AppendCmdC(CSTR(" and keyx = "));
	sql.AppendInt32(keyx);
	sql.AppendCmdC(CSTR(" and keyy = "));
	sql.AppendInt32(keyy);
	if (this->conn->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			this->conn->CloseReader(r);
			mutUsage.EndUse();
			return sptr;
		}
		this->conn->CloseReader(r);
	}


	UOSInt i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItemNoCheck(this->nextCoder)->SearchName(buff, buffSize, pos, lcid);
		if (sptr.IsNull() || buff[0] == 0)
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
		Data::Timestamp ts = Data::Timestamp::UtcNow();
		sql.Clear();
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
		this->conn->ExecuteNonQuery(sql.ToCString());
		return sptr;
	}
	else
	{
		return nullptr;
	}
}

UnsafeArrayOpt<UTF8Char> Map::DBAssistedReverseGeocoder::CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	NN<DB::DBReader> r;
	UnsafeArrayOpt<UTF8Char> sptr = nullptr;
	Int32 keyx = Double2Int32(pos.GetLon() * 5000);
	Int32 keyy = Double2Int32(pos.GetLat() * 5000);

	Sync::MutexUsage mutUsage(this->mut);
	DB::SQLBuilder sql(this->conn);
	sql.AppendCmdC(CSTR("select address from addrdb where lcid = "));
	sql.AppendInt32((Int32)lcid);
	sql.AppendCmdC(CSTR(" and keyx = "));
	sql.AppendInt32(keyx);
	sql.AppendCmdC(CSTR(" and keyy = "));
	sql.AppendInt32(keyy);
	if (this->conn->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			this->conn->CloseReader(r);
			mutUsage.EndUse();
			return sptr;
		}
		this->conn->CloseReader(r);
	}


	UOSInt i = this->revGeos.GetCount();
	while (i-- > 0)
	{
		sptr = this->revGeos.GetItemNoCheck(this->nextCoder)->CacheName(buff, buffSize, pos, lcid);
		if (sptr.IsNull() || buff[0] == 0)
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
		Data::Timestamp ts = Data::Timestamp::UtcNow();
		sql.Clear();
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
		this->conn->ExecuteNonQuery(sql.ToCString());
		mutUsage.EndUse();
		return sptr;
	}
	else
	{
		mutUsage.EndUse();
		return nullptr;
	}
}

void Map::DBAssistedReverseGeocoder::AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo)
{
	this->revGeos.Add(revGeo);
}
