#include "Stdafx.h"
#include "DB/DBList.h"
#include "Sync/MutexUsage.h"

DB::DBList::DBList()
{
	NEW_CLASS(this->dbList, Data::ArrayList<DBInfo *>());
	NEW_CLASS(this->dbMut, Sync::Mutex());
	NEW_CLASS(this->dbEvt, Sync::Event(true, (const UTF8Char*)"DB.DBList.dbEvt"));
	this->nextIndex = 0;
}

DB::DBList::~DBList()
{
	DBInfo *db;
	UOSInt i = this->dbList->GetCount();
	while (i-- > 0)
	{
		db = this->dbList->GetItem(i);
		DEL_CLASS(db->db);
		MemFree(db);
	}
	DEL_CLASS(this->dbList);
	DEL_CLASS(this->dbMut);
	DEL_CLASS(this->dbEvt);
}

void DB::DBList::AddDB(DB::DBTool *db)
{
	DBInfo *dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	dbInfo = MemAlloc(DBInfo, 1);
	dbInfo->db = db;
	dbInfo->isUsing = false;
	this->dbList->Add(dbInfo);
	mutUsage.EndUse();
}

DB::DBTool *DB::DBList::UseDB()
{
	DBInfo *dbInfo;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (this->dbList->GetCount() <= 0)
		return 0;

	while (true)
	{
		Sync::MutexUsage mutUsage(this->dbMut);
		j = this->dbList->GetCount();
		i = j;
		k = this->nextIndex;
		while (i-- > 0)
		{
			dbInfo = this->dbList->GetItem(k);
			k++;
			if (k >= j)
			{
				k -= j;
			}

			if (!dbInfo->isUsing)
			{
				dbInfo->isUsing = true;
				this->nextIndex = k;
				mutUsage.EndUse();
				return dbInfo->db;
			}
		}
		mutUsage.EndUse();

		this->dbEvt->Wait(1000);
	}
}

void DB::DBList::UnuseDB(DB::DBTool *db)
{
	OSInt i;
	DBInfo *dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	i = this->dbList->GetCount();
	while (i-- > 0)
	{
		dbInfo = this->dbList->GetItem(i);
		if (dbInfo->db == db)
		{
			dbInfo->isUsing = false;
			this->dbEvt->Set();
			break;
		}
	}
	mutUsage.EndUse();
}

UOSInt DB::DBList::GetCount()
{
	return this->dbList->GetCount();
}
