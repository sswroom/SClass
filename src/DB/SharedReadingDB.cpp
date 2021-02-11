#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedReadingDB.h"
#include "Sync/MutexUsage.h"

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
	Sync::MutexUsage mutUsage(this->mutDB);
	this->db->Reconnect();
}

void DB::SharedReadingDB::UseObject()
{
	Sync::MutexUsage mutUsage(this->mutObj);
	this->useCnt++;
}

void DB::SharedReadingDB::UnuseObject()
{
	OSInt i;
	Sync::MutexUsage mutUsage(this->mutObj);
	i = --this->useCnt;
	mutUsage.EndUse();
	if (i <= 0)
	{
		DEL_CLASS(this);
	}
}

DB::ReadingDB *DB::SharedReadingDB::BeginUseDB()
{
	this->mutDB->Use();
	return this->db;
}

void DB::SharedReadingDB::EndUseDB()
{
	this->mutDB->Unuse();
}
