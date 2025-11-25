#ifndef _SM_DATA_SYNCCIRCULARBUFF
#define _SM_DATA_SYNCCIRCULARBUFF
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <typename T> class SyncCircularBuff
	{
	private:
		Sync::Mutex mut;
		UnsafeArray<T> buff;
		UOSInt capacity;
		UOSInt getIndex;
		UOSInt putIndex;

	public:
		SyncCircularBuff();
		~SyncCircularBuff();

		Bool HasItems();
		void Put(T item);
		T Get();
		T GetNoRemove();
		T GetLastNoRemove();
		UOSInt GetCount();
		UOSInt IndexOf(T item);
	};
}

template<typename T> Data::SyncCircularBuff<T>::SyncCircularBuff()
{
	this->capacity = 32;
	this->buff = MemAllocArr(T, 32);
	this->getIndex = 0;
	this->putIndex = 0;
}

template<typename T> Data::SyncCircularBuff<T>::~SyncCircularBuff()
{
	MemFreeArr(this->buff);
}

template<typename T> Bool Data::SyncCircularBuff<T>::HasItems()
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->getIndex != this->putIndex;
}

template<typename T> void Data::SyncCircularBuff<T>::Put(T item)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (((this->putIndex + 1) & (this->capacity - 1)) == this->getIndex)
	{
		UOSInt oldCapacity = this->capacity;
		this->capacity = oldCapacity << 1;
		UnsafeArray<T> newBuff = MemAllocArr(T, this->capacity);
		if (this->getIndex < this->putIndex)
		{
			MemCopyNO(newBuff.Ptr(), this->buff.Ptr(), oldCapacity * sizeof(T));
		}
		else
		{
			MemCopyNO(newBuff.Ptr(), &this->buff[this->getIndex], (oldCapacity - this->getIndex) * sizeof(T));
			MemCopyNO(&newBuff[oldCapacity - this->getIndex], this->buff.Ptr(), this->putIndex * sizeof(T));
			this->getIndex = 0;
			this->putIndex = oldCapacity - 1;
		}
		MemFreeArr(this->buff);
		this->buff = newBuff;
	}
	this->buff[this->putIndex] = item;
	this->putIndex = (this->putIndex + 1) & (this->capacity - 1);
}

template<typename T> T Data::SyncCircularBuff<T>::Get()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	T ret = this->buff[this->getIndex];
	this->getIndex = (this->getIndex + 1) & (this->capacity - 1);
	return ret;
}

template<typename T> T Data::SyncCircularBuff<T>::GetNoRemove()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	return this->buff[this->getIndex];
}

template<typename T> T Data::SyncCircularBuff<T>::GetLastNoRemove()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	UOSInt lastIndex = (this->putIndex - 1) & (this->capacity - 1);
	return this->buff[lastIndex];
}

template<typename T> UOSInt Data::SyncCircularBuff<T>::GetCount()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex <= this->putIndex)
	{
		return this->putIndex - this->getIndex;
	}
	else
	{
		return this->putIndex + this->capacity - this->getIndex;
	}
}

template<typename T> UOSInt Data::SyncCircularBuff<T>::IndexOf(T item)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt andVal = this->capacity - 1;
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = (this->getIndex + i) & andVal;
		if (j == this->putIndex)
		{
			return INVALID_INDEX;
		}
		if (this->buff[j] == item)
		{
			return i;
		}
		i++;
	}
}

#endif
