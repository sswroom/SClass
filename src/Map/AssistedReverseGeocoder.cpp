#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "Map/AssistedReverseGeocoder.h"
#include "Math/Math.h"

Map::AssistedReverseGeocoder::AssistedReverseGeocoder(DB::DBTool *db, IO::IWriter *errWriter)
{
	NEW_CLASS(revGeos, Data::ArrayList<Map::IReverseGeocoder*>());
	NEW_CLASS(mut, Sync::Mutex());
	this->conn = db;
	this->errWriter = errWriter;
	this->nextCoder = 0;
}

Map::AssistedReverseGeocoder::~AssistedReverseGeocoder()
{
	OSInt i = revGeos->GetCount();
	while (i-- > 0)
	{
		Map::IReverseGeocoder *revGeo;
		revGeo = revGeos->RemoveAt(i);
		DEL_CLASS(revGeo);
	}
	DEL_CLASS(revGeos);
	if (this->conn)
	{
		DEL_CLASS(this->conn);
	}
	DEL_CLASS(mut);
}

UTF8Char *Map::AssistedReverseGeocoder::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	DB::SQLBuilder *sql;
	DB::DBReader *r;
	UTF8Char *sptr = 0;
	if (this->conn == 0)
		return 0;

	Int32 keyx = Math::Double2Int32(lon * 5000);
	Int32 keyy = Math::Double2Int32(lat * 5000);
	if (keyx == 0 && keyy == 0)
		return 0;

	mut->Lock();
	NEW_CLASS(sql, DB::SQLBuilder(this->conn));
	sql->AppendCmd((const UTF8Char*)"select address from addrdb where lcid = ");
	sql->AppendInt32(lcid);
	sql->AppendCmd((const UTF8Char*)" and keyx = ");
	sql->AppendInt32(keyx);
	sql->AppendCmd((const UTF8Char*)" and keyy = ");
	sql->AppendInt32(keyy);
	r = this->conn->ExecuteReader(sql->ToString());
	if (r)
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			this->conn->CloseReader(r);
			DEL_CLASS(sql);
			mut->Unlock();
			return sptr;
		}
		this->conn->CloseReader(r);
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
		dt.SetCurrTimeUTC();
		sql->Clear();
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

UTF8Char *Map::AssistedReverseGeocoder::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	DB::SQLBuilder *sql;
	DB::DBReader *r;
	UTF8Char *sptr = 0;
	if (this->conn == 0)
		return 0;

	Int32 keyx = Math::Double2Int32(lon * 5000);
	Int32 keyy = Math::Double2Int32(lat * 5000);

	mut->Lock();
	NEW_CLASS(sql, DB::SQLBuilder(this->conn));
	sql->AppendCmd((const UTF8Char*)"select address from addrdb where lcid = ");
	sql->AppendInt32(lcid);
	sql->AppendCmd((const UTF8Char*)" and keyx = ");
	sql->AppendInt32(keyx);
	sql->AppendCmd((const UTF8Char*)" and keyy = ");
	sql->AppendInt32(keyy);
	r = this->conn->ExecuteReader(sql->ToString());
	if (r)
	{
		if (r->ReadNext())
		{
			sptr = r->GetStr(0, buff, buffSize);
			this->conn->CloseReader(r);
			DEL_CLASS(sql);
			mut->Unlock();
			return sptr;
		}
		this->conn->CloseReader(r);
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
		dt.SetCurrTimeUTC();
		sql->Clear();
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

void Map::AssistedReverseGeocoder::AddReverseGeocoder(Map::IReverseGeocoder *revGeo)
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
