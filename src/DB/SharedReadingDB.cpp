#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedReadingDB.h"
#include "Sync/MutexUsage.h"

DB::SharedReadingDB::~SharedReadingDB()
{
	this->db.Delete();
}

DB::SharedReadingDB::SharedReadingDB(NN<DB::ReadingDB> db)
{
	this->db = db;
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

NN<DB::ReadingDB> DB::SharedReadingDB::UseDB(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mutDB);
	return this->db;
}
