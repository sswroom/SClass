#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/DataSync/SyncClientDataMgr.h"
#include "Sync/MutexUsage.h"

SSWR::DataSync::SyncClientDataMgr::SyncClientDataMgr()
{
}

SSWR::DataSync::SyncClientDataMgr::~SyncClientDataMgr()
{
	LIST_FREE_FUNC(&this->dataList, MemFree);
}

void SSWR::DataSync::SyncClientDataMgr::AddUserData(const UInt8 *data, UOSInt dataSize)
{
	UInt8 *newData = MemAlloc(UInt8, dataSize + 4);
	WriteInt32(newData, (Int32)dataSize);
	MemCopyNO(&newData[4], data, dataSize);
	Sync::MutexUsage mutUsage(this->mut);
	this->dataList.Add(newData);
}

UOSInt SSWR::DataSync::SyncClientDataMgr::GetCount()
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->dataList.GetCount();
}

const UInt8 *SSWR::DataSync::SyncClientDataMgr::GetData(UOSInt index, UOSInt *dataSize)
{
	Sync::MutexUsage mutUsage(this->mut);
	UInt8 *buff = this->dataList.GetItem(index);
	if (buff == 0)
	{
		return 0;
	}
	*dataSize = ReadUInt32(buff);
	return buff + 4;
}

void SSWR::DataSync::SyncClientDataMgr::RemoveData(UOSInt cnt)
{
	Data::ArrayList<UInt8*> tmp;
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt j = this->dataList.GetCount();
	if (j < cnt)
	{
		cnt = j;
	}
	tmp.AddRange(this->dataList.GetArray(&j), cnt);
	this->dataList.RemoveRange(0, cnt);
	mutUsage.EndUse();
	while (cnt-- > 0)
	{
		MemFree(tmp.GetItem(cnt));
	}
}
