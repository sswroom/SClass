#include "Stdafx.h"
#include "DB/DBList.h"
#include "Sync/MutexUsage.h"

DB::DBList::DBList()
{
	this->nextIndex = 0;
}

DB::DBList::~DBList()
{
	this->Close();
}

void DB::DBList::Close()
{
	NotNullPtr<DBInfo> db;
	Data::ArrayIterator<NotNullPtr<DBInfo>> it = this->dbList.Iterator();
	while (it.HasNext())
	{
		db = it.Next();
		db->db.Delete();
		MemFreeNN(db);
	}
	this->dbList.Clear();
}

void DB::DBList::AddDB(NotNullPtr<DB::DBTool> db)
{
	NotNullPtr<DBInfo> dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	dbInfo = MemAllocNN(DBInfo, 1);
	dbInfo->db = db;
	dbInfo->isUsing = false;
	this->dbList.Add(dbInfo);
}

DB::DBTool *DB::DBList::UseDB()
{
	DBInfo *dbInfo;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (this->dbList.GetCount() <= 0)
		return 0;

	while (true)
	{
		Sync::MutexUsage mutUsage(this->dbMut);
		j = this->dbList.GetCount();
		i = j;
		k = this->nextIndex;
		while (i-- > 0)
		{
			dbInfo = this->dbList.GetItem(k);
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
				return dbInfo->db.Ptr();
			}
		}
		mutUsage.EndUse();

		this->dbEvt.Wait(1000);
	}
}

void DB::DBList::UnuseDB(NotNullPtr<DB::DBTool> db)
{
	UOSInt i;
	DBInfo *dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	i = this->dbList.GetCount();
	while (i-- > 0)
	{
		dbInfo = this->dbList.GetItem(i);
		if (dbInfo->db == db)
		{
			dbInfo->isUsing = false;
			this->dbEvt.Set();
			break;
		}
	}
}

UOSInt DB::DBList::GetCount() const
{
	return this->dbList.GetCount();
}
