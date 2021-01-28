#include "Stdafx.h"
#include "DB/DBList.h"

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
	OSInt i = this->dbList->GetCount();
	while (i-- > 0)
	{
		db = this->dbList->GetItem(i);
		DEL_CLASS(db->db);
		MemFree(db);
	}
	DEL_CLASS(this->dbList);

}

void DB::DBList::AddDB(DB::DBTool *db)
{
	DBInfo *dbInfo;
	this->dbMut->Lock();
	dbInfo = MemAlloc(DBInfo, 1);
	dbInfo->db = db;
	dbInfo->isUsing = false;
	this->dbList->Add(dbInfo);
	this->dbMut->Unlock();
}

DB::DBTool *DB::DBList::UseDB()
{
	DBInfo *dbInfo;
	OSInt i;
	OSInt j;
	OSInt k;
	if (this->dbList->GetCount() <= 0)
		return 0;

	while (true)
	{
		this->dbMut->Lock();
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
				this->dbMut->Unlock();
				return dbInfo->db;
			}
		}
		this->dbMut->Unlock();

		this->dbEvt->Wait(1000);
	}
}

void DB::DBList::UnuseDB(DB::DBTool *db)
{
	OSInt i;
	DBInfo *dbInfo;
	this->dbMut->Lock();
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
	this->dbMut->Unlock();
}

OSInt DB::DBList::GetCount()
{
	return this->dbList->GetCount();
}
