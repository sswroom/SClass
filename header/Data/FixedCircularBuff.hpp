#ifndef _SM_DATA_FIXEDCIRCULARBUFF
#define _SM_DATA_FIXEDCIRCULARBUFF
#include "Data/ArrayListNative.hpp"

namespace Data
{
	template <typename T> class FixedCircularBuff
	{
	private:
		UnsafeArray<T> buff;
		UIntOS capacity;
		UIntOS getIndex;
		UIntOS putIndex;

	public:
		FixedCircularBuff(UIntOS capacity);
		~FixedCircularBuff();

		Bool HasItems() const;
		Bool Put(T item);
		void PutAndLoop(T item);
		T Get();
		T GetNoRemove() const;
		T GetLastNoRemove() const;
		UIntOS GetCount() const;
		UIntOS IndexOf(T item) const;
		UIntOS GetPutIndex() const;
		UIntOS GetItems(NN<Data::ArrayListNative<T>> list) const;
	};
}

template<typename T> Data::FixedCircularBuff<T>::FixedCircularBuff(UIntOS capacity)
{
	this->capacity = capacity;
	this->buff = MemAllocArr(T, capacity);
	this->getIndex = 0;
	this->putIndex = 0;
}

template<typename T> Data::FixedCircularBuff<T>::~FixedCircularBuff()
{
	MemFreeArr(this->buff);
}

template<typename T> Bool Data::FixedCircularBuff<T>::HasItems() const
{
	return this->getIndex != this->putIndex;
}

template<typename T> Bool Data::FixedCircularBuff<T>::Put(T item)
{
	if (((this->putIndex + 1) & (this->capacity - 1)) == this->getIndex)
	{
		return false;
	}
	this->buff[this->putIndex] = item;
	this->putIndex = (this->putIndex + 1) & (this->capacity - 1);
	return true;
}

template<typename T> void Data::FixedCircularBuff<T>::PutAndLoop(T item)
{
	this->buff[this->putIndex] = item;
	this->putIndex = (this->putIndex + 1) & (this->capacity - 1);
	if (this->putIndex == this->getIndex)
	{
		this->getIndex = (this->getIndex + 1) & (this->capacity - 1);
	}
}

template<typename T> T Data::FixedCircularBuff<T>::Get()
{
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	T ret = this->buff[this->getIndex];
	this->getIndex = (this->getIndex + 1) & (this->capacity - 1);
	return ret;
}

template<typename T> T Data::FixedCircularBuff<T>::GetNoRemove() const
{
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	return this->buff[this->getIndex];
}

template<typename T> T Data::FixedCircularBuff<T>::GetLastNoRemove() const
{
	if (this->getIndex == this->putIndex)
	{
		return 0;
	}
	UIntOS lastIndex = (this->putIndex - 1) & (this->capacity - 1);
	return this->buff[lastIndex];
}

template<typename T> UIntOS Data::FixedCircularBuff<T>::GetCount() const
{
	if (this->getIndex <= this->putIndex)
	{
		return this->putIndex - this->getIndex;
	}
	else
	{
		return this->putIndex + this->capacity - this->getIndex;
	}
}

template<typename T> UIntOS Data::FixedCircularBuff<T>::IndexOf(T item) const
{
	UIntOS andVal = this->capacity - 1;
	UIntOS i = 0;
	UIntOS j;
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

template<typename T> UIntOS Data::FixedCircularBuff<T>::GetPutIndex() const
{
	return this->putIndex;
}

template<typename T> UIntOS Data::FixedCircularBuff<T>::GetItems(NN<Data::ArrayListNative<T>> list) const
{
	UIntOS i;
	UIntOS j;
	if (this->getIndex <= this->putIndex)
	{
		i = this->getIndex;
		j = this->putIndex;
		while (i < j)
		{
			list->Add(this->buff[i]);
			i++;
		}
		return this->putIndex - this->getIndex;
	}
	else
	{
		i = this->getIndex;
		j = this->capacity;
		while (i < j)
		{
			list->Add(this->buff[i]);
			i++;
		}
		i = 0;
		j = this->putIndex;
		while (i < j)
		{
			list->Add(this->buff[i]);
			i++;
		}
		return this->putIndex + this->capacity - this->getIndex;
	}
}
#endif
