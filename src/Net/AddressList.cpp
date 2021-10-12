#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/AddressList.h"

Net::AddressList::AddressList()
{
	NEW_CLASS(this->addrList, Data::ArrayList<const Net::SocketUtil::AddressInfo *>());
}

Net::AddressList::~AddressList()
{
	this->Clear();
	DEL_CLASS(this->addrList);
}

UOSInt Net::AddressList::Add(const Net::SocketUtil::AddressInfo *val)
{
	return this->addrList->Add(CloneItem(val));
}

UOSInt Net::AddressList::AddRange(const Net::SocketUtil::AddressInfo **arr, UOSInt cnt)
{
	this->addrList->EnsureCapacity(this->GetCapacity() + cnt);
	UOSInt i = 0;
	while (i < cnt)
	{
		this->Add(arr[i]);
	}
	return cnt;
}

Bool Net::AddressList::Remove(const Net::SocketUtil::AddressInfo *val)
{
	UOSInt i = this->IndexOf(val);
	if (i == INVALID_INDEX)
		return false;
	this->FreeItem(this->addrList->RemoveAt(i));
	return true;
}

const Net::SocketUtil::AddressInfo *Net::AddressList::RemoveAt(UOSInt index)
{
	return this->addrList->RemoveAt(index);
}

void Net::AddressList::Insert(UOSInt index, const Net::SocketUtil::AddressInfo *val)
{
	this->addrList->Insert(index, CloneItem(val));
}

UOSInt Net::AddressList::IndexOf(const Net::SocketUtil::AddressInfo *val)
{
	UOSInt i = 0;
	UOSInt j = this->addrList->GetCount();
	while (i < j)
	{
		if (Net::SocketUtil::AddrEquals(this->addrList->GetItem(i), val))
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

void Net::AddressList::Clear()
{
	LIST_FREE_FUNC(this->addrList, FreeItem);
	this->addrList->Clear();
}

UOSInt Net::AddressList::GetCount()
{
	return this->addrList->GetCount();
}

UOSInt Net::AddressList::GetCapacity()
{
	return this->addrList->GetCapacity();
}

const Net::SocketUtil::AddressInfo *Net::AddressList::GetItem(UOSInt index)
{
	return this->addrList->GetItem(index);
}

void Net::AddressList::SetItem(UOSInt index, const Net::SocketUtil::AddressInfo *val)
{
	UOSInt objCnt = this->addrList->GetCount();
	if (index == objCnt)
	{
		this->Add(val);
	}
	else if (index < objCnt)
	{
		MemCopyNO((Net::SocketUtil::AddressInfo*)this->addrList->GetItem(index), val, sizeof(Net::SocketUtil::AddressInfo));
	}
	else
	{
		return;
	}
}

const Net::SocketUtil::AddressInfo *Net::AddressList::CloneItem(const Net::SocketUtil::AddressInfo *addr)
{
	Net::SocketUtil::AddressInfo *newVal = MemAlloc(Net::SocketUtil::AddressInfo, 1);
	MemCopyNO(newVal, addr, sizeof(Net::SocketUtil::AddressInfo));
	return newVal;
}

void Net::AddressList::FreeItem(const Net::SocketUtil::AddressInfo *addr)
{
	MemFree((Net::SocketUtil::AddressInfo*)addr);
}
