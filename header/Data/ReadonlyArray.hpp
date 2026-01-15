#ifndef _SM_DATA_READONLYARRAY
#define _SM_DATA_READONLYARRAY
#include "Data/DataComparer.h"

namespace Data
{
	template <class T> class ReadonlyArray
	{
	protected:
		UnsafeArray<T> arr;
		UIntOS cnt;

	public:
		ReadonlyArray(UnsafeArray<const T> arr, UIntOS cnt);
		~ReadonlyArray();

		UIntOS GetCount() const;
		const T *GetArray() const;
		T GetItem(UIntOS index) const;
		T operator [](UIntOS index) const;
		NN<ReadonlyArray<T>> Clone() const;
		Bool Equals(ReadonlyArray<T> *arr2) const;
	};

	template <class T> ReadonlyArray<T>::ReadonlyArray(UnsafeArray<const T> arr, UIntOS cnt)
	{
		this->arr = MemAllocArr(T, cnt);
		this->arr.CopyFromNO(arr, cnt);
		this->cnt = cnt;
	}

	template <class T> ReadonlyArray<T>::~ReadonlyArray()
	{
		MemFreeArr(this->arr);
	}

	template <class T> UIntOS ReadonlyArray<T>::GetCount() const
	{
		return this->cnt;
	}

	template <class T> const T *ReadonlyArray<T>::GetArray() const
	{
		return this->arr.Ptr();
	}

	template <class T> T ReadonlyArray<T>::GetItem(UIntOS index) const
	{
		return NNTHIS[index];
	}
	
	template <class T> T ReadonlyArray<T>::operator [](UIntOS index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->arr[index];
	}

	template <class T> NN<ReadonlyArray<T>> ReadonlyArray<T>::Clone() const
	{
		NN<ReadonlyArray<T>> ret;
		NEW_CLASSNN(ret, ReadonlyArray<T>(this->arr, this->cnt));
		return ret;
	}

	template <class T> Bool ReadonlyArray<T>::Equals(ReadonlyArray<T> *arr2) const
	{
		if (arr2 == 0)
		{
			return false;
		}
		if (this->cnt != arr2->cnt)
		{
			return false;
		}
		UIntOS i = this->cnt;
		while (i-- > 0)
		{
			if (Data::DataComparer::Compare(this->arr[i], arr2->arr[i])	!= 0)
			{
				return false;
			}
		}
		return true;
	}
}
#endif
