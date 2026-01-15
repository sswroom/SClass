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
	NN<DBInfo> db;
	Data::ArrayIterator<NN<DBInfo>> it = this->dbList.Iterator();
	while (it.HasNext())
	{
		db = it.Next();
		db->db.Delete();
		MemFreeNN(db);
	}
	this->dbList.Clear();
}

void DB::DBList::AddDB(NN<DB::DBTool> db)
{
	NN<DBInfo> dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	dbInfo = MemAllocNN(DBInfo);
	dbInfo->db = db;
	dbInfo->isUsing = false;
	this->dbList.Add(dbInfo);
}

Optional<DB::DBTool> DB::DBList::UseDB()
{
	Optional<DBInfo> dbInfo;
	NN<DBInfo> nndbInfo;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	if (this->dbList.GetCount() <= 0)
		return nullptr;

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

			if (dbInfo.SetTo(nndbInfo) && !nndbInfo->isUsing)
			{
				nndbInfo->isUsing = true;
				this->nextIndex = k;
				return nndbInfo->db;
			}
		}
		mutUsage.EndUse();

		this->dbEvt.Wait(1000);
	}
}

void DB::DBList::UnuseDB(NN<DB::DBTool> db)
{
	UIntOS i;
	NN<DBInfo> dbInfo;
	Sync::MutexUsage mutUsage(this->dbMut);
	i = this->dbList.GetCount();
	while (i-- > 0)
	{
		if (this->dbList.GetItem(i).SetTo(dbInfo) && dbInfo->db == db)
		{
			dbInfo->isUsing = false;
			this->dbEvt.Set();
			break;
		}
	}
}

UIntOS DB::DBList::GetCount() const
{
	return this->dbList.GetCount();
}
