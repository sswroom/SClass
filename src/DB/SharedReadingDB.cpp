#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedReadingDB.h"

DB::SharedReadingDB::~SharedReadingDB()
{
	DEL_CLASS(this->db);
	DEL_CLASS(this->mutDB);
	DEL_CLASS(this->mutObj);
}

DB::SharedReadingDB::SharedReadingDB(DB::ReadingDB *db)
{
	this->db = db;
	NEW_CLASS(this->mutDB, Sync::Mutex());
	NEW_CLASS(this->mutObj, Sync::Mutex());
	this->useCnt = 1;
}

void DB::SharedReadingDB::Reconnect()
{
	this->mutDB->Lock();
	this->db->Reconnect();
	this->mutDB->Unlock();
}

void DB::SharedReadingDB::UseObject()
{
	this->mutObj->Lock();
	this->useCnt++;
	this->mutObj->Unlock();
}

void DB::SharedReadingDB::UnuseObject()
{
	OSInt i;
	this->mutObj->Lock();
	i = --this->useCnt;
	this->mutObj->Unlock();
	if (i <= 0)
	{
		DEL_CLASS(this);
	}
}

DB::ReadingDB *DB::SharedReadingDB::BeginUseDB()
{
	this->mutDB->Lock();
	return this->db;
}

void DB::SharedReadingDB::EndUseDB()
{
	this->mutDB->Unlock();
}
