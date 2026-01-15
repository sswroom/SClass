#ifndef _SM_DATA_SYNCCIRCULARBUFFNN
#define _SM_DATA_SYNCCIRCULARBUFFNN
#include "Stdafx.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <typename T> class SyncCircularBuffNN
	{
	private:
		Sync::Mutex mut;
		UnsafeArray<NN<T>> buff;
		UOSInt capacity;
		UOSInt getIndex;
		UOSInt putIndex;

	public:
		SyncCircularBuffNN();
		~SyncCircularBuffNN();

		Bool HasItems();
		void Put(NN<T> item);
		Optional<T> Get();
		Optional<T> GetNoRemove();
		Optional<T> GetLastNoRemove();
		UOSInt GetCount();
		UOSInt IndexOf(NN<T> item);
	};
}

template<typename T> Data::SyncCircularBuffNN<T>::SyncCircularBuffNN()
{
	this->capacity = 32;
	this->buff = MemAllocArr(NN<T>, 32);
	this->getIndex = 0;
	this->putIndex = 0;
}

template<typename T> Data::SyncCircularBuffNN<T>::~SyncCircularBuffNN()
{
	MemFreeArr(this->buff);
}

template<typename T> Bool Data::SyncCircularBuffNN<T>::HasItems()
{
	Sync::MutexUsage mutUsage(this->mut);
	return this->getIndex != this->putIndex;
}

template<typename T> void Data::SyncCircularBuffNN<T>::Put(NN<T> item)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (((this->putIndex + 1) & (this->capacity - 1)) == this->getIndex)
	{
		UOSInt oldCapacity = this->capacity;
		this->capacity = oldCapacity << 1;
		UnsafeArray<NN<T>> newBuff = MemAllocArr(NN<T>, this->capacity);
		if (this->getIndex < this->putIndex)
		{
			newBuff.CopyFromNO(this->buff, oldCapacity);
		}
		else
		{
			MemCopyNO(newBuff.Ptr(), &this->buff[this->getIndex], (oldCapacity - this->getIndex) * sizeof(NN<T>));
			MemCopyNO(&newBuff[oldCapacity - this->getIndex], this->buff.Ptr(), this->putIndex * sizeof(NN<T>));
			this->getIndex = 0;
			this->putIndex = oldCapacity - 1;
		}
		MemFreeArr(this->buff);
		this->buff = newBuff;
	}
	this->buff[this->putIndex] = item;
	this->putIndex = (this->putIndex + 1) & (this->capacity - 1);
}

template<typename T> Optional<T> Data::SyncCircularBuffNN<T>::Get()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return nullptr;
	}
	NN<T> ret = this->buff[this->getIndex];
	this->getIndex = (this->getIndex + 1) & (this->capacity - 1);
	return ret;
}

template<typename T> Optional<T> Data::SyncCircularBuffNN<T>::GetNoRemove()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	return this->buff[this->getIndex];
}

template<typename T> Optional<T> Data::SyncCircularBuffNN<T>::GetLastNoRemove()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	UOSInt lastIndex = (this->putIndex - 1) & (this->capacity - 1);
	return this->buff[lastIndex];
}

template<typename T> UOSInt Data::SyncCircularBuffNN<T>::GetCount()
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

template<typename T> UOSInt Data::SyncCircularBuffNN<T>::IndexOf(NN<T> item)
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
