#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "SSWR/DataSync/SyncClientDataMgr.h"
#include "Sync/MutexUsage.h"

SSWR::DataSync::SyncClientDataMgr::SyncClientDataMgr()
{
}

SSWR::DataSync::SyncClientDataMgr::~SyncClientDataMgr()
{
	this->dataList.MemFreeAll();
}

void SSWR::DataSync::SyncClientDataMgr::AddUserData(UnsafeArray<const UInt8> data, UIntOS dataSize)
{
	UnsafeArray<UInt8> newData = MemAllocArr(UInt8, dataSize + 4);
	WriteInt32(&newData[0], (Int32)dataSize);
	MemCopyNO(&newData[4], &data[0], dataSize);
	Sync::MutexUsage mutUsage(this->mut);
	this->dataList.Add(newData);
}

UIntOS SSWR::DataSync::SyncClientDataMgr::GetCount()
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->dataList.GetCount();
}

UnsafeArrayOpt<const UInt8> SSWR::DataSync::SyncClientDataMgr::GetData(UIntOS index, OutParam<UIntOS> dataSize)
{
	Sync::MutexUsage mutUsage(this->mut);
	UnsafeArray<UInt8> buff;
	if (!this->dataList.GetItem(index).SetTo(buff))
	{
		return nullptr;
	}
	dataSize.Set(ReadUInt32(&buff[0]));
	return UnsafeArray<const UInt8>(buff) + 4;
}

void SSWR::DataSync::SyncClientDataMgr::RemoveData(UIntOS cnt)
{
	Data::ArrayListArr<UInt8> tmp;
	Sync::MutexUsage mutUsage(this->mut);
	UIntOS j = this->dataList.GetCount();
	if (j < cnt)
	{
		cnt = j;
	}
	tmp.AddRange(this->dataList.Arr(), cnt);
	this->dataList.RemoveRange(0, cnt);
	mutUsage.EndUse();
	while (cnt-- > 0)
	{
		MemFreeArr(tmp.GetItemNoCheck(cnt));
	}
}
