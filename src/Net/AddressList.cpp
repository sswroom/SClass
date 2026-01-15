#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/AddressList.h"

Net::AddressList::AddressList()
{
}

Net::AddressList::~AddressList()
{
	this->Clear();
}

UIntOS Net::AddressList::Add(NN<const Net::SocketUtil::AddressInfo> val)
{
	return this->addrList.Add(CloneItem(val));
}

UIntOS Net::AddressList::AddRange(UnsafeArray<NN<const Net::SocketUtil::AddressInfo>> arr, UIntOS cnt)
{
	this->addrList.EnsureCapacity(this->GetCapacity() + cnt);
	UIntOS i = 0;
	while (i < cnt)
	{
		this->Add(CloneItem(arr[i]));
	}
	return cnt;
}

Bool Net::AddressList::Remove(NN<const Net::SocketUtil::AddressInfo> val)
{
	NN<const Net::SocketUtil::AddressInfo> addr;
	UIntOS i = this->IndexOf(val);
	if (i == INVALID_INDEX)
		return false;
	if (!this->addrList.RemoveAt(i).SetTo(addr))
		return false;
	this->FreeItem(addr);
	return true;
}

Optional<const Net::SocketUtil::AddressInfo> Net::AddressList::RemoveAt(UIntOS index)
{
	return this->addrList.RemoveAt(index);
}

void Net::AddressList::Insert(UIntOS index, NN<const Net::SocketUtil::AddressInfo> val)
{
	this->addrList.Insert(index, CloneItem(val));
}

UIntOS Net::AddressList::IndexOf(NN<const Net::SocketUtil::AddressInfo> val)
{
	UIntOS i = 0;
	UIntOS j = this->addrList.GetCount();
	while (i < j)
	{
		if (Net::SocketUtil::AddrEquals(this->addrList.GetItemNoCheck(i), val))
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

void Net::AddressList::Clear()
{
	this->addrList.FreeAll(FreeItem);
}

UIntOS Net::AddressList::GetCount() const
{
	return this->addrList.GetCount();
}

UIntOS Net::AddressList::GetCapacity() const
{
	return this->addrList.GetCapacity();
}

NN<const Net::SocketUtil::AddressInfo> Net::AddressList::GetItemNoCheck(UIntOS index) const
{
	return this->addrList.GetItemNoCheck(index);
}

Optional<const Net::SocketUtil::AddressInfo> Net::AddressList::GetItem(UIntOS index) const
{
	return this->addrList.GetItem(index);
}

void Net::AddressList::SetItem(UIntOS index, NN<const Net::SocketUtil::AddressInfo> val)
{
	UIntOS objCnt = this->addrList.GetCount();
	if (index == objCnt)
	{
		this->Add(val);
	}
	else if (index < objCnt)
	{
		MemCopyNO((Net::SocketUtil::AddressInfo*)this->addrList.GetItemNoCheck(index).Ptr(), val.Ptr(), sizeof(Net::SocketUtil::AddressInfo));
	}
	else
	{
		return;
	}
}

NN<const Net::SocketUtil::AddressInfo> Net::AddressList::CloneItem(NN<const Net::SocketUtil::AddressInfo> addr)
{
	NN<Net::SocketUtil::AddressInfo> newVal = MemAllocNN(Net::SocketUtil::AddressInfo);
	newVal.CopyFrom(addr);
	return newVal;
}

void Net::AddressList::FreeItem(NN<const Net::SocketUtil::AddressInfo> addr)
{
	MemFreeNN(NN<Net::SocketUtil::AddressInfo>::ConvertFrom(addr));
}
